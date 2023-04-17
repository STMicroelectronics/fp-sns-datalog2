/**
  ******************************************************************************
  * @file    BLE_Manager.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.8.0
  * @date    02-December-2022
  * @brief   Add bluetooth services using vendor specific profiles.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ---------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>

#include "BLE_Manager.h"
#include "BLE_ManagerCommon.h"
#include "BLE_ManagerControl.h"

/* Private define ---------------------------------------------------------------*/

/* Max Number of Bonded Devices */
#define BLE_MANAGER_MAX_BONDED_DEVICES 3

/* Hardware & Software Characteristics Service */
#define COPY_FEATURES_SERVICE_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,0x00,0x01,0x11,0xe1,0x9a,0xb4,0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_EXT_CONFIG_CHAR_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x14,0x00,0x02,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Console Service */
#define COPY_CONSOLE_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,0x00,0x0E,0x11,0xe1,0x9a,0xb4,0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_TERM_CHAR_UUID(uuid_struct)        COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x01,0x00,0x0E,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_STDERR_CHAR_UUID(uuid_struct)      COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x02,0x00,0x0E,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Configuration Service */
#define COPY_CONFIG_SERVICE_UUID(uuid_struct)   COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,0x00,0x0F,0x11,0xe1,0x9a,0xb4,0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_CONFIG_CHAR_UUID(uuid_struct)      COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x02,0x00,0x0F,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private Types ----------------------------------------------------------------*/
typedef enum 
{
  BLE_COMM_TP_START_PACKET = 0x00,
  BLE_COMM_TP_START_END_PACKET = 0x20,
  BLE_COMM_TP_MIDDLE_PACKET = 0x40, 
  BLE_COMM_TP_END_PACKET = 0x80
} BLE_COMM_TP_Packet_Typedef;

typedef enum 
{
  BLE_COMM_TP_WAIT_START = 0, 
  BLE_COMM_TP_WAIT_END = 1
} BLE_COMM_TP_Status_Typedef;

//Typedef for Standard Command types
typedef enum 
{
  //Command not recognized
  EXT_CONFIG_COM_NOT_VALID     = 0,
  
  //Commands Without Argument
  EXT_CONFIG_COM_READ_COMMAND,
  EXT_CONFIG_COM_READ_CUSTOM_COMMAND,
  EXT_CONFIG_COM_READ_CERT,
  EXT_CONFIG_COM_READ_UID,
  EXT_CONFIG_COM_READ_VER_FW,
  EXT_CONFIG_COM_READ_INFO,
  EXT_CONFIG_COM_READ_HELP,
  EXT_CONFIG_COM_READ_POWER,
  EXT_CONFIG_COM_SET_DFU,
  EXT_CONFIG_COM_SET_OFF,
  EXT_CONFIG_COM_CLEAR_DB,
  EXT_CONFIG_COM_READ_SENSOR_CONFIG,
  EXT_CONFIG_COM_READ_BANKS_FW_ID,
  EXT_CONFIG_COM_BANKS_SWAP,
  
  //Commands With Argument
  EXT_CONFIG_COM_SET_WIFI,
  EXT_CONFIG_COM_SET_DATE,
  EXT_CONFIG_COM_SET_TIME,
  EXT_CONFIG_COM_SET_NAME,
  EXT_CONFIG_COM_CHANGE_PIN,
  EXT_CONFIG_COM_SET_CERT,
  EXT_CONFIG_COM_SET_SENSOR_CONFIG,
  
  //Total Number of Commands
  EXT_CONFIG_COMMAND_NUMBER
} BLE_ExtConfigCommandType;

//Structure used for Specifying the Standard Commands
typedef struct {
  BLE_ExtConfigCommandType CommandType;
  char *CommandString;
} BLE_ExtConfigCommand_t;

/* Exported variables -----------------------------------------------------------*/
/* Identifies if the configuration service are enabled or not */
BLE_ServEnab_t BLE_Conf_Service;

/* Identifies if the standard terminal service are enabled or not */
BLE_ServEnab_t BLE_StdTerm_Service;

/* Identifies if the standard error message service are enabled or not */
BLE_ServEnab_t BLE_StdErr_Service;

/* Identifies if the Extended Configuration characteristic value service are enabled or not */
BLE_ServEnab_t BLE_ExtConf_Service;

/* Contains the message of the standard terminal or of the standard error */
uint8_t BufferToWrite[256];

/* Total number of characters written for the message in the standard terminal or for the standard error */
uint8_t BytesToWrite;

uint8_t set_connectable;

/* BLE Stack */
BLE_StackTypeDef BLE_StackValue;

/* Manufacter Advertise data */
uint8_t manuf_data[BLE_MANAGER_ADVERTISE_DATA_LENGHT];

/**************** Bluetooth Comunication *************************/
CustomPairingCompleted_t                CustomPairingCompleted;
CustomMTUExchangeRespEvent_t CustomMTUExchangeRespEvent;
CustomSetConnectable_t                  CustomSetConnectable;
CustomConnectionCompleted_t             CustomConnectionCompleted;
CustomDisconnectionCompleted_t          CustomDisconnectionCompleted;
CustomAciGattTxPoolAvailableEvent_t     CustomAciGattTxPoolAvailableEvent;
CustomHardwareErrorEventHandler_t       CustomHardwareErrorEventHandler;

/**************** Debug Console *************************/
CustomDebugConsoleParsing_t CustomDebugConsoleParsingCallback;

/******************* Config Char *************************/
CustomAttrModConfig_t CustomAttrModConfigCallback;
CustomWriteRequestConfig_t CustomWriteRequestConfigCallback;

#ifndef BLE_MANAGER_NO_PARSON
/**************** Extended Configuration *************************/
//For Reboot on DFU Command
CustomExtConfigRebootOnDFUModeCommand_t CustomExtConfigRebootOnDFUModeCommandCallback;
//For Power off Command
CustomExtConfigPowerOffCommand_t CustomExtConfigPowerOffCommandCallback;
//For Set board Name Command
CustomExtConfigSetNameCommand_t CustomExtConfigSetNameCommandCallback;
//For Set Time Command
CustomExtConfigSetTimeCommand_t CustomExtConfigSetTimeCommandCallback;
//For Set Date Command
CustomExtConfigSetDateCommand_t CustomExtConfigSetDateCommandCallback;
//For Set Wi-Fi Command
CustomExtConfigSetWiFiCommand_t CustomExtConfigSetWiFiCommandCallback;
//For Change Secure PIN Command
CustomExtConfigChangePinCommand_t CustomExtConfigChangePinCommandCallback;
//For Clear Secure Data Base Command
CustomExtConfigClearDBCommand_t CustomExtConfigClearDBCommandCallback;
//For Reading the Flash Banks Fw Ids
CustomExtConfigReadBanksFwIdCommand_t CustomExtConfigReadBanksFwIdCommandCallback;
//For Swapping the Flash Banks
CustomExtConfigBanksSwapCommand_t CustomExtConfigBanksSwapCommandCallback;
//For UID Command
CustomExtConfigUidCommand_t CustomExtConfigUidCommandCallback;
//For Info Command
CustomExtConfigInfoCommand_t CustomExtConfigInfoCommandCallback;
//For Help Command
CustomExtConfigHelpCommand_t CustomExtConfigHelpCommandCallback;
//For PowerStatus Command
CustomExtConfigPowerStatusCommand_t CustomExtConfigPowerStatusCommandCallback;
//For VersionFw Command
CustomExtConfigPowerStatusCommand_t CustomExtConfigVersionFwCommandCallback;
//For Custom Command
CustomExtConfigReadCustomCommands_t CustomExtConfigReadCustomCommandsCallback;
CustomExtConfigCustomCommand_t CustomExtConfigCustomCommandCallback;
//For Read Certificate Command
CustomExtConfigReadCertCommand_t CustomExtConfigReadCertCommandCallback;
//For Set Certificate Commnad
CustomExtConfigSetCertCommand_t CustomExtConfigSetCertCommandCallback;
// Sensor Configuration
CustomExtConfigReadSensorsConfigCommands_t CustomExtConfigReadSensorsConfigCommandsCallback;
CustomExtConfigSetSensorsConfigCommands_t CustomExtConfigSetSensorsConfigCommandsCallback;

/* Private variables ------------------------------------------------------------*/

//Table of Standard Commands
static BLE_ExtConfigCommand_t StandardExtConfigCommands[EXT_CONFIG_COMMAND_NUMBER] = {
  {EXT_CONFIG_COM_NOT_VALID,"NULL"},
  {EXT_CONFIG_COM_READ_COMMAND,"ReadCommand"},
  {EXT_CONFIG_COM_READ_CUSTOM_COMMAND,BLE_MANAGER_READ_CUSTOM_COMMAND},
  {EXT_CONFIG_COM_READ_CERT,"ReadCert"},
  {EXT_CONFIG_COM_READ_UID,"UID"},
  {EXT_CONFIG_COM_READ_VER_FW,"VersionFw"},
  {EXT_CONFIG_COM_READ_INFO,"Info"},
  {EXT_CONFIG_COM_READ_HELP,"Help"},
  {EXT_CONFIG_COM_READ_POWER,"PowerStatus"},
  {EXT_CONFIG_COM_SET_DFU,"DFU"},
  {EXT_CONFIG_COM_SET_OFF,"Off"},
  {EXT_CONFIG_COM_CLEAR_DB,"ClearDB"},
  {EXT_CONFIG_COM_READ_SENSOR_CONFIG,"ReadSensorsConfig"},
  {EXT_CONFIG_COM_READ_BANKS_FW_ID,"ReadBanksFwId"},
  {EXT_CONFIG_COM_BANKS_SWAP,"BanksSwap"},
  {EXT_CONFIG_COM_SET_WIFI,"SetWiFi"},
  {EXT_CONFIG_COM_SET_DATE,"SetDate"},
  {EXT_CONFIG_COM_SET_TIME,"SetTime"},
  {EXT_CONFIG_COM_SET_NAME,"SetName"},
  {EXT_CONFIG_COM_CHANGE_PIN,"ChangePIN"},
  {EXT_CONFIG_COM_SET_CERT,"SetCert"},
  {EXT_CONFIG_COM_SET_SENSOR_CONFIG,"SetSensorsConfig"}
};

//Table for Custom Commands:
BLE_ExtCustomCommand_t *ExtConfigCustomCommands=NULL;
BLE_ExtCustomCommand_t *ExtConfigLastCustomCommand=NULL;
#endif /* BLE_MANAGER_NO_PARSON */

static uint8_t LastStderrBuffer[DEFAULT_MAX_STDERR_CHAR_LEN];
static uint8_t LastStderrLen;
static uint8_t LastTermBuffer[DEFAULT_MAX_STDOUT_CHAR_LEN];
static uint8_t LastTermLen;
static uint16_t connection_handle;

uint8_t MaxBleCharStdOutLen;
uint8_t MaxBleCharStdErrLen;

static BleCharTypeDef BleCharConfig;
static BleCharTypeDef BleCharStdOut;
static BleCharTypeDef BleCharStdErr;
#ifndef BLE_MANAGER_NO_PARSON
static BleCharTypeDef BleCharExtConfig;

static uint8_t *hs_command_buffer;
#endif /* BLE_MANAGER_NO_PARSON */

static BleCharTypeDef *BleCharsArray[BLE_MANAGER_MAX_ALLOCABLE_CHARS];
static uint8_t UsedBleChars;
static uint8_t UsedStandardBleChars;

#if (BLUE_CORE == BLUENRG_MS)
/* ***************** BlueNRG-MS Stack functions prototype ***********************/
void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy);

void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason);

void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[]);

void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
                                    uint16_t Attribute_Handle,
                                    uint16_t Offset);

void aci_att_exchange_mtu_resp_event(uint16_t Connection_Handle,
                                     uint16_t Server_RX_MTU);

void aci_gap_pairing_complete_event(uint16_t Connection_Handle,
                                    uint8_t Status,
                                    uint8_t Reason);

void aci_gap_pass_key_req_event(uint16_t Connection_Handle);

#endif /* (BLUE_CORE == BLUENRG_MS) */

/* Private functions prototype --------------------------------------------------*/
#if (BLUE_CORE == BLUENRG_MS)
static void HCI_Event_CB(void *pckt);
#else /* (BLUE_CORE == BLUENRG_MS) */
#if (BLUE_CORE != BLUE_WB)
static void APP_UserEvtRx(void *pData);
#endif /* (BLUE_CORE != BLUE_WB) */
static void UpdateWhiteList(void);
#endif /* (BLUE_CORE == BLUENRG_MS) */

#if (BLUE_CORE != BLUE_WB)
static tBleStatus InitBleManager_BLE_Stack(void);
#endif /* (BLUE_CORE != BLUE_WB) */

static tBleStatus InitBleManagerServices(void);

#ifndef BLE_MANAGER_NO_PARSON
static tBleStatus BLE_UpdateExtConf(uint8_t *data,uint8_t length);
#endif /* BLE_MANAGER_NO_PARSON */

static tBleStatus BLE_Manager_AddFeaturesService(void);
static tBleStatus BLE_Manager_AddConsoleService(void);
static tBleStatus BLE_Manager_AddConfigService(void);

static tBleStatus UpdateTermStdOut(uint8_t *data,uint8_t length);
static tBleStatus UpdateTermStdErr(uint8_t *data,uint8_t length);

#if (BLUE_CORE != BLUENRG_LP)
  static void Read_Request_StdErr(void *VoidCharPointer,uint16_t handle);
  static void Read_Request_Term(void *VoidCharPointer,uint16_t handle);
#else /* (BLUE_CORE != BLUENRG_LP) */
  static void Read_Request_StdErr(void *VoidCharPointer,
                                 uint16_t handle,
                                 uint16_t Connection_Handle,
                                 uint8_t Operation_Type,
                                 uint16_t Attr_Val_Offset,
                                 uint8_t Data_Length,
                                 uint8_t Data[]);
  static void Read_Request_Term(void *VoidCharPointer,
                                 uint16_t handle,
                                 uint16_t Connection_Handle,
                                 uint8_t Operation_Type,
                                 uint16_t Attr_Val_Offset,
                                 uint8_t Data_Length,
                                 uint8_t Data[]);
#endif /* (BLUE_CORE != BLUENRG_LP) */

#ifndef BLE_MANAGER_NO_PARSON
static BLE_ExtConfigCommandType BLE_ExtConfig_ExtractCommandType(uint8_t *hs_command_buffer);

static void AttrMod_Request_ExtConfig(void *VoidCharPointer,uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data);
static void Write_Request_ExtConfig(void *VoidCharPointer,uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data);
static void ClearSingleCommand(BLE_ExtCustomCommand_t *Command);

static void create_JSON_SensorDescriptor(COM_SensorDescriptor_t *sensor_descriptor, JSON_Value *tempJSON);
static void create_JSON_SensorStatus(COM_Sensor_t *sensor, JSON_Value *tempJSON);
static void create_JSON_SubSensorDescriptor(COM_SubSensorDescriptor_t *sub_sensor_descriptor, JSON_Value *tempJSON);
static void create_JSON_SubSensorStatus(COM_SubSensorStatus_t *sub_sensor_status, JSON_Value *tempJSON);
#endif /* BLE_MANAGER_NO_PARSON */

static void ResetBleManagerCallbackFunctionPointer(void);

/* Private functions ------------------------------------------------------------*/
/**
* @brief  Add the Config service using a vendor specific profile
* @param  None
* @retval tBleStatus Status
*/
static tBleStatus BLE_Manager_AddConfigService(void)
{
  tBleStatus ret;
  
#if (BLUE_CORE != BLUENRG_MS)
  Service_UUID_t service_uuid;
  Char_UUID_t char_uuid;
#endif /* (BLUE_CORE != BLUENRG_MS) */
  
  uint8_t uuid[16];
  
  COPY_CONFIG_SERVICE_UUID(uuid);
  
#if (BLUE_CORE == BLUENRG_MS)
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, 1+3,&(BleCharConfig.Service_Handle));
#else /* (BLUE_CORE == BLUENRG_MS) */
  BLE_MemCpy(&service_uuid.Service_UUID_128, uuid, 16);
  #if (BLUE_CORE != BLUENRG_LP)
    ret = aci_gatt_add_service(UUID_TYPE_128,  &service_uuid, PRIMARY_SERVICE, 1+3,&(BleCharConfig.Service_Handle));
  #else /* (BLUE_CORE != BLUENRG_LP) */
    ret = aci_gatt_srv_add_service_nwk(UUID_TYPE_128,  &service_uuid, PRIMARY_SERVICE, 1+3,&(BleCharConfig.Service_Handle));
  #endif /* (BLUE_CORE != BLUENRG_LP) */
#endif /* (BLUE_CORE == BLUENRG_MS) */
  
  if (ret != (tBleStatus)BLE_STATUS_SUCCESS) {
    goto EndLabel;
  }
  
#if (BLUE_CORE != BLUENRG_MS)
  BLE_MemCpy(&char_uuid.Char_UUID_128, BleCharConfig.uuid, 16);
#endif /* (BLUE_CORE != BLUENRG_MS) */

#if (BLUE_CORE != BLUENRG_LP)
  ret =  aci_gatt_add_char(BleCharConfig.Service_Handle,
#else /* (BLUE_CORE != BLUENRG_LP) */
  ret =  aci_gatt_srv_add_char_nwk(BleCharConfig.Service_Handle,
#endif /* (BLUE_CORE != BLUENRG_LP) */
                           BleCharConfig.Char_UUID_Type,
#if (BLUE_CORE == BLUENRG_MS)
                           BleCharConfig.uuid,
#else /* (BLUE_CORE == BLUENRG_MS) */						   
                           &char_uuid,
#endif /* (BLUE_CORE == BLUENRG_MS) */
                           BleCharConfig.Char_Value_Length,
                           BleCharConfig.Char_Properties,
                           BleCharConfig.Security_Permissions,
                           BleCharConfig.GATT_Evt_Mask,
                           BleCharConfig.Enc_Key_Size,
                           BleCharConfig.Is_Variable,
                           &(BleCharConfig.attr_handle));
  
EndLabel:
  return ret;
}


/**
* @brief  Add the Console service using a vendor specific profile
* @param  None
* @retval tBleStatus Status
*/
static tBleStatus BLE_Manager_AddConsoleService(void)
{
  tBleStatus ret;
  
#if (BLUE_CORE != BLUENRG_MS)
  Service_UUID_t service_uuid;
  Char_UUID_t char_uuid;
#endif /* (BLUE_CORE != BLUENRG_MS) */
  
  uint8_t uuid[16];
  
  COPY_CONSOLE_SERVICE_UUID(uuid);
  
#if (BLUE_CORE == BLUENRG_MS)
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, 1+(3*2),&(BleCharStdOut.Service_Handle));
#else /* (BLUE_CORE == BLUENRG_MS) */  
  BLE_MemCpy(&service_uuid.Service_UUID_128, uuid, 16);
  #if (BLUE_CORE != BLUENRG_LP)
    ret = aci_gatt_add_service(UUID_TYPE_128,  &service_uuid, PRIMARY_SERVICE, 1+(3*2),&(BleCharStdOut.Service_Handle));
  #else /* (BLUE_CORE != BLUENRG_LP) */
    ret = aci_gatt_srv_add_service_nwk(UUID_TYPE_128,  &service_uuid, PRIMARY_SERVICE, 1+(3*2),&(BleCharStdOut.Service_Handle));
  #endif /* (BLUE_CORE != BLUENRG_LP) */
#endif /* (BLUE_CORE == BLUENRG_MS) */
  
  if (ret != (tBleStatus)BLE_STATUS_SUCCESS) {
    goto EndLabel;
  }  
  
#if (BLUE_CORE != BLUENRG_MS)
  BLE_MemCpy(&char_uuid.Char_UUID_128, BleCharStdOut.uuid, 16);
#endif /* (BLUE_CORE != BLUENRG_MS) */

#if (BLUE_CORE != BLUENRG_LP)
  ret =  aci_gatt_add_char(BleCharStdOut.Service_Handle,
#else /* (BLUE_CORE != BLUENRG_LP) */
  ret =  aci_gatt_srv_add_char_nwk(BleCharStdOut.Service_Handle,
#endif /* (BLUE_CORE != BLUENRG_LP) */
                           BleCharStdOut.Char_UUID_Type, 
#if (BLUE_CORE == BLUENRG_MS)
                           BleCharStdOut.uuid,
#else /* (BLUE_CORE == BLUENRG_MS) */						   
                           &char_uuid,
#endif /* (BLUE_CORE == BLUENRG_MS) */
                           BleCharStdOut.Char_Value_Length,
                           BleCharStdOut.Char_Properties,
                           BleCharStdOut.Security_Permissions,
                           BleCharStdOut.GATT_Evt_Mask,
                           BleCharStdOut.Enc_Key_Size,
                           BleCharStdOut.Is_Variable,
                           &(BleCharStdOut.attr_handle));
  
  if (ret != (tBleStatus)BLE_STATUS_SUCCESS) {
    goto EndLabel;
  }
  
  BleCharStdErr.Service_Handle = BleCharStdOut.Service_Handle;
  
#if (BLUE_CORE != BLUENRG_MS)
  BLE_MemCpy(&char_uuid.Char_UUID_128, BleCharStdErr.uuid, 16);
#endif /* (BLUE_CORE != BLUENRG_MS) */

 
#if (BLUE_CORE != BLUENRG_LP)
  ret =  aci_gatt_add_char(BleCharStdErr.Service_Handle,
#else /* (BLUE_CORE != BLUENRG_LP) */
  ret =  aci_gatt_srv_add_char_nwk(BleCharStdErr.Service_Handle,
#endif /* (BLUE_CORE != BLUENRG_LP) */
                           BleCharStdErr.Char_UUID_Type, 
#if (BLUE_CORE == BLUENRG_MS)
                           BleCharStdErr.uuid,
#else /* (BLUE_CORE == BLUENRG_MS) */						   
                           &char_uuid,
#endif /* (BLUE_CORE == BLUENRG_MS) */
                           BleCharStdErr.Char_Value_Length,
                           BleCharStdErr.Char_Properties,
                           BleCharStdErr.Security_Permissions,
                           BleCharStdErr.GATT_Evt_Mask,
                           BleCharStdErr.Enc_Key_Size,
                           BleCharStdErr.Is_Variable,
                           &(BleCharStdErr.attr_handle));
  
EndLabel:
  return ret;
}

/**
* @brief  Update Stdout characteristic value (when lenght is <=MaxBLECharLen)
* @param  uint8_t *data string to write
* @param  uint8_t lenght lengt of string to write
* @retval tBleStatus      Status
*/
static tBleStatus UpdateTermStdOut(uint8_t *data,uint8_t length)
{
  if (ACI_GATT_UPDATE_CHAR_VALUE(&BleCharStdOut, 0, length , data) != (tBleStatus)BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("Error: Updating Stdout Char\r\n");
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}

/**
* @brief  Update Stderr characteristic value (when lenght is <=MaxBLECharLen)
* @param  uint8_t *data string to write
* @param  uint8_t lenght lengt of string to write
* @retval tBleStatus      Status
*/
static tBleStatus UpdateTermStdErr(uint8_t *data,uint8_t length)
{
  if (ACI_GATT_UPDATE_CHAR_VALUE(&BleCharStdErr, 0, length , data) != (tBleStatus)BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("Error: Updating Stdout Char\r\n");
    return BLE_STATUS_ERROR;
  } 
  return BLE_STATUS_SUCCESS;
}

#if (BLUE_CORE != BLUENRG_LP)
/**
* @brief  Update Stderr characteristic value after a read request
* @param None
* @retval tBleStatus Status
*/
static tBleStatus Stderr_Update_AfterRead(BleCharTypeDef *BleCharPointer)
{
  tBleStatus ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(BleCharPointer, 0, LastStderrLen , LastStderrBuffer);
  return ret;
}

/**
* @brief  Update Terminal characteristic value after a read request
* @param None
* @retval tBleStatus Status
*/
static tBleStatus Term_Update_AfterRead(void *BleCharPointer)
{
  tBleStatus ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(BleCharPointer, 0, LastTermLen , LastTermBuffer);
  if (ret != (tBleStatus)BLE_STATUS_SUCCESS) {
    if(BLE_StdErr_Service==BLE_SERV_ENABLE){
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Stdout Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      BLE_MANAGER_PRINTF("Error: Updating Stdout Char\r\n");
    }
  }
  return ret;
}

 /**
* @brief  This event is given when a read request is received by the server from the client.
* @param  void *VoidCharPointer
* @param  uint16_t handle Handle of the attribute
* @retval None
*/
static void Read_Request_Term(void *VoidCharPointer,uint16_t handle)
{
  BleCharTypeDef *BleCharPointer = (BleCharTypeDef *) VoidCharPointer;
  
  /* Send again the last packet for StdOut */
  Term_Update_AfterRead(BleCharPointer);
#if (BLE_DEBUG_LEVEL>1)
  BLE_MANAGER_PRINTF("Read for Term\r\n");
#endif
}

/**
* @brief  This event is given when a read request is received by the server from the client.
* @param  void *VoidCharPointer
* @param  uint16_t handle Handle of the attribute
* @retval None
*/
static void Read_Request_StdErr(void *VoidCharPointer,uint16_t handle)
{
  BleCharTypeDef *BleCharPointer = (BleCharTypeDef *) VoidCharPointer;
  
  /* Send again the last packet for StdError */
  Stderr_Update_AfterRead(BleCharPointer);
#if (BLE_DEBUG_LEVEL>1)
  BLE_MANAGER_PRINTF("Read for StdErr\r\n");
#endif
}
#else /* (BLUE_CORE != BLUENRG_LP) */
/**
* @brief  This event is given when a read request is received by the server from the client.
* @param  void *VoidCharPointer
* @param  uint16_t handle Handle of the attribute
* @param  uint16_t Connection_Handle Connection handle
* @param  uint8_t Operation_Type see aci_gatt_srv_authorize_nwk_event
* @param  uint16_t Attr_Val_Offset ffset from which the value needs to be read or write
* @param  uint8_t Data_Length Length of Data field
* @param  uint8_t Data[] The data that the client has requested to write
* @retval None
*/
static void Read_Request_Term(void *VoidCharPointer,
                         uint16_t handle,
                         uint16_t Connection_Handle,
                         uint8_t Operation_Type,
                         uint16_t Attr_Val_Offset,
                         uint8_t Data_Length,
                         uint8_t Data[])
{
  tBleStatus ret;
  ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, LastTermLen,LastTermBuffer);
  if (ret != (tBleStatus)BLE_STATUS_SUCCESS) {
    if(BLE_StdErr_Service==BLE_SERV_ENABLE){
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Stdout Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      BLE_MANAGER_PRINTF("Error: Updating Stdout Char\r\n");
    }
  }

#if (BLE_DEBUG_LEVEL>1)
  BLE_MANAGER_PRINTF("Read for Term\r\n");
#endif
}
                           
/**
* @brief  This event is given when a read request is received by the server from the client.
* @param  void *VoidCharPointer
* @param  uint16_t handle Handle of the attribute
* @param  uint16_t Connection_Handle Connection handle
* @param  uint8_t Operation_Type see aci_gatt_srv_authorize_nwk_event
* @param  uint16_t Attr_Val_Offset ffset from which the value needs to be read or write
* @param  uint8_t Data_Length Length of Data field
* @param  uint8_t Data[] The data that the client has requested to write
* @retval None
*/
static void Read_Request_StdErr(void *VoidCharPointer,
                         uint16_t handle,
                         uint16_t Connection_Handle,
                         uint8_t Operation_Type,
                         uint16_t Attr_Val_Offset,
                         uint8_t Data_Length,
                         uint8_t Data[])
{
  tBleStatus ret;

  /* Send again the last packet for StdError */
  ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, LastStderrLen , LastStderrBuffer);
  if (ret != (tBleStatus)BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("Error: Updating Stderr Char\r\n");
  }
#if (BLE_DEBUG_LEVEL>1)
  BLE_MANAGER_PRINTF("Read for StdErr\r\n");
#endif
}
#endif /* (BLUE_CORE != BLUENRG_LP) */

/**
* @brief  This function is called when there is a change on the gatt attribute
*         With this function it's possible to understand if config is subscribed or not to the one service
* @param  void *VoidCharPointer
* @param  uint16_t attr_handle Handle of the attribute
* @param  uint16_t Offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode: 
*                          - Bits 0-14: offset of the reported value inside the attribute.
*                          - Bit 15: if the entire value of the attribute does not fit inside a single ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event,
*                            this bit is set to 1 to notify that other ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.                  
* @param  uint8_t data_length length of the data
* @param  uint8_t *att_data attribute data
* @retval None
*/
static void AttrMod_Request_Config(void *VoidCharPointer,uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == (uint8_t)01) {
    BLE_Conf_Service= BLE_SERV_ENABLE;
  } else if (att_data[0] == 0U){
    BLE_Conf_Service= BLE_SERV_NOT_ENABLE;
  }
  
  if(CustomAttrModConfigCallback!=NULL) {
    CustomAttrModConfigCallback(att_data,data_length);
  }
  
#if (BLE_DEBUG_LEVEL>1)
  if(BLE_StdTerm_Service==BLE_SERV_ENABLE) {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite,"--->Conf=%s\n", (BLE_Conf_Service == BLE_SERV_ENABLE) ? "ON" : "OFF");
    Term_Update(BufferToWrite,BytesToWrite);
  } else {
    BLE_MANAGER_PRINTF("--->Conf=%s\r\n", (BLE_Conf_Service == BLE_SERV_ENABLE) ? "ON" : "OFF");
  }
#endif
}

/**
* @brief  This function is called when there is a change on the gatt attribute as consequence of write request for the Config service
* @param  void *VoidCharPointer
* @param  uint16_t attr_handle Handle of the attribute
* @param  uint16_t Offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode: 
*                          - Bits 0-14: offset of the reported value inside the attribute.
*                          - Bit 15: if the entire value of the attribute does not fit inside a single ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event,
*                            this bit is set to 1 to notify that other ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.                  
* @param  uint8_t data_length length of the data
* @param  uint8_t *att_data attribute data
* @retval None
*/
static void Write_Request_Config(void *VoidCharPointer,uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data)
{ 
  /* Received one write command from Client on Configuration characteristc */
  if(CustomWriteRequestConfigCallback!=NULL) {
    CustomWriteRequestConfigCallback(att_data,data_length);
  }
}

/**
* @brief  This function is called when there is a change on the gatt attribute
*         With this function it's possible to understand if Std Err is subscribed or not to the one service
* @param  void *VoidCharPointer
* @param  uint16_t attr_handle Handle of the attribute
* @param  uint16_t Offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode: 
*                          - Bits 0-14: offset of the reported value inside the attribute.
*                          - Bit 15: if the entire value of the attribute does not fit inside a single ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event,
*                            this bit is set to 1 to notify that other ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.                  
* @param  uint8_t data_length length of the data
* @param  uint8_t *att_data attribute data
* @retval None
*/
static void AttrMod_Request_StdErr(void *VoidCharPointer,uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U) {
    BLE_StdErr_Service= BLE_SERV_ENABLE;
  } else if (att_data[0] == 0U){
    BLE_StdErr_Service= BLE_SERV_NOT_ENABLE;
  }
}

/**
* @brief  This function is called when there is a change on the gatt attribute
*         With this function it's possible to understand if Term is subscribed or not to the one service
* @param  void *VoidCharPointer
* @param  uint16_t attr_handle Handle of the attribute
* @param  uint16_t Offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode: 
*                          - Bits 0-14: offset of the reported value inside the attribute.
*                          - Bit 15: if the entire value of the attribute does not fit inside a single ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event,
*                            this bit is set to 1 to notify that other ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.                  
* @param  uint8_t data_length length of the data
* @param  uint8_t *att_data attribute data
* @retval None
*/
static void AttrMod_Request_Term(void *VoidCharPointer,uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U) {
    BLE_StdTerm_Service= BLE_SERV_ENABLE;
  } else if (att_data[0] == 0U){
    BLE_StdTerm_Service= BLE_SERV_NOT_ENABLE;
  }
}

#ifndef BLE_MANAGER_NO_PARSON
/**
* @brief  This function is called when there is a change on the gatt attribute
*         With this function it's possible to understand if Extended Configuration characteristic value is subscribed or not to the one service
* @param  void *VoidCharPointer
* @param  uint16_t attr_handle Handle of the attribute
* @param  uint16_t Offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode: 
*                          - Bits 0-14: offset of the reported value inside the attribute.
*                          - Bit 15: if the entire value of the attribute does not fit inside a single ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event,
*                            this bit is set to 1 to notify that other ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.                  
* @param  uint8_t data_length length of the data
* @param  uint8_t *att_data attribute data
* @retval None
*/
static void AttrMod_Request_ExtConfig(void *VoidCharPointer,uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U) {
    BLE_ExtConf_Service= BLE_SERV_ENABLE;
  } else if (att_data[0] == 0U){
    BLE_ExtConf_Service= BLE_SERV_NOT_ENABLE;
  }
}


static void create_JSON_SensorDescriptor(COM_SensorDescriptor_t *sensor_descriptor, JSON_Value *tempJSON)
{
  uint32_t ii = 0;
  
  JSON_Object *JSON_SensorDescriptor = json_value_get_object(tempJSON);
  JSON_Array *JSON_SensorArray1;
  JSON_Value *tempJSON1;
  
  json_object_dotset_value(JSON_SensorDescriptor, "subSensorDescriptor", json_value_init_array());
  JSON_SensorArray1 = json_object_dotget_array(JSON_SensorDescriptor, "subSensorDescriptor");
  for (ii = 0; ii < sensor_descriptor->nSubSensors; ii++)
  {
    tempJSON1 = json_value_init_object();
    create_JSON_SubSensorDescriptor(&sensor_descriptor->subSensorDescriptor[ii], tempJSON1);
    json_array_append_value(JSON_SensorArray1,tempJSON1);
  }
}

static void create_JSON_SensorStatus(COM_Sensor_t *sensor, JSON_Value *tempJSON)
{  
  uint32_t ii = 0;
  COM_SensorStatus_t sensor_status = sensor->sensorStatus;
  COM_SensorDescriptor_t pSensorDescriptor = sensor->sensorDescriptor;
  
  JSON_Object *JSON_SensorStatus = json_value_get_object(tempJSON);
  JSON_Array *JSON_SensorArray2;
  JSON_Value *tempJSON2;
  
  json_object_dotset_value(JSON_SensorStatus, "subSensorStatus", json_value_init_array());
  JSON_SensorArray2= json_object_dotget_array(JSON_SensorStatus, "subSensorStatus");
  for (ii = 0; ii < pSensorDescriptor.nSubSensors; ii++)
  {
    tempJSON2 = json_value_init_object();
    create_JSON_SubSensorStatus(&sensor_status.subSensorStatus[ii], tempJSON2);
    json_array_append_value(JSON_SensorArray2,tempJSON2);
  }
}

static void create_JSON_SubSensorStatus(COM_SubSensorStatus_t *sub_sensor_status, JSON_Value *tempJSON)
{
#define PRECISION6(n) floor(1000000.0*(n))/1000000.0   
  JSON_Object *JSON_SubSensorStatus = json_value_get_object(tempJSON);
  
  json_object_dotset_number(JSON_SubSensorStatus, "ODR", sub_sensor_status->ODR);
  json_object_dotset_number(JSON_SubSensorStatus, "ODRMeasured", sub_sensor_status->measuredODR);
  json_object_dotset_number(JSON_SubSensorStatus, "initialOffset", PRECISION6((double)sub_sensor_status->initialOffset));
  json_object_dotset_number(JSON_SubSensorStatus, "FS", sub_sensor_status->FS);
  json_object_dotset_number(JSON_SubSensorStatus, "sensitivity", PRECISION6((double)sub_sensor_status->sensitivity));
  json_object_dotset_boolean(JSON_SubSensorStatus, "isActive", (int)sub_sensor_status->isActive);
  json_object_dotset_number(JSON_SubSensorStatus, "samplesPerTs", (double)(sub_sensor_status->samplesPerTimestamp));
  json_object_dotset_number(JSON_SubSensorStatus, "usbDataPacketSize", (double)(sub_sensor_status->usbDataPacketSize));
  json_object_dotset_number(JSON_SubSensorStatus, "sdWriteBufferSize", (double)(sub_sensor_status->sdWriteBufferSize));
  json_object_dotset_number(JSON_SubSensorStatus, "wifiDataPacketSize", (double)(sub_sensor_status->wifiDataPacketSize));
  json_object_dotset_number(JSON_SubSensorStatus, "comChannelNumber", (double)(sub_sensor_status->comChannelNumber));
  json_object_dotset_boolean(JSON_SubSensorStatus, "ucfLoaded", (int)sub_sensor_status->ucfLoaded);
#undef PRECISION6
}

static void create_JSON_SubSensorDescriptor(COM_SubSensorDescriptor_t *sub_sensor_descriptor, JSON_Value *tempJSON)
{
  uint32_t ii = 0;
  
  JSON_Value *tempJSONarray = json_value_init_object();
  JSON_Array *JSON_SensorArray = json_value_get_array(tempJSONarray);
  JSON_Object *JSON_SubSensorDescriptor= json_value_get_object(tempJSON);

  json_object_dotset_number(JSON_SubSensorDescriptor, "id", (double)(sub_sensor_descriptor->id));

  switch (sub_sensor_descriptor->sensorType)
  {
  case COM_TYPE_ACC:
    json_object_dotset_string(JSON_SubSensorDescriptor, "sensorType", "ACC");
    break;
  case COM_TYPE_MAG:
    json_object_dotset_string(JSON_SubSensorDescriptor, "sensorType", "MAG");
    break;
  case COM_TYPE_GYRO:
    json_object_dotset_string(JSON_SubSensorDescriptor, "sensorType", "GYRO");
    break;
  case COM_TYPE_TEMP:
    json_object_dotset_string(JSON_SubSensorDescriptor, "sensorType", "TEMP");
    break;
  case COM_TYPE_PRESS:
    json_object_dotset_string(JSON_SubSensorDescriptor, "sensorType", "PRESS");
    break;
  case COM_TYPE_HUM:
    json_object_dotset_string(JSON_SubSensorDescriptor, "sensorType", "HUM");
    break;
  case COM_TYPE_MIC:
    json_object_dotset_string(JSON_SubSensorDescriptor, "sensorType", "MIC");
    break;
  case COM_TYPE_MLC:
    json_object_dotset_string(JSON_SubSensorDescriptor, "sensorType", "MLC");
    break;
  default:
    json_object_dotset_string(JSON_SubSensorDescriptor, "sensorType", "NA");
    break;
  }

  json_object_dotset_number(JSON_SubSensorDescriptor, "dimensions", (double)sub_sensor_descriptor->dimensions);

  json_object_dotset_value(JSON_SubSensorDescriptor, "dimensionsLabel", json_value_init_array());
  JSON_SensorArray = json_object_dotget_array(JSON_SubSensorDescriptor, "dimensionsLabel");
  
  for (ii=0; ii < sub_sensor_descriptor->dimensions; ii++)
  {
    json_array_append_string(JSON_SensorArray, sub_sensor_descriptor->dimensionsLabel[ii]);
  }

  json_object_dotset_string(JSON_SubSensorDescriptor, "unit", sub_sensor_descriptor->unit);

  switch (sub_sensor_descriptor->dataType)
  {
  case DATA_TYPE_UINT8:
    json_object_dotset_string(JSON_SubSensorDescriptor, "dataType", "uint8_t");
    break;
  case DATA_TYPE_INT8:
    json_object_dotset_string(JSON_SubSensorDescriptor, "dataType", "int8_t");
    break;
  case DATA_TYPE_UINT16:
    json_object_dotset_string(JSON_SubSensorDescriptor, "dataType", "uint16_t");
    break;
  case DATA_TYPE_INT16:
    json_object_dotset_string(JSON_SubSensorDescriptor, "dataType", "int16_t");
    break;
  case DATA_TYPE_UINT32:
    json_object_dotset_string(JSON_SubSensorDescriptor, "dataType", "uint32_t");
    break;
  case DATA_TYPE_INT32:
    json_object_dotset_string(JSON_SubSensorDescriptor, "dataType", "int32_t");
    break;
  case DATA_TYPE_FLOAT:
    json_object_dotset_string(JSON_SubSensorDescriptor, "dataType", "float");
    break;
  default:
    json_object_dotset_string(JSON_SubSensorDescriptor, "dataType", "NA");
    break;
  }
  ii=0;

  json_object_dotset_value(JSON_SubSensorDescriptor, "FS", json_value_init_array());
  JSON_SensorArray = json_object_dotget_array(JSON_SubSensorDescriptor, "FS");
  while (sub_sensor_descriptor->FS[ii] > 0.0f)
  {
    json_array_append_number(JSON_SensorArray, sub_sensor_descriptor->FS[ii]);
    ii++;
  }
  
  ii=0;

  json_object_dotset_value(JSON_SubSensorDescriptor, "ODR", json_value_init_array());
  JSON_SensorArray = json_object_dotget_array(JSON_SubSensorDescriptor, "ODR");
  while (sub_sensor_descriptor->ODR[ii] > 0.0f)
  {
    json_array_append_number(JSON_SensorArray, sub_sensor_descriptor->ODR[ii]);
    ii++;
  }

  json_object_dotset_number(JSON_SubSensorDescriptor, "samplesPerTs.min", (double)(sub_sensor_descriptor->samplesPerTimestamp[0]));
  json_object_dotset_number(JSON_SubSensorDescriptor, "samplesPerTs.max", (double)(sub_sensor_descriptor->samplesPerTimestamp[1]));
  json_object_dotset_string(JSON_SubSensorDescriptor, "samplesPerTs.dataType", "int16_t");
  
  json_value_free(tempJSONarray);
}

/**
* @brief  This function is called when there is a change on the gatt attribute as consequence of write request for the Extended Configuration characteristic value service
* @param  void *VoidCharPointer
* @param  uint16_t attr_handle Handle of the attribute
* @param  uint16_t Offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode: 
*                          - Bits 0-14: offset of the reported value inside the attribute.
*                          - Bit 15: if the entire value of the attribute does not fit inside a single ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event,
*                            this bit is set to 1 to notify that other ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.                  
* @param  uint8_t data_length length of the data
* @param  uint8_t *att_data attribute data
* @retval None
*/
static void Write_Request_ExtConfig(void *VoidCharPointer,uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data)
{
  uint32_t CommandBufLen=0;
  
  /* Received one write command from Client on Extended Configuration characteristic*/
  CommandBufLen = BLE_Command_TP_Parse(&hs_command_buffer, att_data, data_length);
  
  if(CommandBufLen) {
    /* There is a valid command to execute */
    BLE_ExtConfigCommandType CommandType;
    uint8_t LocalBufferToWrite[2048];
    
    CommandType = BLE_ExtConfig_ExtractCommandType(hs_command_buffer);
    
    switch(CommandType)
    {
    case EXT_CONFIG_COM_READ_COMMAND:
      {
        JSON_Value *tempJSON = json_value_init_object();
        JSON_Object *tempJSON_Obj = json_value_get_object(tempJSON);
        char* JSON_string_command = NULL;
        uint32_t JSON_size =0;
        int32_t WritingPointer=0;
        if(CustomExtConfigReadCustomCommandsCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_READ_CUSTOM_COMMAND].CommandString);
        }
        if(CustomExtConfigReadSensorsConfigCommandsCallback!=NULL) {
           WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_READ_SENSOR_CONFIG].CommandString);
        }
        if(CustomExtConfigRebootOnDFUModeCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_SET_DFU].CommandString);
        }
        if(CustomExtConfigPowerOffCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_SET_OFF].CommandString);
        }
        if(CustomExtConfigSetNameCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_SET_NAME].CommandString);
        }
        if(CustomExtConfigSetTimeCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_SET_TIME].CommandString);
        }
        if(CustomExtConfigSetDateCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_SET_DATE].CommandString);
        }
        if(CustomExtConfigSetWiFiCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_SET_WIFI].CommandString);
        }
        if(CustomExtConfigChangePinCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_CHANGE_PIN].CommandString);
        }
        if(CustomExtConfigClearDBCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_CLEAR_DB].CommandString);
        }
        if(CustomExtConfigReadBanksFwIdCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_READ_BANKS_FW_ID].CommandString);
        }
        if(CustomExtConfigBanksSwapCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_BANKS_SWAP].CommandString);
        }
        if(CustomExtConfigReadCertCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_READ_CERT].CommandString);
        }
        if(CustomExtConfigSetCertCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_SET_CERT].CommandString);
        }
        if(CustomExtConfigUidCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_READ_UID].CommandString);
        }
        if(CustomExtConfigInfoCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_READ_INFO].CommandString);
        }
        if(CustomExtConfigHelpCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_READ_HELP].CommandString);
        }
        if(CustomExtConfigPowerStatusCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_READ_POWER].CommandString);
        }
        if(CustomExtConfigVersionFwCommandCallback!=NULL) {
          WritingPointer+=sprintf((char *)LocalBufferToWrite+WritingPointer,"%s,",StandardExtConfigCommands[EXT_CONFIG_COM_READ_VER_FW].CommandString);
        }
        
        if(WritingPointer!=0) {
          //Replace  the Latest ',' with the String Termination
          LocalBufferToWrite[WritingPointer-1]=0U;
        }
        
        BLE_MANAGER_PRINTF("Command ReadCommand\r\n");

        json_object_dotset_string(tempJSON_Obj, "Commands", (char *)LocalBufferToWrite);
        
        /* convert to a json string and write as string */
        JSON_string_command = json_serialize_to_string(tempJSON);
        JSON_size = json_serialization_size(tempJSON);
        
        BLE_ExtConfiguration_Update((uint8_t*) JSON_string_command,JSON_size);
        BLE_FreeFunction(JSON_string_command);
        json_value_free(tempJSON);
        
        break;
      }
      
      /* Board Report Command */
    case EXT_CONFIG_COM_READ_UID:
      if(CustomExtConfigUidCommandCallback!=NULL) {
        JSON_Value *tempJSON = json_value_init_object();
        JSON_Object *tempJSON_Obj = json_value_get_object(tempJSON);
        char* JSON_string_command = NULL;
        uint32_t JSON_size =0;
        uint8_t *uid;
        
        BLE_MANAGER_PRINTF("Command UID\r\n");
   
        CustomExtConfigUidCommandCallback(&uid);
        
        sprintf((char *)LocalBufferToWrite,"%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",
                uid[ 3],uid[ 2],uid[ 1],uid[ 0],
                uid[ 7],uid[ 6],uid[ 5],uid[ 4],
                uid[11],uid[ 10],uid[9],uid[8]);
        json_object_dotset_string(tempJSON_Obj, "UID", (char *)LocalBufferToWrite);
        
        /* convert to a json string and write as string */
        JSON_string_command = json_serialize_to_string(tempJSON);
        JSON_size = json_serialization_size(tempJSON);
        
        BLE_ExtConfiguration_Update((uint8_t*) JSON_string_command,JSON_size);
        BLE_FreeFunction(JSON_string_command);
        json_value_free(tempJSON);
      }
      break;
      
    case EXT_CONFIG_COM_READ_SENSOR_CONFIG:
      if(CustomExtConfigReadSensorsConfigCommandsCallback!=NULL) {
        JSON_Value *tempJSON = json_value_init_object();
        JSON_Object *tempJSON_Obj = json_value_get_object(tempJSON);
        JSON_Array *JSON_SensorArray;
        char* JSON_string_command = NULL;
        uint32_t JSON_size =0;
        
        BLE_MANAGER_PRINTF("Command ReadSensorsConfigCommand\r\n");

        json_object_dotset_value(tempJSON_Obj, "sensor", json_value_init_array());
        JSON_SensorArray = json_object_dotget_array(tempJSON_Obj, "sensor");
        
        //Filling the array
        CustomExtConfigReadSensorsConfigCommandsCallback(JSON_SensorArray);
        
        /* convert to a json string and write as string */
        JSON_string_command = json_serialize_to_string(tempJSON);
        JSON_size = json_serialization_size(tempJSON);

        BLE_ExtConfiguration_Update((uint8_t*) JSON_string_command,JSON_size);
        BLE_FreeFunction(JSON_string_command);
        json_value_free(tempJSON);
      }
      break;
      
    case EXT_CONFIG_COM_READ_VER_FW:
      if(CustomExtConfigVersionFwCommandCallback!=NULL) {
        JSON_Value *tempJSON = json_value_init_object();
        JSON_Object *tempJSON_Obj = json_value_get_object(tempJSON);
        char* JSON_string_command = NULL;
        uint32_t JSON_size =0;
        
        BLE_MANAGER_PRINTF("Command VersionFw\r\n");

        CustomExtConfigVersionFwCommandCallback(LocalBufferToWrite);
        
        json_object_dotset_string(tempJSON_Obj, "VersionFw", (char *)LocalBufferToWrite);
        
        /* convert to a json string and write as string */
        JSON_string_command = json_serialize_to_string(tempJSON);
        JSON_size = json_serialization_size(tempJSON);
        
        BLE_ExtConfiguration_Update((uint8_t*) JSON_string_command,JSON_size);
        BLE_FreeFunction(JSON_string_command);
      }
      break;
      
    case EXT_CONFIG_COM_READ_INFO:
      if(CustomExtConfigInfoCommandCallback!=NULL) {
        JSON_Value *tempJSON = json_value_init_object();
        JSON_Object *tempJSON_Obj = json_value_get_object(tempJSON);
        char* JSON_string_command = NULL;
        uint32_t JSON_size =0;
        
        BLE_MANAGER_PRINTF("Command Info\r\n");
        
        
        CustomExtConfigInfoCommandCallback(LocalBufferToWrite);
        
        json_object_dotset_string(tempJSON_Obj, "Info", (char *)LocalBufferToWrite);
        
        /* convert to a json string and write as string */
        JSON_string_command = json_serialize_to_string(tempJSON);
        JSON_size = json_serialization_size(tempJSON);
        
        BLE_ExtConfiguration_Update((uint8_t*) JSON_string_command,JSON_size);
        BLE_FreeFunction(JSON_string_command);
      }
      break;
      
    case EXT_CONFIG_COM_READ_HELP:
      if(CustomExtConfigHelpCommandCallback!=NULL) {
        JSON_Value *tempJSON = json_value_init_object();
        JSON_Object *tempJSON_Obj = json_value_get_object(tempJSON);
        char* JSON_string_command = NULL;
        uint32_t JSON_size =0;
        
        BLE_MANAGER_PRINTF("Command Help\r\n");
           
        CustomExtConfigHelpCommandCallback(LocalBufferToWrite);
        
        json_object_dotset_string(tempJSON_Obj, "Help", (char *)LocalBufferToWrite);
        
        /* convert to a json string and write as string */
        JSON_string_command = json_serialize_to_string(tempJSON);
        JSON_size = json_serialization_size(tempJSON);
        
        BLE_ExtConfiguration_Update((uint8_t*) JSON_string_command,JSON_size);
        BLE_FreeFunction(JSON_string_command);
      }
      break;
      
    case EXT_CONFIG_COM_READ_POWER:
       if(CustomExtConfigPowerStatusCommandCallback!=NULL) {
        JSON_Value *tempJSON = json_value_init_object();
        JSON_Object *tempJSON_Obj = json_value_get_object(tempJSON);
        char* JSON_string_command = NULL;
        uint32_t JSON_size =0;
        BLE_MANAGER_PRINTF("Command PowerStatus\r\n");
        
        CustomExtConfigPowerStatusCommandCallback(LocalBufferToWrite);
        
        json_object_dotset_string(tempJSON_Obj, "PowerStatus", (char *)LocalBufferToWrite);
        
        /* convert to a json string and write as string */
        JSON_string_command = json_serialize_to_string(tempJSON);
        JSON_size = json_serialization_size(tempJSON);
        
        BLE_ExtConfiguration_Update((uint8_t*) JSON_string_command,JSON_size);
        BLE_FreeFunction(JSON_string_command);
      }
      break;
      
    case EXT_CONFIG_COM_READ_CUSTOM_COMMAND:
      if(CustomExtConfigReadCustomCommandsCallback!=NULL) {
        SendNewCustomCommandList();
      }
      break;
      
    case EXT_CONFIG_COM_READ_CERT:
      if(CustomExtConfigReadCertCommandCallback!=NULL) {
        JSON_Value *tempJSON = json_value_init_object();
        JSON_Object *tempJSON_Obj = json_value_get_object(tempJSON);
        char* JSON_string_command = NULL;
        uint32_t JSON_size =0;
        
        BLE_MANAGER_PRINTF("Command PowerStatus\r\n");
        
        CustomExtConfigReadCertCommandCallback(LocalBufferToWrite);
        
        json_object_dotset_string(tempJSON_Obj, "Certificate", (char *)LocalBufferToWrite);
        
        /* convert to a json string and write as string */
        JSON_string_command = json_serialize_to_string(tempJSON);
        JSON_size = json_serialization_size(tempJSON);
        
        BLE_ExtConfiguration_Update((uint8_t*) JSON_string_command,JSON_size);
        
      }
      break;
      
    case EXT_CONFIG_COM_SET_DFU:
      if(CustomExtConfigRebootOnDFUModeCommandCallback!=NULL) {
        BLE_MANAGER_PRINTF("Command DFU\r\n");
        CustomExtConfigRebootOnDFUModeCommandCallback();
      }
      break;
      
    case EXT_CONFIG_COM_SET_OFF:
      if(CustomExtConfigPowerOffCommandCallback!=NULL) {
        BLE_MANAGER_PRINTF("Command Off\r\n");
        CustomExtConfigPowerOffCommandCallback();
      }
      break;
      
    case EXT_CONFIG_COM_CLEAR_DB:
      if(CustomExtConfigClearDBCommandCallback!=NULL) {
        BLE_MANAGER_PRINTF("Command ClearDB\r\n");
        CustomExtConfigClearDBCommandCallback();
      }
      break;
      
    case EXT_CONFIG_COM_READ_BANKS_FW_ID:
      if(CustomExtConfigReadBanksFwIdCommandCallback!=NULL) {
        JSON_Value *tempJSON = json_value_init_object();
        JSON_Object *tempJSON_Obj = json_value_get_object(tempJSON);
        char* JSON_string_command = NULL;
        uint32_t JSON_size =0;
        uint8_t CurBank;
        uint16_t FwId1,FwId2;

        BLE_MANAGER_PRINTF("Command ReadBanksFwId\r\n");
        CustomExtConfigReadBanksFwIdCommandCallback(&CurBank,&FwId1,&FwId2);

        json_object_dotset_number(tempJSON_Obj, "BankStatus.currentBank", (double)CurBank);
        sprintf((char *)LocalBufferToWrite,"0x%02X",FwId1);
        json_object_dotset_string(tempJSON_Obj, "BankStatus.fwId1", (char *)LocalBufferToWrite);
        sprintf((char *)LocalBufferToWrite,"0x%02X",FwId2);
        json_object_dotset_string(tempJSON_Obj, "BankStatus.fwId2", (char *)LocalBufferToWrite);

        /* convert to a json string and write as string */
        JSON_string_command = json_serialize_to_string(tempJSON);
        JSON_size = json_serialization_size(tempJSON);

        BLE_ExtConfiguration_Update((uint8_t*) JSON_string_command,JSON_size);
        json_value_free(tempJSON);
      }
      break;

    case EXT_CONFIG_COM_BANKS_SWAP:
      if(CustomExtConfigBanksSwapCommandCallback!=NULL) {
        BLE_MANAGER_PRINTF("Command BanksSwap\r\n");
        CustomExtConfigBanksSwapCommandCallback();
      }
      break;
      
      // Command with argument
    case EXT_CONFIG_COM_SET_DATE:
      if(CustomExtConfigSetDateCommandCallback!=NULL) {
        BLE_MANAGER_PRINTF("Command SetDate\r\n");
      
        JSON_Value *tempJSON = json_parse_string( (char *) hs_command_buffer);
        JSON_Object *JSON_ParseHandler = json_value_get_object(tempJSON);
        if (strcmp(json_object_dotget_string(JSON_ParseHandler,"command"),"SetDate") == 0) {
          if(json_object_dothas_value(JSON_ParseHandler,"argString")) {
            uint8_t *NewDate = (uint8_t *)json_object_dotget_string(JSON_ParseHandler,"argString");
            CustomExtConfigSetDateCommandCallback(NewDate);
          }
        }
        json_value_free(tempJSON);
      }
      break;
      
    case EXT_CONFIG_COM_SET_TIME:
       if(CustomExtConfigSetTimeCommandCallback!=NULL) {
        BLE_MANAGER_PRINTF("Command SetTime\r\n");
     
        JSON_Value *tempJSON = json_parse_string( (char *) hs_command_buffer);
        JSON_Object *JSON_ParseHandler = json_value_get_object(tempJSON);
        if (strcmp(json_object_dotget_string(JSON_ParseHandler,"command"),"SetTime") == 0) {
          if(json_object_dothas_value(JSON_ParseHandler,"argString")) {
            uint8_t *NewTime = (uint8_t *)json_object_dotget_string(JSON_ParseHandler,"argString");
            CustomExtConfigSetTimeCommandCallback(NewTime);
          }
        }
        json_value_free(tempJSON);
      }
      break;
      
    case EXT_CONFIG_COM_SET_NAME:
       if(CustomExtConfigSetNameCommandCallback!=NULL) {
        BLE_MANAGER_PRINTF("Command SetName\r\n");
        JSON_Value *tempJSON = json_parse_string( (char *) hs_command_buffer);
        JSON_Object *JSON_ParseHandler = json_value_get_object(tempJSON);
        if (strcmp(json_object_dotget_string(JSON_ParseHandler,"command"),"SetName") == 0) {
          if(json_object_dothas_value(JSON_ParseHandler,"argString")) {
            uint8_t *NewBoardName = (uint8_t *)json_object_dotget_string(JSON_ParseHandler,"argString");
            CustomExtConfigSetNameCommandCallback(NewBoardName);
          }
        }
        json_value_free(tempJSON);
      }
      break;
      
    case EXT_CONFIG_COM_SET_WIFI:
      if(CustomExtConfigSetWiFiCommandCallback!=NULL) {
        BLE_MANAGER_PRINTF("Command SetWiFi\r\n");
        JSON_Value *tempJSON = json_parse_string( (char *) hs_command_buffer);
        JSON_Object *JSON_ParseHandler = json_value_get_object(tempJSON);
        if (strcmp(json_object_dotget_string(JSON_ParseHandler,"command"),"SetWiFi") == 0) {
          JSON_Object *JSON_Wifi = json_object_dotget_object(JSON_ParseHandler,"argJsonElement");
          if(json_object_dothas_value(JSON_Wifi,"ssid")) {
            BLE_WiFi_CredAcc_t NewWiFiCred;
            NewWiFiCred.SSID = (uint8_t *)json_object_dotget_string(JSON_Wifi,"ssid");
            if(json_object_dothas_value(JSON_Wifi,"password")) {
              NewWiFiCred.PassWd = (uint8_t *)json_object_dotget_string(JSON_Wifi,"password");
              if(json_object_dothas_value(JSON_Wifi,"securityType")) {
                NewWiFiCred.Security = (uint8_t *)json_object_dotget_string(JSON_Wifi,"securityType");
                CustomExtConfigSetWiFiCommandCallback(NewWiFiCred);
              }
            }
          }
        }
        json_value_free(tempJSON);
      }
      break;
      
    case EXT_CONFIG_COM_CHANGE_PIN:
       if(CustomExtConfigChangePinCommandCallback!=NULL) {
        BLE_MANAGER_PRINTF("Command ChangePIN\r\n");
     
        JSON_Value *tempJSON = json_parse_string( (char *) hs_command_buffer);
        JSON_Object *JSON_ParseHandler = json_value_get_object(tempJSON);
        if (strcmp(json_object_dotget_string(JSON_ParseHandler,"command"),"ChangePIN") == 0) {
          if(json_object_dothas_value(JSON_ParseHandler,"argNumber")) {
            uint32_t NewBoardPin = (uint32_t)json_object_dotget_number(JSON_ParseHandler,"argNumber");
            CustomExtConfigChangePinCommandCallback(NewBoardPin);
          }
        }
        json_value_free(tempJSON);
      }
      break;
      
    case EXT_CONFIG_COM_SET_CERT:
      if(CustomExtConfigSetCertCommandCallback!=NULL) {
        BLE_MANAGER_PRINTF("Command SetCert\r\n");
        
        JSON_Value *tempJSON = json_parse_string( (char *) hs_command_buffer);
        JSON_Object *JSON_ParseHandler = json_value_get_object(tempJSON);
        if (strcmp(json_object_dotget_string(JSON_ParseHandler,"command"),"SetCert") == 0) {
          if(json_object_dothas_value(JSON_ParseHandler,"argString")) {
            uint8_t *NewCertificate = (uint8_t *)json_object_dotget_string(JSON_ParseHandler,"argString");
            CustomExtConfigSetCertCommandCallback(NewCertificate);
          }
        }
        json_value_free(tempJSON);
      }
      break;
      
    case EXT_CONFIG_COM_SET_SENSOR_CONFIG:
      if(CustomExtConfigSetSensorsConfigCommandsCallback!=NULL) {
        BLE_MANAGER_PRINTF("Command SetSensorsConfigCommand\r\n");
        CustomExtConfigSetSensorsConfigCommandsCallback(hs_command_buffer);
      }
      break;
      
    default:
      //Check if it's a custom Command or not
      if(CustomExtConfigCustomCommandCallback!=NULL) {
        /* we need at least one Custom Command */
        if(ExtConfigCustomCommands!=NULL) {
          BLE_CustomCommadResult_t *CommandResult = ParseCustomCommand(ExtConfigCustomCommands,hs_command_buffer);
          if(CommandResult!=NULL) {
            CustomExtConfigCustomCommandCallback(CommandResult);
            if(CommandResult->CommandName!=NULL) {
              BLE_FreeFunction(CommandResult->CommandName);
            }
            if(CommandResult->StringValue!=NULL) {
              BLE_FreeFunction(CommandResult->StringValue);
            }
            BLE_FreeFunction(CommandResult);
          }
        } else {
          BLE_MANAGER_PRINTF("Error: Command Not Valid\r\n");
        }
      } else {
        BLE_MANAGER_PRINTF("Error: Command Not Valid\r\n");
      }
      break;
    }
    BLE_FreeFunction(hs_command_buffer);
  }
}

/**
* @brief  This function Try to search if there is a valid Custom Command
* @param  BLE_ExtCustomCommand_t *LocCustomCommands Pointer to the Custom Commands List
* @param  uint8_t *hs_command_buffer pointer to json formatted string
* @retval None
*/
BLE_CustomCommadResult_t *ParseCustomCommand(BLE_ExtCustomCommand_t *LocCustomCommands,uint8_t *hs_command_buffer)                        
{
  BLE_CustomCommadResult_t *CommandResult=NULL;
  uint8_t ValidCustomCommand=0;
  JSON_Value *tempJSON = json_parse_string( (char *) hs_command_buffer);
  JSON_Object *JSON_ParseHandler = json_value_get_object(tempJSON);
  /* Start from beginning of Custom Commands list*/
  BLE_ExtCustomCommand_t *LocLastCustomCommand = LocCustomCommands;
  
  /* Search if it's a custom Command defined by user */
  while((ValidCustomCommand==0U) && (LocLastCustomCommand!=NULL)){
    /* Check the command name */
    if (strncmp(json_object_dotget_string(JSON_ParseHandler,"command"),LocLastCustomCommand->CommandName,strlen(json_object_dotget_string(JSON_ParseHandler,"command"))) == 0) {
      ValidCustomCommand=1;
    }
    /* Move to the Next Command if we didn't find nothing*/
    if(ValidCustomCommand==0U) {
      LocLastCustomCommand = (BLE_ExtCustomCommand_t *) LocLastCustomCommand->NextCommand;
    }
  }
  /* If we have found a valid Custom Command extract the values */
  if(ValidCustomCommand) {
    CommandResult = (BLE_CustomCommadResult_t *) BLE_MallocFunction(sizeof(BLE_CustomCommadResult_t));
    if(CommandResult == NULL) {
      BLE_MANAGER_PRINTF("Error: Mem alloc error: %d@%s\r\n", __LINE__, __FILE__);
    }
    
    CommandResult->CommandName = (uint8_t*)BLE_MallocFunction(strlen((char*)LocLastCustomCommand->CommandName)+1U);
    if(CommandResult->CommandName==NULL) {
      BLE_MANAGER_PRINTF("Error: Mem alloc error: %d@%s\r\n", __LINE__, __FILE__);
      BLE_FreeFunction(CommandResult);
    } else {
      sprintf((char *)CommandResult->CommandName,"%s",(char *)LocLastCustomCommand->CommandName);
      CommandResult->CommandType= LocLastCustomCommand->CommandType;
    }
    
    switch(LocLastCustomCommand->CommandType) { 
    case BLE_CUSTOM_COMMAND_VOID:
      {
      BLE_MANAGER_PRINTF("Called Custom Void Command <%s>\r\n",LocLastCustomCommand->CommandName);
      CommandResult->IntValue= 0;
      CommandResult->StringValue= NULL;
      break;
      }
    case BLE_CUSTOM_COMMAND_INTEGER:
    case BLE_CUSTOM_COMMAND_ENUM_INTEGER:
      if(json_object_dothas_value(JSON_ParseHandler,"argNumber")) {
        int32_t NewValue = (int32_t)json_object_dotget_number(JSON_ParseHandler,"argNumber");
        CommandResult->IntValue= NewValue;
        CommandResult->StringValue= NULL;
        BLE_MANAGER_PRINTF("Called Custom Integer Command <%s>\r\n",LocLastCustomCommand->CommandName);
        BLE_MANAGER_PRINTF("\tNumber=%ld\r\n",NewValue);
      }
      break;
    case BLE_CUSTOM_COMMAND_BOOLEAN:
      if(json_object_dothas_value(JSON_ParseHandler,"argString")) {
        uint8_t *NewString = (uint8_t *)json_object_dotget_string(JSON_ParseHandler,"argString");
        
        if(strncmp((char*)NewString,"true",4)==0)
          CommandResult->IntValue= 1;
        
        if(strncmp((char*)NewString,"false",5)==0)
          CommandResult->IntValue= 0;
        
        CommandResult->StringValue= NULL;
        BLE_MANAGER_PRINTF("Called Custom Boolean Command <%s>\r\n",LocLastCustomCommand->CommandName);
        BLE_MANAGER_PRINTF("\tBoolean=<%s>\r\n",NewString);
      }
      break;
    case BLE_CUSTOM_COMMAND_STRING:
    case BLE_CUSTOM_COMMAND_ENUM_STRING:
      if(json_object_dothas_value(JSON_ParseHandler,"argString")) {
        uint8_t *NewString = (uint8_t *)json_object_dotget_string(JSON_ParseHandler,"argString");
        CommandResult->IntValue= 0;
        CommandResult->StringValue = (uint8_t*)BLE_MallocFunction(strlen((char*)NewString)+1U);
        if(CommandResult->StringValue==NULL) {
          BLE_MANAGER_PRINTF("Error: Mem alloc error: %d@%s\r\n", __LINE__, __FILE__);
          BLE_FreeFunction(CommandResult);
        } else {
          sprintf((char *)CommandResult->StringValue,"%s",(char *)NewString);
          BLE_MANAGER_PRINTF("Called Custom String Command <%s>\r\n",LocLastCustomCommand->CommandName);
          BLE_MANAGER_PRINTF("\tString=<%s>\r\n",NewString);
        }
      }
      break;
    default:
      break;
    }
  }
  json_value_free(tempJSON);
  
  if(ValidCustomCommand==0U) {
    BLE_MANAGER_PRINTF("Error: Custom Command Not Valid\r\n");
  }
  
  return CommandResult;
}

/**
* @brief  Clear the Custom Command List 
* @param  BLE_ExtCustomCommand_t **LocCustomCommands Custom Commands Table
* @param  BLE_ExtCustomCommand_t **LocLastCustomCommand Poiter to last Custom Command
* @retval None
*/
void GenericClearCustomCommandsList(BLE_ExtCustomCommand_t **LocCustomCommands, BLE_ExtCustomCommand_t **LocLastCustomCommand) {
  if((*LocCustomCommands)!=NULL) {
    if((*LocCustomCommands)->NextCommand!=NULL) {
      ClearSingleCommand((BLE_ExtCustomCommand_t *)(*LocCustomCommands)->NextCommand);
    }
    BLE_FreeFunction((*LocCustomCommands));
    *LocLastCustomCommand = *LocCustomCommands = NULL;
  }
}

/**
* @brief  Clear One Single Custom Command
* @param  BLE_ExtCustomCommand_t *NextCommand poiter to the next Custom Commands
* @retval None
*/
static void ClearSingleCommand(BLE_ExtCustomCommand_t *Command)
{
  if(Command->NextCommand!=NULL) {
    ClearSingleCommand((BLE_ExtCustomCommand_t *)Command->NextCommand);
  }
#if (BLE_DEBUG_LEVEL>1)
  BLE_MANAGER_PRINTF("Deleting Custom Command<%s>\r\n",Command->CommandName);
#endif
  BLE_FreeFunction(Command->CommandName);
  BLE_FreeFunction(Command);
}

/**
* @brief  This function add a Custom Command
* @param  BLE_ExtCustomCommand_t **LocCustomCommands Pointer to Custom Commands Table
* @param  BLE_ExtCustomCommand_t **LocLastCustomCommand Poiter to Pointer to slast Custom Command
* @param  char *CommandName Command Name
* @param  BLE_CustomCommandTypes_t CommandType Command Type
* @param  int32_t DefaultValue (for BLE_CUSTOM_COMMAND_INTEGER,BLE_CUSTOM_COMMAND_ENUM_INTEGER,BLE_CUSTOM_COMMAND_ENUM_STRING or BLE_CUSTOM_COMMAND_BOOLEAN)
* @param  int32_t Min  Optional Minim value (BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN if not needed)
* @param  int32_t Max Max value
* @param  int32_t *ValidValuesInt array of Valid Integer Values (NULL if not needed)
* @param  char **ValidValuesString array of Valid String Values (NULL if not needed)
* @param  char *ShortDesc Optionally Short Description (NULL if not needed)
* @param  JSON_Array *JSON_SensorArray
* @retval uint8_t Valid/NotValid Command (1/0)
*/
uint8_t GenericAddCustomCommand(BLE_ExtCustomCommand_t **LocCustomCommands, BLE_ExtCustomCommand_t **LocLastCustomCommand,
                         char *CommandName,BLE_CustomCommandTypes_t CommandType, int32_t DefaultValue,
                         int32_t Min, int32_t Max, int32_t *ValidValuesInt, char **ValidValuesString,char *ShortDesc,JSON_Array *JSON_SensorArray)
{
  uint8_t Valid =1U;
  uint8_t Index;
  
  //check that we are not using a Standard Command
  for(Index=((uint8_t)EXT_CONFIG_COM_READ_COMMAND); ((Index<((uint8_t)EXT_CONFIG_COMMAND_NUMBER)) && (Valid==1U)); Index++) {
    if(strncmp(StandardExtConfigCommands[Index].CommandString,CommandName,strlen(CommandName)) == 0) {
      Valid =0U;
    }
  }
  
  //If the Command Name is different from one Standard Command Name
  if(Valid) {
    JSON_Value *tempJSON1;
    JSON_Object *tempJSON1_Obj;
    tempJSON1 = json_value_init_object();
    tempJSON1_Obj = json_value_get_object(tempJSON1);
    
    //Add the Command Name
    json_object_dotset_string(tempJSON1_Obj, "Name", CommandName);
    
    //Add the Command Type */
    switch(CommandType) {
    case BLE_CUSTOM_COMMAND_VOID:
      json_object_dotset_string(tempJSON1_Obj, "Type", "Void");
      break;
    case BLE_CUSTOM_COMMAND_INTEGER:
      json_object_dotset_string(tempJSON1_Obj, "Type", "Integer");
      break;
    case BLE_CUSTOM_COMMAND_BOOLEAN:
      json_object_dotset_string(tempJSON1_Obj, "Type", "Boolean");
      break;
    case  BLE_CUSTOM_COMMAND_STRING:
      json_object_dotset_string(tempJSON1_Obj, "Type", "String");
      break;
    case  BLE_CUSTOM_COMMAND_ENUM_INTEGER:
      json_object_dotset_string(tempJSON1_Obj, "Type", "EnumInteger");
      break;
    case  BLE_CUSTOM_COMMAND_ENUM_STRING:
      json_object_dotset_string(tempJSON1_Obj, "Type", "EnumString");
      break;
    }
    
    //Add the Optional Default Value
    if((CommandType==BLE_CUSTOM_COMMAND_INTEGER) ||
       (CommandType==BLE_CUSTOM_COMMAND_BOOLEAN) ||
       (CommandType==BLE_CUSTOM_COMMAND_ENUM_INTEGER) ||
       (CommandType==BLE_CUSTOM_COMMAND_ENUM_STRING)) {
      if(DefaultValue!= (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN) {
        json_object_dotset_number(tempJSON1_Obj, "DefaultValue", (double)DefaultValue);
      }
    }
    
    //Add the Optional Description
    if(ShortDesc!=NULL) {
      json_object_dotset_string(tempJSON1_Obj, "Desc", ShortDesc);
    }
    
    //Add the Min/Max Value
    if(Min!= (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN) {
      json_object_dotset_number(tempJSON1_Obj, "Min", (double)Min);
    }
    if(Max!= (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN) {
      json_object_dotset_number(tempJSON1_Obj, "Max", (double)Max);
    }
    
    //Add the array of Valid Integer Values
    if((ValidValuesInt!=NULL) && (CommandType==BLE_CUSTOM_COMMAND_ENUM_INTEGER)) {
      JSON_Value *tempJSONarray = json_value_init_object();
      JSON_Array *JSON_SensorArray2 = json_value_get_array(tempJSONarray);
      int32_t counter=0;
      //Allocate
      json_object_dotset_value(tempJSON1_Obj, "IntegerValues", json_value_init_array());
      //Add Elements
      JSON_SensorArray2 = json_object_dotget_array(tempJSON1_Obj, "IntegerValues");
      while(ValidValuesInt[counter]!=(int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN) {
        json_array_append_number(JSON_SensorArray2, (double)(ValidValuesInt[counter]));
        counter++;
      }
      //Free
      json_value_free(tempJSONarray);
    }
    
    //Add the array of Valid String Values
    if((ValidValuesString!=NULL) && (CommandType==BLE_CUSTOM_COMMAND_ENUM_STRING)) {
      JSON_Value *tempJSONarray = json_value_init_object();
      JSON_Array *JSON_SensorArray2 = json_value_get_array(tempJSONarray);
      int32_t counter=0;
      //Allocate
      json_object_dotset_value(tempJSON1_Obj, "StringValues", json_value_init_array());
      //Add Elements
      JSON_SensorArray2 = json_object_dotget_array(tempJSON1_Obj, "StringValues");
      while(ValidValuesString[counter]!=NULL) {
        json_array_append_string(JSON_SensorArray2, (char const*)ValidValuesString[counter]);
        counter++;
      }
      //Free
      json_value_free(tempJSONarray);
    }
    
    json_array_append_value(JSON_SensorArray,tempJSON1);
    
    //Allocate a New Custom Command entry
    if((*LocCustomCommands)==NULL) {
      (*LocCustomCommands)=(BLE_ExtCustomCommand_t*)BLE_MallocFunction(sizeof(BLE_ExtCustomCommand_t));
      
      if((*LocCustomCommands)==NULL) {
        BLE_MANAGER_PRINTF("Errror: Mem calloc error: %d@%s\r\n",__LINE__,__FILE__);
        return 0;
      }
      
      (*LocLastCustomCommand) = (*LocCustomCommands);
    } else {
      (*LocLastCustomCommand)->NextCommand = (void *) BLE_MallocFunction( sizeof(BLE_ExtCustomCommand_t));
      if((*LocCustomCommands)==NULL) {
        BLE_MANAGER_PRINTF("Error: Mem calloc error %d@%s\r\n",__LINE__,__FILE__);
        return 0;
      }
      (*LocLastCustomCommand) = (BLE_ExtCustomCommand_t*) (*LocLastCustomCommand)->NextCommand;
    }
    //Fill the Custom Command
    (*LocLastCustomCommand)->CommandType = CommandType;
    
    //Alloc the size for commandName
    (*LocLastCustomCommand)->CommandName = BLE_MallocFunction(strlen(CommandName)+1U);
     if(((*LocLastCustomCommand)->CommandName)==NULL) {
       BLE_MANAGER_PRINTF("Error: Mem calloc error %d@%s\r\n",__LINE__,__FILE__);
       return 0;
     }
    sprintf((*LocLastCustomCommand)->CommandName,"%s",CommandName);
    (*LocLastCustomCommand)->NextCommand = NULL;
#if (BLE_DEBUG_LEVEL>1)
    BLE_MANAGER_PRINTF("Adding Custom Command<%s>\r\n",(*LocLastCustomCommand)->CommandName);
#endif
  }
  
  return Valid;
}

/**
* @brief  Send a new List of Custom Commands for Extended Configuration
* @param  None
* @retval None
*/
void SendNewCustomCommandList(void) {
  JSON_Value *tempJSON = json_value_init_object();
  JSON_Object *tempJSON_Obj = json_value_get_object(tempJSON);
  JSON_Array *JSON_SensorArray;
  char* JSON_string_command = NULL;
  uint32_t JSON_size =0;
  
  BLE_MANAGER_PRINTF("Command SendNewCustomCommandList\r\n");

  json_object_dotset_value(tempJSON_Obj, "CustomCommands", json_value_init_array());
  JSON_SensorArray = json_object_dotget_array(tempJSON_Obj, "CustomCommands");
  
  //Filling the array
  CustomExtConfigReadCustomCommandsCallback(JSON_SensorArray);
  
  /* convert to a json string and write as string */
  JSON_string_command = json_serialize_to_string(tempJSON);
  JSON_size = json_serialization_size(tempJSON);
  
  BLE_ExtConfiguration_Update((uint8_t*) JSON_string_command,JSON_size);
  BLE_FreeFunction(JSON_string_command);
  json_value_free(tempJSON);
}

/**
* @brief  Send one error message
* @param  char *message error message
* @retval None
*/
extern void SendError(char *message)
{
  JSON_Value *tempJSON = json_value_init_object();
  JSON_Object *tempJSON_Obj = json_value_get_object(tempJSON);
  char* JSON_string_command = NULL;
  uint32_t JSON_size =0;
  
  BLE_MANAGER_PRINTF("Command SendError\r\n");
  
  json_object_dotset_string(tempJSON_Obj, "Error", message);
  
  /* convert to a json string and write as string */
  JSON_string_command = json_serialize_to_string(tempJSON);
  JSON_size = json_serialization_size(tempJSON);
  
  BLE_ExtConfiguration_Update((uint8_t*) JSON_string_command,JSON_size);
  BLE_FreeFunction(JSON_string_command);
  
}

/**
* @brief  Send one Info message
* @param  char *message Info message
* @retval None
*/
void SendInfo(char *message)
{
  JSON_Value *tempJSON = json_value_init_object();
  JSON_Object *tempJSON_Obj = json_value_get_object(tempJSON);
  char* JSON_string_command = NULL;
  uint32_t JSON_size =0;
  
  BLE_MANAGER_PRINTF("Command SendInfo\r\n");
  
  json_object_dotset_string(tempJSON_Obj, "Info", message);
  
  /* convert to a json string and write as string */
  JSON_string_command = json_serialize_to_string(tempJSON);
  JSON_size = json_serialization_size(tempJSON);
  
  BLE_ExtConfiguration_Update((uint8_t*) JSON_string_command,JSON_size);
  BLE_FreeFunction(JSON_string_command);
}

void create_JSON_Sensor(COM_Sensor_t *sensor, JSON_Value *tempJSON)
{
  uint8_t nSensor = sensor->sensorDescriptor.id;
  
  JSON_Object *JSON_Sensor = json_value_get_object(tempJSON);
  
  json_object_dotset_number(JSON_Sensor, "id", (double)nSensor);
  json_object_dotset_string(JSON_Sensor, "name", sensor->sensorDescriptor.name);
  
  JSON_Value *DescriptorJSON = json_value_init_object();
  json_object_set_value(JSON_Sensor, "sensorDescriptor", DescriptorJSON);
  create_JSON_SensorDescriptor(&sensor->sensorDescriptor, DescriptorJSON);
  
  JSON_Value *statusJSON = json_value_init_object();
  json_object_set_value(JSON_Sensor, "sensorStatus", statusJSON);
  create_JSON_SensorStatus(sensor, statusJSON);
}
#endif /* BLE_MANAGER_NO_PARSON */

/**
* @brief  This function is called when there is a change on the gatt attribute as consequence of write request for the Term service
* @param  void *VoidCharPointer
* @param  uint16_t attr_handle Handle of the attribute
* @param  uint16_t Offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode: 
*                          - Bits 0-14: offset of the reported value inside the attribute.
*                          - Bit 15: if the entire value of the attribute does not fit inside a single ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event,
*                            this bit is set to 1 to notify that other ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.                  
* @param  uint8_t data_length length of the data
* @param  uint8_t *att_data attribute data
* @retval None
*/
static void Write_Request_Term(void *VoidCharPointer,uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data)
{
  /* By default Answer with the same message received */
  uint32_t SendBackData =1; 
  
  /* Received one write from Client on Terminal characteristc */
  if(CustomDebugConsoleParsingCallback!=NULL) {
    SendBackData = CustomDebugConsoleParsingCallback(att_data,data_length);
  }
  
  /* Send it back if it's necessary */
  if(SendBackData) {
    Term_Update(att_data,data_length);
  }
}

/**
* @brief  Add the HW Features service using a vendor specific profile
* @param  None
* @retval tBleStatus Status
*/
static tBleStatus BLE_Manager_AddFeaturesService(void)
{
  tBleStatus ret;
  
#if (BLUE_CORE != BLUENRG_MS)
  Service_UUID_t service_uuid;
  Char_UUID_t char_uuid;
#endif /* (BLUE_CORE != BLUENRG_MS) */ 
  
  uint8_t BleChar;
  uint8_t uuid[16];
  uint16_t Service_Handle;
  uint8_t NumberCustomBLEChars = UsedBleChars-UsedStandardBleChars;
  
  COPY_FEATURES_SERVICE_UUID(uuid);
#if (BLUE_CORE == BLUENRG_MS)
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, (1U+(3U*NumberCustomBLEChars)),&Service_Handle);
#else /* (BLUE_CORE == BLUENRG_MS) */
  BLE_MemCpy(&service_uuid.Service_UUID_128, uuid, 16);
  #if (BLUE_CORE != BLUENRG_LP)
    ret = aci_gatt_add_service(UUID_TYPE_128,  &service_uuid, PRIMARY_SERVICE, (1U+(3U*NumberCustomBLEChars)),&Service_Handle);
  #else /* (BLUE_CORE != BLUENRG_LP) */
    ret = aci_gatt_srv_add_service_nwk(UUID_TYPE_128,  &service_uuid, PRIMARY_SERVICE, (1U+(3U*NumberCustomBLEChars)),&Service_Handle);
  #endif /* (BLUE_CORE != BLUENRG_LP) */
#endif /* (BLUE_CORE == BLUENRG_MS) */ 
  
  if (ret != (tBleStatus)BLE_STATUS_SUCCESS) {
    goto EndLabel;
  }
  
  for(BleChar=UsedStandardBleChars;BleChar<UsedBleChars;BleChar++) {
    BleCharsArray[BleChar]->Service_Handle = Service_Handle;
#if (BLUE_CORE != BLUENRG_MS)
    BLE_MemCpy(&char_uuid.Char_UUID_128, BleCharsArray[BleChar]->uuid, 16);
#endif /* (BLUE_CORE != BLUENRG_MS) */ 
    
#if (BLUE_CORE != BLUENRG_LP)
    ret =  aci_gatt_add_char(BleCharsArray[BleChar]->Service_Handle,
#else /* (BLUE_CORE != BLUENRG_LP) */
   ret =  aci_gatt_srv_add_char_nwk(BleCharsArray[BleChar]->Service_Handle,
#endif /* (BLUE_CORE != BLUENRG_LP) */
                             BleCharsArray[BleChar]->Char_UUID_Type,
#if (BLUE_CORE == BLUENRG_MS)
                             BleCharsArray[BleChar]->uuid,
#else /* (BLUE_CORE == BLUENRG_MS) */							 
                             &char_uuid,
#endif /* (BLUE_CORE == BLUENRG_MS) */ 
                             BleCharsArray[BleChar]->Char_Value_Length,
                             BleCharsArray[BleChar]->Char_Properties,
                             BleCharsArray[BleChar]->Security_Permissions,
                             BleCharsArray[BleChar]->GATT_Evt_Mask,
                             BleCharsArray[BleChar]->Enc_Key_Size,
                             BleCharsArray[BleChar]->Is_Variable,
                             &(BleCharsArray[BleChar]->attr_handle));
    
    if (ret != (tBleStatus)BLE_STATUS_SUCCESS) {
      goto EndLabel;
    }
  }
  
EndLabel:
  return ret;
}

#ifdef ACC_BLUENRG_CONGESTION
static int32_t breath=0;

/* @brief  Update the value of a characteristic avoiding (for a short time) to
*         send the next updates if an error in the previous sending has
*         occurred.
* @param  BleCharPointer pointer to the BleCharTypeDef for the current ble char
* @param  charValOffset The offset of the characteristic
* @param  charValueLen The length of the characteristic
* @param  charValue The pointer to the characteristic
* @retval tBleStatus Status
*/
tBleStatus safe_aci_gatt_update_char_value(BleCharTypeDef *BleCharPointer,
                                           uint8_t charValOffset,
                                           uint8_t charValueLen,
                                           uint8_t *charValue)
{
  tBleStatus ret = BLE_STATUS_INSUFFICIENT_RESOURCES;
  if (breath==0){
    #if (BLUE_CORE != BLUENRG_LP)
      ret = aci_gatt_update_char_value(BleCharPointer->Service_Handle,BleCharPointer->attr_handle,charValOffset,charValueLen,charValue);
    #else /* (BLUE_CORE != BLUENRG_LP) */
      ret = aci_gatt_srv_notify(connection_handle, BleCharPointer->attr_handle+1, GATT_NOTIFICATION, charValueLen, charValue);
    #endif /* (BLUE_CORE != BLUENRG_LP) */
    
    if (ret != (tBleStatus)BLE_STATUS_SUCCESS){
#if (BLE_DEBUG_LEVEL>2)
      BLE_MANAGER_PRINTF("Error: Updating Char handle=%x ret=%x\r\n",BleCharPointer->attr_handle,ret);
#endif
      if(ret==(tBleStatus)BLE_STATUS_INSUFFICIENT_RESOURCES){
#if (BLE_DEBUG_LEVEL>2)
        BLE_MANAGER_PRINTF("Char handle=%x insufficient resources\r\n",BleCharPointer->attr_handle);
#endif
        breath = 1;
      }
    }
  }
  return ret;
}
#endif /* ACC_BLUENRG_CONGESTION */

#ifndef BLE_MANAGER_NO_PARSON
/**
* @brief Parse Ext Configuration Command Type
* @param  uint8_t *hs_command_buffer
* @retval BLE_ExtConfigCommandType
*/
static BLE_ExtConfigCommandType BLE_ExtConfig_ExtractCommandType(uint8_t *hs_command_buffer)
{
  BLE_ExtConfigCommandType ReturnCode = EXT_CONFIG_COM_NOT_VALID;
  
  /* Parse the Json for taking the pointer to the CommandName */
  JSON_Value *tempJSON = json_parse_string( (char *) hs_command_buffer);
  JSON_Object *JSON_ParseHandler = json_value_get_object(tempJSON);
  
  if (json_object_dothas_value(JSON_ParseHandler,"command")) {
    uint8_t SearchCommand=(uint8_t)EXT_CONFIG_COM_READ_COMMAND;
    //Search the Command
    while((ReturnCode == EXT_CONFIG_COM_NOT_VALID) && (SearchCommand<((uint8_t)EXT_CONFIG_COMMAND_NUMBER))) {
      if (strncmp(json_object_dotget_string(JSON_ParseHandler,"command"),StandardExtConfigCommands[SearchCommand].CommandString,strlen(json_object_dotget_string(JSON_ParseHandler,"command"))) == 0) {
        ReturnCode = StandardExtConfigCommands[SearchCommand].CommandType;
      }
      SearchCommand++;
    }
  }
  
  json_value_free(tempJSON);
  return ReturnCode;
}

/**
* @brief Parse Configuration Command Type
* @param  uint8_t *hs_command_buffer
* @retval BLE_CustomCommadResult_t *CommandResult
*/
BLE_CustomCommadResult_t *AskGenericCustomCommands(uint8_t *hs_command_buffer)
{
  BLE_CustomCommadResult_t *CommandResult=NULL;
  
  /* Parse the Json for taking the pointer to the CommandName */
  JSON_Value *tempJSON = json_parse_string( (char *) hs_command_buffer);
  JSON_Object *JSON_ParseHandler = json_value_get_object(tempJSON);
  
  if (json_object_dothas_value(JSON_ParseHandler,"command")) {
    if (strncmp(json_object_dotget_string(JSON_ParseHandler,"command"),StandardExtConfigCommands[EXT_CONFIG_COM_READ_CUSTOM_COMMAND].CommandString,strlen(json_object_dotget_string(JSON_ParseHandler,"command"))) == 0) {
     /* The User has asked a List of Custom Command */
      CommandResult = (BLE_CustomCommadResult_t *) BLE_MallocFunction(sizeof(BLE_CustomCommadResult_t));
      if(CommandResult == NULL) {
        BLE_MANAGER_PRINTF("Error: Mem alloc error: %d@%s\r\n", __LINE__, __FILE__);
      } else {
        CommandResult->CommandType= BLE_CUSTOM_COMMAND_VOID;
        
        CommandResult->CommandName = BLE_MallocFunction(strlen(BLE_MANAGER_READ_CUSTOM_COMMAND) + 1U);
        if((CommandResult->CommandName)==NULL) {
          BLE_MANAGER_PRINTF("Error: Mem alloc error: %d@%s\r\n", __LINE__, __FILE__);
          BLE_FreeFunction(CommandResult);
          CommandResult=NULL;
        } else {
          sprintf((char *) CommandResult->CommandName,"%s",BLE_MANAGER_READ_CUSTOM_COMMAND);
        }
      }
    }
  }
  json_value_free(tempJSON);
  return CommandResult;
}


/**
* @brief  Update Extended Configuration characteristic value (when lenght is <=MaxBLECharLen)
* @param  uint8_t *data string to write
* @param  uint8_t lenght lengt of string to write
* @retval tBleStatus      Status
-*/
static tBleStatus BLE_UpdateExtConf(uint8_t *data,uint8_t length)
{
  tBleStatus ret;
  
  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharExtConfig, 0, length,data);
  
  if (ret != (tBleStatus)BLE_STATUS_SUCCESS){
    if(BLE_StdErr_Service==BLE_SERV_ENABLE){
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Bat Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      BLE_MANAGER_PRINTF("Error: Updating Bat Char\r\n");
    }
  }
  
  return ret;
}
#endif /* BLE_MANAGER_NO_PARSON */

/* Exported functions -----------------------------------------------------------*/

#ifndef BLE_MANAGER_NO_PARSON
/**
* @brief  Update Extended Configuration characteristic value
* @param  uint8_t *data string to write
* @param  uint32_t lenght lengt of string to write
* @retval tBleStatus      Status
*/
tBleStatus BLE_ExtConfiguration_Update(uint8_t *data,uint32_t length)
{
  uint32_t tot_len,j;
  uint8_t *JSON_string_command_wTP;
  uint32_t length_wTP;
  uint32_t len;
  
  if ((length % 19U) == 0U) {
    length_wTP = (length/19U)+length;
  } else {
    length_wTP = (length/19U)+1U+length;
  }
  
  JSON_string_command_wTP = BLE_MallocFunction(sizeof(uint8_t) * length_wTP);
  
  if(JSON_string_command_wTP==NULL) {
    BLE_MANAGER_PRINTF("Error: Mem calloc error [%lu]: %d@%s\r\n",length,__LINE__,__FILE__);
    return BLE_STATUS_ERROR;
  } else {
    tot_len = BLE_Command_TP_Encapsulate(JSON_string_command_wTP, data, length,20);
    
    /* Data are sent as notifications*/
    j = 0;
    while (j < tot_len) {
      len = MIN(20U, (tot_len - j));
      if(BLE_UpdateExtConf(JSON_string_command_wTP+j,(uint8_t)len)!=(tBleStatus)BLE_STATUS_SUCCESS) {
        return BLE_STATUS_ERROR;
      }
      BLE_MANAGER_DELAY(20);
      j += len;
    }
    BLE_FreeFunction(JSON_string_command_wTP);
    return BLE_STATUS_SUCCESS;
  }
}
#endif /* BLE_MANAGER_NO_PARSON */

/**
* @brief Each time BLE FW stack raises the error code @ref ble_status_insufficient_resources (0x64),
the @ref aci_gatt_tx_pool_available_event event is generated as soon as the available buffer size 
is greater than maximum ATT MTU (on stack versions below v2.1 this event is generated when at least 2 packets
with MTU of 23 bytes are available).
* @param Connection_Handle Connection handle related to the request
* @param Available_Buffers Not used.
* @retval None
*/
void aci_gatt_tx_pool_available_event(uint16_t Connection_Handle,
                                      uint16_t Available_Buffers)
{
#if (BLE_DEBUG_LEVEL>1)
  BLE_MANAGER_PRINTF("aci_gatt_tx_pool_available_event\r\n");
#endif
  
#ifdef ACC_BLUENRG_CONGESTION
  breath=0;
#endif /* ACC_BLUENRG_CONGESTION */
  
  if(CustomAciGattTxPoolAvailableEvent != NULL) {
    CustomAciGattTxPoolAvailableEvent();
  }
}

/* @brief  Update the value of a characteristic
* @param  BleCharPointer pointer to the BleCharTypeDef for the current ble char
* @param  charValOffset The offset of the characteristic
* @param  charValueLen The length of the characteristic
* @param  charValue The pointer to the characteristic
* @retval tBleStatus Status
*/
tBleStatus aci_gatt_update_char_value_wrapper(BleCharTypeDef *BleCharPointer,
                                              uint8_t charValOffset,
                                              uint8_t charValueLen,
                                              uint8_t *charValue)
{
  tBleStatus ret = BLE_STATUS_INSUFFICIENT_RESOURCES;
  #if (BLUE_CORE != BLUENRG_LP)
    ret = aci_gatt_update_char_value(BleCharPointer->Service_Handle,BleCharPointer->attr_handle,charValOffset,charValueLen,charValue);
  #else /* (BLUE_CORE != BLUENRG_LP) */
    ret = aci_gatt_srv_notify(connection_handle, BleCharPointer->attr_handle+1, GATT_NOTIFICATION, charValueLen, charValue);
  #endif /* (BLUE_CORE != BLUENRG_LP) */
        
  if (ret != (tBleStatus)BLE_STATUS_SUCCESS){
#if (BLE_DEBUG_LEVEL>2)
    BLE_MANAGER_PRINTF("Error: Updating Char handle=%x ret=%x\r\n",BleCharPointer->attr_handle,ret);
#endif
  }
  return ret;
}

/**
* @brief  Update Stderr characteristic value
* @param  uint8_t *data string to write
* @param  uint8_t lenght lengt of string to write
* @retval tBleStatus      Status
*/
tBleStatus Stderr_Update(uint8_t *data,uint8_t length)
{
  uint8_t Offset;
  uint8_t DataToSend;
  /* Split the code in Chunks */
  /* First Chunk */
  DataToSend = (length>MaxBleCharStdErrLen) ?  MaxBleCharStdErrLen : length;
  
  /* keep a copy */
  memcpy(LastStderrBuffer,data,DataToSend);
  LastStderrLen = DataToSend;
  
  if(UpdateTermStdErr(data,DataToSend)!=(tBleStatus)BLE_STATUS_SUCCESS) {
    return BLE_STATUS_ERROR;
  }
  
  /* Following Chunks if necessary */
  Offset = MaxBleCharStdErrLen;
  for(; Offset<length; Offset +=MaxBleCharStdErrLen){
    /* Add a Delay respect previous chunk */
    BLE_MANAGER_DELAY(20);
    
    DataToSend = (length-Offset);
    DataToSend = (DataToSend>MaxBleCharStdErrLen) ?  MaxBleCharStdErrLen : DataToSend;
    
    /* keep a copy */
    memcpy(LastStderrBuffer,data+Offset,DataToSend);
    LastStderrLen = DataToSend;
    
    if(UpdateTermStdErr(data+Offset,DataToSend)!=(tBleStatus)BLE_STATUS_SUCCESS) {
      return BLE_STATUS_ERROR;
    }
  }
  return BLE_STATUS_SUCCESS;
}

/**
* @brief  
* @param  uint8_t* buffer
* @param  uint8_t len
* @retval tBleStatus   Status
*/
tBleStatus BLE_StdOutSendBuffer(uint8_t* buffer, uint8_t len)
{
  tBleStatus ret;
  
  #if (BLUE_CORE != BLUENRG_LP)
    ret = aci_gatt_update_char_value(BleCharStdOut.Service_Handle, BleCharStdOut.attr_handle, 0, len, buffer);
  #else /* (BLUE_CORE != BLUENRG_LP) */
    ret = aci_gatt_srv_notify(connection_handle, BleCharStdOut.attr_handle+1, GATT_NOTIFICATION, len, buffer);
  #endif /* (BLUE_CORE != BLUENRG_LP) */
  
  return ret;
}

/**
* @brief  Update Terminal characteristic value
* @param  uint8_t *data string to write
* @param  uint8_t lenght lengt of string to write
* @retval tBleStatus      Status
*/
tBleStatus Term_Update(uint8_t *data,uint8_t length)
{
  uint8_t   Offset;
  uint8_t   DataToSend;
  
  /* Split the code in Chunks */
  /* First Chunk */
  DataToSend = (length>MaxBleCharStdOutLen) ?  MaxBleCharStdOutLen : length;
  
  /* keep a copy */
  memcpy(LastTermBuffer,data,DataToSend);
  LastTermLen = DataToSend;
  
  if(UpdateTermStdOut(data,DataToSend)!=(tBleStatus)BLE_STATUS_SUCCESS) {
    return BLE_STATUS_ERROR;
  }
  
  /* Following Chunks if necessary */
  Offset = MaxBleCharStdOutLen;
  for(; Offset<length; Offset +=MaxBleCharStdOutLen){
    
    /* Add a Delay respect previous chunk */
    BLE_MANAGER_DELAY(20);
    
    DataToSend = (length-Offset);
    DataToSend = (DataToSend>MaxBleCharStdOutLen) ?  MaxBleCharStdOutLen : DataToSend;
    
    /* keep a copy */
    memcpy(LastTermBuffer,data+Offset,DataToSend);
    LastTermLen = DataToSend;
    
    if(UpdateTermStdOut(data+Offset,DataToSend)!=(tBleStatus)BLE_STATUS_SUCCESS) {
      return BLE_STATUS_ERROR;
    }
  }
  return BLE_STATUS_SUCCESS;
}

/* @brief  Send a BLE notification for answering to a configuration command for Accelerometer events
* @param  uint32_t Feature Feature type
* @param  uint8_t Command Replay to this Command
* @param  uint8_t data result to send back
* @retval tBleStatus Status
*/
tBleStatus Config_Update(uint32_t Feature,uint8_t Command,uint8_t data)
{
  uint8_t buff[2+4+1+1];
  
  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  STORE_BE_32(buff+2,Feature);
  buff[6] = Command;
  buff[7] = data;
  
  if (ACI_GATT_UPDATE_CHAR_VALUE(&BleCharConfig, 0, 8 , buff) != (tBleStatus)BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("Error: Updating Configuration Char\r\n");
    return BLE_STATUS_ERROR;
  } 
  return BLE_STATUS_SUCCESS;
  
}

/* @brief  Send a BLE notification for answering to a configuration command for Accelerometer events
* @param  uint32_t Feature Feature type
* @param  uint8_t Command Replay to this Command
* @param  uint32_t data result to send back
* @retval tBleStatus Status
*/
tBleStatus Config_Update_32(uint32_t Feature,uint8_t Command,uint32_t data)
{
  uint8_t buff[2+4+1+4];
  
  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  STORE_BE_32(buff+2,Feature);
  buff[6] = Command;
  buff[7]  = (uint8_t)((data       ) & 0xFFU);
  buff[8]  = (uint8_t)((data >>  8U) & 0xFFU);
  buff[9]  = (uint8_t)((data >> 16U) & 0xFFU);
  buff[10] = (uint8_t)((data >> 24U) & 0xFFU);
  
  if (ACI_GATT_UPDATE_CHAR_VALUE(&BleCharConfig, 0, 11 , buff) != (tBleStatus)BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("Error: Updating Configuration Char\r\n");
    return BLE_STATUS_ERROR;
  } 
  return BLE_STATUS_SUCCESS;
}


#if (BLUE_CORE != BLUENRG_LP)
/**
* @brief  Update the Advertise Data
* @param  None
* @retval None
*/
void updateAdvData()
{
  /* Filling Manufacter Advertise data */
  manuf_data[0 ] = 8U;
  manuf_data[1 ] = 0x09U;
  manuf_data[2 ] = (uint8_t)BLE_StackValue.BoardName[0];/* Complete Name */
  manuf_data[3 ] = (uint8_t)BLE_StackValue.BoardName[1];
  manuf_data[4 ] = (uint8_t)BLE_StackValue.BoardName[2];
  manuf_data[5 ] = (uint8_t)BLE_StackValue.BoardName[3];
  manuf_data[6 ] = (uint8_t)BLE_StackValue.BoardName[4];
  manuf_data[7 ] = (uint8_t)BLE_StackValue.BoardName[5];
  manuf_data[8 ] = (uint8_t)BLE_StackValue.BoardName[6];           
  manuf_data[9 ] = 15U;
  manuf_data[10] = 0xFFU;
  manuf_data[11] = 0x30U;/* STM Manufacter AD */
  manuf_data[12] = 0x00U;
#ifdef BLE_MANAGER_SDKV2
  manuf_data[13] = 0x02U;
#else /* BLE_MANAGER_SDKV2 */
  manuf_data[13] = 0x01U;
#endif /* BLE_MANAGER_SDKV2 */
  manuf_data[14] = BLE_MANAGER_USED_PLATFORM; /* BoardType */
  manuf_data[15] = 0x00U;
  manuf_data[16] = 0x00U;
  manuf_data[17] = 0x00U;
  manuf_data[18] = 0x00U;
  manuf_data[19] = BLE_StackValue.BleMacAddress[5]; /* BLE MAC start */
  manuf_data[20] = BLE_StackValue.BleMacAddress[4];
  manuf_data[21] = BLE_StackValue.BleMacAddress[3];
  manuf_data[22] = BLE_StackValue.BleMacAddress[2];
  manuf_data[23] = BLE_StackValue.BleMacAddress[1];
  manuf_data[24] = BLE_StackValue.BleMacAddress[0]; /* BLE MAC stop */
  
  /* Set the Custom BLE Advertise Data */
  BLE_SetCustomAdvertiseData(manuf_data);
  
  if(CustomSetConnectable!=NULL){
    CustomSetConnectable(manuf_data);
  }
  
  /* Send Advertising data */
  tBleStatus RetStatus = aci_gap_update_adv_data(BLE_MANAGER_ADVERTISE_DATA_LENGHT, manuf_data);
  if(RetStatus != (tBleStatus)BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("Error: aci_gap_update_adv_data [%x]\r\n",RetStatus);
  } else {
    BLE_MANAGER_PRINTF("aci_gap_update_adv_data OK\r\n");
  } 
}

/**
* @brief  Puts the device in connectable mode.
* @param  None
* @retval None
*/
void setConnectable(void)
{
#if (BLUE_CORE == BLUENRG_MS)
  char local_name[8] = {AD_TYPE_COMPLETE_LOCAL_NAME,
#else /* (BLUE_CORE == BLUENRG_MS) */
  uint8_t local_name[8] = {AD_TYPE_COMPLETE_LOCAL_NAME,
#endif /* (BLUE_CORE == BLUENRG_MS) */
  BLE_StackValue.BoardName[0],
  BLE_StackValue.BoardName[1],
  BLE_StackValue.BoardName[2],
  BLE_StackValue.BoardName[3],
  BLE_StackValue.BoardName[4],
  BLE_StackValue.BoardName[5],
  BLE_StackValue.BoardName[6]};
  tBleStatus RetStatus= BLE_STATUS_SUCCESS; 
  
  /* disable scan response */
  RetStatus = hci_le_set_scan_response_data(0U,NULL);
  if(RetStatus !=(tBleStatus)BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("Error: hci_le_set_scan_response_data [%x]\r\n",RetStatus);
    goto EndLabel;
  }
  
  /* Set the board discoverable */
  if(BLE_StackValue.AdvertisingFilter == ((uint8_t)NO_WHITE_LIST_USE)) {
    RetStatus = aci_gap_set_discoverable(ADV_IND, BLE_StackValue.AdvIntervalMin, BLE_StackValue.AdvIntervalMax,
                                         BLE_StackValue.OwnAddressType,
                                         BLE_StackValue.AdvertisingFilter,
                                         (uint8_t)(sizeof(local_name)), local_name, 0, NULL, 0, 0);
    if(RetStatus != (tBleStatus)BLE_STATUS_SUCCESS) {
      BLE_MANAGER_PRINTF("Error: aci_gap_set_discoverable [%x] Filter=%x\r\n",RetStatus,BLE_StackValue.AdvertisingFilter);
      goto EndLabel;
    } else {
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("aci_gap_set_discoverable OK Filter=%x\r\n",BLE_StackValue.AdvertisingFilter);
#endif
    }
  } else {
    /* Advertising filter is enabled: enter in undirected connectable mode in order to use the advertising filter on bonded device */
#if (BLUE_CORE == BLUENRG_MS)
	RetStatus = aci_gap_set_undirected_connectable(BLE_StackValue.OwnAddressType, BLE_StackValue.AdvertisingFilter);
#else /* (BLUE_CORE == BLUENRG_MS) */
    RetStatus = aci_gap_set_undirected_connectable(0,0,BLE_StackValue.OwnAddressType, BLE_StackValue.AdvertisingFilter);
#endif /* (BLUE_CORE == BLUENRG_MS) */
    if(RetStatus != (tBleStatus)BLE_STATUS_SUCCESS) {
      BLE_MANAGER_PRINTF("Error: aci_gap_set_undirected_connectable [%x] Filter=%x\r\n",RetStatus,BLE_StackValue.AdvertisingFilter);
      goto EndLabel;
    } else {
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("aci_gap_set_undirected_connectable OK Filter=%x\r\n",BLE_StackValue.AdvertisingFilter);
#endif
    }
  }
  
  updateAdvData();
  
EndLabel:
  return;
}
#else /* (BLUE_CORE != BLUENRG_LP) */
/**
* @brief  Puts the device in connectable mode.
* @param  None
* @retval None
*/
void setConnectable(void)
{
  tBleStatus ret;
  Advertising_Set_Parameters_t Advertising_Set_Parameters[1];

  manuf_data[0 ]=0x02;
  manuf_data[1 ]=AD_TYPE_FLAGS;
  manuf_data[2 ]=FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE|FLAG_BIT_BR_EDR_NOT_SUPPORTED;
  manuf_data[3 ]=8;
  manuf_data[4 ]=0x09;
  manuf_data[5 ]=BLE_StackValue.BoardName[0];/* Complete Name */
  manuf_data[6 ]=BLE_StackValue.BoardName[1];
  manuf_data[7 ]=BLE_StackValue.BoardName[2];
  manuf_data[8 ]=BLE_StackValue.BoardName[3];
  manuf_data[9 ]=BLE_StackValue.BoardName[4];
  manuf_data[10]=BLE_StackValue.BoardName[5];
  manuf_data[11]=BLE_StackValue.BoardName[6];
  manuf_data[12]=15;
  manuf_data[13]=0xFF;
  manuf_data[14]=0x30;
  manuf_data[15]=0x00; /* STM Manufacter AD */
  manuf_data[16]=0x02;  /* SDK version */
  manuf_data[17]=BLE_MANAGER_USED_PLATFORM; /* BoardType*/
  manuf_data[18]=0x00; /* Fw ID*/
  manuf_data[19]=0x00; /* Second Custom Byte -> Not Used */
  manuf_data[20]=0x00; /* Third  Custom Byte -> Not Used */
  manuf_data[21]=0x00; /* Fourth Custom Byte -> Not Used */
  manuf_data[22]=BLE_StackValue.BleMacAddress[5]; /* BLE MAC start - MSB first - */
  manuf_data[23]=BLE_StackValue.BleMacAddress[4];
  manuf_data[24]=BLE_StackValue.BleMacAddress[3];
  manuf_data[25]=BLE_StackValue.BleMacAddress[2];
  manuf_data[26]=BLE_StackValue.BleMacAddress[1];
  manuf_data[27]=BLE_StackValue.BleMacAddress[0];  /* BLE MAC stop */

  /* Set the Custom BLE Advertise Data */
  BLE_SetCustomAdvertiseData(manuf_data);

  if(CustomSetConnectable!=NULL){
    CustomSetConnectable(manuf_data);
  }

  ret = aci_gap_set_advertising_configuration(0, GAP_MODE_GENERAL_DISCOVERABLE,
                                              ADV_PROP_CONNECTABLE | ADV_PROP_SCANNABLE | ADV_PROP_LEGACY,
                                              ADV_INTERV_MIN,
                                              ADV_INTERV_MAX,
                                              ADV_CH_ALL,
                                              STATIC_RANDOM_ADDR, NULL,
                                              ADV_NO_WHITE_LIST_USE,
                                              0, /* 0 dBm */
                                              LE_1M_PHY, /* Primary advertising PHY */
                                              0, /* 0 skips */
                                              LE_1M_PHY, /* Secondary advertising PHY. Not used with legacy advertising. */
                                              0, /* SID */
                                              0 /* No scan request notifications */);

  if(ret != BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_configuration failed: 0x%02x\r\n", ret);
    goto EndLabel;
  } else {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_configuration\r\n");
  }

  ret = aci_gap_set_advertising_data_nwk(0, ADV_COMPLETE_DATA, BLE_MANAGER_ADVERTISE_DATA_LENGHT, manuf_data);
  if(ret != BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_data_nwk failed: 0x%02x\r\n", ret);
    goto EndLabel;
  } else {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_data_nwk\r\n");
  }

  Advertising_Set_Parameters[0].Advertising_Handle = 0;
  Advertising_Set_Parameters[0].Duration = 0;
  Advertising_Set_Parameters[0].Max_Extended_Advertising_Events = 0;

  /* enable advertising */
  ret = aci_gap_set_advertising_enable(ENABLE, 1, Advertising_Set_Parameters);
  if(ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_enable failed: 0x%02x\r\n",ret);
  } else {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_enable\r\n");
  }
  
EndLabel:
  return;
}

/**
* @brief  Update Advertise Data
* @param  None
* @retval None
*/

void updateAdvData()
{
  tBleStatus ret;
  /* Set the Custom BLE Advertise Data */
  BLE_SetCustomAdvertiseData(manuf_data);

  ret = aci_gap_set_advertising_data_nwk(0, ADV_COMPLETE_DATA, BLE_MANAGER_ADVERTISE_DATA_LENGHT, manuf_data);
  if(ret != BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_data_nwk failed: 0x%02x\r\n", ret);
  } else {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_data_nwk\r\n");
  }

  return;
}
#endif /* (BLUE_CORE != BLUENRG_LP) */

#if (BLUE_CORE != BLUENRG_LP)
/**
* @brief  Exits the device from connectable mode.
* @param  None
* @retval None
*/
void setNotConnectable(void)
{
  aci_gap_set_non_discoverable();
}
#endif /* (BLUE_CORE != BLUENRG_LP) */

/**
* @brief  Added BLE service
* @param  BleCharTypeDef *BleChar: Data structure pointer for BLE service
* @retval 1 in case of success
*/
int32_t BleManagerAddChar(BleCharTypeDef *BleChar)
{
  int32_t retValue=0;
  
  if(BleChar != NULL) {
    if(UsedBleChars<(uint8_t)BLE_MANAGER_MAX_ALLOCABLE_CHARS) {
      BleCharsArray[UsedBleChars] = BleChar;
      UsedBleChars++;
      retValue=1;
    }
  }
  
  return retValue;
}

#if (BLUE_CORE != BLUE_WB)
/**
* @brief  ResetBleManager
* @param  None
* @retval None
*/
void ResetBleManager(void)
{
  BLE_MANAGER_PRINTF("\r\nReset BleManager\r\n\r\n");
  HAL_GPIO_WritePin(HCI_TL_RST_PORT, HCI_TL_RST_PIN, GPIO_PIN_RESET);
  BLE_MANAGER_DELAY(300);
  HAL_GPIO_WritePin(HCI_TL_RST_PORT, HCI_TL_RST_PIN, GPIO_PIN_SET);
  BLE_MANAGER_DELAY(300);
}
#endif /* (BLUE_CORE != BLUE_WB) */

/**
* @brief  ResetBleManagerCallbackFunctionPointer
* @param  None
* @retval None
*/
static void ResetBleManagerCallbackFunctionPointer(void)
{
  /**************** Bluetooth Comunication *************************/
  CustomPairingCompleted=NULL;
  CustomMTUExchangeRespEvent = NULL;
  CustomSetConnectable=NULL;
  CustomConnectionCompleted=NULL;
  CustomDisconnectionCompleted=NULL;
  CustomAciGattTxPoolAvailableEvent=NULL;
  CustomHardwareErrorEventHandler=NULL;

  /**************** Debug Console *************************/
  CustomDebugConsoleParsingCallback=NULL;

  /******************* Config Char *************************/
  CustomAttrModConfigCallback=NULL;
  CustomWriteRequestConfigCallback=NULL;

#ifndef BLE_MANAGER_NO_PARSON
  /**************** Extended Configuration *************************/
  //For Reboot on DFU Command
  CustomExtConfigRebootOnDFUModeCommandCallback=NULL;
  //For Power off Command
  CustomExtConfigPowerOffCommandCallback=NULL;
  //For Set board Name Command
  CustomExtConfigSetNameCommandCallback=NULL;
  //For Set Time Command
  CustomExtConfigSetTimeCommandCallback=NULL;
  //For Set Date Command
  CustomExtConfigSetDateCommandCallback=NULL;
  //For Set Wi-Fi Command
  CustomExtConfigSetWiFiCommandCallback=NULL;
  //For Change Secure PIN Command
  CustomExtConfigChangePinCommandCallback=NULL;
  //For Clear Secure Data Base Command
  CustomExtConfigClearDBCommandCallback=NULL;
  //For Reading the Flash Banks Fw Ids
  CustomExtConfigReadBanksFwIdCommandCallback=NULL;
  //For Swapping the Flash Banks
  CustomExtConfigBanksSwapCommandCallback=NULL;
  //For UID Command
  CustomExtConfigUidCommandCallback=NULL;
  //For Info Command
  CustomExtConfigInfoCommandCallback=NULL;
  //For Help Command
  CustomExtConfigHelpCommandCallback=NULL;
  //For PowerStatus Command
  CustomExtConfigPowerStatusCommandCallback=NULL;
  //For VersionFw Command
  CustomExtConfigVersionFwCommandCallback=NULL;
  //For Custom Command
  CustomExtConfigReadCustomCommandsCallback=NULL;
  CustomExtConfigCustomCommandCallback=NULL;
  //For Read Certificate Command
  CustomExtConfigReadCertCommandCallback=NULL;
  //For Set Certificate Commnad
  CustomExtConfigSetCertCommandCallback=NULL;
  //For Sensor Configuration
  CustomExtConfigReadSensorsConfigCommandsCallback=NULL;
  CustomExtConfigSetSensorsConfigCommandsCallback=NULL;
#endif /* BLE_MANAGER_NO_PARSON */
}

/**
* @brief  Init Ble Manager 
* @param  None
* @retval tBleStatus Status
*/
tBleStatus InitBleManager(void)
{
  tBleStatus ret= BLE_STATUS_SUCCESS;
  
  BLE_Conf_Service = BLE_SERV_NOT_ENABLE;
  BLE_StdTerm_Service = BLE_SERV_NOT_ENABLE;
  BLE_StdErr_Service = BLE_SERV_NOT_ENABLE;
  BLE_ExtConf_Service = BLE_SERV_NOT_ENABLE;

  UsedBleChars =0;
  UsedStandardBleChars = 0;
  connection_handle = 0;
  set_connectable = FALSE;
  MaxBleCharStdOutLen = DEFAULT_MAX_STDOUT_CHAR_LEN;
  MaxBleCharStdErrLen = DEFAULT_MAX_STDERR_CHAR_LEN;
 
#if (BLUE_CORE != BLUE_WB)
  /* BLE stack initialization */
  ret = InitBleManager_BLE_Stack();
#endif /* (BLUE_CORE != BLUE_WB) */
  
  ResetBleManagerCallbackFunctionPointer();
#ifndef BLE_MANAGER_NO_PARSON
  ClearCustomCommandsList();
#endif /* BLE_MANAGER_NO_PARSON */
  
  if(ret==(tBleStatus)BLE_STATUS_SUCCESS) {
    /* Ble Manager services initialization */
    ret = InitBleManagerServices();
  }
  
  set_connectable=TRUE;
  
  return ret;
}

#if (BLUE_CORE == BLUENRG_MS)
/**
 * @brief  This function is called whenever there is an ACI event to be processed.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  void *pckt Pointer to the ACI packet
 * @retval None
 */
void HCI_Event_CB(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  
  if(hci_pckt->type != (uint8_t)HCI_EVENT_PKT) {
    return;
  }
 
  switch(event_pckt->evt){
    
  case EVT_DISCONN_COMPLETE:
    {
      evt_disconn_complete *dc = (void *)event_pckt->data;
      hci_disconnection_complete_event(dc->status,
                                       dc->handle,
                                       dc->reason);
      break;
    }
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      switch(evt->subevent){
      case EVT_LE_CONN_COMPLETE:
        {
          evt_le_connection_complete *cc = (void *)evt->data;
          hci_le_connection_complete_event(cc->status,
                                           cc->handle,
                                           cc->role,
                                           cc->peer_bdaddr_type,
                                           cc->peer_bdaddr,
                                           cc->interval,
                                           cc->interval,
                                           cc->supervision_timeout,
                                           cc->master_clock_accuracy);
          break;
        }
      case EVT_LE_CONN_UPDATE_COMPLETE:
        {
          evt_le_connection_update_complete * con_update = (evt_le_connection_update_complete *) evt->data;
          #if (BLE_DEBUG_LEVEL>1)
            BLE_MANAGER_PRINTF("EVT_LE_CONN_UPDATE_COMPLETE status=%d\r\n",con_update->status);
          #else
            UNUSED(con_update);
          #endif
          break;
        }
      }
      break;
    }
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      
      switch(blue_evt->ecode){
      case EVT_BLUE_GATT_READ_PERMIT_REQ:
        {          
          evt_gatt_read_permit_req *pr = (void*)blue_evt->data;
          aci_gatt_read_permit_req_event(pr->conn_handle,
                                         pr->attr_handle,
                                         pr->offset);
          break;
        }
      case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
        {
          #if (BLE_DEBUG_LEVEL>2)
            BLE_MANAGER_PRINTF("EVT_BLUE_GATT_ATTRIBUTE_MODIFIED\r\n" );
          #endif
          evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
          aci_gatt_attribute_modified_event(evt->conn_handle,
                                            evt->attr_handle,
                                            evt->offset,
                                            evt->data_length,
                                            evt->att_data);
          break;
        }
      case EVT_BLUE_GAP_AUTHORIZATION_REQUEST:
          #if (BLE_DEBUG_LEVEL>2)
            BLE_MANAGER_PRINTF("EVT_BLUE_GAP_AUTHORIZATION_REQUEST\r\n" );
          #endif
        break;
      case EVT_BLUE_GAP_PASS_KEY_REQUEST:
        {
          #if (BLE_DEBUG_LEVEL>2)
            BLE_MANAGER_PRINTF("EVT_BLUE_GAP_PASS_KEY_REQUEST\r\n" );
          #endif
        
          evt_gap_pass_key_req *kr = (void*)blue_evt->data;
          aci_gap_pass_key_req_event(kr->conn_handle);
          break;
        }
      case EVT_BLUE_GAP_PAIRING_CMPLT:
        {
          evt_gap_pairing_cmplt *pairing = (evt_gap_pairing_cmplt *)blue_evt->data;
          #if (BLE_DEBUG_LEVEL>2)
            BLE_MANAGER_PRINTF("EVT_BLUE_GAP_PAIRING_CMPLT\r\n" );
          #endif
          aci_gap_pairing_complete_event(pairing->conn_handle,pairing->status, 0); 
          break;
        }
      case EVT_BLUE_GAP_SLAVE_SECURITY_INITIATED:
        BLE_MANAGER_PRINTF("EVT_BLUE_GAP_SLAVE_SECURITY_INITIATED\r\n");
        break;
      case EVT_BLUE_GAP_BOND_LOST:
        {
          tBleStatus ret;
          evt_gatt_procedure_timeout *evt = (evt_gatt_procedure_timeout*)blue_evt->data;
          #if (BLE_DEBUG_LEVEL>2)
            BLE_MANAGER_PRINTF("EVT_BLUE_GAP_BOND_LOST\r\n" );
          #endif
          ret = aci_gap_allow_rebond_IDB05A1(evt->conn_handle);
          if (ret != (uint8_t)BLE_STATUS_SUCCESS) {
            BLE_MANAGER_PRINTF("\r\nGAP allow Rebound failed\r\n");
          }
          break;
        }
      case EVT_BLUE_ATT_EXCHANGE_MTU_RESP:
        {
          #if (BLE_DEBUG_LEVEL>2)
            BLE_MANAGER_PRINTF("EVT_BLUE_ATT_EXCHANGE_MTU_RESP\r\n" );
          #endif
          evt_att_exchange_mtu_resp *mtu = (evt_att_exchange_mtu_resp*)blue_evt->data;
          aci_att_exchange_mtu_resp_event(mtu->conn_handle,
                                          mtu->server_rx_mtu);
          
          break;
        }
      }
      break;
    }
  case EVT_ENCRYPT_CHANGE:
    {
      evt_encrypt_change *encrypt = (evt_encrypt_change *)event_pckt->data;
      UNUSED(encrypt);
      #if (BLE_DEBUG_LEVEL>1)
        BLE_MANAGER_PRINTF("EVT_ENCRYPT_CHANGE\r\n");
      #endif
      break;
    }
  }
}

/** @brief Initialize the BLE Stack
* @param None
* @retval tBleStatus
*/
static tBleStatus InitBleManager_BLE_Stack(void)
{
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  tBleStatus ret;
  uint8_t data_len_out;
  uint8_t random_number[8];
  
  /* Initialize the BlueNRG HCI */
  hci_init(HCI_Event_CB, NULL);
  
  /* we will let the BLE chip to use its Random MAC address */
  ret = aci_hal_read_config_data(CONFIG_DATA_RANDOM_ADDRESS, 6, &data_len_out, BLE_StackValue.BleMacAddress);
  
  if(ret != (tBleStatus)BLE_STATUS_SUCCESS){
    BLE_MANAGER_PRINTF("\r\nReading  Random BD_ADDR failed\r\n");
    goto fail;
  }
  
  if ((BLE_StackValue.BleMacAddress[5] & 0xC0U) != 0xC0U) {
    BLE_MANAGER_PRINTF("\r\nStatic Random address not well formed.\r\n");
    goto fail;
  }
  
  /* Generate Random Key at every boot */
  if(BLE_StackValue.EnableRandomSecurePIN) {
    BLE_StackValue.SecurePIN = 99999;
    
    /* get a random number from BlueNRG-1 */
    if(hci_le_rand(random_number) != BLE_STATUS_SUCCESS) {
      BLE_MANAGER_PRINTF("hci_le_rand() call failed\r\n");
    }
    
    /* setup random_key with random number */
    for (uint8_t i=0; i<8U; i++) {
      BLE_StackValue.SecurePIN += (435U*((uint32_t)random_number[i]));
    }
    
    /* Control section because we need 6 digits */
    if (BLE_StackValue.SecurePIN <99999U) {
      BLE_StackValue.SecurePIN += 100000U;
    }
  }
  
  ret = aci_hal_write_config_data(BLE_StackValue.ConfigValueOffsets,
                                  BLE_StackValue.ConfigValuelength,
                                  BLE_StackValue.BleMacAddress);
  
  /* Sw reset of the device */
  hci_reset();
  
  ret = aci_gatt_init();
  if(ret != (tBleStatus)BLE_STATUS_SUCCESS){
    BLE_MANAGER_PRINTF("\r\nGATT_Init failed\r\n");
    goto fail;
  }
  
  aci_gap_clear_security_database();
  
  ret = aci_gap_init_IDB05A1(BLE_StackValue.GAP_Roles, 0, (uint8_t) strlen(BLE_StackValue.BoardName), &service_handle, &dev_name_char_handle, &appearance_char_handle);
  
  if(ret != (tBleStatus)BLE_STATUS_SUCCESS){
    BLE_MANAGER_PRINTF("\r\nGAP_Init failed\r\n");
    goto fail;
  }
  
  ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
                                   (uint8_t) strlen(BLE_StackValue.BoardName), (uint8_t *)BLE_StackValue.BoardName);
  
  if(ret != (tBleStatus)BLE_STATUS_SUCCESS){
    BLE_MANAGER_PRINTF("\r\naci_gatt_update_char_value failed\r\n");
    goto fail;
  }
  
  if(BLE_StackValue.EnableSecureConnection) {
    /* Set the I/O capability  Otherwise the Smartphone will propose a Pin
    * that will be acepted without any control */
    if(aci_gap_set_io_capability(BLE_StackValue.IO_capabilities)==(tBleStatus)BLE_STATUS_SUCCESS) {
      BLE_MANAGER_PRINTF("I/O Capability Configurated\r\n");
    } else {
      BLE_MANAGER_PRINTF("Error Setting I/O Capability\r\n");
    }
    
    if(BLE_StackValue.EnableRandomSecurePIN) {
      ret = aci_gap_set_auth_requirement(BLE_StackValue.MITM_ProtectionRequirements,
                                         BLE_StackValue.Out_Of_Band_EnableData,
                                         NULL,
                                         7, 
                                         16,
                                         DONOT_USE_FIXED_PIN_FOR_PAIRING,
                                         BLE_StackValue.SecurePIN,
                                         BLE_StackValue.AuthenticationRequirements);
    } else {
      ret = aci_gap_set_auth_requirement(BLE_StackValue.MITM_ProtectionRequirements,
                                         BLE_StackValue.Out_Of_Band_EnableData,
                                         NULL,
                                         7, 
                                         16,
                                         USE_FIXED_PIN_FOR_PAIRING,
                                         BLE_StackValue.SecurePIN,
                                         BLE_StackValue.AuthenticationRequirements);
    }
    
  } else {
      ret = aci_gap_set_auth_requirement(BLE_StackValue.MITM_ProtectionRequirements,
                                         BLE_StackValue.Out_Of_Band_EnableData,
                                         NULL,
                                         7, 
                                         16,
                                         USE_FIXED_PIN_FOR_PAIRING,
                                         BLE_StackValue.SecurePIN,
                                         BLE_StackValue.AuthenticationRequirements);
  }
  
  if (ret != (tBleStatus)BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("\r\nGAP setting Authentication failed\r\n");
    goto fail;
  }
  
  BLE_MANAGER_PRINTF("\r\nSERVER: BLE Stack Initialized \r\n"
                     "\t\tBoardName= %s\r\n"
                       "\t\tBoardMAC = %x:%x:%x:%x:%x:%x\r\n",
                       BLE_StackValue.BoardName,
                       BLE_StackValue.BleMacAddress[5],
                       BLE_StackValue.BleMacAddress[4],
                       BLE_StackValue.BleMacAddress[3],
                       BLE_StackValue.BleMacAddress[2],
                       BLE_StackValue.BleMacAddress[1],
                       BLE_StackValue.BleMacAddress[0]);
  
  if(BLE_StackValue.EnableSecureConnection) {
    BLE_MANAGER_PRINTF("\t-->ONLY SECURE CONNECTION<--\r\n");
    
    if(BLE_StackValue.EnableRandomSecurePIN) {
      BLE_MANAGER_PRINTF("\t\tRandom Key = %ld\r\n",BLE_StackValue.SecurePIN);
    } else {
      BLE_MANAGER_PRINTF("\t\tFixed  Key = %ld\r\n",BLE_StackValue.SecurePIN);
    }
  }
  
  /* Set output power level */
  aci_hal_set_tx_power_level(BLE_StackValue.EnableHighPowerMode,
                             BLE_StackValue.PowerAmplifierOutputLevel); 
  
fail:
  return ret;
}

#else /* (BLUE_CORE == BLUENRG_MS) */

#if (BLUE_CORE != BLUENRG_LP)
#if (BLUE_CORE != BLUE_WB)
/** @brief HCI Transport layer user function
* @param void *pData pointer to HCI event data
* @retval None
*/
static void APP_UserEvtRx(void *pData)
{
  uint32_t i;
  
  hci_spi_pckt *hci_pckt = (hci_spi_pckt *)pData;
  
  if(hci_pckt->type == (uint8_t)HCI_EVENT_PKT) {
    hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
    
    if(event_pckt->evt == (uint8_t)EVT_LE_META_EVENT) {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      for (i = 0; i < (sizeof(hci_le_meta_events_table)/sizeof(hci_le_meta_events_table_type)); i++) {
        if (evt->subevent == hci_le_meta_events_table[i].evt_code) {
          hci_le_meta_events_table[i].process((void *)evt->data);
        }
      }
    } else if(event_pckt->evt == (uint8_t)EVT_VENDOR) {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;        
      
      for (i = 0; i < (sizeof(hci_vendor_specific_events_table)/sizeof(hci_vendor_specific_events_table_type)); i++) {
        if (blue_evt->ecode == hci_vendor_specific_events_table[i].evt_code) {
          hci_vendor_specific_events_table[i].process((void *)blue_evt->data);
        }
      }
    } else {
      for (i = 0; i < (sizeof(hci_events_table)/sizeof(hci_events_table_type)); i++) {
        if (event_pckt->evt == hci_events_table[i].evt_code) {
          hci_events_table[i].process((void *)event_pckt->data);
        }
      }
    }
  }
}
#endif /* (BLUE_CORE != BLUE_WB) */

#else /* (BLUE_CORE != BLUENRG_LP) */
/**
 * @brief  Callback processing the ACI events
 * @note   Inside this function each event must be identified and correctly
 *         parsed
 * @param  void* Pointer to the ACI packet
 * @retval None
 */
static void APP_UserEvtRx(void *pData)
{
  uint32_t i;

  hci_spi_pckt *hci_pckt = (hci_spi_pckt *)pData;

  if(hci_pckt->type == HCI_EVENT_PKT || hci_pckt->type == HCI_EVENT_EXT_PKT)
  {
    void *data;
    hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

    if(hci_pckt->type == HCI_EVENT_PKT){
      data = event_pckt->data;
    }
    else {
      hci_event_ext_pckt *event_pckt = (hci_event_ext_pckt*)hci_pckt->data;
      data = event_pckt->data;
    }

    if(event_pckt->evt == EVT_LE_META_EVENT)
    {
      evt_le_meta_event *evt = data;

      for (i = 0; i < (sizeof(hci_le_meta_events_table)/sizeof(hci_le_meta_events_table_type)); i++)
      {
        if (evt->subevent == hci_le_meta_events_table[i].evt_code)
        {
          hci_le_meta_events_table[i].process((void *)evt->data);
          break;
        }
      }
    }
    else if(event_pckt->evt == EVT_VENDOR)
    {
      evt_blue_aci *blue_evt = data;

      for (i = 0; i < (sizeof(hci_vendor_specific_events_table)/sizeof(hci_vendor_specific_events_table_type)); i++)
      {
        if (blue_evt->ecode == hci_vendor_specific_events_table[i].evt_code)
        {
          hci_vendor_specific_events_table[i].process((void *)blue_evt->data);
          break;
        }
      }
    }
    else
    {
      for (i = 0; i < (sizeof(hci_events_table)/sizeof(hci_events_table_type)); i++)
      {
        if (event_pckt->evt == hci_events_table[i].evt_code)
        {
          hci_events_table[i].process(data);
          break;
        }
      }
    }
  }
}

#endif /* (BLUE_CORE != BLUENRG_LP) */


#if (BLUE_CORE != BLUE_WB)

/** @brief Initialize the BLE Stack
* @param None
* @retval tBleStatus
*/
static tBleStatus InitBleManager_BLE_Stack(void)
{
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  tBleStatus ret;
  uint8_t data_len_out;
  uint8_t random_number[8];
  uint8_t  hwVersion;
  uint16_t fwVersion;
  
  /* Initialize the BlueNRG HCI */
  hci_init(APP_UserEvtRx, NULL);
  
#if (BLUE_CORE == BLUENRG_LP)
  InitBLEIntForBlueNRGLP();
#endif /* (BLUE_CORE == BLUENRG_LP) */
  
  /* Sw reset of the device */
  hci_reset();

  /* Wait some time for the BlueNRG to be fully operational */
  HAL_Delay(2000);
  
  /* get the BlueNRG HW and FW versions */
  getBlueNRGVersion(&hwVersion, &fwVersion);
  
  /* we will let the BLE chip to use its Random MAC address */
#define CONFIG_DATA_RANDOM_ADDRESS          (0x80) /**< Stored static random address. Read-only. */
  ret = aci_hal_read_config_data(CONFIG_DATA_RANDOM_ADDRESS, &data_len_out, BLE_StackValue.BleMacAddress);
  
  if(ret != BLE_STATUS_SUCCESS){
    BLE_MANAGER_PRINTF("\r\nReading  Random BD_ADDR failed\r\n");
    goto fail;
  }
  
  /* Check Random MAC */
  if ((BLE_StackValue.BleMacAddress[5] & 0xC0U) != 0xC0U) {
    BLE_MANAGER_PRINTF("\tStatic Random address not well formed\r\n");
    goto fail;
  }
  
  /* Generate Random Key at every boot */
  if(BLE_StackValue.EnableRandomSecurePIN) {
    BLE_StackValue.SecurePIN = 99999;
    
    /* get a random number from BlueNRG-1 */
    if(hci_le_rand(random_number) != BLE_STATUS_SUCCESS) {
      BLE_MANAGER_PRINTF("hci_le_rand() call failed\r\n");
    }
    
    /* setup random_key with random number */
    for (uint8_t i=0; i<8U; i++) {
      BLE_StackValue.SecurePIN += (435U*((uint32_t)random_number[i]));
    }
    
    /* Control section because we need 6 digits */
    if (BLE_StackValue.SecurePIN <99999U) {
      BLE_StackValue.SecurePIN += 100000U;
    }
  }
  
  ret = aci_hal_write_config_data(BLE_StackValue.ConfigValueOffsets,
                                  BLE_StackValue.ConfigValuelength,
                                  BLE_StackValue.BleMacAddress);
  if(ret != BLE_STATUS_SUCCESS){
    BLE_MANAGER_PRINTF("\r\nSetting Public BD_ADDR failed\r\n");
    goto fail;
  }
  
#if (BLUE_CORE != BLUENRG_LP)
  ret = aci_gatt_init();
#else /* (BLUE_CORE != BLUENRG_LP) */
  ret = aci_gatt_srv_init();
#endif /* (BLUE_CORE != BLUENRG_LP) */
  if(ret != BLE_STATUS_SUCCESS){
    BLE_MANAGER_PRINTF("\r\nGATT_Init failed\r\n");
    goto fail;
  }

#if (BLUE_CORE != BLUENRG_LP)
  ret = aci_gap_init(BLE_StackValue.GAP_Roles, 0, (uint8_t) strlen(BLE_StackValue.BoardName), &service_handle, &dev_name_char_handle, &appearance_char_handle);
#else /* (BLUE_CORE != BLUENRG_LP) */
  ret = aci_gap_init(BLE_StackValue.GAP_Roles, 0x00, (uint8_t) strlen(BLE_StackValue.BoardName), STATIC_RANDOM_ADDR, &service_handle, &dev_name_char_handle,
                     &appearance_char_handle);
#endif /* (BLUE_CORE != BLUENRG_LP) */
  
  
  if(ret != BLE_STATUS_SUCCESS){
    BLE_MANAGER_PRINTF("\r\nGAP_Init failed\r\n");
    goto fail;
  }
  
#if (BLUE_CORE != BLUENRG_LP)
  ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
                                   (uint8_t) strlen(BLE_StackValue.BoardName), (uint8_t *)BLE_StackValue.BoardName);
  
  if(ret != BLE_STATUS_SUCCESS){
    BLE_MANAGER_PRINTF("\r\naci_gatt_update_char_value failed\r\n");
    goto fail;
  }
#else /* (BLUE_CORE != BLUENRG_LP) */
    ret = aci_gatt_srv_write_handle_value_nwk(dev_name_char_handle + 1, 0,(uint8_t) strlen(BLE_StackValue.BoardName),(uint8_t*) BLE_StackValue.BoardName);
  if (ret != BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("\taci_gatt_srv_write_handle_value_nwk failed: 0x%02x\r\n", ret);
     goto fail;
  } else {
    BLE_MANAGER_PRINTF("\taci_gatt_srv_write_handle_value_nwk\r\n");
  }
#endif /* (BLUE_CORE != BLUENRG_LP) */
  
  if(BLE_StackValue.EnableSecureConnection) {
    /* Set the I/O capability  Otherwise the Smartphone will propose a Pin
    * that will be acepted without any control */
    if(aci_gap_set_io_capability(BLE_StackValue.IO_capabilities)==BLE_STATUS_SUCCESS) {
      BLE_MANAGER_PRINTF("I/O Capability Configurated\r\n");
    } else {
      BLE_MANAGER_PRINTF("Error Setting I/O Capability\r\n");
    }
    
    if(BLE_StackValue.EnableRandomSecurePIN) {
      ret = aci_gap_set_authentication_requirement(BLE_StackValue.AuthenticationRequirements,
                                                   BLE_StackValue.MITM_ProtectionRequirements,
                                                   BLE_StackValue.SecureConnectionSupportOptionCode,
                                                   BLE_StackValue.SecureConnectionKeypressNotification,
                                                   7, 
                                                   16,
                                                   DONOT_USE_FIXED_PIN_FOR_PAIRING,
                                                   BLE_StackValue.SecurePIN
                                                   #if (BLUE_CORE != BLUENRG_LP)
                                                     ,0x01);
                                                   #else /* (BLUE_CORE != BLUENRG_LP) */
                                                     );
                                                   #endif /* (BLUE_CORE != BLUENRG_LP)*/
    } else {
      ret = aci_gap_set_authentication_requirement(BLE_StackValue.AuthenticationRequirements,
                                                   BLE_StackValue.MITM_ProtectionRequirements,
                                                   BLE_StackValue.SecureConnectionSupportOptionCode,
                                                   BLE_StackValue.SecureConnectionKeypressNotification,
                                                   7, 
                                                   16,
                                                   USE_FIXED_PIN_FOR_PAIRING,
                                                   BLE_StackValue.SecurePIN
                                                   #if (BLUE_CORE != BLUENRG_LP)
                                                     ,0x01);
                                                   #else /* (BLUE_CORE != BLUENRG_LP) */
                                                     );
                                                   #endif /* (BLUE_CORE != BLUENRG_LP)*/
    }
    
  } else {
    ret = aci_gap_set_authentication_requirement(BLE_StackValue.AuthenticationRequirements,
                                                 BLE_StackValue.MITM_ProtectionRequirements,
                                                 BLE_StackValue.SecureConnectionSupportOptionCode,
                                                 BLE_StackValue.SecureConnectionKeypressNotification,
                                                 7, 
                                                 16,
                                                 USE_FIXED_PIN_FOR_PAIRING,
                                                 BLE_StackValue.SecurePIN
                                                 #if (BLUE_CORE != BLUENRG_LP)
                                                   ,0x01);
                                                 #else /* (BLUE_CORE != BLUENRG_LP) */
                                                   );
                                                 #endif /* (BLUE_CORE != BLUENRG_LP)*/
  }
  
  if (ret != BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("\r\nGAP setting Authentication failed\r\n");
    goto fail;
  }
  
  BLE_MANAGER_PRINTF("\r\nSERVER: BLE Stack Initialized \r\n"
                     "\t\tBoardName= %s\r\n"
                       "\t\tBoardMAC = %x:%x:%x:%x:%x:%x\r\n",
                       BLE_StackValue.BoardName,
                       BLE_StackValue.BleMacAddress[5],
                       BLE_StackValue.BleMacAddress[4],
                       BLE_StackValue.BleMacAddress[3],
                       BLE_StackValue.BleMacAddress[2],
                       BLE_StackValue.BleMacAddress[1],
                       BLE_StackValue.BleMacAddress[0]);

#if (BLUE_CORE != BLUENRG_LP)
  BLE_MANAGER_PRINTF("\t\tBlueNRG-2 HW ver%d.%d\r\n", ((hwVersion>>4)&0x0FU), (hwVersion&0x0FU));
  BLE_MANAGER_PRINTF("\t\tBlueNRG-2 FW ver%d.%d.%c\r\n\r\n", (fwVersion>>8)&0xFU, (fwVersion>>4)&0xFU, ('a' + (fwVersion&0xFU)));
#else /* (BLUE_CORE != BLUENRG_LP) */
  BLE_MANAGER_PRINTF("BlueNRG-LP HWver %d FWver %d\r\n", hwVersion, fwVersion);
#endif /* (BLUE_CORE != BLUENRG_LP)*/
  
  if(BLE_StackValue.EnableSecureConnection) {
    BLE_MANAGER_PRINTF("\t-->ONLY SECURE CONNECTION<--\r\n");
    
    if(BLE_StackValue.EnableRandomSecurePIN) {
      BLE_MANAGER_PRINTF("\t\tRandom Key = %lu\r\n",BLE_StackValue.SecurePIN);
    } else {
      BLE_MANAGER_PRINTF("\t\tFixed  Key = %lu\r\n",BLE_StackValue.SecurePIN);
    }
  }
  
  /* Set output power level */
  aci_hal_set_tx_power_level(BLE_StackValue.EnableHighPowerMode,
                             BLE_StackValue.PowerAmplifierOutputLevel);
#if (BLUE_CORE == BLUENRG_LP)
  ret = hci_le_write_suggested_default_data_length(247,(247+14)*8); 
  if (ret != BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("\thci_le_write_suggested_default_data_length failed: 0x%02x\r\n", ret);
  } else {
    BLE_MANAGER_PRINTF("\thci_le_write_suggested_default_data_length\r\n");
  }
#endif /*(BLUE_CORE == BLUENRG_LP) */
  
fail:
  return ret;
}
#endif /* (BLUE_CORE != BLUE_WB) */

/**
 * @brief  Get hardware and firmware version
 *
 * @param  Hardware version
 * @param  Firmware version
 * @retval Status
 */
uint8_t getBlueNRGVersion(uint8_t *hwVersion, uint16_t *fwVersion)
{
  tBleStatus status;
  uint8_t hci_version, lmp_pal_version;
  uint16_t hci_revision, manufacturer_name, lmp_pal_subversion;

  status = hci_read_local_version_information(&hci_version, &hci_revision, &lmp_pal_version,
                                              &manufacturer_name, &lmp_pal_subversion);

  if (status == BLE_STATUS_SUCCESS) {
    *hwVersion = (uint8_t)(hci_revision >> 8);
    *fwVersion = (hci_revision & 0xFFU) << 8;              // Major Version Number
    *fwVersion |= ((lmp_pal_subversion >> 4) & 0xFU) << 4; // Minor Version Number
    *fwVersion |= lmp_pal_subversion & 0xFU;               // Patch Version Number
  }
  return status;
}
#endif /* (BLUE_CORE == BLUENRG_MS) */

/**
* @brief  Init Ble Manager Services
* @param  None
* @retval tBleStatus Status
*/
static tBleStatus InitBleManagerServices(void)
{
  tBleStatus Status = BLE_ERROR_UNSPECIFIED;
  BleCharTypeDef *BleCharPointer;
  
#ifndef BLE_MANAGER_NO_PARSON
  //Set the Malloc/Free Functions  used inside the Json Parser
  json_set_allocation_functions(BLE_MallocFunction, BLE_FreeFunction);
#endif /* BLE_MANAGER_NO_PARSON */
  
#ifdef BLE_MANAGER_SDKV2
   BLE_MANAGER_PRINTF("BlueST-SDK V2\r\n");
#else /* BLE_MANAGER_SDKV2 */
   BLE_MANAGER_PRINTF("BlueST-SDK V1\r\n");
#endif /* BLE_MANAGER_SDKV2 */
  
  if(BLE_StackValue.EnableConfig) {
    BleCharPointer = &BleCharConfig;
    memset(BleCharPointer,0,sizeof(BleCharTypeDef));
    BleCharPointer->AttrMod_Request_CB = AttrMod_Request_Config;
    BleCharPointer->Write_Request_CB = Write_Request_Config;
    COPY_CONFIG_CHAR_UUID((BleCharPointer->uuid));
    BleCharPointer->Char_UUID_Type =UUID_TYPE_128;
    BleCharPointer->Char_Value_Length=20;
    BleCharPointer->Char_Properties=((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP);
    BleCharPointer->Security_Permissions=ATTR_PERMISSION_NONE;
    BleCharPointer->GATT_Evt_Mask= ((uint8_t)GATT_NOTIFY_ATTRIBUTE_WRITE) | ((uint8_t)GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP);
    BleCharPointer->Enc_Key_Size=16;
    BleCharPointer->Is_Variable=1;
    BleManagerAddChar(BleCharPointer);
    
    Status = BLE_Manager_AddConfigService();
    if(Status == (tBleStatus)BLE_STATUS_SUCCESS) {
      BLE_MANAGER_PRINTF("Config  Service added successfully\r\n");
    } else {
      BLE_MANAGER_PRINTF("Error: while adding Config Service\r\n");
    }
  }
  
  if(BLE_StackValue.EnableConsole) {
    BleCharPointer = &BleCharStdOut;
    memset(BleCharPointer,0,sizeof(BleCharTypeDef));    
    BleCharPointer->AttrMod_Request_CB = AttrMod_Request_Term;
    BleCharPointer->Write_Request_CB = Write_Request_Term;
    BleCharPointer->Read_Request_CB = Read_Request_Term;
    COPY_TERM_CHAR_UUID((BleCharPointer->uuid));
    BleCharPointer->Char_UUID_Type =UUID_TYPE_128;
    BleCharPointer->Char_Value_Length=DEFAULT_MAX_STDOUT_CHAR_LEN;
    BleCharPointer->Char_Properties= ((uint8_t)CHAR_PROP_NOTIFY)| ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP) | ((uint8_t)CHAR_PROP_WRITE) | ((uint8_t)CHAR_PROP_READ);
    BleCharPointer->Security_Permissions=ATTR_PERMISSION_NONE;
    BleCharPointer->GATT_Evt_Mask= ((uint8_t)GATT_NOTIFY_ATTRIBUTE_WRITE) | ((uint8_t)GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP);
    BleCharPointer->Enc_Key_Size=16;
    BleCharPointer->Is_Variable=1;
    BleManagerAddChar(BleCharPointer);
    
    BleCharPointer = &BleCharStdErr;
    memset(BleCharPointer,0,sizeof(BleCharTypeDef));
    BleCharPointer->AttrMod_Request_CB = AttrMod_Request_StdErr;
    BleCharPointer->Read_Request_CB = Read_Request_StdErr;
    COPY_STDERR_CHAR_UUID((BleCharPointer->uuid));
    BleCharPointer->Char_UUID_Type =UUID_TYPE_128;
    BleCharPointer->Char_Value_Length=DEFAULT_MAX_STDERR_CHAR_LEN;
    BleCharPointer->Char_Properties= ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_READ);
    BleCharPointer->Security_Permissions=ATTR_PERMISSION_NONE;
    BleCharPointer->GATT_Evt_Mask=GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
    BleCharPointer->Enc_Key_Size=16;
    BleCharPointer->Is_Variable=1;
    BleManagerAddChar(BleCharPointer);
    
    Status = BLE_Manager_AddConsoleService();
    if(Status == (tBleStatus)BLE_STATUS_SUCCESS) {
      BLE_MANAGER_PRINTF("Console Service added successfully\r\n");
    } else {
      BLE_MANAGER_PRINTF("Error: while adding Console Service\r\n");
    }
  }
  
  UsedStandardBleChars = UsedBleChars;
  
#ifndef BLE_MANAGER_NO_PARSON
  /* Extended Configuration characteristic value */
  if(BLE_StackValue.EnableExtConfig)
  {
    BleCharPointer = &BleCharExtConfig;
    memset(BleCharPointer,0,sizeof(BleCharTypeDef));
    BleCharPointer->AttrMod_Request_CB = AttrMod_Request_ExtConfig;
    BleCharPointer->Write_Request_CB = Write_Request_ExtConfig;
    COPY_EXT_CONFIG_CHAR_UUID((BleCharPointer->uuid));
    BleCharPointer->Char_UUID_Type =UUID_TYPE_128;
    BleCharPointer->Char_Value_Length=20;
    BleCharPointer->Char_Properties= ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP);
    BleCharPointer->Security_Permissions=ATTR_PERMISSION_NONE;
    BleCharPointer->GATT_Evt_Mask= ((uint8_t)GATT_NOTIFY_ATTRIBUTE_WRITE) | ((uint8_t)GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP);
    BleCharPointer->Enc_Key_Size=16;
    BleCharPointer->Is_Variable=1;
    BleManagerAddChar(BleCharPointer);
  }
#endif /* BLE_MANAGER_NO_PARSON */
  
  /* Set Custom Configuration and Services */
  BLE_InitCustomService();
  
  if((UsedBleChars-UsedStandardBleChars) > 0U)
  {
    Status = BLE_Manager_AddFeaturesService();
    if(Status == (tBleStatus)BLE_STATUS_SUCCESS) {
      BLE_MANAGER_PRINTF("Features Service added successfully (Status= 0x%x)\r\n", Status);
    } else {
      BLE_MANAGER_PRINTF("Error: while adding Features Service (Status= 0x%x)\r\n", Status);
    }
  }
  
  return Status;
}

#ifndef BLE_MANAGER_NO_PARSON
/**
* @brief  This function is called to parse a BLE_COMM_TP packet.
* @param  buffer_out: pointer to the output buffer.
* @param  buffer_in: pointer to the input data.
* @param  len: buffer in length
* @retval Buffer out length.
*/
uint32_t BLE_Command_TP_Parse(uint8_t** buffer_out, uint8_t* buffer_in, uint32_t len) 
{
  static uint32_t tot_len = 0;
  uint32_t buff_out_len = 0;
  static BLE_COMM_TP_Status_Typedef status = BLE_COMM_TP_WAIT_START;
  BLE_COMM_TP_Packet_Typedef packet_type;
  
  packet_type = (BLE_COMM_TP_Packet_Typedef) buffer_in[0];
  
  switch (status)
  {
  case BLE_COMM_TP_WAIT_START:
    if (packet_type == BLE_COMM_TP_START_PACKET) 
    {
      /*First part of an BLE Command packet*/
      /*packet is enqueued*/        
      uint16_t message_length = buffer_in[1];
      message_length = message_length << 8;
      message_length |= buffer_in[2];
      
      
      *buffer_out = (uint8_t*)BLE_MallocFunction((message_length) * sizeof(uint8_t));
      
      if(*buffer_out == NULL) {
        BLE_MANAGER_PRINTF("Error: Mem alloc error [%d]: %d@%s\r\n", message_length, __LINE__, __FILE__);
      }
      
      memcpy(*buffer_out + tot_len, (uint8_t*) &buffer_in[3], (len - 3U));
      
      
      tot_len += len - 3U;
      status = BLE_COMM_TP_WAIT_END;
      buff_out_len = 0; 
    }
    else if (packet_type == BLE_COMM_TP_START_END_PACKET) 
    {
      /*Final part of an BLE Command packet*/
      /*packet is enqueued*/      
      uint16_t message_length = buffer_in[1];
      message_length = message_length << 8;
      message_length |= buffer_in[2];
            
      *buffer_out = (uint8_t*)BLE_MallocFunction((message_length) * sizeof(uint8_t));        
      if(*buffer_out == NULL) {
        BLE_MANAGER_PRINTF("Error: Mem alloc error [%d]: %d@%s\r\n", message_length, __LINE__, __FILE__);
      }
      
      memcpy(*buffer_out + tot_len, (uint8_t*) &buffer_in[3], (len - 3U));
      
      
      tot_len += len - 3U;
      /*number of bytes of the output packet*/
      buff_out_len = tot_len;
      /*total length set to zero*/
      tot_len = 0;
      /*reset status*/
      status = BLE_COMM_TP_WAIT_START;
    }
    else
    {
      /* Error */
      buff_out_len = 0;
    }
    break;
  case BLE_COMM_TP_WAIT_END: 
    if (packet_type == BLE_COMM_TP_MIDDLE_PACKET) 
    {
      /*Central part of an BLE Command packet*/
      /*packet is enqueued*/
            
      memcpy(*buffer_out + tot_len, (uint8_t*) &buffer_in[1], (len - 1U));
      
      tot_len += len - 1U;
      
      buff_out_len = 0; 
    }
    else if (packet_type == BLE_COMM_TP_END_PACKET) 
    {
      /*Final part of an BLE Command packet*/
      /*packet is enqueued*/
      memcpy(*buffer_out + tot_len, (uint8_t*) &buffer_in[1], (len - 1U));
            
      tot_len += len - 1U;
      /*number of bytes of the output packet*/
      buff_out_len = tot_len;
      /*total length set to zero*/
      tot_len = 0;
      /*reset status*/
      status = BLE_COMM_TP_WAIT_START; 
    } 
    else 
    {
      /*reset status*/
      status = BLE_COMM_TP_WAIT_START;
      /*total length set to zero*/
      tot_len = 0;
            
      buff_out_len = 0; /* error */
    }
    break;
  }
  return buff_out_len;
}

/**
  * @brief  This function is called to prepare a BLE_COMM_TP packet.
  * @param  buffer_out: pointer to the buffer used to save BLE_COMM_TP packet.
  * @param  buffer_in: pointer to the input data.
  * @param  len: buffer in length
  * @param  BytePacketSize: Packet Size in Bytes
  * @retval Buffer out length.
  */
uint32_t BLE_Command_TP_Encapsulate(uint8_t* buffer_out, uint8_t* buffer_in, uint32_t len,int32_t BytePacketSize)
{
  uint32_t size = 0, tot_size = 0;
  uint32_t counter = 0;
  BLE_COMM_TP_Packet_Typedef packet_type = BLE_COMM_TP_START_PACKET;
  int32_t BytePacketSizeMinus1= BytePacketSize-1;
  
  /* One byte header is added to each BLE packet */
  while (counter < len) 
  {
    size = MIN(BytePacketSizeMinus1, (len - counter));
    
    if ((len - counter) <= BytePacketSizeMinus1) 
    {    
      if (counter == 0U) 
      {
        packet_type = BLE_COMM_TP_START_END_PACKET;
      }
      else
      {
        packet_type = BLE_COMM_TP_END_PACKET;
      }
    }
    
    switch (packet_type) 
    {
    case BLE_COMM_TP_START_PACKET: 
      /*First part of an BLE Command packet*/
      buffer_out[tot_size] = ((uint8_t) (BLE_COMM_TP_START_PACKET));
      tot_size++;
      packet_type = BLE_COMM_TP_MIDDLE_PACKET;
      break;
    case BLE_COMM_TP_START_END_PACKET: 
      /*First and last part of an BLE Command packet*/
      buffer_out[tot_size] = ((uint8_t) (BLE_COMM_TP_START_END_PACKET));
      tot_size++;
      packet_type = BLE_COMM_TP_START_PACKET;
      break;
    case BLE_COMM_TP_MIDDLE_PACKET: 
      /*Central part of an BLE Command packet*/
      buffer_out[tot_size] = ((uint8_t) (BLE_COMM_TP_MIDDLE_PACKET));
      tot_size++;
      break;
    case BLE_COMM_TP_END_PACKET: 
      /*Last part of an BLE Command packet*/
      buffer_out[tot_size] = ((uint8_t) (BLE_COMM_TP_END_PACKET));
      tot_size++;
      packet_type = BLE_COMM_TP_START_PACKET;
      break;
    }
    
    /*Input data is incapsulated*/
    memcpy((uint8_t*) &buffer_out[tot_size], (uint8_t *) &buffer_in[counter], size);
    
    /*length variables update*/
    counter += size;
    tot_size += size;
  }
  return tot_size;
}
#endif /* BLE_MANAGER_NO_PARSON */

/* ***************** BlueNRG-1 Stack Callbacks ********************************/

/*******************************************************************************
* Function Name  : hci_le_connection_complete_event.
* Description    : This event indicates that a new connection has been created.
* Input          : See file bluenrg1_events.h
* Output         : See file bluenrg1_events.h
* Return         : See file bluenrg1_events.h
*******************************************************************************/
void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)
{
  connection_handle = Connection_Handle;
  
  BLE_MANAGER_PRINTF(">>>>>>CONNECTED %x:%x:%x:%x:%x:%x\r\n",Peer_Address[5],Peer_Address[4],Peer_Address[3],Peer_Address[2],Peer_Address[1],Peer_Address[0]);

#if (BLUE_CORE != BLUENRG_MS)
  if(BLE_StackValue.EnableSecureConnection) {
    tBleStatus RetStatus;
    /* Check if the device is already bonded */
    RetStatus = aci_gap_is_device_bonded(Peer_Address_Type,Peer_Address);
    if( RetStatus != (tBleStatus)BLE_STATUS_SUCCESS) {
      /* Send a slave security request to the master */
      RetStatus = aci_gap_slave_security_req(Connection_Handle);
      if (RetStatus != (tBleStatus)BLE_STATUS_SUCCESS) {
        BLE_MANAGER_PRINTF("Error: GAP Slave secury request failed %d\r\n",RetStatus);
      } else {
#if (BLE_DEBUG_LEVEL>1)
        BLE_MANAGER_PRINTF("GAP Slave secury request Done\r\n");
#endif
      }
    } else {
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("Device already bounded\r\n");
#endif
    }
  }
#endif /* (BLUE_CORE != BLUENRG_MS) */
  
  /* Start one Exchange configuration for understaning the maxium ATT_MTU */
#if (BLUE_CORE != BLUENRG_LP)
  aci_gatt_exchange_config(connection_handle);
#else /* (BLUE_CORE != BLUENRG_LP) */
  aci_gatt_clt_exchange_config(Connection_Handle);
#endif /* (BLUE_CORE != BLUENRG_LP) */
  
  if(CustomConnectionCompleted!=NULL){
#if (BLUE_CORE == BLUENRG_MS)
    CustomConnectionCompleted(connection_handle,Peer_Address);
#elif (BLUE_CORE == BLUE_WB)
    CustomConnectionCompleted(connection_handle);
#else /* (BLUE_CORE == BLUENRG_MS) */
  CustomConnectionCompleted(connection_handle, Peer_Address_Type,Peer_Address);
#endif /* (BLUE_CORE == BLUENRG_MS) */
  }
  
}/* end hci_le_connection_complete_event() */

/*******************************************************************************
* Function Name  : hci_disconnection_complete_event.
* Description    : This event occurs when a connection is terminated.
* Input          : See file bluenrg1_events.h
* Output         : See file bluenrg1_events.h
* Return         : See file bluenrg1_events.h
*******************************************************************************/
void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{  
  /* No Device Connected */
  connection_handle =0;
  
  BLE_MANAGER_PRINTF("<<<<<<DISCONNECTED\r\n");
  
  /* Make the device connectable again. */
  set_connectable = TRUE;
  
  if(CustomDisconnectionCompleted!=NULL){
    CustomDisconnectionCompleted();
  }
  
}/* end hci_disconnection_complete_event() */

#if (BLUE_CORE != BLUENRG_LP)
/*******************************************************************************
* Function Name  : aci_gatt_read_permit_req_event.
* Description    : This event is given when a read request is received
*                  by the server from the client.
* Input          : See file bluenrg1_events.h
* Output         : See file bluenrg1_events.h
* Return         : See file bluenrg1_events.h
*******************************************************************************/
void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
                                    uint16_t Attribute_Handle,
                                    uint16_t Offset)
{
  uint32_t FoundHandle=0;
  uint8_t RegisteredHandle;
  
  //Search inside all the registed handles
  for(RegisteredHandle=0;((RegisteredHandle<UsedBleChars) && (FoundHandle==0U));RegisteredHandle++) {
    if(BleCharsArray[RegisteredHandle]->Read_Request_CB!=NULL) {
      if(Attribute_Handle==(BleCharsArray[RegisteredHandle]->attr_handle+1U)) {
        BleCharsArray[RegisteredHandle]->Read_Request_CB(BleCharsArray[RegisteredHandle],Attribute_Handle);
      }
    }
  }
  
  if(connection_handle != 0U)
    aci_gatt_allow_read(connection_handle);
}
#else /* (BLUE_CORE != BLUENRG_LP) */
void aci_gatt_srv_authorize_nwk_event(uint16_t Connection_Handle,
                                                    uint16_t Attr_Handle,
                                                    uint8_t Operation_Type,
                                                    uint16_t Attr_Val_Offset,
                                                    uint8_t Data_Length,
                                                    uint8_t Data[])
{
  if (Operation_Type == 0) /* Read */
  {
    uint32_t FoundHandle=0;
    uint8_t RegisteredHandle;
    
    //Search inside all the registed handles
    for(RegisteredHandle=0;((RegisteredHandle<UsedBleChars) && (FoundHandle==0U));RegisteredHandle++) {
      if(BleCharsArray[RegisteredHandle]->Read_Request_CB!=NULL) {
        if(Attr_Handle==(BleCharsArray[RegisteredHandle]->attr_handle+1U)) {
          BleCharsArray[RegisteredHandle]->Read_Request_CB(BleCharsArray[RegisteredHandle],Attr_Handle,Connection_Handle,Operation_Type,Attr_Val_Offset,Data_Length,Data);
        }
      }
    }
  }
}
#endif /* (BLUE_CORE != BLUENRG_LP) */

/*******************************************************************************
* Function Name  : aci_gatt_attribute_modified_event.
* Description    : This event is given when an attribute change his value.
* Input          : See file bluenrg1_events.h
* Output         : See file bluenrg1_events.h
* Return         : See file bluenrg1_events.h
*******************************************************************************/
void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[])
{
  uint32_t FoundHandle=0;
  uint8_t RegisteredHandle;
  
  if (Attr_Handle==((uint16_t)(0x0002+2))) {
    BLE_MANAGER_PRINTF("Notification on Service Change Characteristic\r\n");
    FoundHandle=1;
    if(BLE_StackValue.ForceRescan) {
      /* Force one UUID rescan */
      tBleStatus ret = BLE_STATUS_INSUFFICIENT_RESOURCES;
      uint8_t buff[4];
      
      /* Delete all the Handles from 0x0001 to 0xFFFF */
      STORE_LE_16(buff  ,0x0001U);
      STORE_LE_16(buff+2,0xFFFFU);
      
#if (BLUE_CORE != BLUENRG_LP)
      ret = aci_gatt_update_char_value(0x0001,0x0002,0,4,buff);
#else /* (BLUE_CORE != BLUENRG_LP) */
       ret = aci_gatt_srv_notify(Connection_Handle, 0x0002 + 1, GATT_INDICATION, 4, buff);
#endif /* (BLUE_CORE != BLUENRG_LP) */
      
      if (ret == (tBleStatus)BLE_STATUS_SUCCESS){
        BLE_MANAGER_PRINTF("\r\nUUID Rescan Forced\r\n");
      } else {
        BLE_MANAGER_PRINTF("\r\nError: Problem forcing UUID Rescan\r\n");
      }
    }
  }
  
  //Search inside all the registed handles  
  for(RegisteredHandle=0;((RegisteredHandle<UsedBleChars) && (FoundHandle==0U));RegisteredHandle++) {
    /* Notification */
    if(BleCharsArray[RegisteredHandle]->AttrMod_Request_CB!=NULL) {
      if(Attr_Handle==(BleCharsArray[RegisteredHandle]->attr_handle+2U)) {
        FoundHandle = 1U;
        BleCharsArray[RegisteredHandle]->AttrMod_Request_CB(BleCharsArray[RegisteredHandle],Attr_Handle, Offset, Attr_Data_Length, Attr_Data);
      }
    }
    
    /* Write */
    if(FoundHandle==0U) {
      if(BleCharsArray[RegisteredHandle]->Write_Request_CB!=NULL) {
        if(Attr_Handle==(BleCharsArray[RegisteredHandle]->attr_handle+1U)) {
          FoundHandle = 1U;
          BleCharsArray[RegisteredHandle]->Write_Request_CB(BleCharsArray[RegisteredHandle],Attr_Handle, Offset, Attr_Data_Length, Attr_Data);
        }
      }
    }
  }
  
  if(FoundHandle==0U) {
    if(BLE_StdErr_Service==BLE_SERV_ENABLE){
      BytesToWrite =(uint8_t)sprintf((char *)BufferToWrite, "Notification UNKNOWN handle\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      BLE_MANAGER_PRINTF("Notification UNKNOWN handle =%d\r\n",Attr_Handle);
    }
  }
}

#if (BLUE_CORE == BLUENRG_LP)
/**
* @brief  This event indicates that a new connection has been created
*
* @param  See file bluenrg_lp_events.h
* @retval See file bluenrg_lp_events.h
*/
void hci_le_enhanced_connection_complete_event(uint8_t Status,
                                               uint16_t Connection_Handle,
                                               uint8_t Role,
                                               uint8_t Peer_Address_Type,
                                               uint8_t Peer_Address[6],
                                               uint8_t Local_Resolvable_Private_Address[6],
                                               uint8_t Peer_Resolvable_Private_Address[6],
                                               uint16_t Conn_Interval,
                                               uint16_t Conn_Latency,
                                               uint16_t Supervision_Timeout,
                                               uint8_t Master_Clock_Accuracy)
{
  
  hci_le_connection_complete_event(Status,
                                   Connection_Handle,
                                   Role,
                                   Peer_Address_Type,
                                   Peer_Address,
                                   Conn_Interval,
                                   Conn_Latency,
                                   Supervision_Timeout,
                                   Master_Clock_Accuracy);
}


/**
* @brief  This event is given when an attribute changes his value
* @param  See file bluenrg_lp_events.h
* @retval See file bluenrg_lp_events.h
*/
void aci_gatt_srv_attribute_modified_event(uint16_t Connection_Handle,
                                           uint16_t Attr_Handle,
                                           uint16_t Attr_Data_Length,
                                           uint8_t Attr_Data[])
{
  aci_gatt_attribute_modified_event(Connection_Handle,
                                        Attr_Handle,
                                       0,
                                       Attr_Data_Length,
                                       Attr_Data);
}
#endif /* (BLUE_CORE == BLUENRG_LP) */

/*******************************************************************************
* Function Name  : aci_att_exchange_mtu_resp_event
* Description    : This event is generated in response to an Exchange MTU request
* Input          : See file bluenrg1_events.h
* Output         : See file bluenrg1_events.h
* Return         : See file bluenrg1_events.h
*******************************************************************************/
void aci_att_exchange_mtu_resp_event(uint16_t Connection_Handle,
                                     uint16_t Server_RX_MTU)
{
  if((Server_RX_MTU-3U)<MaxBleCharStdOutLen) {
    MaxBleCharStdOutLen = (uint8_t)(Server_RX_MTU-3U);
  }
  
  if((Server_RX_MTU-3U)<MaxBleCharStdErrLen) {
    MaxBleCharStdErrLen = (uint8_t)(Server_RX_MTU-3U);
  }

#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("aci_att_exchange_mtu_resp_event Server_RX_MTU=%d\r\n",Server_RX_MTU);
#endif
  
 if(CustomMTUExchangeRespEvent!=NULL){
    CustomMTUExchangeRespEvent(Server_RX_MTU-3U);
  }
}

void aci_gap_pass_key_req_event(uint16_t Connection_Handle)
{
  tBleStatus status;
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("aci_gap_pass_key_req_event [Requested PassWd=%ld]\r\n", BLE_StackValue.SecurePIN);
#endif
  status = aci_gap_pass_key_resp(connection_handle, BLE_StackValue.SecurePIN);
  if (status != (uint8_t)BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("Error: aci_gap_pass_key_resp failed:0x%02x\r\n", status);
#if (BLE_DEBUG_LEVEL>1)
  } else {
    BLE_MANAGER_PRINTF("aci_gap_pass_key_resp OK\r\n");
#endif
  }
  
}

/**
 * @brief  This event is generated when the pairing process has completed successfully or a pairing
 *         procedure timeout has occurred or the pairing has failed. This is to notify the application that
 *         we have paired with a remote device so that it can take further actions or to notify that a
 *         timeout has occurred so that the upper layer can decide to disconnect the link.
 * @param  See file bluenrg_lp_events.h
 * @retval See file bluenrg_lp_events.h
 */
void aci_gap_pairing_complete_event(uint16_t ConnectionHandle, uint8_t status, uint8_t reason)
{
  char *StatusString[] = {
   /* 0x00 */ "Success",
   /* 0x01 */ "Timeout",
   /* 0x02 */ "Pairing Failed",
   /* 0x03 */ "Encryption failed, LTK missing on local device",
   /* 0x04 */ "Encryption failed, LTK missing on peer device",
   /* 0x05 */ "Encryption not supported by remote device"
  };
  
  char *ReasonString[] = {
    /* 0x00 */ "NaN", 
    /* 0x01 */ "PASSKEY_ENTRY_FAILED",
    /* 0x02 */ "OOB_NOT_AVAILABLE",
    /* 0x03 */ "AUTH_REQ_CANNOT_BE_MET",
    /* 0x04 */ "CONFIRM_VALUE_FAILED",
    /* 0x05 */ "PAIRING_NOT_SUPPORTED",
    /* 0x06 */ "INSUFF_ENCRYPTION_KEY_SIZE",
    /* 0x07 */ "CMD_NOT_SUPPORTED",
    /* 0x08 */ "UNSPECIFIED_REASON",
    /* 0x09 */ "VERY_EARLY_NEXT_ATTEMPT",
    /* 0x0A */ "SM_INVALID_PARAMS",
    /* 0x0B */ "SMP_SC_DHKEY_CHECK_FAILED",
    /* 0x0C */ "SMP_SC_NUMCOMPARISON_FAILED"
  };

  switch(status) {
    case 0x00: //Success
      BLE_MANAGER_PRINTF("aci_gap_pairing_complete_event %s\r\n", StatusString[status]);
#if (BLUE_CORE != BLUENRG_MS)
      UpdateWhiteList();
#endif /* (BLUE_CORE != BLUENRG_MS) */
      BLE_MANAGER_DELAY(100);
      break;
    case 0x02: //Pairing Failed
      BLE_MANAGER_PRINTF("aci_gap_pairing_complete_event failed:\r\n\tstatus= %s\r\n\treason= %s\r\n",
                          StatusString[status],
                          ReasonString[reason]);
#if 0
      //Not sure...
      BLE_MANAGER_PRINTF("Issue one aci_gap_terminate\r\n");
      /*
       0x05: Authentication Failure
       0x13: Remote User Terminated Connection
       0x14: Remote Device Terminated Connection due to Low Resources
       0x15: Remote Device Terminated Connection due to Power Off
       0x1A: Unsupported Remote Feature
       0x3B: Unacceptable Connection Parameters
      */
      aci_gap_terminate(ConnectionHandle,0x05);
#endif      
      break;
    case 0x01: //Timeout
    case 0x03: //Encryption failed, LTK missing on local device
    case 0x04: //Encryption failed, LTK missing on peer device
    case 0x05: //Encryption not supported by remote device
      BLE_MANAGER_PRINTF("aci_gap_pairing_complete_event failed:\r\n\tstatus= %s\r\n\treason= %s\r\n",
                StatusString[status],
                ReasonString[reason]);
      break;
  }
  
  UNUSED(StatusString);
  UNUSED(ReasonString);
  
  if(CustomPairingCompleted!=NULL){
    CustomPairingCompleted(status);
  }
}

#if (BLUE_CORE != BLUENRG_MS)
/*******************************************************************************
* Function Name  : aci_l2cap_connection_update_resp_event
* Description    : This event is generated when the master responds to the connection
*                  update request packet with a connection update response packet
* Input          : See file bluenrg1_events.h
* Output         : See file bluenrg1_events.h
* Return         : See file bluenrg1_events.h
*******************************************************************************/
void aci_l2cap_connection_update_resp_event(uint16_t Connection_Handle,
                                            uint16_t Result)
{
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("aci_l2cap_connection_update_resp_event Result=%d\r\n",Result);
#endif
}

/*******************************************************************************
* Function Name  : hci_le_connection_update_complete_event
* Description    : It indicates that the Controller process to update the connection has completed
* Input          : See file bluenrg1_events.h
* Output         : See file bluenrg1_events.h
* Return         : See file bluenrg1_events.h
*******************************************************************************/
void hci_le_connection_update_complete_event(uint8_t Status,
                                             uint16_t Connection_Handle,
                                             uint16_t Conn_Interval,
                                             uint16_t Conn_Latency,
                                             uint16_t Supervision_Timeout)
{
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("hci_le_connection_update_complete_event:\r\n");
  BLE_MANAGER_PRINTF("\tStatus=%d\r\n",Status);
  BLE_MANAGER_PRINTF("\tConn_Interval=%d\r\n",Conn_Interval);
  BLE_MANAGER_PRINTF("\tConn_Latency=%d\r\n",Conn_Latency);
  BLE_MANAGER_PRINTF("\tSupervision_Timeout=%d\r\n",Supervision_Timeout);
#endif
}

/*******************************************************************************
* Function Name  : aci_gatt_proc_complete_event
* Description    : This event is generated when a GATT client procedure completes
*                  either with error or successfully
* Input          : See file bluenrg1_events.h
* Output         : See file bluenrg1_events.h
* Return         : See file bluenrg1_events.h
*******************************************************************************/
void aci_gatt_proc_complete_event(uint16_t Connection_Handle,
                                  uint8_t Error_Code)
{
  if(Error_Code!=0U) {
    BLE_MANAGER_PRINTF("Error: aci_gatt_proc_complete_event Error Code=%d\r\n",Error_Code);
#if (BLE_DEBUG_LEVEL>2)
  } else {
    BLE_MANAGER_PRINTF("aci_gatt_proc_complete_event Success\r\n");
#endif
  }
}

/*******************************************************************************
* Function Name  : hci_le_data_length_change_event
* Description    : The LE Data Length Change event notifies the Host of a change
*                  to either the maximum Payload length or the maximum transmission
*                  time of Data Channel PDUs in either direction
* Input          : See file bluenrg1_events.h
* Output         : See file bluenrg1_events.h
* Return         : See file bluenrg1_events.h
*******************************************************************************/
void hci_le_data_length_change_event(uint16_t Connection_Handle,
                                     uint16_t MaxTxOctets,
                                     uint16_t MaxTxTime,
                                     uint16_t MaxRxOctets,
                                     uint16_t MaxRxTime)
{
#if (BLUE_CORE == BLUENRG_LP)
  tBleStatus RetStatus;
#endif /* (BLUE_CORE == BLUENRG_LP) */
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("hci_le_data_length_change_event\r\n");
#endif
  
#if (BLUE_CORE == BLUENRG_LP)
  BLE_MANAGER_DELAY(200);
  RetStatus = aci_gatt_clt_exchange_config(Connection_Handle);
  if( RetStatus !=BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("Error: ACI GATT Exchange Config Failed (0x%x)\r\n", RetStatus);
  } else {
    BLE_MANAGER_PRINTF("ACI GATT Exchange Config Done\r\n");
  }
#endif /* (BLUE_CORE == BLUENRG_LP) */
}

#if (BLUE_CORE == BLUENRG_LP)
/**
 * @brief This event is generated when a GATT client procedure completes either
 *        with error or successfully.
 * @param Connection_Handle Connection handle related to the response
 * @param Error_Code Indicates whether the procedure completed with an error or
 *        was successful.
 * @retval None
 */
/* Error Code Values:
  - 0x00: Success
  - 0x01: Unknown HCI Command
  - 0x02: Unknown Connection Identifier
  - 0x03: Hardware Failure
  - 0x04: Page Timeout
  - 0x05: Authentication Failure
  - 0x06: PIN or Key Missing
  - 0x07: Memory Capacity Exceeded
  - 0x08: Connection Timeout
  - 0x09: Connection Limit Exceeded
  - 0x0A: Synchronous Connection Limit to a Device Exceeded
  - 0x0B: ACL Connection Already Exists
  - 0x0C: Command Disallowed
  - 0x0D: Connection Rejected Due To Limited Resources
  - 0x0E: Connection Rejected Due To Security Reasons
  - 0x0F: Connection Rejected due to Unacceptable BD_ADDR
  - 0x10: Connection Accept Timeout Exceeded
  - 0x11: Unsupported Feature Or Parameter Value
  - 0x12: Invalid HCI Command Parameters
  - 0x13: Remote User Terminated Connection
  - 0x14: Remote Device Terminated Connection due to Low Resources
  - 0x15: Remote Device Terminated Connection due to Power Off
  - 0x16: Connection Terminated By Local Host
  - 0x17: Repeated Attempts
  - 0x18: Pairing Not Allowed
  - 0x19: Unknown LMP PDU
  - 0x1A: Unsupported Remote Feature / Unsupported LMP Feature
  - 0x1B: SCO Offset Rejected
  - 0x1C: SCO Interval Rejected
  - 0x1D: SCO Air Mode Rejected
  - 0x1E: Invalid LMP Parameters
  - 0x1F: Unspecified Error
  - 0x20: Unsupported LMP Parameter Value
  - 0x21: Role Change Not Allowed
  - 0x22: LMP Response Timeout / LL Response Timeout
  - 0x23: LMP Error Transaction Collision
  - 0x24: LMP PDU Not Allowed
  - 0x25: Encryption Mode Not Acceptable
  - 0x26: Link Key cannot be Changed
  - 0x27: Requested QoS Not Supported
  - 0x28: Instant Passed
  - 0x29: Pairing With Unit Key Not Supported
  - 0x2A: Different Transaction Collision
  - 0x2C: QoS Unacceptable Parameter
  - 0x2D: QoS Rejected
  - 0x2E: Channel Assessment Not Supported
  - 0x2F: Insufficient Security
  - 0x30: Parameter Out Of Mandatory Range
  - 0x32: Role Switch Pending
  - 0x34: Reserved Slot Violation
  - 0x35: Role Switch Failed
  - 0x36: Extended Inquiry Response Too Large
  - 0x37: Secure Simple Pairing Not Supported by Host
  - 0x38: Host Busy - Pairing
  - 0x39: Connection Rejected due to No Suitable Channel Found
  - 0x3A: Controller Busy
  - 0x3B: Unacceptable Connection Interval
  - 0x3C: Directed Advertising Timeout
  - 0x3D: Connection Terminated Due to MIC Failure
  - 0x3E: Connection Failed to be Established
  - 0x3F: MAC of the 802.11 AMP
  - 0x41: Failed
  - 0x42: Invalid parameters
  - 0x43: Busy
  - 0x44: Invalid length
  - 0x45: Pending
  - 0x46: Not allowed
  - 0x47: GATT error
  - 0x48: Address not resolved
  - 0x50: Invalid CID
  - 0x5A: CSRK not found
  - 0x5B: IRK not found
  - 0x5C: Device not found in DB
  - 0x5D: Security DB full
  - 0x5E: Device not bonded
  - 0x5F: Device in blacklist
  - 0x60: Invalid handle
  - 0x61: Invalid parameter
  - 0x62: Out of handles
  - 0x63: Invalid operation
  - 0x64: Insufficient resources
  - 0x65: Insufficient encryption key size
  - 0x66: Characteristic already exist
  - 0x82: No valid slot
  - 0x83: Short window
  - 0x84: New interval failed
  - 0x85: Too large interval
  - 0x86: Slot length failed
  - 0xFA: Flash read failed
  - 0xFB: Flash write failed
  - 0xFC: Flash erase failed
*/

void aci_gatt_clt_proc_complete_event(uint16_t Connection_Handle,uint8_t Error_Code)
{
  if(Error_Code!=0x0) {
    BLE_MANAGER_PRINTF("aci_gatt_clt_proc_complete_event Error_Code=0x%x\r\n",Error_Code);
  } else {
    BLE_MANAGER_PRINTF("aci_gatt_clt_proc_complete_event Ok\r\n");
  }
}

/**
 * @brief The LE PHY Update Complete Event is used to indicate that the
 *        Controller has changed the transmitter PHY or receiver PHY in use. If
 *        the Controller changes the transmitter PHY, the receiver PHY, or both
 *        PHYs, this event shall be issued. If an LE_Set_PHY command was sent
 *        and the Controller determines that neither PHY will change as a
 *        result, it issues this event immediately.
 * @param Status For standard error codes see Bluetooth specification, Vol. 2,
 *        part D. For proprietary error code refer to Error codes section
 * @param Connection_Handle Connection_Handle to be used to identify a
 *        connection.
 * @param TX_PHY The transmitter PHY for the connection
 *        Values:
 *        - 0x01: The transmitter PHY for the connection is LE 1M
 *        - 0x02: The transmitter PHY for the connection is LE 2M
 *        - 0x03: The transmitter PHY for the connection is LE Coded
 * @param RX_PHY The receiver PHY for the connection
 *        Values:
 *        - 0x01: The receiver PHY for the connection is LE 1M
 *        - 0x02: The receiver PHY for the connection is LE 2M
 *        - 0x03: The receiver PHY for the connection is LE Coded
 * @retval None
 */
void hci_le_phy_update_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t TX_PHY,
                                      uint8_t RX_PHY)
{

  BLE_MANAGER_PRINTF("hci_le_phy_update_complete_event Status=0x%x\r\n",Status);
  BLE_MANAGER_PRINTF("\tTX_PHY=0x%x\r\n",TX_PHY);
  BLE_MANAGER_PRINTF("\tRX_PHY=0x%x\r\n",RX_PHY);
}
#endif /* (BLUE_CORE == BLUENRG_LP) */

#if (BLUE_CORE != BLUENRG_LP)
/**
* @brief This event is generated when an indication is received from the server.
* @param Connection_Handle Connection handle related to the response
* @param Attribute_Handle The handle of the attribute
* @param Attribute_Value_Length Length of Attribute_Value in octets
* @param Attribute_Value The current value of the attribute
* @retval None
*/
void aci_gatt_indication_event(uint16_t Connection_Handle,
                               uint16_t Attribute_Handle,
                               uint8_t Attribute_Value_Length,
                               uint8_t Attribute_Value[])
{
  tBleStatus RetStatus;
  
  /* This callback should be called when we connect the .box also to something
  * that could work also like server mode.
  * In our case we don't need to do nothing when we receive this indication,
  * except it's confirmation
  */
  
  BLE_MANAGER_PRINTF("aci_gatt_indication_event\r\n");
#ifdef BLE_MANAGER_DEBUG
  #if (BLE_DEBUG_LEVEL>2)
    BLE_MANAGER_PRINTF("aci_gatt_indication_event:\r\n");
    BLE_MANAGER_PRINTF("\tConnection_Handle=0x%x\r\n",Connection_Handle);
    BLE_MANAGER_PRINTF("\tAttribute_Handle=0x%x\r\n",Attribute_Handle);
    if(Attribute_Value_Length==4U) {
      uint16_t StartHandle = 0;
      uint16_t StopHandle  = 0;
      /* Should be the range of Handles */
      StartHandle = (((uint16_t) Attribute_Value[1])<<8);
      StartHandle = StartHandle  | ((uint16_t)Attribute_Value[0]);
      StopHandle  = (((uint16_t) Attribute_Value[3])<<8);
      StopHandle = StopHandle | ((uint16_t)Attribute_Value[2]);
      BLE_MANAGER_PRINTF("\tFrom Handles =0x%x to 0x%x\r\n", StartHandle,StopHandle);
    }
  #endif 
    BLE_MANAGER_PRINTF("Nothing to do except send confirmation\r\n");
#endif /* BLE_MANAGER_DEBUG */

  RetStatus = aci_gatt_confirm_indication(Connection_Handle);
  
  if (RetStatus != BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("Error: aci_gatt_confirm_indicationt failed %d\r\n",RetStatus);
#if (BLE_DEBUG_LEVEL>2)
  } else {
    BLE_MANAGER_PRINTF("aci_gatt_confirm_indication Done\r\n");
#endif
  }
}

#else /* (BLUE_CORE != BLUENRG_LP) */
/**
* @brief This event is generated when an indication is received from the server.
* @param Connection_Handle Connection handle related to the response
* @param Attribute_Handle The handle of the attribute
* @param Attribute_Value_Length Length of Attribute_Value in octets
* @param Attribute_Value The current value of the attribute
* @retval None
*/
void aci_gatt_clt_indication_event(uint16_t Connection_Handle,
                                                 uint16_t Attribute_Handle,
                                                 uint16_t Attribute_Value_Length,
                                                 uint8_t Attribute_Value[])
{
  tBleStatus RetStatus;
  
  /* This callback should be called when we connect the .box also to something
  * that could work also like server mode.
  * In our case we don't need to do nothing when we receive this indication,
  * except it's confirmation
  */
#ifdef BLE_MANAGER_DEBUG
  
   BLE_MANAGER_PRINTF("aci_gatt_clt_indication_event:\r\n");
   
  #if (BLE_DEBUG_LEVEL>2)
    BLE_MANAGER_PRINTF("\tConnection_Handle=0x%x\r\n",Connection_Handle);
    BLE_MANAGER_PRINTF("\tAttribute_Handle=0x%x\r\n",Attribute_Handle);
    if(Attribute_Value_Length==4U) {
      uint16_t StartHandle = 0;
      uint16_t StopHandle  = 0;
      /* Should be the range of Handles */
      StartHandle = (((uint16_t) Attribute_Value[1])<<8);
      StartHandle = StartHandle  | ((uint16_t)Attribute_Value[0]);
      StopHandle  = (((uint16_t) Attribute_Value[3])<<8);
      StopHandle = StopHandle | ((uint16_t)Attribute_Value[2]);
      BLE_MANAGER_PRINTF("\tFrom Handles =0x%x to 0x%x\r\n", StartHandle,StopHandle);
    }
  #endif
    BLE_MANAGER_PRINTF("Nothing to do except send confirmation\r\n");
#endif /* BLE_MANAGER_DEBUG */

  RetStatus = aci_gatt_clt_confirm_indication(Connection_Handle);
  
  if (RetStatus != BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("Error: aci_gatt_clt_indication_event failed %d\r\n",RetStatus);
#if (BLE_DEBUG_LEVEL>2)
  } else {
    BLE_MANAGER_PRINTF("aci_gatt_confirm_indication Done\r\n");
#endif
  }
}
#endif /* (BLUE_CORE != BLUENRG_LP) */

/**
  * @brief This event is generated by the client/server to the application on a GATT timeout (30
seconds). This is a critical event that should not happen during normal operating conditions. It is an indication of either a major disruption in the communication link or a mistake in the application which does not provide a reply to GATT procedures. After this event, the GATT channel is closed and no more GATT communication can be performed. The applications is exptected to issue an @ref aci_gap_terminate to disconnect from the peer device. It is important to leave an 100 ms blank window before sending the @ref aci_gap_terminate, since immediately after this event, system could save important information in non volatile memory.
  * @param Connection_Handle Connection handle on which the GATT procedure has timed out
  * @retval None
*/
void aci_gatt_proc_timeout_event(uint16_t Connection_Handle)
{
  BLE_MANAGER_PRINTF("\r\n-------->aci_gatt_proc_timeout_event<--------\r\n");

  /* ... it is important to leave an 100 ms blank window before sending the @ref aci_gap_terminate...*/
  BLE_MANAGER_DELAY(500);

  /*
   0x05: Authentication Failure
   0x13: Remote User Terminated Connection
   0x14: Remote Device Terminated Connection due to Low Resources
   0x15: Remote Device Terminated Connection due to Power Off
   0x1A: Unsupported Remote Feature
   0x3B: Unacceptable Connection Parameters
  */
  BLE_MANAGER_PRINTF("Issue one aci_gap_terminate\r\n");
  aci_gap_terminate(Connection_Handle,0x13);
}

/**
  * @brief The Hardware Error event is used to indicate some implementation specific type of hardware failure for the controller. This event is used to notify the Host that a hardware failure has occurred in the Controller.
  * @param Hardware_Code Hardware Error Event code.
Error code 0x01 and 0x02 are errors generally caused by hardware issue on the PCB; another possible cause is a slow crystal startup.
In the latter case, the HS_STARTUP_TIME in the device configuration needs to be tuned.
Error code 0x03 indicates an internal error of the protocol stack.
After this event is recommended to force device reset.
  * Values:
  - 0x01: Radio state error
  - 0x02: Timer overrun error
  - 0x03: Internal queue overflow error
  * @retval None
*/
void hci_hardware_error_event(uint8_t Hardware_Code)
{
  if(CustomHardwareErrorEventHandler!=NULL)
  {
    CustomHardwareErrorEventHandler(Hardware_Code);
  }
  else
  {
#if (BLE_DEBUG_LEVEL>2)
#if 0
    char *HWCodeString[] = {
      /* 0x00 */ "NaN", 
      /* 0x01 */ "Radio state error",
      /* 0x02 */ "Timer overrun error",
      /* 0x03 */ "Internal queue overflow error"
    };
    BLE_MANAGER_PRINTF("\r\n-------->hci_hardware_error_event Hardware_Code=\r\n\t%s<--------\r\n",HWCodeString[Hardware_Code]);
#else
    switch(Hardware_Code)
    {
    case 0:
      BLE_MANAGER_PRINTF("\r\n-------->hci_hardware_error_event Hardware_Code=\r\n\tNaN<--------\r\n");
      break;
    case 1:
      BLE_MANAGER_PRINTF("\r\n-------->hci_hardware_error_event Hardware_Code=\r\n\tRadio state error<--------\r\n");
      break;
    case 2:
      BLE_MANAGER_PRINTF("\r\n-------->hci_hardware_error_event Hardware_Code=\r\n\tTimer overrun error<--------\r\n");
      break;
    case 3:
      BLE_MANAGER_PRINTF("\r\n-------->hci_hardware_error_event Hardware_Code=\r\n\tInternal queue overflow error<--------\r\n");
      break;
    }
#endif
#endif
    BLE_MANAGER_DELAY(1000);
    HAL_NVIC_SystemReset();
  }
}

/*******************************************************************************
* Function Name  : aci_gap_bond_lost_event
* Description    : This event is generated on the slave when a 
*                  ACI_GAP_SLAVE_SECURITY_REQUEST is called to reestablish the bond.
* Input          : See file bluenrg1_events.h
* Output         : See file bluenrg1_events.h
* Return         : See file bluenrg1_events.h
*******************************************************************************/
void aci_gap_bond_lost_event(void) {
  aci_gap_allow_rebond(connection_handle);
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("aci_gap_allow_rebond()\r\n");
#endif
}

/**
* @brief  This function Updates the White list for BLE Connection
* @param None
* @retval None
*/
static void UpdateWhiteList(void)
{
  tBleStatus RetStatus;
  uint8_t NumOfAddresses; 
  Bonded_Device_Entry_t BondedDeviceEntry[BLE_MANAGER_MAX_BONDED_DEVICES];

#if (BLUE_CORE != BLUENRG_LP)
  RetStatus =  aci_gap_get_bonded_devices(&NumOfAddresses, BondedDeviceEntry);
#else /* (BLUE_CORE != BLUENRG_LP) */
  RetStatus =  aci_gap_get_bonded_devices(0,BLE_MANAGER_MAX_BONDED_DEVICES,&NumOfAddresses, BondedDeviceEntry);
#endif /* (BLUE_CORE != BLUENRG_LP) */
  
  if (RetStatus == BLE_STATUS_SUCCESS) {
    if (NumOfAddresses > 0U) {
#if (BLE_DEBUG_LEVEL>2)
      BLE_MANAGER_PRINTF("Bonded with %d Device(s): \r\n", NumOfAddresses);
#endif

#if (BLUE_CORE != BLUENRG_LP)
      RetStatus = aci_gap_configure_whitelist();
#else /* (BLUE_CORE != BLUENRG_LP) */
      RetStatus = aci_gap_configure_white_and_resolving_list(0x01 /* White List */);
#endif /* (BLUE_CORE != BLUENRG_LP) */
      if (RetStatus != BLE_STATUS_SUCCESS) {
        BLE_MANAGER_PRINTF("Error: aci_gap_configure_whitelist() failed:0x%02x\r\n", RetStatus);
#if (BLE_DEBUG_LEVEL>2)
      } else {
        BLE_MANAGER_PRINTF("aci_gap_configure_whitelist --> SUCCESS\r\n");
#endif
      }
    }
  }
}

/*******************************************************************************
* Function Name  : aci_gap_numeric_comparison_value_event
* Description    : This event is sent only during SC v.4.2 Pairing
*                  when Numeric Comparison Association model is selected
* Input          : See file bluenrg1_events.h
* Output         : See file bluenrg1_events.h
* Return         : See file bluenrg1_events.h
*******************************************************************************/
void aci_gap_numeric_comparison_value_event(uint16_t Connection_Handle, uint32_t Numeric_Value)
{
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("aci_gap_numeric_comparison_value_event Numeric_Value=%ld\r\n",Numeric_Value);
#endif
  
  /* Confirm Yes... without control of Numeric Value received from Master */
  aci_gap_numeric_comparison_value_confirm_yesno(Connection_Handle,0x01);
}

/*******************************************************************************
* Function Name  : hci_encryption_change_event
* Description    : It is used to indicate that the change of the encryption
*                  mode has been completed
* Input          : See file bluenrg1_events.h
* Output         : See file bluenrg1_events.h
* Return         : See file bluenrg1_events.h
*******************************************************************************/
void hci_encryption_change_event(uint8_t Status,uint16_t Connection_Handle,uint8_t Encryption_Enabled)
{
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("hci_encryption_change_event\r\n");  
#endif
}
#endif /* (BLUE_CORE != BLUENRG_MS) */

