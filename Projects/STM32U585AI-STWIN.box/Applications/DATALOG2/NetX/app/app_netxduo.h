/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.h
  * @author  MCD Application Team
  * @brief   NetXDuo applicative header file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_NETXDUO_H__
#define __APP_NETXDUO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nx_api.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <time.h>
#include "main.h"
#include "nx_ip.h"
#include "nxd_ftp_server.h"
#include "nxd_dhcp_client.h"
#include "nxd_sntp_client.h"
#include "nxd_dns.h"
#include "nx_driver_emw3080.h"
#include "stm32u5xx_hal_rtc.h"
#include "fx_api.h"
#include "PnPLDef.h"
#include "PnPLCompManager.h"
#include "BLE_Manager_Conf.h"
#include "IWifi_Config.h"
#include "IWifi_Config_vtbl.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define USE_STATIC_ALLOCATION        (1)

#define APP_NETX_QUEUE_MSG_NUM       (3)

#define PASSIVE_MODE                    // set FTP passive mode connection
#define SNTP_SERVER_NAME             "time1.google.com"

#define SNTP_UPDATE_EVENT            (1)
#define SNTP_KOD_EVENT               (2)

/* Define how often the demo checks for SNTP updates. */
#define PERIODIC_CHECK_INTERVAL      (60 * NX_IP_PERIODIC_RATE)

/* Define how often we check on SNTP server status. */
#define CHECK_SNTP_UPDATES_TIMEOUT   (180 * NX_IP_PERIODIC_RATE)

#define PAYLOAD_SIZE                 (1544)
#define NX_PACKET_POOL_SIZE          (( PAYLOAD_SIZE + sizeof(NX_PACKET)) * 10)
#define DEFAULT_MEMORY_SIZE          (1024)
#define ARP_MEMORY_SIZE              (DEFAULT_MEMORY_SIZE)
#ifdef SNTP_ENABLED
#define SNTP_CLIENT_THREAD_MEMORY    (3 * DEFAULT_MEMORY_SIZE)
#else
#define SNTP_CLIENT_THREAD_MEMORY    (0)
#endif
#define NETX_THREAD_MEMORY           (3 * DEFAULT_MEMORY_SIZE)

#define FTP_SERVER_POOL_SIZE         (12 * DEFAULT_MEMORY_SIZE)
#define FTP_SERVER_STACK_SIZE        (4 * DEFAULT_MEMORY_SIZE)
#define FTP_SERVER_MEMORY            (FTP_SERVER_STACK_SIZE + FTP_SERVER_POOL_SIZE)

#define IP_MEMORY_SIZE               (3 * DEFAULT_MEMORY_SIZE)
#define NX_APP_MEM_POOL_SIZE         (NX_PACKET_POOL_SIZE\
                                      + ARP_MEMORY_SIZE + IP_MEMORY_SIZE + NETX_THREAD_MEMORY + SNTP_CLIENT_THREAD_MEMORY + FTP_SERVER_MEMORY + 5*DEFAULT_MEMORY_SIZE )

/*+ 10*DEFAULT_MEMORY_SIZE*/

#define DEFAULT_NETX_PRIORITY        (10)
#define DEFAULT_PRIORITY             (10)

#define NULL_ADDRESS                 (0)
#define USER_DNS_ADDRESS             IP_ADDRESS(1, 1, 1, 1)

#define DEFAULT_TIMEOUT              (10 * NX_IP_PERIODIC_RATE)

/* EPOCH_TIME_DIFF is equivalent to 70 years in sec
   calculated with www.epochconverter.com/date-difference
   This constant is used to delete difference between :
   Epoch converter (referenced to 1970) and SNTP (referenced to 1900) */
#define EPOCH_TIME_DIFF              (2208988800)
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define PRINT_IP_ADDRESS(addr)    do { \
                                       printf("STM32 %s: %lu.%lu.%lu.%lu \n", #addr, \
                                       (addr >> 24) & 0xff,                          \
                                       (addr >> 16) & 0xff,                          \
                                       (addr >> 8) & 0xff,                           \
                                       (addr & 0xff));                               \
                                     } while(0)

#define PRINT_SNTP_SERVER(addr)  do { \
                                       printf("Client connected to SNTP server: [%lu.%lu.%lu.%lu] \n", \
                                       (addr >> 24) & 0xff,                      \
                                       (addr >> 16) & 0xff,                    \
                                       (addr >> 8) & 0xff,                   \
                                       (addr & 0xff));                     \
                                    } while(0)

#define PRINT_CNX_SUCC()          do { \
                                        printf("SNTP client connected to NTP server : < %s > \n", SNTP_SERVER_NAME);\
                                     } while(0)
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
extern UINT MX_NetXDuo_Init(VOID *memory_ptr);

IWifi_Config_t *AppNetXDuo_GetIWifi_ConfigIF(void);

void netx_app_set_media(FX_MEDIA *sdio_disk);
uint32_t netx_app_wait_command_execution(void);
uint8_t netx_app_start_server(void);
uint8_t netx_app_stop_server(void);

typedef void (*NetX_App_Connect_Callback)(bool);

void netx_app_set_connect_callback(NetX_App_Connect_Callback callback);

/* USER CODE BEGIN EFP */

#define   WIFI_CONNECT         (UINT)(1)
#define   WIFI_DISCONNECT      (UINT)(2)
#define   WIFI_CONNECT_SYNC    (UINT)(3)
#define   WIFI_DISCONNECT_SYNC (UINT)(4)
#define   FTP_SERVER_INIT      (UINT)(5)
#define   FTP_SERVER_DEINIT    (UINT)(6)
#define   FTP_SERVER_START     (UINT)(7)
#define   FTP_SERVER_STOP      (UINT)(8)

typedef struct
{
  UINT MsgCode;    // minimum size of msg for threadx queue must be 4 byte
} NetxAppQueueMsgType;

#define   SSID_MAX_LENGTH   (32)
#define   PASSW_MAX_LENGTH    (64)
#define   IP_MAX_LENGTH       (40)

extern char         wifi_ssid[];
extern char         wifi_password[];
extern char         ftp_username[];
extern char         ftp_password[];
extern char         ip_str[IP_MAX_LENGTH];

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* __APP_NETXDUO_H__ */
