/**
  ******************************************************************************
  * @file    BLE_Manager.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   BLE Manager services APIs
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BLE_MANAGER_H_
#define _BLE_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>

#include "BLE_Manager_Conf.h"

#ifndef BLE_MANAGER_NO_PARSON
#include "parson.h"
#endif /* BLE_MANAGER_NO_PARSON */

/* Bluetooth core supported */
#define BLUENRG_1_2     0x00
#define BLUENRG_MS      0x01
#define BLUENRG_LP      0x02
#define BLUE_WB         0x03

#if (BLUE_CORE == BLUENRG_MS)
#include "hci.h"
#include "hci_tl.h"
#include "hci_le.h"
#include "hci_const.h"
#include "bluenrg_types.h"
#include "bluenrg_gatt_server.h"
#include "bluenrg_gap.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_gatt_aci.h"
#include "bluenrg_hal_aci.h"
#include "bluenrg_aci_const.h"
#include "bluenrg_utils.h"
#include "bluenrg_l2cap_aci.h"
#include "sm.h"
#elif (BLUE_CORE == BLUENRG_LP)
#include "bluenrg_lp_aci.h"
#include "bluenrg_lp_hci_le.h"
#include "bluenrg_lp_events.h"
#include "hci_const.h"
#include "hci_tl.h"
#include "hci.h"
#include "bluenrg_utils.h"
#elif (BLUE_CORE == BLUE_WB)
#include "ble.h"
#include "ble_hal_aci.h"
#include "ble_gatt_aci.h"
#include "ble_gap_aci.h"
#include "ble_hci_le.h"
#include "ble_l2cap_aci.h"
#else /* BLUENRG_1_2 */
#include "hci.h"
#include "bluenrg1_hal_aci.h"
#include "bluenrg1_gatt_aci.h"
#include "bluenrg1_gap_aci.h"
#include "bluenrg1_hci_le.h"
#include "bluenrg1_l2cap_aci.h"
#endif /* (BLUE_CORE == BLUENRG_MS) */

/* Exported Defines ----------------------------------------------------------*/

/* BLE Manager Version only numbers 0->9 */
#define BLE_MANAGER_VERSION_MAJOR '1'
#define BLE_MANAGER_VERSION_MINOR '9'
#define BLE_MANAGER_VERSION_PATCH '0'

/* Length of AdvData in octets */
#if (BLUE_CORE != BLUENRG_LP)
#define BLE_MANAGER_ADVERTISE_DATA_LENGHT 25
#else /* (BLUE_CORE != BLUENRG_LP) */
#define BLE_MANAGER_ADVERTISE_DATA_LENGHT 28
#endif /* (BLUE_CORE != BLUENRG_LP) */

/* SDK value for used platform */
#define BLE_MANAGER_STEVAL_WESU1_PLATFORM          0x01U
#define BLE_MANAGER_SENSOR_TILE_PLATFORM           0x02U
#define BLE_MANAGER_BLUE_COIN_PLATFORM             0x03U
#define BLE_MANAGER_STEVAL_IDB008VX_PLATFORM       0x04U
#define BLE_MANAGER_STEVAL_BCN002V1_PLATFORM       0x05U
#define BLE_MANAGER_SENSOR_TILE_BOX_PLATFORM       0x06U
#define BLE_MANAGER_DISCOVERY_IOT01A_PLATFORM      0x07U
#define BLE_MANAGER_STEVAL_STWINKT1_PLATFORM       0x08U
#define BLE_MANAGER_STEVAL_STWINKT1B_PLATFORM      0x09U
#define BLE_MANAGER_STEVAL_STWINBX1_PLATFORM       0x0EU
#define BLE_MANAGER_STEVAL_ASTRA1_PLATFORM         0x0CU
#define BLE_MANAGER_SENSOR_TILE_BOX_PRO_PLATFORM   0x0DU
#define BLE_MANAGER_STEVAL_PROTEUS1_PLATFORM       0x0FU
#define BLE_MANAGER_STSYS_SBU06_PLATFORM           0x10U
#define BLE_MANAGER_SENSOR_TILE_BOX_PRO_B_PLATFORM 0x11U
#define BLE_MANAGER_STEVAL_STWINBX1_B_PLATFORM     0x12U
#define BLE_MANAGER_NUCLEO_PLATFORM                0x80U
#define BLE_MANAGER_STM32F446RE_NUCLEO_PLATFORM    0x7CU
#define BLE_MANAGER_STM32L053R8_NUCLEO_PLATFORM    0x7DU
#define BLE_MANAGER_STM32L476RG_NUCLEO_PLATFORM    0x7EU
#define BLE_MANAGER_STM32F401RE_NUCLEO_PLATFORM    0x7FU
#define BLE_MANAGER_UNDEF_PLATFORM                 0xFFU

#define BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN 0xDEADBEEF

#define BLE_MANAGER_READ_CUSTOM_COMMAND "ReadCustomCommand"

#ifdef BLE_MANAGER_SDKV2
#if (BLUE_CORE != BLUENRG_LP)
#define BLE_MANAGER_CUSTOM_FIELD1 15
#define BLE_MANAGER_CUSTOM_FIELD2 16
#define BLE_MANAGER_CUSTOM_FIELD3 17
#define BLE_MANAGER_CUSTOM_FIELD4 18
#else /* (BLUE_CORE != BLUENRG_LP) */
#define BLE_MANAGER_CUSTOM_FIELD1 18
#define BLE_MANAGER_CUSTOM_FIELD2 19
#define BLE_MANAGER_CUSTOM_FIELD3 20
#define BLE_MANAGER_CUSTOM_FIELD4 21
#endif /* (BLUE_CORE != BLUENRG_LP) */
#endif /* BLE_MANAGER_SDKV2 */

#define COM_TYPE_ACC    1
#define COM_TYPE_MAG    2
#define COM_TYPE_GYRO   3
#define COM_TYPE_TEMP   4
#define COM_TYPE_PRESS  5
#define COM_TYPE_HUM    6
#define COM_TYPE_MIC    7
#define COM_TYPE_MLC    8

#define DATA_TYPE_UINT8     (uint8_t)(0x00)
#define DATA_TYPE_INT8      (uint8_t)(0x01)
#define DATA_TYPE_UINT16    (uint8_t)(0x02)
#define DATA_TYPE_INT16     (uint8_t)(0x03)
#define DATA_TYPE_UINT32    (uint8_t)(0x04)
#define DATA_TYPE_INT32     (uint8_t)(0x05)
#define DATA_TYPE_FLOAT     (uint8_t)(0x06)

#if (BLUE_CORE == BLUENRG_MS)
#define BLE_ERROR_UNSPECIFIED         ERR_UNSPECIFIED_ERROR
#define hci_le_set_scan_response_data hci_le_set_scan_resp_data
#define aci_gatt_exchange_config      aci_gatt_exchange_configuration
#define aci_gap_pass_key_resp         aci_gap_pass_key_response
#endif /* (BLUE_CORE == BLUENRG_MS) */

#if (BLUE_CORE == BLUE_WB)
#define BLE_ERROR_UNSPECIFIED ERR_UNSPECIFIED_ERROR
#endif /* (BLUE_CORE == BLUE_WB) */

/* Exported Macros -----------------------------------------------------------*/
#ifndef MIN
#define MIN(a,b)            ((a) < (b) )? (a) : (b)
#endif /* MIN */

/* Exported Types ------------------------------------------------------------*/

typedef struct
{
  /* BLE stack setting */
  uint8_t ConfigValueOffsets;
  uint8_t ConfigValuelength;
  uint8_t GAP_Roles;
  uint8_t IO_capabilities;
  uint8_t AuthenticationRequirements;
  uint8_t MITM_ProtectionRequirements;
#if (BLUE_CORE == BLUENRG_MS)
  uint8_t Out_Of_Band_EnableData;
  uint8_t *oob_data;
#else /* (BLUE_CORE == BLUENRG_MS) */
  uint8_t SecureConnectionSupportOptionCode;
  uint8_t SecureConnectionKeypressNotification;
#endif /* (BLUE_CORE == BLUENRG_MS) */

  /* To set the TX power level of the bluetooth device */
  /* ----------------------
     | 0x00: Normal Power |
     | 0x01: High Power   |
     ---------------------- */
  uint8_t EnableHighPowerMode;

  /* Values: 0x00 ... 0x31 - The value depends on the device */
  uint8_t PowerAmplifierOutputLevel;

  /* BLE Manager services setting */
  uint8_t EnableConfig;
  uint8_t EnableConsole;
#ifndef BLE_MANAGER_NO_PARSON
  uint8_t EnableExtConfig;
#endif /* BLE_MANAGER_NO_PARSON */

  /* BLE Board Name */
  char BoardName[8];

  /* For enabling the Secure BLE connection */
  uint8_t EnableSecureConnection;

  /* Secure Connection PIN */
  uint32_t SecurePIN;

  /* For creating a Random Connection PIN */
  uint8_t EnableRandomSecurePIN;
#if (BLUE_CORE != BLUENRG_LP)
  uint8_t AdvertisingFilter;
#endif /* (BLUE_CORE != BLUENRG_LP) */

  /* Set to 1 for forcing a full BLE rescan for the Android/iOS "ST BLE Sensor" application */
  /* with the Secure connection it should not necessary because it will be managed directly by BLE Chip */
  uint8_t ForceRescan;

  /* Set to PUBLIC_ADDR or RANDOM_ADDR */
  uint8_t OwnAddressType;

  /* Bluetooth Board Mac Address */
  uint8_t BleMacAddress[6];

  /* Advertising parameters */
  uint16_t AdvIntervalMin;
  uint16_t AdvIntervalMax;

  uint8_t BoardId;

} BLE_StackTypeDef;

typedef struct
{
  /* BLE Char Definition */
  uint8_t uuid[16];
  uint8_t Char_UUID_Type;
#if (BLUE_CORE == BLUENRG_MS)
  uint8_t Char_Value_Length;
#else /* (BLUE_CORE == BLUENRG_MS) */
  uint16_t Char_Value_Length;
#endif /* (BLUE_CORE == BLUENRG_MS) */
  uint8_t Char_Properties;
  uint8_t Security_Permissions;
  uint8_t GATT_Evt_Mask;
  uint8_t Enc_Key_Size;
  uint8_t Is_Variable;

  /* BLE Attribute handle */
  uint16_t attr_handle;
  /*LE Service handle  */
  uint16_t Service_Handle;

  /*  Callback function pointers */
  /*  Attribute Modify */
  void (*AttrMod_Request_CB)(void *BleCharPointer,
                             uint16_t attr_handle,
                             uint16_t Offset,
                             uint8_t data_length,
                             uint8_t *att_data);
  /* Read Request */
#if (BLUE_CORE != BLUENRG_LP)
  void (*Read_Request_CB)(void *BleCharPointer, uint16_t handle);
#else /* (BLUE_CORE != BLUENRG_LP) */
  void (*Read_Request_CB)(void *BleCharPointer,
                          uint16_t handle,
                          uint16_t Connection_Handle,
                          uint8_t Operation_Type,
                          uint16_t Attr_Val_Offset,
                          uint8_t Data_Length,
                          uint8_t Data[]);
#endif /* (BLUE_CORE != BLUENRG_LP) */
  /* Write Request */
  void (*Write_Request_CB)(void *BleCharPointer,
                           uint16_t attr_handle,
                           uint16_t Offset,
                           uint8_t data_length,
                           uint8_t *att_data);
} BleCharTypeDef;

/* Enum type for Service Notification Change */
typedef enum
{
  BLE_NOTIFY_NOTHING = 0, /* No Event */
  BLE_NOTIFY_SUB     = 1, /* Subscription Event */
  BLE_NOTIFY_UNSUB   = 2  /* Unsubscription Event */
} BLE_NotifyEvent_t;

/* Enum type for Standard Service Enabled or Not */
typedef enum
{
  BLE_SERV_NOT_ENABLE = 0, /* Service Not Enable */
  BLE_SERV_ENABLE    = 1  /* Service Enabled */
} BLE_ServEnab_t;

/* Typedef for Wi-Fi credential */
typedef struct
{
  uint8_t *SSID;
  uint8_t *PassWd;
  uint8_t *Security;
} BLE_WiFi_CredAcc_t;

/* Typedef for Custom Command types */
typedef enum
{
  BLE_CUSTOM_COMMAND_VOID,
  BLE_CUSTOM_COMMAND_INTEGER,
  BLE_CUSTOM_COMMAND_BOOLEAN,
  BLE_CUSTOM_COMMAND_STRING,
  BLE_CUSTOM_COMMAND_ENUM_INTEGER,
  BLE_CUSTOM_COMMAND_ENUM_STRING
} BLE_CustomCommandTypes_t;

/* Structure for saving the Custom Commands */
typedef struct
{
  char *CommandName;
  BLE_CustomCommandTypes_t CommandType;
  void *NextCommand;
} BLE_ExtCustomCommand_t;

/* Typedef for Custom Command */
typedef struct
{
  uint8_t *CommandName;
  BLE_CustomCommandTypes_t CommandType;
  int32_t IntValue;
  uint8_t *StringValue;
} BLE_CustomCommadResult_t;

/* Exported Variables ------------------------------------------------------- */

extern  BLE_ServEnab_t BLE_Conf_Service;
extern  BLE_ServEnab_t BLE_StdTerm_Service;
extern  BLE_ServEnab_t BLE_StdErr_Service;
extern  BLE_ServEnab_t BLE_ExtConf_Service;

extern uint8_t BufferToWrite[256];
extern uint8_t BytesToWrite;
extern uint8_t set_connectable;

extern BLE_StackTypeDef BLE_StackValue;

/* Manufacter Advertise data */
extern uint8_t manuf_data[BLE_MANAGER_ADVERTISE_DATA_LENGHT];

extern uint8_t MaxBleCharStdOutLen;
extern uint8_t MaxBleCharStdErrLen;

extern BLE_ExtCustomCommand_t *ExtConfigCustomCommands;
extern BLE_ExtCustomCommand_t *ExtConfigLastCustomCommand;

/* Exported Function prototypes --------------------------------------------- */

/**************** Bluetooth Communication *************************/
typedef void (*CustomPairingCompleted_t)(uint8_t PairingStatus);
extern CustomPairingCompleted_t CustomPairingCompleted;

typedef void (*CustomMTUExchangeRespEvent_t)(int32_t MaxCharLength);
extern CustomMTUExchangeRespEvent_t CustomMTUExchangeRespEvent;


typedef void (*CustomSetConnectable_t)(uint8_t *ManufData);
extern CustomSetConnectable_t CustomSetConnectable;
#if (BLUE_CORE == BLUENRG_MS)
typedef void (*CustomConnectionCompleted_t)(uint16_t ConnectionHandle, uint8_t addr[6]);
#elif (BLUE_CORE == BLUE_WB)
typedef void (*CustomConnectionCompleted_t)(uint16_t ConnectionHandle);
#else /* (BLUE_CORE == BLUENRG_MS) */
typedef void (*CustomConnectionCompleted_t)(uint16_t ConnectionHandle, uint8_t Address_Type, uint8_t addr[6]);
#endif /* (BLUE_CORE == BLUENRG_MS) */
extern CustomConnectionCompleted_t CustomConnectionCompleted;

#if (BLUE_CORE == BLUENRG_LP)
extern void InitBLEIntForBlueNRGLP(void);
#endif /* (BLUE_CORE == BLUENRG_LP) */

typedef void (*CustomDisconnectionCompleted_t)(void);
extern CustomDisconnectionCompleted_t CustomDisconnectionCompleted;

typedef void (*CustomAciGattTxPoolAvailableEvent_t)(void);
extern CustomAciGattTxPoolAvailableEvent_t CustomAciGattTxPoolAvailableEvent;

typedef void (*CustomHardwareErrorEventHandler_t)(uint8_t Hardware_Code);
extern CustomHardwareErrorEventHandler_t CustomHardwareErrorEventHandler;

/**************** Debug Console *************************/
typedef uint32_t (*CustomDebugConsoleParsing_t)(uint8_t *att_data, uint8_t data_length);
extern CustomDebugConsoleParsing_t CustomDebugConsoleParsingCallback;

/******************* Config Char *************************/
typedef void (*CustomAttrModConfig_t)(uint8_t *att_data, uint8_t data_length);
extern CustomAttrModConfig_t CustomAttrModConfigCallback;

typedef void (*CustomWriteRequestConfig_t)(uint8_t *att_data, uint8_t data_length);
extern CustomWriteRequestConfig_t CustomWriteRequestConfigCallback;

#ifndef BLE_MANAGER_NO_PARSON
/*********** Extended Configuration Char ****************/
/* For Reboot on DFU Command */
typedef void (*CustomExtConfigRebootOnDFUModeCommand_t)(void);
extern CustomExtConfigRebootOnDFUModeCommand_t CustomExtConfigRebootOnDFUModeCommandCallback;

/* For Power off Command */
typedef void (*CustomExtConfigPowerOffCommand_t)(void);
extern CustomExtConfigPowerOffCommand_t CustomExtConfigPowerOffCommandCallback;

/* For Set board Name Command */
typedef void (*CustomExtConfigSetNameCommand_t)(uint8_t *NewName);
extern CustomExtConfigSetNameCommand_t CustomExtConfigSetNameCommandCallback;

/* For Set Time Command */
typedef void (*CustomExtConfigSetTimeCommand_t)(uint8_t *NewTime);
extern CustomExtConfigSetTimeCommand_t CustomExtConfigSetTimeCommandCallback;

/* For Set Date Command */
typedef void (*CustomExtConfigSetDateCommand_t)(uint8_t *NewDate);
extern CustomExtConfigSetDateCommand_t CustomExtConfigSetDateCommandCallback;

/* For Set Wi-Fi Command */
typedef void (*CustomExtConfigSetWiFiCommand_t)(BLE_WiFi_CredAcc_t NewWiFiCred);
extern CustomExtConfigSetWiFiCommand_t CustomExtConfigSetWiFiCommandCallback;

/* For Change Secure PIN Command */
typedef void (*CustomExtConfigChangePinCommand_t)(uint32_t NewPin);
extern CustomExtConfigChangePinCommand_t CustomExtConfigChangePinCommandCallback;

/* For Clear Secure Data Base Command */
typedef void (*CustomExtConfigClearDBCommand_t)(void);
extern CustomExtConfigClearDBCommand_t CustomExtConfigClearDBCommandCallback;

/* For Reading the Flash Banks Fw Ids */
typedef void (*CustomExtConfigReadBanksFwIdCommand_t)(uint8_t *CurBank, uint16_t *FwId1, uint16_t *FwId2);
extern CustomExtConfigReadBanksFwIdCommand_t CustomExtConfigReadBanksFwIdCommandCallback;

/* For Swapping the Flash Banks */
typedef void (*CustomExtConfigBanksSwapCommand_t)(void);
extern CustomExtConfigBanksSwapCommand_t CustomExtConfigBanksSwapCommandCallback;

/* For UID Command */
typedef void (*CustomExtConfigUidCommand_t)(uint8_t **UID);
extern CustomExtConfigUidCommand_t CustomExtConfigUidCommandCallback;

/* For Info Command */
typedef void (*CustomExtConfigInfoCommand_t)(uint8_t *Answer);
extern CustomExtConfigInfoCommand_t CustomExtConfigInfoCommandCallback;

/* For Help Command */
typedef void (*CustomExtConfigHelpCommand_t)(uint8_t *Answer);
extern CustomExtConfigHelpCommand_t CustomExtConfigHelpCommandCallback;

/* For PowerStatus Command */
typedef void (*CustomExtConfigPowerStatusCommand_t)(uint8_t *Answer);
extern CustomExtConfigPowerStatusCommand_t CustomExtConfigPowerStatusCommandCallback;

/* For VersionFw Command */
typedef void (*CustomExtConfigVersionFwCommand_t)(uint8_t *Answer);
extern CustomExtConfigPowerStatusCommand_t CustomExtConfigVersionFwCommandCallback;

typedef void (*CustomExtConfigCustomCommand_t)(BLE_CustomCommadResult_t *CustomCommand);
extern CustomExtConfigCustomCommand_t CustomExtConfigCustomCommandCallback;

/* For Read Certificate Command */
typedef void (*CustomExtConfigReadCertCommand_t)(uint8_t *Certificate);
extern CustomExtConfigReadCertCommand_t CustomExtConfigReadCertCommandCallback;

/* For Set Certificate Command */
typedef void (*CustomExtConfigSetCertCommand_t)(uint8_t *Certificate);
extern CustomExtConfigSetCertCommand_t CustomExtConfigSetCertCommandCallback;

/* For Custom Command */
typedef void (*CustomExtConfigReadCustomCommands_t)(JSON_Array *JSON_SensorArray);
extern CustomExtConfigReadCustomCommands_t CustomExtConfigReadCustomCommandsCallback;


typedef void (*CustomExtConfigSetSensorsConfigCommands_t)(uint8_t *Answer);
extern CustomExtConfigSetSensorsConfigCommands_t CustomExtConfigSetSensorsConfigCommandsCallback;
#endif /* BLE_MANAGER_NO_PARSON */

/* Exported functions ------------------------------------------------------- */
#if (BLUE_CORE == BLUE_WB)
#define hci_le_connection_complete_event  hci_le_connection_complete_event_BLE
#define aci_gatt_attribute_modified_event aci_gatt_attribute_modified_event_BLE
#define aci_gatt_indication_event         aci_gatt_indication_event_BLE

extern void hci_le_connection_complete_event(uint8_t Status,
                                             uint16_t Connection_Handle,
                                             uint8_t Role,
                                             uint8_t Peer_Address_Type,
                                             uint8_t Peer_Address[6],
                                             uint16_t Conn_Interval,
                                             uint16_t Conn_Latency,
                                             uint16_t Supervision_Timeout,
                                             uint8_t Master_Clock_Accuracy);

extern void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                              uint16_t Attr_Handle,
                                              uint16_t Offset,
                                              uint16_t Attr_Data_Length,
                                              uint8_t Attr_Data[]);

extern void aci_gatt_indication_event(uint16_t Connection_Handle,
                                      uint16_t Attribute_Handle,
                                      uint8_t Attribute_Value_Length,
                                      uint8_t Attribute_Value[]);

#endif /* (BLUE_CORE == BLUE_WB) */

extern tBleStatus Stderr_Update(uint8_t *data, uint8_t length);
extern tBleStatus Term_Update(uint8_t *data, uint8_t length);
extern tBleStatus Config_Update(uint32_t Feature, uint8_t Command, uint8_t data);
extern tBleStatus Config_Update_32(uint32_t Feature, uint8_t Command, uint32_t data);
extern void       setConnectable(void);
extern void updateAdvData(void);
extern void       setNotConnectable(void);
extern void       setConnectionParameters(int min, int max, int latency, int timeout);

#if (BLUE_CORE != BLUE_WB)
extern void ResetBleManager(void);
#endif /* (BLUE_CORE != BLUE_WB) */

extern tBleStatus InitBleManager(void);
extern int32_t BleManagerAddChar(BleCharTypeDef *BleChar);

extern tBleStatus aci_gatt_update_char_value_wrapper(BleCharTypeDef *BleCharPointer, uint8_t charValOffset,
                                                     uint8_t charValueLen, uint8_t *charValue);
extern tBleStatus safe_aci_gatt_update_char_value(BleCharTypeDef *BleCharPointer, uint8_t charValOffset,
                                                  uint8_t charValueLen, uint8_t *charValue);

#ifndef BLE_MANAGER_NO_PARSON
/* Add a Custom Command to a Generic Feature */
extern uint8_t GenericAddCustomCommand(BLE_ExtCustomCommand_t **CustomCommands,
                                       BLE_ExtCustomCommand_t **LastCustomCommand,
                                       char *CommandName,
                                       BLE_CustomCommandTypes_t CommandType,
                                       int32_t DefaultValue,
                                       int32_t Min,
                                       int32_t Max,
                                       int32_t *ValidValuesInt,
                                       char **ValidValuesString,
                                       char *ShortDesc,
                                       JSON_Array *JSON_SensorArray);
/* Little specialization for Ext configuration */
#define AddCustomCommand(...) GenericAddCustomCommand(&\
                                                      ExtConfigCustomCommands, &ExtConfigLastCustomCommand, __VA_ARGS__)

/* Clear the Custom Commands List for a Generic Feature */
extern void GenericClearCustomCommandsList(BLE_ExtCustomCommand_t **CustomCommands,
                                           BLE_ExtCustomCommand_t **LastCustomCommand);
/* Little specialization for Ext configuration */
#define ClearCustomCommandsList() GenericClearCustomCommandsList(&ExtConfigCustomCommands, &ExtConfigLastCustomCommand)

extern void SendNewCustomCommandList(void);
extern void SendError(char *message);
extern void SendInfo(char *message);
#endif /* BLE_MANAGER_NO_PARSON */

extern uint8_t getBlueNRGVersion(uint8_t *hwVersion, uint16_t *fwVersion);

/**
  * @brief
  * @param  uint8_t* buffer
  * @param  uint8_t len
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_StdOutSendBuffer(uint8_t *buffer, uint8_t len);

#ifndef BLE_MANAGER_NO_PARSON
/**
  * @brief  This function is called to parse a BLE_COMM_TP packet.
  * @param  buffer_out: pointer to the output buffer.
  * @param  buffer_in: pointer to the input data.
  * @param  len: buffer in length
  * @retval Buffer out length.
  */
extern uint32_t BLE_Command_TP_Parse(uint8_t **buffer_out, uint8_t *buffer_in, uint32_t len);

/**
  * @brief  This function is called to prepare a BLE_COMM_TP packet.
  * @param  buffer_out: pointer to the buffer used to save BLE_COMM_TP packet.
  * @param  buffer_in: pointer to the input data.
  * @param  len: buffer in length
  * @param  BytePacketSize: Packet Size in Bytes
  * @retval Buffer out length.
  */
extern uint32_t BLE_Command_TP_Encapsulate(uint8_t *buffer_out, uint8_t *buffer_in, uint32_t len,
                                           int32_t BytePacketSize);

extern tBleStatus BLE_ExtConfiguration_Update(uint8_t *data, uint32_t length);

extern BLE_CustomCommadResult_t *ParseCustomCommand(BLE_ExtCustomCommand_t *LocCustomCommands,
                                                    uint8_t *hs_command_buffer);
extern BLE_CustomCommadResult_t *AskGenericCustomCommands(uint8_t *hs_command_buffer);
#endif /* BLE_MANAGER_NO_PARSON */

#ifdef ACC_BLUENRG_CONGESTION
#define ACI_GATT_UPDATE_CHAR_VALUE safe_aci_gatt_update_char_value
#else /* ACC_BLUENRG_CONGESTION */
#define ACI_GATT_UPDATE_CHAR_VALUE aci_gatt_update_char_value_wrapper
#endif /* ACC_BLUENRG_CONGESTION */
#ifdef __cplusplus
}
#endif

#include "BLE_Implementation.h"

#endif /* _BLE_MANAGER_H_ */

