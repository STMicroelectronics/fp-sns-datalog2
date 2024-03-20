/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  STMicroelectronics
  * @brief   NetXDuo applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"
#include "ux_api.h"
#include "lptim.h"
#include "spi.h"
#include "gpio.h"
#include "mx_wifi.h"
#include "mx_wifi_io.h"
#include "mx.h"
#include "services/sysdebug.h"
#include "services/sysmem.h"
#include "STWIN.box.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

TX_BYTE_POOL *byte_pool = 0;

static TX_THREAD nx_app_thread;
static CHAR *nx_thread_memory_pointer;

static TX_SEMAPHORE dhcp_sem;
static TX_SEMAPHORE wait_command_sem;
static TX_QUEUE netx_app_queue;

static NX_PACKET_POOL ip_pool;
static NX_PACKET_POOL ftp_server_pool;
static NX_IP ip_instance;
static NX_DHCP dhcp_client;

static ULONG ip_address;
static ULONG net_mask;

__ALIGN_BEGIN UINT NetxAppQueue_Mem[APP_NETX_QUEUE_MSG_NUM * sizeof(NetxAppQueueMsgType)];
__ALIGN_END  // 4 bytes aligned

static UCHAR *pFtpSrvPackets; // not reentrant: only single FTP srv instance is allowed
static UCHAR *pFtpSrvMem;     // not reentrant: only single FTP srv instance is allowed
static UCHAR *pArpMem;        // not reentrant: only single instance is allowed
static UCHAR *pIpMem;         // not reentrant: only single instance is allowed
static UCHAR *pIpPktMem;      // not reentrant: only single instance is allowed

char wifi_ssid[SSID_MAX_LENGTH];
char wifi_password[PASSW_MAX_LENGTH];
char ip_str[IP_MAX_LENGTH];

char ftp_username[NX_FTP_USERNAME_SIZE];
char ftp_password[NX_FTP_PASSWORD_SIZE];

NX_FTP_SERVER ftp_server;
FX_MEDIA *pSdioDisk;

#ifdef SNTP_ENABLED
NX_SNTP_CLIENT SntpClient;
TX_THREAD AppSNTPThread;
TX_SEMAPHORE Semaphore_SNTP_ready;
CHAR buffer[64];
TX_EVENT_FLAGS_GROUP SntpFlags;

extern RTC_HandleTypeDef hrtc;
struct tm timeInfos;

/* set the SNTP network interface to the primary interface. */
UINT iface_index = 0;
#endif

IWifi_Config_t wifi_config;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
uint8_t wifi_config_wifi_connect_func(IWifi_Config_t *_this, const char *password);
uint8_t wifi_config_wifi_disconnect_func(IWifi_Config_t *_this);
uint8_t wifi_config_set_ftp_credentials_func(IWifi_Config_t *_this, const char *password);

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
void WIFI_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
void WIFI_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void WIFI_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
#endif

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
void WIFI_LPTIM_IC_CaptureCallback(LPTIM_HandleTypeDef *hlptim);
#endif

static VOID NetX_Thread_Entry(ULONG thread_input);

#ifdef SNTP_ENABLED
static VOID App_SNTP_Thread_Entry(ULONG thread_input);
static VOID display_rtc_time(RTC_HandleTypeDef *hrtc);
static VOID rtc_time_update(NX_SNTP_CLIENT *client_ptr);
static UINT kiss_of_death_handler(NX_SNTP_CLIENT *client_ptr, UINT KOD_code);
static VOID time_update_callback(NX_SNTP_TIME_MESSAGE *time_update_ptr, NX_SNTP_TIME *local_time);
#endif
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr);

static UINT server_login(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, ULONG client_ip_address, UINT client_port,
                         CHAR *name, CHAR *password, CHAR *extra_info);
static UINT server_logout(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, ULONG client_ip_address, UINT client_port,
                          CHAR *name, CHAR *password, CHAR *extra_info);
static int FTP_Srv_Init(void);
static int FTP_Srv_Start(void);
static int FTP_Srv_Stop(void);
static int FTP_Srv_DeInit(void);
static int WIFI_Connect(void);
static int WIFI_Disconnect(void);

static void netx_app_notify_command_execution(void);

static sys_error_code_t WIFI_MX_HW_Init(void);

static NetX_App_Connect_Callback connect_callback = NULL;

void netx_app_set_connect_callback(NetX_App_Connect_Callback callback)
{
  connect_callback = callback;
}

#ifdef TX_ENABLE_EVENT_TRACE
#define TRACE_BUFFER_SIZE         30000
#define TRACE_EVENT_NUM              35
UCHAR my_trace_buffer[TRACE_BUFFER_SIZE];
VOID full_buffer_callback(VOID *par)
{
//  printf ("T buff full");
  static n = 0;
  tx_trace_disable();
  tx_trace_enable(&my_trace_buffer, TRACE_BUFFER_SIZE, TRACE_EVENT_NUM);
//  tx_trace_event_filter(UX_TRACE_ALL_EVENTS);
  n++;
  if (n >= 10)
  {
    n = 0;
  }
}
#endif

static const IWifi_Config_vtbl wifi_config_functions =
{
  wifi_config_wifi_connect_func,
  wifi_config_wifi_disconnect_func,
  wifi_config_set_ftp_credentials_func
};
/* USER CODE END PFP */
/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  byte_pool = (TX_BYTE_POOL *) memory_ptr;

  wifi_config.vptr = &wifi_config_functions;

  /* USER CODE BEGIN App_NetXDuo_MEM_POOL */
  ip_address = 0;
  strcpy(ip_str, "0.0.0.0");
  strcpy(wifi_ssid, "");
  strcpy(wifi_password, "");
  strcpy(ftp_username, "anonymous");
  strcpy(ftp_password, "");

#ifdef SNTP_ENABLED /* we don't start SNTP service as RTC is already managed by DatalogAppTask.c */
  /* Allocate the memory for SNTP client thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, SNTP_CLIENT_THREAD_MEMORY, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* create the SNTP client thread */
  ret = tx_thread_create(&AppSNTPThread, "SNTP App Thread", App_SNTP_Thread_Entry, 0, pointer, SNTP_CLIENT_THREAD_MEMORY,
                         DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE, TX_DONT_START);

  if (ret != TX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  /* Create the event flags. */
  ret = tx_event_flags_create(&SntpFlags, "SNTP event flags");

  /* Check for errors */
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  tx_semaphore_create(&Semaphore_SNTP_ready, "SNTP ready Sem", 0);
#endif

  /* create App NetX cmd queue*/
  ret = tx_queue_create(&netx_app_queue, "App Netx Queue", sizeof(NetxAppQueueMsgType) / sizeof(UINT), NetxAppQueue_Mem,
                        APP_NETX_QUEUE_MSG_NUM * sizeof(NetxAppQueueMsgType));
  if (ret != TX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }
  /* create DHCP notification callback semaphore  */
  ret = tx_semaphore_create(&dhcp_sem, "DHCP Sem", 0);
  if (ret != TX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  /* create DHCP notification callback semaphore  */
  ret = tx_semaphore_create(&wait_command_sem, "WAITCMD Sem", 0);
  if (ret != TX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  /* Allocate the memory for NetX app thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &nx_thread_memory_pointer, NETX_THREAD_MEMORY, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the NetX app thread */
  ret = tx_thread_create(&nx_app_thread, "NetX App thread", NetX_Thread_Entry, 0, nx_thread_memory_pointer, NETX_THREAD_MEMORY, DEFAULT_NETX_PRIORITY, DEFAULT_NETX_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }
#ifdef TX_ENABLE_EVENT_TRACE
  tx_trace_buffer_full_notify(full_buffer_callback);
#endif
  /* USER CODE END MX_NetXDuo_Init */
  return NX_SUCCESS;
}

/* USER CODE BEGIN 1 */
/**
  * @brief  ip address change callback.
  * @param ip_instance: NX_IP instance
  * @param ptr: user data
  * @retval none
  */
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr)
{

  if (nx_ip_address_get(ip_instance, &ip_address, &net_mask) != TX_SUCCESS)
  {
    return;
  }

  snprintf(ip_str, IP_MAX_LENGTH, "%lu.%lu.%lu.%lu", (ip_address >> 24) & 0xff, (ip_address >> 16) & 0xff,
           (ip_address >> 8) & 0xff, (ip_address & 0xff));
  PRINT_IP_ADDRESS(ip_address);

  if (strcmp(ip_str, "0.0.0.0") != 0)
  {
    /* valid IP assigned or reassigned by DHCP serv: signal the thread waiting */
    tx_semaphore_put(&dhcp_sem);

  }
  else
  {
    /* IP assigned 0.0.0.0 means WiFi DHCP release during disconnection*/

  }
}

/**
  * @brief  Main thread entry.
  * @param thread_input: ULONG user argument used by the thread entry
  * @retval none
  */
static VOID NetX_Thread_Entry(ULONG thread_input)
{
  UINT ret;
  bool wifi_is_connected = false;
  bool ftp_srv_is_on = false;
  bool ftp_srv_initialized = false;
  NetxAppQueueMsgType RxMsg, TxMsg;

  for (;;)
  {
    /* receive cmds from PnPl thread */
    if (!wifi_is_connected)
    {
      tx_queue_receive(&netx_app_queue, &RxMsg, TX_WAIT_FOREVER);
    }
    else
    {
      /* if wifi is connected 5s periodic check for the IP status to detect passive disconnection
       * (FIXME needs to check periodically as the nx_ip_link_status_change_notify_set() do not trigger cb on wifi status change) */
      ret = tx_queue_receive(&netx_app_queue, &RxMsg, TX_TIMER_TICKS_PER_SECOND * 5);
      if (ret == TX_WAIT_ABORTED || ret == TX_QUEUE_EMPTY)
      {
        ULONG actual_status;
        ret = nx_ip_status_check(&ip_instance, NX_IP_INITIALIZE_DONE | NX_IP_LINK_ENABLED | NX_IP_INTERFACE_LINK_ENABLED, &actual_status, NX_NO_WAIT);
        if (ret != NX_SUCCESS)
        {
          printf("WIFI IP connection (nx_ip_status_check) lost, status: 0x%x, ret: 0x%x, line: %d\n\r", (UINT) actual_status, ret,
                 __LINE__);
          printf("WIFI IP connection roll back   ...\n\r");

          /* stop FTP srv by means of self generated cmd msg */
          TxMsg.MsgCode = FTP_SERVER_DEINIT;
          tx_queue_send(&netx_app_queue, &TxMsg, TX_WAIT_FOREVER);

          /* disconnect wifi by means of self generated cmd msg */
          TxMsg.MsgCode = WIFI_DISCONNECT;
          tx_queue_send(&netx_app_queue, &TxMsg, TX_WAIT_FOREVER);
        }
        continue;
      }
    }

    switch (RxMsg.MsgCode)
    {

      case WIFI_CONNECT:
      case WIFI_CONNECT_SYNC:
      {
        if (wifi_is_connected)
        {
          /* rejected cmd PnPL answ ? */
          break;
        }

        ret = WIFI_Connect();
        if (ret == 0)
        {
          wifi_is_connected = 1;
        }

        if (RxMsg.MsgCode == WIFI_CONNECT_SYNC)
        {
          netx_app_notify_command_execution();
        }
        else if (RxMsg.MsgCode == WIFI_DISCONNECT)
        {
          connect_callback(true);
        }
        break;
      }

      case WIFI_DISCONNECT:
      case WIFI_DISCONNECT_SYNC:
      {
        if (!wifi_is_connected)
        {
          /* reject cmd? what PnPL answ ? now simply ignore it! */
          break;
        }

        ret = WIFI_Disconnect();
        if (ret == 0)
        {
          wifi_is_connected = false;
          ftp_srv_is_on = false;   // ftp srv must be already stopped by FTP_SERVER_STOP

          ip_address = 0;
          strcpy(ip_str, "0.0.0.0");
          strcpy(wifi_ssid, "");
          strcpy(wifi_password, "");

          if (RxMsg.MsgCode == WIFI_DISCONNECT_SYNC)
          {
            netx_app_notify_command_execution();
          }
          else if (RxMsg.MsgCode == WIFI_DISCONNECT)
          {
            connect_callback(false);
          }
        }
        break;
      }

      case FTP_SERVER_INIT:
      {
        if (!wifi_is_connected || ftp_srv_is_on)
        {
          break;
        }
        if (FTP_Srv_Init() != 0)
        {
          printf("ERROR FTP_Srv_Init\n\r");

          break;
        }
        ftp_srv_initialized = true;

        if (FTP_Srv_Start() != 0)
        {
          printf("ERROR FTP_Srv_Start\n\r");

          break;
        }
        ftp_srv_is_on = true;

        ULONG actual_status;
        /* this check if everything is ok with IP stack and WiFi connection */
        /* FIXME drv to be fixed according to tkt 155760, for the moment ip link and others checks are commented  */
        ret = nx_ip_status_check(&ip_instance,
                                 NX_IP_INITIALIZE_DONE /* | NX_IP_LINK_ENABLED | NX_IP_ADDRESS_RESOLVED | NX_IP_UDP_ENABLED | NX_IP_IGMP_ENABLED */,
                                 &actual_status, NX_IP_PERIODIC_RATE * 10);  // give 10 Sec to wifi drv connection init
        if (ret != NX_SUCCESS)
        {
          printf("nx_ip_status_check failed status: 0x%x, ret: 0x%x\n\r", (unsigned int) actual_status, ret);
          /* FIXME roll back resources */
//            return NX_NOT_ENABLED;
        }
#ifdef TX_ENABLE_EVENT_TRACE
        /* to start tracing at FTP srv start */
//          tx_trace_enable(&my_trace_buffer, TRACE_BUFFER_SIZE, TRACE_EVENT_NUM);
//          tx_trace_event_filter(UX_TRACE_ALL_EVENTS);
#endif
        break;
      }
      case FTP_SERVER_START :
      {
        if (ftp_srv_initialized)
        {
          if (FTP_Srv_Start() != 0)
          {
            printf("ERROR FTP_Srv_Start\n\r");

            break;
          }
          ftp_srv_is_on = true;
        }
        break;
      }
      case FTP_SERVER_STOP :
      {
        if (ftp_srv_is_on)
        {
          if (FTP_Srv_Stop() != 0)
          {
            printf("ERROR FTP_Srv_Stop\n\r");

            break;
          }
          ftp_srv_is_on = false;
        }
        break;
      }
      case FTP_SERVER_DEINIT :
      {
        if (!wifi_is_connected || !ftp_srv_is_on)
        {
          break;
        }

        if (FTP_Srv_DeInit() != 0)
        {
          printf("ERROR FTP_Srv_Stop\n\r");
          break;
        }
        ftp_srv_initialized = false;
        ftp_srv_is_on = false;
        break;
      }

      default:
        printf("ERROR: unknown msg on netx_app_queue\n\r");
        break;
    }
  }
}

#ifdef SNTP_ENABLED
/**
  * @brief  DNS Create Function.
  * @param dns_ptr
  * @retval ret
  */
UINT dns_create(NX_DNS *dns_ptr)
{
  UINT ret = NX_SUCCESS;

  /* Create a DNS instance for the Client */
  ret = nx_dns_create(dns_ptr, &ip_instance, (UCHAR *)"DNS Client");

  if (ret)
  {
    Error_Handler();
  }

  /* Initialize DNS instance with the DNS server Address */
  ret = nx_dns_server_add(dns_ptr, USER_DNS_ADDRESS);
  if (ret)
  {
    Error_Handler();
  }

  return ret;
}

/**
  * @brief  SNTP thread entry.
  * @param thread_input: ULONG user argument used by the thread entry
  * @retval none
  */
/* Define the client thread.  */
static void App_SNTP_Thread_Entry(ULONG info)
{
  UINT ret;
  ULONG  seconds, fraction;
  ULONG  events = 0;
  UINT   server_status;
  NXD_ADDRESS sntp_server_ip;
  NX_PARAMETER_NOT_USED(info);

  sntp_server_ip.nxd_ip_version = 4;

  /* Create a DNS client */
  ret = dns_create(&DnsClient);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* Look up SNTP Server address. */
  ret = nx_dns_host_by_name_get(&DnsClient, (UCHAR *)SNTP_SERVER_NAME,
                                &sntp_server_ip.nxd_ip_address.v4, DEFAULT_TIMEOUT);

  /* Check for error. */
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* Create the SNTP Client */
  ret =  nx_sntp_client_create(&SntpClient, &ip_instance, iface_index, &NetXPool, NULL, kiss_of_death_handler, NULL);

  /* Check for error. */
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* Setup time update callback function. */
  nx_sntp_client_set_time_update_notify(&SntpClient, time_update_callback);

  /* Use the IPv4 service to set up the Client and set the IPv4 SNTP server. */
  ret = nx_sntp_client_initialize_unicast(&SntpClient, sntp_server_ip.nxd_ip_address.v4);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* Run whichever service the client is configured for. */
  ret = nx_sntp_client_run_unicast(&SntpClient);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }
  else
  {
    PRINT_CNX_SUCC();
  }
  uint8_t first_sync = 1;

  do
  {

    /* Wait for a server update event, periodic (based on NX_SNTP_CLIENT_UNICAST_POLL_INTERVAL) synchronization */
    tx_event_flags_get(&SntpFlags, SNTP_UPDATE_EVENT, TX_OR_CLEAR, &events, TX_WAIT_FOREVER);

    if (events == SNTP_UPDATE_EVENT)
    {
      /* Check for valid SNTP server status. */
      ret = nx_sntp_client_receiving_updates(&SntpClient, &server_status);

      if ((ret != NX_SUCCESS) || (server_status == NX_FALSE))
      {
        /* We do not have a valid update. */
        Error_Handler();
      }
      /* We have a valid update.  Get the SNTP Client time.  */
      ret = nx_sntp_client_get_local_time_extended(&SntpClient, &seconds, &fraction, NX_NULL, 0);

      /* Set Current time from SNTP TO RTC */
      rtc_time_update(&SntpClient);
//      display_rtc_time(&hrtc);

      ret = nx_sntp_client_utility_display_date_time(&SntpClient, buffer, 64);

      if (ret != NX_SUCCESS)
      {
        printf("Internal error with getting local time 0x%x\n", ret);
        Error_Handler();
      }
      else
      {
        printf("\nSNTP update :\n");
        printf("%s\n\n", buffer);

        if (first_sync)
        {
          first_sync  = 0;
          /* notify app_threadx of SNTP sinchronization: AI & Module play can start now */
          tx_semaphore_put(&Semaphore_SNTP_ready);
        }
      }
    }
    else if (events == SNTP_KOD_EVENT)
    {
      /* KOD event rx from srv: we can stop the SNTP clnt or restart with different SNTP server */
      ret = nx_sntp_client_stop(&SntpClient);
      if (ret != NX_SUCCESS)
      {
        Error_Handler();
      }

      /* When done with the SNTP Client, we delete it */
      ret = nx_sntp_client_delete(&SntpClient);
      if (ret != NX_SUCCESS)
      {
        Error_Handler();
      }
    }
  } while (events != SNTP_KOD_EVENT);

  /* SNTP thread commit suicide */
  printf("====>>> SNTP KOD event received from server: Client suicide\n\r");
  tx_thread_terminate(tx_thread_identify());

}

/* This application defined handler for handling a Kiss of Death packet is not
required by the SNTP Client. A KOD handler should determine
if the Client task should continue vs. abort sending/receiving time data
from its current time server, and if aborting if it should remove
the server from its active server list.

Note that the KOD list of codes is subject to change. The list
below is current at the time of this software release. */

static UINT kiss_of_death_handler(NX_SNTP_CLIENT *client_ptr, UINT KOD_code)
{
  UINT    remove_server_from_list = NX_FALSE;
  UINT    status = NX_SUCCESS;

  NX_PARAMETER_NOT_USED(client_ptr);

  /* Handle kiss of death by code group. */
  switch (KOD_code)
  {

    case NX_SNTP_KOD_RATE:
    case NX_SNTP_KOD_NOT_INIT:
    case NX_SNTP_KOD_STEP:

      /* Find another server while this one is temporarily out of service.  */
      status =  NX_SNTP_KOD_SERVER_NOT_AVAILABLE;

      break;

    case NX_SNTP_KOD_AUTH_FAIL:
    case NX_SNTP_KOD_NO_KEY:
    case NX_SNTP_KOD_CRYP_FAIL:

      /* These indicate the server will not service client with time updates
      without successful authentication. */

      remove_server_from_list =  NX_TRUE;

      break;


    default:

      /* All other codes. Remove server before resuming time updates. */

      remove_server_from_list =  NX_TRUE;
      break;
  }

  /* Removing the server from the active server list? */
  if (remove_server_from_list)
  {

    /* Let the caller know it has to bail on this server before resuming service. */
    status = NX_SNTP_KOD_REMOVE_SERVER;
  }

  /* send event to sntp client thread */
  tx_event_flags_set(&SntpFlags, SNTP_KOD_EVENT, TX_OR);

  return status;
}

/* This application defined handler for notifying SNTP time update event.  */
static VOID time_update_callback(NX_SNTP_TIME_MESSAGE *time_update_ptr, NX_SNTP_TIME *local_time)
{
  NX_PARAMETER_NOT_USED(time_update_ptr);
  NX_PARAMETER_NOT_USED(local_time);
  tx_event_flags_set(&SntpFlags, SNTP_UPDATE_EVENT, TX_OR);
}

/* This routine updates Time from SNTP to STM32 RTC */
static void rtc_time_update(NX_SNTP_CLIENT *client_ptr)
{
  RTC_DateTypeDef sdatestructure = {0};
  RTC_TimeTypeDef stimestructure = {0};
  struct tm ts;
  CHAR  temp[32] = {0};

  /* convert SNTP time (seconds since 01-01-1900 to 01-01-1970)

  EPOCH_TIME_DIFF is equivalent to 70 years in sec
  calculated with www.epochconverter.com/date-difference
  This constant is used to delete difference between :
  Epoch converter (referenced to 1970) and SNTP (referenced to 1900) */
  time_t timestamp = client_ptr->nx_sntp_current_server_time_message.receive_time.seconds - EPOCH_TIME_DIFF;

  /* convert time in yy/mm/dd hh:mm:sec */
  ts = *localtime(&timestamp);

  /* convert date composants to hex format */
  sprintf(temp, "%d", (ts.tm_year - 100));
  sdatestructure.Year = strtol(temp, NULL, 16);
  sprintf(temp, "%d", ts.tm_mon + 1);
  sdatestructure.Month = strtol(temp, NULL, 16);
  sprintf(temp, "%d", ts.tm_mday);
  sdatestructure.Date = strtol(temp, NULL, 16);
  /* dummy weekday */
  sdatestructure.WeekDay = 0x00;

  if (HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* convert time composants to hex format */
  sprintf(temp, "%d", ts.tm_hour);
  stimestructure.Hours = strtol(temp, NULL, 16);
  sprintf(temp, "%d", ts.tm_min);
  stimestructure.Minutes = strtol(temp, NULL, 16);
  sprintf(temp, "%d", ts.tm_sec);
  stimestructure.Seconds = strtol(temp, NULL, 16);

  if (HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

}

/* this routine displays time from RTC */
static void display_rtc_time(RTC_HandleTypeDef *hrtc)
{
  RTC_TimeTypeDef RTC_Time = {0};
  RTC_DateTypeDef RTC_Date = {0};

  HAL_RTC_GetTime(hrtc, &RTC_Time, RTC_FORMAT_BCD);
  HAL_RTC_GetDate(hrtc, &RTC_Date, RTC_FORMAT_BCD);

  printf("%02x-%02x-20%02x / %02x:%02x:%02x\n", \
         RTC_Date.Date, RTC_Date.Month, RTC_Date.Year, RTC_Time.Hours, RTC_Time.Minutes, RTC_Time.Seconds);
}

/* this routine retrieve time from RTC */
void get_rtc_date_time(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *RTC_Date, RTC_TimeTypeDef *RTC_Time)
{
  HAL_RTC_GetTime(hrtc, RTC_Time, RTC_FORMAT_BCD);
  HAL_RTC_GetDate(hrtc, RTC_Date, RTC_FORMAT_BCD);
}
#endif


static UINT server_login(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, ULONG client_ip_address, UINT client_port,
                         CHAR *name, CHAR *password, CHAR *extra_info)
{
  char ipaddr_string[IP_MAX_LENGTH];
  NX_PARAMETER_NOT_USED(extra_info);

  if (strcmp(name, ftp_username) != 0 || strcmp(password, ftp_password) != 0)
  {
    printf("FTP Wrong username or password!\n");
    return (NX_INVALID_PARAMETERS);
  }

  snprintf(ipaddr_string, IP_MAX_LENGTH, "%lu.%lu.%lu.%lu", (client_ip_address >> 24) & 0xff,
           (client_ip_address >> 16) & 0xff, (client_ip_address >> 8) & 0xff,
           (client_ip_address & 0xff));
  printf("FTP User: %s IP: %s logged in!\n", ftp_username, ipaddr_string);
  /* Return success.  */
  return (NX_SUCCESS);
}

static UINT server_logout(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, ULONG client_ip_address, UINT client_port,
                          CHAR *name, CHAR *password, CHAR *extra_info)
{
  char ipaddr_string[IP_MAX_LENGTH];
  NX_PARAMETER_NOT_USED(extra_info);

  snprintf(ipaddr_string, IP_MAX_LENGTH, "%lu.%lu.%lu.%lu", (client_ip_address >> 24) & 0xff,
           (client_ip_address >> 16) & 0xff, (client_ip_address >> 8) & 0xff,
           (client_ip_address & 0xff));

  printf("FTP User: %s IP: %s logged out!\n", name, ipaddr_string);

  /* FIXME if ftp client disconnect (passive or active) does it need to roll back the FTP srv and inform phone app somehow ? */

  /* Always return success.  */
  return (NX_SUCCESS);
}

VOID link_status_change_notify_callback(NX_IP *ip_ptr, UINT interface_index, UINT link_up)
{
  /* FIXME if wifi link disconnect (passive or active) does it need to roll back the resources and inform phone app somehow ? */
  /* because of https://intbugzilla.st.com/show_bug.cgi?id=155760 this API is not triggered */
  printf("link_status_change_notify_callback link status: %d\n\r", link_up);
  if (link_up)
  {

  }
  else
  {
    // link down

  }
}

static int WIFI_Connect(void)
{
  INT ret = 0;

  printf("WiFi connecting to: %s, passw:%s...\n\r", wifi_ssid, wifi_password);

  // Wi-Fi module reset via DCDC2 power OFF/ON
  BSP_Disable_DCDC2();
  tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 3); // 300ms
  BSP_Enable_DCDC2();
  tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 3); // 300ms

  WIFI_MX_HW_Init();

  nx_system_initialize();
  /* Allocate the memory for packet_pool.  */
  ret = tx_byte_allocate(byte_pool, (VOID **) &pIpPktMem, NX_PACKET_POOL_SIZE, TX_NO_WAIT);
  if (ret != TX_SUCCESS)
  {
    printf("WIFI_Connect stack memory allocation failed : 0x%02x\n", ret);
    return ret;
  }

  /* Create the IP Packet pool to be used for IP packet allocation */
  ret = nx_packet_pool_create(&ip_pool, "Main Packet Pool", PAYLOAD_SIZE, pIpPktMem, NX_PACKET_POOL_SIZE);
  if (ret != NX_SUCCESS)
  {
    printf("WIFI_Connect nx_packet_pool_create failed : 0x%02x\n", ret);
    /* FIXME roll back */
    return ret;
  }

  /* Allocate the memory for Ip_Instance */
  ret = tx_byte_allocate(byte_pool, (VOID **) &pIpMem, IP_MEMORY_SIZE, TX_NO_WAIT);
  if (ret != TX_SUCCESS)
  {
    printf("WIFI_Connect ip mem allocation failed : 0x%02x\n", ret);
    return ret;
  }

  /** FIXME the wifi SSID and passw should be acquired before call to ip_create. (tiket 150063)
    * NB possibility to use UINT nx_ip_driver_direct_command(NX_IP *ip_ptr, UINT command, ULONG *return_value_ptr);
    * to  direct interface the drv for SSDID discovery
    * wifi Drv need to be fixed (tkt 155760) also to notify passive disconnections generating _nx_ip_driver_link_status_event()
    * and allowing the correct response from nx_ip_status_check()
    * This fix allow also a correct behavior of nx_ip_link_status_change_notify_set(,,cb)
    * that currently do not trigger the cb in case of passive disconnection
    */
  /* Create the main NX_IP instance */
  ret = nx_ip_create(&ip_instance, "Main Ip instance", NULL_ADDRESS, NULL_ADDRESS, &ip_pool, nx_driver_emw3080_entry, pIpMem, IP_MEMORY_SIZE,
                     DEFAULT_NETX_PRIORITY);
  if (ret != NX_SUCCESS)
  {
    printf("WIFI_Connect nx_ip_create failed : 0x%02x\n", ret);
    return ret;
  }

  /* wait for wifi drv to start */
  ULONG actual_status;
  ret = nx_ip_status_check(&ip_instance, NX_IP_INITIALIZE_DONE | NX_IP_LINK_ENABLED | NX_IP_INTERFACE_LINK_ENABLED, &actual_status, NX_IP_PERIODIC_RATE * 5); // give 5 Sec to wifi drv connection init
  if (ret != NX_SUCCESS)
  {
    printf("WIFI_Connect nx_ip_status_check failed status: 0x%x, ret: 0x%x\n\r", (UINT) actual_status, ret);
    /* FIXME roll back resources ? or go ahead ?*/
//      return ret;
  }

  /* create the DHCP client */
  ret = nx_dhcp_create(&dhcp_client, &ip_instance, "DHCP Client");
  if (ret != NX_SUCCESS)
  {
    printf("WIFI_Connect nx_dhcp_create failed : 0x%02x\n", ret);
    return ret;
  }

  /* Allocate the memory for ARP */
  ret = tx_byte_allocate(byte_pool, (VOID **) &pArpMem, ARP_MEMORY_SIZE, TX_NO_WAIT);
  if (ret != TX_SUCCESS)
  {
    printf("WIFI_Connect ARP mem allocation failed : 0x%02x\n", ret);
    return ret;
  }

  /* Enable the ARP protocol and provide the ARP cache size for the IP instance */
  ret = nx_arp_enable(&ip_instance, (VOID *) pArpMem, ARP_MEMORY_SIZE);
  if (ret != NX_SUCCESS)
  {
    printf("WIFI_Connect nx_arp_enable failed : 0x%02x\n", ret);
    return ret;
  }

  /* Enable the ICMP */
  ret = nx_icmp_enable(&ip_instance);
  if (ret != NX_SUCCESS)
  {
    printf("WIFI_Connect nx_icmp_enable failed : 0x%02x\n", ret);
    return ret;
  }

  /* Enable the UDP protocol required for  DHCP communication */
  ret = nx_udp_enable(&ip_instance);
  if (ret != NX_SUCCESS)
  {
    printf("WIFI_Connect nx_udp_enable failed : 0x%02x\n", ret);
    return ret;
  }

  /* Enable the TCP protocol */
  ret = nx_tcp_enable(&ip_instance);
  if (ret != NX_SUCCESS)
  {
    printf("WIFI_Connect nx_tcp_enable failed : 0x%02x\n", ret);
    return ret;
  }

  /* register DHCP clnt notification cb */
  ret = nx_ip_address_change_notify(&ip_instance, ip_address_change_notify_callback, NULL);
  if (ret != NX_SUCCESS)
  {
    printf("WIFI_Connect nx_ip_address_change_notify failed : 0x%02x\n", ret);
    return ret;
  }

  ret = nx_ip_link_status_change_notify_set(&ip_instance, link_status_change_notify_callback);
  if (ret != NX_SUCCESS)
  {
    printf("WIFI_Connect nx_ip_link_status_change_notify_set failed : 0x%02x\n", ret);
    return ret;
  }

  /* start DHCP clnt */
  ret = nx_dhcp_start(&dhcp_client);
  if (ret != NX_SUCCESS)
  {
    printf("WIFI_Connect nx_dhcp_start failed : 0x%02x\n", ret);
    return ret;
  }

  /* Start (just for debugging) WiFi SSID scan */
  //scan_cmd(0, NULL);

  printf("Waiting for IP address ...\n");

  /* wait until an IP address is ready */
  ret = tx_semaphore_get(&dhcp_sem, NX_IP_PERIODIC_RATE * 20);
  if (ret != TX_SUCCESS)
  {
    printf("ERROR IP Addr not acquired in %d sec\n\r", NX_IP_PERIODIC_RATE * 20 / NX_IP_PERIODIC_RATE);
    if (nx_dhcp_release(&dhcp_client) != NX_SUCCESS)
    {
      /* what to do in this case of double error ? */
      printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
    }

    if (nx_dhcp_stop(&dhcp_client) != NX_SUCCESS)
    {
      printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
    }

    if (nx_dhcp_delete(&dhcp_client) != NX_SUCCESS)
    {
      printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
    }

    if (nx_ip_delete(&ip_instance) != NX_SUCCESS)
    {
      printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
    }

    if (tx_byte_release(pArpMem) != TX_SUCCESS)
    {
      printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
    }

    if (tx_byte_release(pIpMem) != TX_SUCCESS)
    {
      printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
    }

    if (nx_packet_pool_delete(&ip_pool) != NX_SUCCESS)
    {
      printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
    }

    if (tx_byte_release(pIpPktMem) != TX_SUCCESS)
    {
      printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
    }
    return ret;
  }

#ifdef SNTP_ENABLED
  /* the network is correctly initialized, start the SNTP client thread */
  tx_thread_resume(&AppSNTPThread);
#endif
  return ret;
}

static int WIFI_Disconnect(void)
{
  INT ret = 0;
  /* WiFi disconnection and resources roll back  */

  printf("WiFi disconnecting from %s \n\r", wifi_ssid);

#ifdef SNTP_ENABLED
  /* first stop the SNTP client thread */
  tx_thread_terminate(
    &AppSNTPThread);   // FIXME this is unsafe: thread should not be terminated asynchronously, better to send a msg to th */
  tx_thread_reset(&AppSNTPThread);       // FIXME to release th stack the th should be deleted instead of reset
#endif

  /* release the acquired IP */
  ret = nx_dhcp_release(&dhcp_client);
  if (ret != NX_SUCCESS)
  {
//      Error_Handler();
    printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
  }

  /* stop DHCP client */
  ret = nx_dhcp_stop(&dhcp_client);
  if (ret != NX_SUCCESS)
  {
//      Error_Handler();
    printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
  }

  /* free DHCP client resources*/
  ret = nx_dhcp_delete(&dhcp_client);
  if (ret != NX_SUCCESS)
  {
//      Error_Handler();
    printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
  }

  /* ARP static cache delete */
  ret = nx_arp_static_entries_delete(&ip_instance);
  if (ret != NX_SUCCESS)
  {
//      Error_Handler();
    printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
  }

  /* ARP dynamic cache delete */
  ret = nx_arp_dynamic_entries_invalidate(&ip_instance);
  if (ret != NX_SUCCESS)
  {
//      Error_Handler();
    printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
  }

  /* free IP layer resources and wifi drv */
  ret = nx_ip_delete(&ip_instance);
  if (ret != NX_SUCCESS)
  {
//      Error_Handler();
    printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
  }
  /* give drv thread time to delete */
  tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 3);

  /* free the ARP mem */
  ret = tx_byte_release(pArpMem);
  if (ret != TX_SUCCESS)
  {
//      Error_Handler();
    printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
  }

  /* free the IP mem */
  ret = tx_byte_release(pIpMem);
  if (ret != TX_SUCCESS)
  {
//      Error_Handler();
    printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
  }

  /* free the IP packets pool */
  ret = nx_packet_pool_delete(&ip_pool);
  if (ret != NX_SUCCESS)
  {
//      Error_Handler();
    printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
  }

  /* free the IP packet mem */
  ret = tx_byte_release(pIpPktMem);
  if (ret != TX_SUCCESS)
  {
//      Error_Handler();
    printf("ERROR: file: %s, line: %d\n\r", __FILE__, __LINE__);
  }

//      BSP_WIFI_MX_GPIO_DeInit();  // FIXME to be implemented ?
#if 0
#include "mx_wifi.h"
  MX_WIFIObject_t *pMxWifiObj = wifi_obj_get();   // FIXME to be called from nx_ip_delete() tkt 154401
  MX_WIFI_DeInit(pMxWifiObj);
#endif
  return ret;
}

static int FTP_Srv_Init(void)
{
  INT ret = 0;

  ret = tx_byte_allocate(byte_pool, (VOID **) &pFtpSrvPackets, FTP_SERVER_POOL_SIZE, TX_NO_WAIT);

  /* Check server stack memory allocation. */
  if (ret != NX_SUCCESS)
  {
    printf("FTP Server stack memory allocation failed : 0x%02x\n", ret);
    return ret;
  }

  /* Create the server packet pool. */
  ret = nx_packet_pool_create(&ftp_server_pool, "FTP Server Packet Pool", 512, pFtpSrvPackets, FTP_SERVER_POOL_SIZE);

  /* Check server stack memory allocation. */
  if (ret != NX_SUCCESS)
  {
    tx_byte_release((VOID **) &pFtpSrvPackets);
    printf("FTP Server nx_packet_pool_create failed : 0x%02x\n", ret);
    return -ret;
  }

  /* Allocate the server stack. */
  ret = tx_byte_allocate(byte_pool, (VOID **) &pFtpSrvMem, FTP_SERVER_STACK_SIZE, TX_NO_WAIT);

  /* Check server stack memory allocation. */
  if (ret != NX_SUCCESS)
  {
    nx_packet_pool_delete(&ftp_server_pool);
    tx_byte_release(pFtpSrvPackets);
    printf("FTP Server stack memory allocation failed : 0x%02x\n", ret);
    return ret;
  }

  /* Create/start an FTP srv using IP NetXPool pkt */
  ret = nx_ftp_server_create(&ftp_server, "FTP Server Instance", &ip_instance, pSdioDisk, pFtpSrvMem, FTP_SERVER_STACK_SIZE, &ftp_server_pool, server_login, server_logout);
  if (ret != NX_SUCCESS)
  {
    nx_packet_pool_delete(&ftp_server_pool);
    tx_byte_release(pFtpSrvMem);
    tx_byte_release(pFtpSrvPackets);
    printf("FTP Server creation failed: 0x%02x\n", ret);
    return ret;
  }
  printf("FTP_Srv_Init complete, user: %s\n\r", ftp_username);

  return ret;
}

static int FTP_Srv_Start(void)
{
  int ret = nx_ftp_server_start(&ftp_server);

  if (ret != NX_SUCCESS)
  {
    printf("FTP Server start failed: 0x%02x\n", ret);
  }
  printf("FTP Server started\n");
  return ret;
}

static int FTP_Srv_Stop(void)
{
  int ret = nx_ftp_server_stop(&ftp_server);
  if (ret != NX_SUCCESS)
  {
    printf("FTP Server stop failed: 0x%02x\n", ret);
  }
  else
  {
    printf("FTP Server stopped\n");
  }
  return ret;
}

static int FTP_Srv_DeInit(void)
{
  INT ret = 0;

  printf("FTP_Srv_DeInit\n\r");
  /* delete FTP srv  */
  ret = nx_ftp_server_delete(&ftp_server);
  if (ret != NX_SUCCESS)
  {
    printf("FTP Server delete failed: 0x%02x\n", ret);
  }

  /* Delete the server packet pool. */
  ret = nx_packet_pool_delete(&ftp_server_pool);

  /* Check server pkt pool memory delete. */
  if (ret != NX_SUCCESS)
  {
    printf("FTP Server nx_packet_pool_delete failed : 0x%02x\n", ret);
  }

  /* Deallocate the server stack. */
  ret = tx_byte_release(pFtpSrvMem);

  /* Check server stack memory release. */
  if (ret != NX_SUCCESS)
  {
    printf("FTP Server stack memory release failed : 0x%02x\n", ret);
  }

  ret = tx_byte_release(pFtpSrvPackets);

  /* Check server stack memory release. */
  if (ret != NX_SUCCESS)
  {
    printf("FTP Server stack memory release failed : 0x%02x\n", ret);
  }
  return ret;
}


void netx_app_set_media(FX_MEDIA *sdio_disk)
{
  assert_param(sdio_disk != NULL);
  pSdioDisk = sdio_disk;
}

uint32_t netx_app_wait_command_execution(void)
{
  uint32_t ret = TX_SUCCESS;
  /* wait until an the command is executed */
  ret = tx_semaphore_get(&wait_command_sem, NX_IP_PERIODIC_RATE * 20);
  if (ret != TX_SUCCESS)
  {
    ret = TX_WAIT_ERROR;
  }
  return ret;
}

static void netx_app_notify_command_execution(void)
{
  tx_semaphore_put(&wait_command_sem);
}

IWifi_Config_t *AppNetXDuo_GetIWifi_ConfigIF(void)
{
  return &wifi_config;
}

uint8_t wifi_config_wifi_connect_func(IWifi_Config_t *_this, const char *password)
{
  NetxAppQueueMsgType TxMsg;
  if (password && (strlen(password) < PASSW_MAX_LENGTH))
  {
    strcpy(wifi_password, password);

    TxMsg.MsgCode = WIFI_CONNECT_SYNC;
    tx_queue_send(&netx_app_queue, &TxMsg, TX_WAIT_FOREVER);

    TxMsg.MsgCode = FTP_SERVER_INIT;
    tx_queue_send(&netx_app_queue, &TxMsg, TX_WAIT_FOREVER);
  }
  return 0;
}

uint8_t wifi_config_wifi_disconnect_func(IWifi_Config_t *_this)
{
  NetxAppQueueMsgType TxMsg;
  TxMsg.MsgCode = FTP_SERVER_DEINIT;
  tx_queue_send(&netx_app_queue, &TxMsg, TX_WAIT_FOREVER);
  TxMsg.MsgCode = WIFI_DISCONNECT_SYNC;
  tx_queue_send(&netx_app_queue, &TxMsg, TX_WAIT_FOREVER);

  return 0;
}

uint8_t wifi_config_set_ftp_credentials_func(IWifi_Config_t *_this, const char *password)
{
  if (password && (strlen(password) < PASSW_MAX_LENGTH))
  {
    strcpy(ftp_password, password);
  }
  return 0;
}


uint8_t netx_app_start_server(void)
{
  NetxAppQueueMsgType msg;
  msg.MsgCode = FTP_SERVER_START;
  tx_queue_send(&netx_app_queue, &msg, TX_WAIT_FOREVER);

  return 0;
}

uint8_t netx_app_stop_server(void)
{
  NetxAppQueueMsgType msg;
  msg.MsgCode = FTP_SERVER_STOP;
  tx_queue_send(&netx_app_queue, &msg, TX_WAIT_FOREVER);

  return 0;
}

/* WiFi HW configurations */

static sys_error_code_t WIFI_MX_HW_Init(void)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* Configure GPIOs for WiFi module */
  MX_GPIO_PE7_Init();  /* Notify */
  MX_GPIO_PE12_Init(); /* Enable */
  MX_GPIO_PH7_Init();  /* CS */

  /* Configure LPTIM1 as FLOW interrupt (because EXTI15 is already in use) */
  MX_LPTIM1_Init();
  HAL_LPTIM_RegisterCallback(&hlptim1, HAL_LPTIM_IC_CAPTURE_CB_ID, WIFI_LPTIM_IC_CaptureCallback);
  if (HAL_LPTIM_IC_Start_IT(&hlptim1, LPTIM_CHANNEL_1) != HAL_OK)
  {
    return SYS_BASE_ERROR_CODE;
  }

  /* Configure SPI1 */
  MX_SPI1_Init();
  HAL_SPI_RegisterCallback(&hspi1, HAL_SPI_TX_COMPLETE_CB_ID, WIFI_SPI_TxCpltCallback);
  HAL_SPI_RegisterCallback(&hspi1, HAL_SPI_RX_COMPLETE_CB_ID, WIFI_SPI_RxCpltCallback);
  HAL_SPI_RegisterCallback(&hspi1, HAL_SPI_TX_RX_COMPLETE_CB_ID, WIFI_SPI_TxRxCpltCallback);

  return res;
}

/**
  * @brief Tx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
void WIFI_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
#else
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
#endif
{
  if (hspi->Instance == SPI1)
  {
    HAL_SPI_TransferCallback(hspi);
  }
}

/**
  * @brief Rx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
void WIFI_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
#else
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
#endif
{
  if (hspi->Instance == SPI1)
  {
    HAL_SPI_TransferCallback(hspi);
  }
}

/**
  * @brief Tx and Rx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
void WIFI_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
#else
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
#endif
{
  if (hspi->Instance == SPI1)
  {
    HAL_SPI_TransferCallback(hspi);
  }
}


/**
  * @brief  Input Capture callback in non-blocking mode
  * @param  hlptim LPTIM handle
  * @retval None
  */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
void WIFI_LPTIM_IC_CaptureCallback(LPTIM_HandleTypeDef *hlptim)
#else
void HAL_LPTIM_IC_CaptureCallback(LPTIM_HandleTypeDef *hlptim)
#endif
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  mxchip_WIFI_ISR(GPIO_PIN_15);
  nx_driver_emw3080_interrupt();
}


void WIFI_NOTIFY_EXTI_Callback(uint16_t nPin)
{
  mxchip_WIFI_ISR(nPin);
  nx_driver_emw3080_interrupt();
}

/* USER CODE END 1 */
