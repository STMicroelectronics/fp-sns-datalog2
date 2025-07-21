/**
  ******************************************************************************
  * @file    ble_manager.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add bluetooth services using vendor specific profiles.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>

#include "ble_manager.h"
#include "ble_manager_common.h"
#include "ble_manager_control.h"

/* Private define ------------------------------------------------------------*/

/* Max Number of Bonded Devices */
#define BLE_MANAGER_MAX_BONDED_DEVICES 3

/* Starting Number For Random Key Secure Pin Generation */\
#define STARTING_NUMBER_SECURE_PIN_GENERATION 99999

/* Hardware & Software Characteristics Service */
#define COPY_FEATURES_SERVICE_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,0x00,0x01,0x11,\
                                                              0xe1,0x9a,0xb4,0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_EXT_CONFIG_CHAR_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x14,0x00,0x02,0x11,\
                                                              0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Console Service */
#define COPY_CONSOLE_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,0x00,0x0E,0x11,\
                                                              0xe1,0x9a,0xb4,0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_TERM_CHAR_UUID(uuid_struct)        COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x01,0x00,0x0E,0x11,\
                                                              0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_STDERR_CHAR_UUID(uuid_struct)      COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x02,0x00,0x0E,0x11,\
                                                              0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Configuration Service */
#define COPY_CONFIG_SERVICE_UUID(uuid_struct)   COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,0x00,0x0F,0x11,\
                                                              0xe1,0x9a,0xb4,0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_CONFIG_CHAR_UUID(uuid_struct)      COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x02,0x00,0x0F,0x11,\
                                                              0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#if (BLUE_CORE == STM32WB05N)
#define GATT_INIT_SERVICE_CHANGED_BIT                       (0x01)
#endif /* (BLUE_CORE == STM32WB05N) */
/* Private Types -------------------------------------------------------------*/

/* Typedef for Standard Command types */
typedef enum
{
  /* Command not recognized */
  EXT_CONFIG_COM_NOT_VALID     = 0,

  /* Commands Without Argument */
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
  EXT_CONFIG_COM_READ_BANKS_FW_ID,
  EXT_CONFIG_COM_BANKS_SWAP,

  /* Commands With Argument */
  EXT_CONFIG_COM_SET_WIFI,
  EXT_CONFIG_COM_SET_DATE,
  EXT_CONFIG_COM_SET_TIME,
  EXT_CONFIG_COM_SET_NAME,
  EXT_CONFIG_COM_CHANGE_PIN,
  EXT_CONFIG_COM_SET_CERT,

  /* Total Number of Commands */
  EXT_CONFIG_COMMAND_NUMBER
} ble_ext_config_command_t;

/* Structure used for Specifying the Standard Commands */
typedef struct
{
  ble_ext_config_command_t command_type;
  char *command_string;
} ble_ext_config_standard_commands_t;

/* Exported variables --------------------------------------------------------*/
/* Identifies if the configuration service are enabled or not */
ble_serv_enab_t ble_conf_service;

/* Identifies if the standard terminal service are enabled or not */
ble_serv_enab_t ble_std_term_service;

/* Identifies if the standard error message service are enabled or not */
ble_serv_enab_t ble_std_err_service;

/* Identifies if the Extended Configuration characteristic value service are enabled or not */
ble_serv_enab_t ble_ext_conf_service;

/* Contains the message of the standard terminal or of the standard error */
uint8_t buffer_to_write[256];

/* Total number of characters written for the message in the standard terminal or for the standard error */
uint8_t bytes_to_write;

uint8_t set_connectable;
uint16_t custom_command_page_level = 0;

/* BLE Stack */
ble_stack_object_t ble_stack_value = {0};
ble_extended_configuration_object_t ble_extended_configuration_value = {0};

/* Manufacter Advertise data */
uint8_t manuf_data[BLE_MANAGER_ADVERTISE_DATA_LENGHT];

/*************************** Bluetooth Communication **************************/
#if (BLUE_CORE == BLUENRG_MS)
__weak void connection_completed_function(uint16_t connection_handle, uint8_t addr[6]);
#elif (BLUE_CORE == BLUE_WB)
__weak void connection_completed_function(uint16_t connection_handle);
#else /* (BLUE_CORE == BLUENRG_MS) */
__weak void connection_completed_function(uint16_t connection_handle, uint8_t address_type, uint8_t addr[6]);
#endif /* (BLUE_CORE == BLUENRG_MS) */

__weak void disconnection_completed_function(void);
__weak void pairing_completed_function(uint8_t pairing_status);
__weak void set_connectable_function(uint8_t *manuf_data);
__weak void mtu_exchange_resp_event_function(uint16_t server_rx_mtu);
__weak void aci_gatt_tx_pool_available_event_function(void);
__weak void hardware_error_event_handler_function(uint8_t hardware_code);

/******************************** Debug Console *******************************/
__weak uint32_t debug_console_parsing(uint8_t *att_data, uint8_t data_length);

/********************************* Config Char ********************************/
__weak void attr_mod_config_function(uint8_t *att_data, uint8_t data_length);
__weak void write_request_config_function(uint8_t *att_data, uint8_t data_length);

#ifndef BLE_MANAGER_NO_PARSON
/*************************** Extended Configuration ***************************/

/***********************************************************************************************
  * Callback functions prototypes to manage the extended configuration characteristic commands *
  **********************************************************************************************/
/* For UID Command */
__weak void ext_ext_config_uid_command_callback(uint8_t **uid);
/* For VersionFw Command */
__weak void ext_config_version_fw_command_callback(uint8_t *answer);
/* For Info Command */
__weak void ext_config_info_command_callback(uint8_t *answer);
/* For Help Command */
__weak void ext_config_help_command_callback(uint8_t *answer);
/* For PowerStatus Command */
__weak void ext_config_power_status_command_callback(uint8_t *answer);

/* For Change Secure PIN Command */
__weak void ext_config_change_pin_command_callback(uint32_t new_pin);
/* For Clear Secure Data Base Command */
__weak void ext_config_clear_db_command_callback(void);
/* For Read Certificate Command */
__weak void ext_config_read_cert_command_callback(uint8_t *certificate);
/* For Set Certificate Command */
__weak void ext_config_set_cert_command_callback(uint8_t *certificate);

/* For Reboot on DFU Command */
__weak void ext_config_reboot_on_dfu_mode_command_callback(void);
/* For Power off Command */
__weak void ext_config_power_off_command_callback(void);
/* For Reading the Flash Banks Fw Ids */
__weak void ext_config_read_banks_fw_id_command_callback(uint8_t *cur_bank, uint16_t *fw_id1, uint16_t *fw_id2);
/* For Swapping the Flash Banks */
__weak void ext_config_banks_swap_command_callback(void);

/* For Set board Name Command */
__weak void ext_config_set_name_command_callback(uint8_t *new_name);
__weak void ext_config_read_custom_commands_callback(JSON_Array *json_command_array);
/* For Custom Command */
__weak void ext_config_custom_command_callback(ble_custom_commad_result_t *custom_command);
/* For Set Date Command */
__weak void ext_config_set_date_command_callback(uint8_t *new_date);
/* For Set Time Command */
__weak void ext_config_set_time_command_callback(uint8_t *new_time);
/* For Set Wi-Fi Command */
__weak void ext_config_set_wi_fi_command_callback(ble_wifi_cred_acc_t new_wifi_cred);

/* Private variables ------------------------------------------------------------*/

/* Table of Standard Commands */
static ble_ext_config_standard_commands_t standard_ext_config_commands[EXT_CONFIG_COMMAND_NUMBER] =
{
  {EXT_CONFIG_COM_NOT_VALID, "NULL"},
  {EXT_CONFIG_COM_READ_COMMAND, "ReadCommand"},
  {EXT_CONFIG_COM_READ_CUSTOM_COMMAND, BLE_MANAGER_READ_CUSTOM_COMMAND},
  {EXT_CONFIG_COM_READ_CERT, "ReadCert"},
  {EXT_CONFIG_COM_READ_UID, "UID"},
  {EXT_CONFIG_COM_READ_VER_FW, "VersionFw"},
  {EXT_CONFIG_COM_READ_INFO, "Info"},
  {EXT_CONFIG_COM_READ_HELP, "Help"},
  {EXT_CONFIG_COM_READ_POWER, "PowerStatus"},
  {EXT_CONFIG_COM_SET_DFU, "DFU"},
  {EXT_CONFIG_COM_SET_OFF, "Off"},
  {EXT_CONFIG_COM_CLEAR_DB, "ClearDB"},
  {EXT_CONFIG_COM_READ_BANKS_FW_ID, "ReadBanksFwId"},
  {EXT_CONFIG_COM_BANKS_SWAP, "BanksSwap"},
  {EXT_CONFIG_COM_SET_WIFI, "SetWiFi"},
  {EXT_CONFIG_COM_SET_DATE, "SetDate"},
  {EXT_CONFIG_COM_SET_TIME, "SetTime"},
  {EXT_CONFIG_COM_SET_NAME, "SetName"},
  {EXT_CONFIG_COM_CHANGE_PIN, "ChangePIN"},
  {EXT_CONFIG_COM_SET_CERT, "SetCert"}
};

/* Table for Custom Commands: */
ble_ext_custom_command_t *ext_config_custom_commands = NULL;
ble_ext_custom_command_t *ext_config_last_custom_command = NULL;
#endif /* BLE_MANAGER_NO_PARSON */

static uint8_t last_std_err_buffer[DEFAULT_MAX_STDERR_CHAR_LEN];
static uint8_t last_std_err_len;
static uint8_t last_term_buffer[DEFAULT_MAX_STDOUT_CHAR_LEN];
static uint8_t last_term_len;
static uint16_t global_connection_handle;

uint8_t max_ble_char_std_out_len;
uint8_t max_ble_char_std_err_len;

static ble_char_object_t ble_char_config;
static ble_char_object_t ble_char_std_out;
static ble_char_object_t ble_char_std_err;
#ifndef BLE_MANAGER_NO_PARSON
static ble_char_object_t ble_char_ext_config;

static uint8_t *hs_command_buffer = NULL;
#endif /* BLE_MANAGER_NO_PARSON */

static ble_char_object_t *ble_chars_array[BLE_MANAGER_MAX_ALLOCABLE_CHARS];
static uint8_t used_ble_chars;
static uint8_t used_standard_ble_chars;

#ifndef BLE_MANAGER_NO_PARSON
static uint32_t tot_len_ble_parse = 0;
static ble_comm_tp_status_t status_ble_parse = BLE_COMM_TP_WAIT_START;
#endif /* BLE_MANAGER_NO_PARSON */

#if (BLUE_CORE == BLUENRG_MS)
/* ***************** BlueNRG-MS Stack functions prototype ***********************/
void hci_le_connection_complete_event(uint8_t status,
                                      uint16_t connection_handle,
                                      uint8_t role,
                                      uint8_t peer_address_type,
                                      uint8_t peer_address[6],
                                      uint16_t conn_interval,
                                      uint16_t conn_latency,
                                      uint16_t supervision_timeout,
                                      uint8_t master_clock_accuracy);

void hci_disconnection_complete_event(uint8_t status,
                                      uint16_t connection_handle,
                                      uint8_t reason);

void aci_gatt_attribute_modified_event(uint16_t connection_handle,
                                       uint16_t attr_handle,
                                       uint16_t offset,
                                       uint16_t attr_data_length,
                                       uint8_t attr_data[]);

void aci_gatt_read_permit_req_event(uint16_t connection_handle,
                                    uint16_t attribute_handle,
                                    uint16_t offset);

void aci_att_exchange_mtu_resp_event(uint16_t connection_handle,
                                     uint16_t server_rx_mtu);

void aci_gap_pairing_complete_event(uint16_t connection_handle,
                                    uint8_t status,
                                    uint8_t reason);

void aci_gap_pass_key_req_event(uint16_t connection_handle);

#endif /* (BLUE_CORE == BLUENRG_MS) */

#if ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06))
__weak void init_ble_int_for_blue_nrglp(void);
#endif /* ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06)) */

#if (BLUE_CORE == STM32WB05N)
__weak void init_ble_int_for_stm32wb05n(void);
#endif /* (BLUE_CORE == STM32WB05N) */

/* Private functions prototype --------------------------------------------------*/
#if (BLUE_CORE == BLUENRG_MS)
static void hci_event_cb(void *pckt);
#else /* (BLUE_CORE == BLUENRG_MS) */
#if (BLUE_CORE != BLUE_WB)
static void app_user_evt_rx(void *pData);
#endif /* (BLUE_CORE != BLUE_WB) */
static void update_white_list(void);
#endif /* (BLUE_CORE == BLUENRG_MS) */

#if (BLUE_CORE != BLUE_WB)
static ble_status_t init_ble_manager_ble_stack(void);
#endif /* (BLUE_CORE != BLUE_WB) */

static ble_status_t init_ble_manager_services(void);

#ifndef BLE_MANAGER_NO_PARSON
static ble_status_t ble_update_ext_conf(uint8_t *data, uint8_t length);
#endif /* BLE_MANAGER_NO_PARSON */

static ble_status_t ble_manager_add_features_service(void);
static ble_status_t ble_manager_add_console_service(void);
static ble_status_t ble_manager_add_config_service(void);

static ble_status_t update_term_std_out(uint8_t *data, uint8_t length);
static ble_status_t update_term_std_err(uint8_t *data, uint8_t length);

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
static void read_request_std_err(void *void_char_pointer, uint16_t handle);
static void read_request_term(void *void_char_pointer, uint16_t handle);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
static void read_request_std_err(void *void_char_pointer,
                                 uint16_t handle,
                                 uint16_t connection_handle,
                                 uint8_t operation_type,
                                 uint16_t attr_val_offset,
                                 uint8_t data_length,
                                 uint8_t data[]);
static void read_request_term(void *void_char_pointer,
                              uint16_t handle,
                              uint16_t connection_handle,
                              uint8_t operation_type,
                              uint16_t attr_val_offset,
                              uint8_t data_length,
                              uint8_t data[]);
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

#ifndef BLE_MANAGER_NO_PARSON
static ble_ext_config_command_t ble_ext_config_extract_command_type(uint8_t *hs_command_buffer);

static void attr_mod_request_ext_config(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                        uint8_t data_length, uint8_t *att_data);
static void write_request_ext_config(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                     uint8_t data_length,
                                     uint8_t *att_data);
static void clear_single_command(ble_ext_custom_command_t *command);
#endif /* BLE_MANAGER_NO_PARSON */

/* Private functions ------------------------------------------------------------*/
/**
  * @brief  Add the Config service using a vendor specific profile
  * @param  None
  * @retval ble_status_t Status
  */
static ble_status_t ble_manager_add_config_service(void)
{
  ble_status_t ret;

#if (BLUE_CORE != BLUENRG_MS)
  service_uuid_t service_uuid;
  char_uuid_t char_uuid;
#endif /* (BLUE_CORE != BLUENRG_MS) */

  uint8_t uuid[16];

  COPY_CONFIG_SERVICE_UUID(uuid);

#if (BLUE_CORE == BLUENRG_MS)
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, 1 + 3, &(ble_char_config.service_handle));
#else /* (BLUE_CORE == BLUENRG_MS) */
  BLE_MEM_CPY(&service_uuid.service_uuid_128, uuid, 16);
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
  ret = aci_gatt_add_service(UUID_TYPE_128,  &service_uuid, PRIMARY_SERVICE, 1 + 3, &(ble_char_config.service_handle));
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
  ret = aci_gatt_srv_add_service_nwk(UUID_TYPE_128,
                                     &service_uuid,
                                     PRIMARY_SERVICE,
                                     1 + 3,
                                     &(ble_char_config.service_handle));
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#endif /* (BLUE_CORE == BLUENRG_MS) */

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    goto end_label;
  }

#if (BLUE_CORE != BLUENRG_MS)
  BLE_MEM_CPY(&char_uuid.char_uuid_128, ble_char_config.uuid, 16);
#endif /* (BLUE_CORE != BLUENRG_MS) */

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
  ret =  aci_gatt_add_char(ble_char_config.service_handle,
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
  ret =  aci_gatt_srv_add_char_nwk(ble_char_config.service_handle,
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
                           ble_char_config.char_uuid_type,
#if (BLUE_CORE == BLUENRG_MS)
                           ble_char_config.uuid,
#else /* (BLUE_CORE == BLUENRG_MS) */
                           &char_uuid,
#endif /* (BLUE_CORE == BLUENRG_MS) */
                           ble_char_config.char_value_length,
                           ble_char_config.char_properties,
                           ble_char_config.security_permissions,
                           ble_char_config.gatt_evt_mask,
                           ble_char_config.enc_key_size,
                           ble_char_config.is_variable,
                           &(ble_char_config.attr_handle));

end_label:
  return ret;
}

/**
  * @brief  Add the Console service using a vendor specific profile
  * @param  None
  * @retval ble_status_t Status
  */
static ble_status_t ble_manager_add_console_service(void)
{
  ble_status_t ret;

#if (BLUE_CORE != BLUENRG_MS)
  service_uuid_t service_uuid;
  char_uuid_t char_uuid;
#endif /* (BLUE_CORE != BLUENRG_MS) */

  uint8_t uuid[16];

  COPY_CONSOLE_SERVICE_UUID(uuid);

#if (BLUE_CORE == BLUENRG_MS)
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, 1 + (3 * 2), &(ble_char_std_out.service_handle));
#else /* (BLUE_CORE == BLUENRG_MS) */
  BLE_MEM_CPY(&service_uuid.service_uuid_128, uuid, 16);
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
  ret = aci_gatt_add_service(UUID_TYPE_128,
                             &service_uuid,
                             PRIMARY_SERVICE,
                             1 + (3 * 2),
                             &(ble_char_std_out.service_handle));
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
  ret = aci_gatt_srv_add_service_nwk(UUID_TYPE_128,
                                     &service_uuid,
                                     PRIMARY_SERVICE,
                                     1 + (3 * 2),
                                     &(ble_char_std_out.service_handle));
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#endif /* (BLUE_CORE == BLUENRG_MS) */

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    goto end_label;
  }

#if (BLUE_CORE != BLUENRG_MS)
  BLE_MEM_CPY(&char_uuid.char_uuid_128, ble_char_std_out.uuid, 16);
#endif /* (BLUE_CORE != BLUENRG_MS) */

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
  ret =  aci_gatt_add_char(ble_char_std_out.service_handle,
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
  ret =  aci_gatt_srv_add_char_nwk(ble_char_std_out.service_handle,
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
                           ble_char_std_out.char_uuid_type,
#if (BLUE_CORE == BLUENRG_MS)
                           ble_char_std_out.uuid,
#else /* (BLUE_CORE == BLUENRG_MS) */
                           &char_uuid,
#endif /* (BLUE_CORE == BLUENRG_MS) */
                           ble_char_std_out.char_value_length,
                           ble_char_std_out.char_properties,
                           ble_char_std_out.security_permissions,
                           ble_char_std_out.gatt_evt_mask,
                           ble_char_std_out.enc_key_size,
                           ble_char_std_out.is_variable,
                           &(ble_char_std_out.attr_handle));

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    goto end_label;
  }

  ble_char_std_err.service_handle = ble_char_std_out.service_handle;

#if (BLUE_CORE != BLUENRG_MS)
  BLE_MEM_CPY(&char_uuid.char_uuid_128, ble_char_std_err.uuid, 16);
#endif /* (BLUE_CORE != BLUENRG_MS) */


#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
  ret =  aci_gatt_add_char(ble_char_std_err.service_handle,
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
  ret =  aci_gatt_srv_add_char_nwk(ble_char_std_err.service_handle,
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
                           ble_char_std_err.char_uuid_type,
#if (BLUE_CORE == BLUENRG_MS)
                           ble_char_std_err.uuid,
#else /* (BLUE_CORE == BLUENRG_MS) */
                           &char_uuid,
#endif /* (BLUE_CORE == BLUENRG_MS) */
                           ble_char_std_err.char_value_length,
                           ble_char_std_err.char_properties,
                           ble_char_std_err.security_permissions,
                           ble_char_std_err.gatt_evt_mask,
                           ble_char_std_err.enc_key_size,
                           ble_char_std_err.is_variable,
                           &(ble_char_std_err.attr_handle));

end_label:
  return ret;
}

/**
  * @brief  Update Stdout characteristic value (when length is <=MaxBLECharLen)
  * @param  uint8_t *data string to write
  * @param  uint8_t length length of string to write
  * @retval ble_status_t      Status
  */
static ble_status_t update_term_std_out(uint8_t *data, uint8_t length)
{
  if (ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_std_out, 0, length, data) != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("Error: Updating Stdout Char\r\n");
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}

/**
  * @brief  Update Stderr characteristic value (when length is <=MaxBLECharLen)
  * @param  uint8_t *data string to write
  * @param  uint8_t length length of string to write
  * @retval ble_status_t      Status
  */
static ble_status_t update_term_std_err(uint8_t *data, uint8_t length)
{
  if (ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_std_err, 0, length, data) != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("Error: Updating Stdout Char\r\n");
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
/**
  * @brief  Update Stderr characteristic value after a read request
  * @param None
  * @retval ble_status_t Status
  */
static ble_status_t std_err_update_after_read(ble_char_object_t *ble_char_pointer)
{
  ble_status_t ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(ble_char_pointer, 0, last_std_err_len, last_std_err_buffer);
  return ret;
}

/**
  * @brief  Update Terminal characteristic value after a read request
  * @param None
  * @retval ble_status_t Status
  */
static ble_status_t term_update_after_read(void *ble_char_pointer)
{
  ble_status_t ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(ble_char_pointer, 0, last_term_len, last_term_buffer);
  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Stdout Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating Stdout Char\r\n");
    }
  }
  return ret;
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *void_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
static void read_request_term(void *void_char_pointer, uint16_t handle)
{
  ble_char_object_t *ble_char_pointer = (ble_char_object_t *) void_char_pointer;

  /* Send again the last packet for StdOut */
  term_update_after_read(ble_char_pointer);
#if (BLE_DEBUG_LEVEL>1)
  BLE_MANAGER_PRINTF("Read for Term\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *void_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
static void read_request_std_err(void *void_char_pointer, uint16_t handle)
{
  ble_char_object_t *ble_char_pointer = (ble_char_object_t *) void_char_pointer;

  /* Send again the last packet for StdError */
  std_err_update_after_read(ble_char_pointer);
#if (BLE_DEBUG_LEVEL>1)
  BLE_MANAGER_PRINTF("Read for StdErr\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
}
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *void_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @param  uint16_t connection_handle Connection handle
  * @param  uint8_t operation_type see aci_gatt_srv_authorize_nwk_event
  * @param  uint16_t attr_val_offset ffset from which the value needs to be read or write
  * @param  uint8_t data_length Length of Data field
  * @param  uint8_t data[] The data that the client has requested to write
  * @retval None
  */
static void read_request_term(void *void_char_pointer,
                              uint16_t handle,
                              uint16_t connection_handle,
                              uint8_t operation_type,
                              uint16_t attr_val_offset,
                              uint8_t data_length,
                              uint8_t data[])
{
  ble_status_t ret;
  ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, last_term_len, last_term_buffer);
  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Stdout Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating Stdout Char\r\n");
    }
  }

#if (BLE_DEBUG_LEVEL>1)
  BLE_MANAGER_PRINTF("Read for Term\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *void_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @param  uint16_t connection_handle Connection handle
  * @param  uint8_t operation_type see aci_gatt_srv_authorize_nwk_event
  * @param  uint16_t attr_val_offset ffset from which the value needs to be read or write
  * @param  uint8_t data_length Length of Data field
  * @param  uint8_t data[] The data that the client has requested to write
  * @retval None
  */
static void read_request_std_err(void *void_char_pointer,
                                 uint16_t handle,
                                 uint16_t connection_handle,
                                 uint8_t operation_type,
                                 uint16_t attr_val_offset,
                                 uint8_t data_length,
                                 uint8_t data[])
{
  ble_status_t ret;

  /* Send again the last packet for StdError */
  ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, last_std_err_len, last_std_err_buffer);
  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("Error: Updating Stderr Char\r\n");
  }
#if (BLE_DEBUG_LEVEL>1)
  BLE_MANAGER_PRINTF("Read for StdErr\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
}
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06)  && (BLUE_CORE != STM32WB05N)) */

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if config is subscribed or not to the one service
  * @param  void *void_char_pointer
  * @param  uint16_t attr_handle Handle of the attribute
  * @param  uint16_t offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
  *                          - Bits 0-14: offset of the reported value inside the attribute.
  *                          - Bit 15: if the entire value of the attribute does not fit inside a single
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void attr_mod_request_config(void *void_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                    uint8_t *att_data)
{
  if (att_data[0] == (uint8_t)01)
  {
    ble_conf_service = BLE_SERV_ENABLE;
  }
  else if (att_data[0] == 0U)
  {
    ble_conf_service = BLE_SERV_NOT_ENABLE;
  }

  if (ble_stack_value.enable_config)
  {
    attr_mod_config_function(att_data, data_length);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->Conf=%s\n",
                                      (ble_conf_service == BLE_SERV_ENABLE) ? "ON" : "OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Conf=%s\r\n", (ble_conf_service == BLE_SERV_ENABLE) ? "ON" : "OFF");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This function is called when there is a change on the gatt attribute as consequence of write request
  *         for the Config service
  * @param  void *void_char_pointer
  * @param  uint16_t attr_handle Handle of the attribute
  * @param  uint16_t offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
  *                          - Bits 0-14: offset of the reported value inside the attribute.
  *                          - Bit 15: if the entire value of the attribute does not fit inside a single
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void write_request_config(void *void_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                 uint8_t *att_data)
{
  /* Received one write command from Client on Configuration characteristc */
  if (ble_stack_value.enable_config)
  {
    write_request_config_function(att_data, data_length);
  }
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Std Err is subscribed or not to the one service
  * @param  void *void_char_pointer
  * @param  uint16_t attr_handle Handle of the attribute
  * @param  uint16_t offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
  *                          - Bits 0-14: offset of the reported value inside the attribute.
  *                          - Bit 15: if the entire value of the attribute does not fit inside a single
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void attr_mod_request_std_err(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                     uint8_t data_length,
                                     uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    ble_std_err_service = BLE_SERV_ENABLE;
  }
  else if (att_data[0] == 0U)
  {
    ble_std_err_service = BLE_SERV_NOT_ENABLE;
  }
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Term is subscribed or not to the one service
  * @param  void *void_char_pointer
  * @param  uint16_t attr_handle Handle of the attribute
  * @param  uint16_t offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
  *                          - Bits 0-14: offset of the reported value inside the attribute.
  *                          - Bit 15: if the entire value of the attribute does not fit inside a single
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void attr_mod_request_term(void *void_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                  uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    ble_std_term_service = BLE_SERV_ENABLE;
  }
  else if (att_data[0] == 0U)
  {
    ble_std_term_service = BLE_SERV_NOT_ENABLE;
  }
}

#ifndef BLE_MANAGER_NO_PARSON
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Extended Configuration
  *         characteristic value is subscribed or not to the one service
  * @param  void *void_char_pointer
  * @param  uint16_t attr_handle Handle of the attribute
  * @param  uint16_t offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
  *                          - Bits 0-14: offset of the reported value inside the attribute.
  *                          - Bit 15: if the entire value of the attribute does not fit inside a single
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void attr_mod_request_ext_config(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                        uint8_t data_length,
                                        uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    ble_ext_conf_service = BLE_SERV_ENABLE;
  }
  else if (att_data[0] == 0U)
  {
    ble_ext_conf_service = BLE_SERV_NOT_ENABLE;
  }
}

/**
  * @brief  This function is called when there is a change on the gatt attribute as consequence of write request
  *         for the Extended Configuration characteristic value service
  * @param  void *void_char_pointer
  * @param  uint16_t attr_handle Handle of the attribute
  * @param  uint16_t offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
  *                          - Bits 0-14: offset of the reported value inside the attribute.
  *                          - Bit 15: if the entire value of the attribute does not fit inside a single
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void write_request_ext_config(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                     uint8_t data_length,
                                     uint8_t *att_data)
{
  uint32_t command_buf_len = 0;

  /* Received one write command from Client on Extended Configuration characteristic*/
  command_buf_len = ble_command_tp_parse(&hs_command_buffer, att_data, data_length);

  if (command_buf_len)
  {
    /* There is a valid command to execute */
    ble_ext_config_command_t command_type;
    uint8_t local_buffer_to_write[2048];

    command_type = ble_ext_config_extract_command_type(hs_command_buffer);

    switch (command_type)
    {
      case EXT_CONFIG_COM_READ_COMMAND:
      {
        JSON_Value *temp_json = json_value_init_object();
        JSON_Object *temp_json_obj = json_value_get_object(temp_json);
        char *json_string_command = NULL;
        uint32_t json_size = 0;
        int32_t writing_pointer = 0;
        if (ble_extended_configuration_value.read_custom_command)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_READ_CUSTOM_COMMAND].command_string);
        }
        if (ble_extended_configuration_value.reboot_on_dfu_mode_command)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_SET_DFU].command_string);
        }
        if (ble_extended_configuration_value.power_off)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_SET_OFF].command_string);
        }
        if (ble_extended_configuration_value.set_name)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_SET_NAME].command_string);
        }
        if (ble_extended_configuration_value.set_time)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_SET_TIME].command_string);
        }
        if (ble_extended_configuration_value.set_date)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_SET_DATE].command_string);
        }
        if (ble_extended_configuration_value.set_wifi)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_SET_WIFI].command_string);
        }
        if (ble_extended_configuration_value.change_secure_pin)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_CHANGE_PIN].command_string);
        }
        if (ble_extended_configuration_value.clear_secure_data)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_CLEAR_DB].command_string);
        }
        if (ble_extended_configuration_value.read_banks)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_READ_BANKS_FW_ID].command_string);
        }
        if (ble_extended_configuration_value.banks_swap)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_BANKS_SWAP].command_string);
        }
        if (ble_extended_configuration_value.read_certificate)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_READ_CERT].command_string);
        }
        if (ble_extended_configuration_value.set_certificate)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_SET_CERT].command_string);
        }
        if (ble_extended_configuration_value.stm32_uid)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_READ_UID].command_string);
        }
        if (ble_extended_configuration_value.info)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_READ_INFO].command_string);
        }
        if (ble_extended_configuration_value.help)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_READ_HELP].command_string);
        }
        if (ble_extended_configuration_value.power_status)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_READ_POWER].command_string);
        }
        if (ble_extended_configuration_value.version_fw)
        {
          writing_pointer += sprintf((char *)local_buffer_to_write + writing_pointer, "%s,",
                                     standard_ext_config_commands[EXT_CONFIG_COM_READ_VER_FW].command_string);
        }

        if (writing_pointer != 0)
        {
          /* Replace  the Latest ',' with the String Termination */
          local_buffer_to_write[writing_pointer - 1] = 0U;
        }

        BLE_MANAGER_PRINTF("Command ReadCommand\r\n");

        json_object_dotset_string(temp_json_obj, "Commands", (char *)local_buffer_to_write);

        /* convert to a json string and write as string */
        json_string_command = json_serialize_to_string(temp_json);
        json_size = json_serialization_size(temp_json);

        ble_ext_configuration_update((uint8_t *) json_string_command, json_size);
        BLE_FREE_FUNCTION(json_string_command);
        json_value_free(temp_json);

        break;
      }

      /* Board Report Command */
      case EXT_CONFIG_COM_READ_UID:
        if (ble_extended_configuration_value.stm32_uid)
        {
          JSON_Value *temp_json = json_value_init_object();
          JSON_Object *temp_json_obj = json_value_get_object(temp_json);
          char *json_string_command = NULL;
          uint32_t json_size = 0;
          uint8_t *uid;

          BLE_MANAGER_PRINTF("Command UID\r\n");

          ext_ext_config_uid_command_callback(&uid);

          sprintf((char *)local_buffer_to_write, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",
                  uid[ 3], uid[ 2], uid[ 1], uid[ 0],
                  uid[ 7], uid[ 6], uid[ 5], uid[ 4],
                  uid[11], uid[ 10], uid[9], uid[8]);
          json_object_dotset_string(temp_json_obj, "UID", (char *)local_buffer_to_write);

          /* convert to a json string and write as string */
          json_string_command = json_serialize_to_string(temp_json);
          json_size = json_serialization_size(temp_json);

          ble_ext_configuration_update((uint8_t *) json_string_command, json_size);
          BLE_FREE_FUNCTION(json_string_command);
          json_value_free(temp_json);
        }
        break;

      case EXT_CONFIG_COM_READ_VER_FW:
        if (ble_extended_configuration_value.version_fw)
        {
          JSON_Value *temp_json = json_value_init_object();
          JSON_Object *temp_json_obj = json_value_get_object(temp_json);
          char *json_string_command = NULL;
          uint32_t json_size = 0;

          BLE_MANAGER_PRINTF("Command VersionFw\r\n");

          ext_config_version_fw_command_callback(local_buffer_to_write);

          json_object_dotset_string(temp_json_obj, "VersionFw", (char *)local_buffer_to_write);

          /* convert to a json string and write as string */
          json_string_command = json_serialize_to_string(temp_json);
          json_size = json_serialization_size(temp_json);

          ble_ext_configuration_update((uint8_t *) json_string_command, json_size);
          BLE_FREE_FUNCTION(json_string_command);
        }
        break;

      case EXT_CONFIG_COM_READ_INFO:
        if (ble_extended_configuration_value.info)
        {
          JSON_Value *temp_json = json_value_init_object();
          JSON_Object *temp_json_obj = json_value_get_object(temp_json);
          char *json_string_command = NULL;
          uint32_t json_size = 0;

          BLE_MANAGER_PRINTF("Command Info\r\n");


          ext_config_info_command_callback(local_buffer_to_write);

          json_object_dotset_string(temp_json_obj, "Info", (char *)local_buffer_to_write);

          /* convert to a json string and write as string */
          json_string_command = json_serialize_to_string(temp_json);
          json_size = json_serialization_size(temp_json);

          ble_ext_configuration_update((uint8_t *) json_string_command, json_size);
          BLE_FREE_FUNCTION(json_string_command);
        }
        break;

      case EXT_CONFIG_COM_READ_HELP:
        if (ble_extended_configuration_value.help)
        {
          JSON_Value *temp_json = json_value_init_object();
          JSON_Object *temp_json_obj = json_value_get_object(temp_json);
          char *json_string_command = NULL;
          uint32_t json_size = 0;

          BLE_MANAGER_PRINTF("Command Help\r\n");

          ext_config_help_command_callback(local_buffer_to_write);

          json_object_dotset_string(temp_json_obj, "Help", (char *)local_buffer_to_write);

          /* convert to a json string and write as string */
          json_string_command = json_serialize_to_string(temp_json);
          json_size = json_serialization_size(temp_json);

          ble_ext_configuration_update((uint8_t *) json_string_command, json_size);
          BLE_FREE_FUNCTION(json_string_command);
        }
        break;

      case EXT_CONFIG_COM_READ_POWER:
        if (ble_extended_configuration_value.power_status)
        {
          JSON_Value *temp_json = json_value_init_object();
          JSON_Object *temp_json_obj = json_value_get_object(temp_json);
          char *json_string_command = NULL;
          uint32_t json_size = 0;
          BLE_MANAGER_PRINTF("Command PowerStatus\r\n");

          ext_config_power_status_command_callback(local_buffer_to_write);

          json_object_dotset_string(temp_json_obj, "PowerStatus", (char *)local_buffer_to_write);

          /* convert to a json string and write as string */
          json_string_command = json_serialize_to_string(temp_json);
          json_size = json_serialization_size(temp_json);

          ble_ext_configuration_update((uint8_t *) json_string_command, json_size);
          BLE_FREE_FUNCTION(json_string_command);
        }
        break;

      case EXT_CONFIG_COM_READ_CUSTOM_COMMAND:
        if (ble_extended_configuration_value.read_custom_command)
        {
          send_new_custom_command_list();
        }
        break;

      case EXT_CONFIG_COM_READ_CERT:
        if (ble_extended_configuration_value.read_certificate)
        {
          JSON_Value *temp_json = json_value_init_object();
          JSON_Object *temp_json_obj = json_value_get_object(temp_json);
          char *json_string_command = NULL;
          uint32_t json_size = 0;

          BLE_MANAGER_PRINTF("Command Certificate\r\n");

          ext_config_read_cert_command_callback(local_buffer_to_write);

          json_object_dotset_string(temp_json_obj, "Certificate", (char *)local_buffer_to_write);

          /* convert to a json string and write as string */
          json_string_command = json_serialize_to_string(temp_json);
          json_size = json_serialization_size(temp_json);

          ble_ext_configuration_update((uint8_t *) json_string_command, json_size);

        }
        break;

      case EXT_CONFIG_COM_SET_DFU:
        if (ble_extended_configuration_value.reboot_on_dfu_mode_command)
        {
          BLE_MANAGER_PRINTF("Command DFU\r\n");
          ext_config_reboot_on_dfu_mode_command_callback();
        }
        break;

      case EXT_CONFIG_COM_SET_OFF:
        if (ble_extended_configuration_value.power_off)
        {
          BLE_MANAGER_PRINTF("Command Off\r\n");
          ext_config_power_off_command_callback();
        }
        break;

      case EXT_CONFIG_COM_CLEAR_DB:
        if (ble_extended_configuration_value.clear_secure_data)
        {
          BLE_MANAGER_PRINTF("Command ClearDB\r\n");
          ext_config_clear_db_command_callback();
        }
        break;

      case EXT_CONFIG_COM_READ_BANKS_FW_ID:
        if (ble_extended_configuration_value.read_banks)
        {
          JSON_Value *temp_json = json_value_init_object();
          JSON_Object *temp_json_obj = json_value_get_object(temp_json);
          char *json_string_command = NULL;
          uint32_t json_size = 0;
          uint8_t cur_bank;
          uint16_t fw_id1;
          uint16_t fw_id2;

          BLE_MANAGER_PRINTF("Command ReadBanksFwId\r\n");
          ext_config_read_banks_fw_id_command_callback(&cur_bank, &fw_id1, &fw_id2);

          json_object_dotset_number(temp_json_obj, "BankStatus.currentBank", (double)cur_bank);
          sprintf((char *)local_buffer_to_write, "0x%02X", fw_id1);
          json_object_dotset_string(temp_json_obj, "BankStatus.fwId1", (char *)local_buffer_to_write);
          sprintf((char *)local_buffer_to_write, "0x%02X", fw_id2);
          json_object_dotset_string(temp_json_obj, "BankStatus.fwId2", (char *)local_buffer_to_write);

          /* convert to a json string and write as string */
          json_string_command = json_serialize_to_string(temp_json);
          json_size = json_serialization_size(temp_json);

          ble_ext_configuration_update((uint8_t *) json_string_command, json_size);
          json_value_free(temp_json);
        }
        break;

      case EXT_CONFIG_COM_BANKS_SWAP:
        if (ble_extended_configuration_value.banks_swap)
        {
          BLE_MANAGER_PRINTF("Command BanksSwap\r\n");
          ext_config_banks_swap_command_callback();
        }
        break;

      /* Command with argument */
      case EXT_CONFIG_COM_SET_DATE:
        if (ble_extended_configuration_value.set_date)
        {
          BLE_MANAGER_PRINTF("Command SetDate\r\n");

          JSON_Value *temp_json = json_parse_string((char *) hs_command_buffer);
          JSON_Object *json_Parse_handler = json_value_get_object(temp_json);
          if (strcmp(json_object_dotget_string(json_Parse_handler, "command"), "SetDate") == 0)
          {
            if (json_object_dothas_value(json_Parse_handler, "argString"))
            {
              uint8_t *new_date = (uint8_t *)json_object_dotget_string(json_Parse_handler, "argString");
              ext_config_set_date_command_callback(new_date);
            }
          }
          json_value_free(temp_json);
        }
        break;

      case EXT_CONFIG_COM_SET_TIME:
        if (ble_extended_configuration_value.set_time)
        {
          BLE_MANAGER_PRINTF("Command SetTime\r\n");

          JSON_Value *temp_json = json_parse_string((char *) hs_command_buffer);
          JSON_Object *json_Parse_handler = json_value_get_object(temp_json);
          if (strcmp(json_object_dotget_string(json_Parse_handler, "command"), "SetTime") == 0)
          {
            if (json_object_dothas_value(json_Parse_handler, "argString"))
            {
              uint8_t *new_time = (uint8_t *)json_object_dotget_string(json_Parse_handler, "argString");
              ext_config_set_time_command_callback(new_time);
            }
          }
          json_value_free(temp_json);
        }
        break;

      case EXT_CONFIG_COM_SET_NAME:
        if (ble_extended_configuration_value.set_name)
        {
          BLE_MANAGER_PRINTF("Command SetName\r\n");
          JSON_Value *temp_json = json_parse_string((char *) hs_command_buffer);
          JSON_Object *json_Parse_handler = json_value_get_object(temp_json);
          if (strcmp(json_object_dotget_string(json_Parse_handler, "command"), "SetName") == 0)
          {
            if (json_object_dothas_value(json_Parse_handler, "argString"))
            {
              uint8_t *NewBoardName = (uint8_t *)json_object_dotget_string(json_Parse_handler, "argString");
              ext_config_set_name_command_callback(NewBoardName);
            }
          }
          json_value_free(temp_json);
        }
        break;

      case EXT_CONFIG_COM_SET_WIFI:
        if (ble_extended_configuration_value.set_wifi)
        {
          BLE_MANAGER_PRINTF("Command SetWiFi\r\n");
          JSON_Value *temp_json = json_parse_string((char *) hs_command_buffer);
          JSON_Object *json_Parse_handler = json_value_get_object(temp_json);
          if (strcmp(json_object_dotget_string(json_Parse_handler, "command"), "SetWiFi") == 0)
          {
            JSON_Object *json_wifi = json_object_dotget_object(json_Parse_handler, "argJsonElement");
            if (json_object_dothas_value(json_wifi, "ssid"))
            {
              ble_wifi_cred_acc_t new_wifi_cred;
              new_wifi_cred.ssid = (uint8_t *)json_object_dotget_string(json_wifi, "ssid");
              if (json_object_dothas_value(json_wifi, "password"))
              {
                new_wifi_cred.pass_wd = (uint8_t *)json_object_dotget_string(json_wifi, "password");
                if (json_object_dothas_value(json_wifi, "securityType"))
                {
                  new_wifi_cred.security = (uint8_t *)json_object_dotget_string(json_wifi, "securityType");
                  ext_config_set_wi_fi_command_callback(new_wifi_cred);
                }
              }
            }
          }
          json_value_free(temp_json);
        }
        break;

      case EXT_CONFIG_COM_CHANGE_PIN:
        if (ble_extended_configuration_value.change_secure_pin)
        {
          BLE_MANAGER_PRINTF("Command ChangePIN\r\n");

          JSON_Value *temp_json = json_parse_string((char *) hs_command_buffer);
          JSON_Object *json_Parse_handler = json_value_get_object(temp_json);
          if (strcmp(json_object_dotget_string(json_Parse_handler, "command"), "ChangePIN") == 0)
          {
            if (json_object_dothas_value(json_Parse_handler, "argNumber"))
            {
              uint32_t new_board_pin = (uint32_t)json_object_dotget_number(json_Parse_handler, "argNumber");
              ext_config_change_pin_command_callback(new_board_pin);
            }
          }
          json_value_free(temp_json);
        }
        break;

      case EXT_CONFIG_COM_SET_CERT:
        if (ble_extended_configuration_value.set_certificate)
        {
          BLE_MANAGER_PRINTF("Command SetCert\r\n");

          JSON_Value *temp_json = json_parse_string((char *) hs_command_buffer);
          JSON_Object *json_Parse_handler = json_value_get_object(temp_json);
          if (strcmp(json_object_dotget_string(json_Parse_handler, "command"), "SetCert") == 0)
          {
            if (json_object_dothas_value(json_Parse_handler, "argString"))
            {
              uint8_t *NewCertificate = (uint8_t *)json_object_dotget_string(json_Parse_handler, "argString");
              ext_config_set_cert_command_callback(NewCertificate);
            }
          }
          json_value_free(temp_json);
        }
        break;
      default:
        /* Check if it's a custom Command or not */
        if (ble_extended_configuration_value.read_custom_command)
        {
          /* we need at least one Custom Command */
          if (ext_config_custom_commands != NULL)
          {
            ble_custom_commad_result_t *command_result = parse_custom_command(ext_config_custom_commands,
                                                                              hs_command_buffer);
            if (command_result != NULL)
            {
              ext_config_custom_command_callback(command_result);
              if (command_result->command_name != NULL)
              {
                BLE_FREE_FUNCTION(command_result->command_name);
              }
              if (command_result->string_value != NULL)
              {
                BLE_FREE_FUNCTION(command_result->string_value);
              }
              BLE_FREE_FUNCTION(command_result);
            }
          }
          else
          {
            BLE_MANAGER_PRINTF("Error: Command Not Valid\r\n");
          }
        }
        else
        {
          BLE_MANAGER_PRINTF("Error: Command Not Valid\r\n");
        }
        break;
    }
    BLE_FREE_FUNCTION(hs_command_buffer);
    hs_command_buffer = NULL;
  }
}

/**
  * @brief  This function Try to search if there is a valid Custom Command
  * @param  ble_ext_custom_command_t *loc_custom_commands Pointer to the Custom Commands List
  * @param  uint8_t *hs_command_buffer pointer to json formatted string
  * @retval None
  */
ble_custom_commad_result_t *parse_custom_command(ble_ext_custom_command_t *loc_custom_commands,
                                                 uint8_t *hs_command_buffer)
{
  ble_custom_commad_result_t *command_result = NULL;
  uint8_t valid_custom_command = 0;
  JSON_Value *temp_json = json_parse_string((char *) hs_command_buffer);
  JSON_Object *json_Parse_handler = json_value_get_object(temp_json);
  /* Start from beginning of Custom Commands list*/
  ble_ext_custom_command_t *loc_last_custom_command = loc_custom_commands;

  /* Search if it's a custom Command defined by user */
  while ((valid_custom_command == 0U) && (loc_last_custom_command != NULL))
  {
    /* Check the command name */
    if (strncmp(json_object_dotget_string(json_Parse_handler, "command"), loc_last_custom_command->command_name,
                strlen(json_object_dotget_string(json_Parse_handler, "command"))) == 0)
    {
      valid_custom_command = 1;
    }
    /* Move to the Next Command if we didn't find nothing*/
    if (valid_custom_command == 0U)
    {
      loc_last_custom_command = (ble_ext_custom_command_t *) loc_last_custom_command->next_command;
    }
  }
  /* If we have found a valid Custom Command extract the values */
  if (valid_custom_command)
  {
    command_result = (ble_custom_commad_result_t *) BLE_MALLOC_FUNCTION(sizeof(ble_custom_commad_result_t));
    if (command_result == NULL)
    {
      BLE_MANAGER_PRINTF("Error: Mem alloc error: %d@%s\r\n", __LINE__, __FILE__);
    }

    command_result->command_name =
      (uint8_t *)BLE_MALLOC_FUNCTION(strlen((char *)loc_last_custom_command->command_name) + 1U);
    if (command_result->command_name == NULL)
    {
      BLE_MANAGER_PRINTF("Error: Mem alloc error: %d@%s\r\n", __LINE__, __FILE__);
      BLE_FREE_FUNCTION(command_result);
    }
    else
    {
      sprintf((char *)command_result->command_name, "%s", (char *)loc_last_custom_command->command_name);
      command_result->command_type = loc_last_custom_command->command_type;
    }

    switch (loc_last_custom_command->command_type)
    {
      case BLE_CUSTOM_COMMAND_VOID:
      {
        BLE_MANAGER_PRINTF("Called Custom Void Command <%s>\r\n", loc_last_custom_command->command_name);
        command_result->int_value = 0;
        command_result->string_value = NULL;
        break;
      }
      case BLE_CUSTOM_COMMAND_INTEGER:
      case BLE_CUSTOM_COMMAND_ENUM_INTEGER:
        if (json_object_dothas_value(json_Parse_handler, "argNumber"))
        {
          int32_t new_value = (int32_t)json_object_dotget_number(json_Parse_handler, "argNumber");
          command_result->int_value = new_value;
          command_result->string_value = NULL;
          BLE_MANAGER_PRINTF("Called Custom Integer Command <%s>\r\n", loc_last_custom_command->command_name);
          BLE_MANAGER_PRINTF("\tNumber=%ld\r\n", (long)new_value);
        }
        break;
      case BLE_CUSTOM_COMMAND_BOOLEAN:
        if (json_object_dothas_value(json_Parse_handler, "argString"))
        {
          uint8_t *new_string = (uint8_t *)json_object_dotget_string(json_Parse_handler, "argString");

          if (strncmp((char *)new_string, "true", 4) == 0)
          {
            command_result->int_value = 1;
          }

          if (strncmp((char *)new_string, "false", 5) == 0)
          {
            command_result->int_value = 0;
          }

          command_result->string_value = NULL;
          BLE_MANAGER_PRINTF("Called Custom Boolean Command <%s>\r\n", loc_last_custom_command->command_name);
          BLE_MANAGER_PRINTF("\tBoolean=<%s>\r\n", new_string);
        }
        break;
      case BLE_CUSTOM_COMMAND_STRING:
      case BLE_CUSTOM_COMMAND_ENUM_STRING:
        if (json_object_dothas_value(json_Parse_handler, "argString"))
        {
          uint8_t *new_string = (uint8_t *)json_object_dotget_string(json_Parse_handler, "argString");
          command_result->int_value = 0;
          command_result->string_value = (uint8_t *)BLE_MALLOC_FUNCTION(strlen((char *)new_string) + 1U);
          if (command_result->string_value == NULL)
          {
            BLE_MANAGER_PRINTF("Error: Mem alloc error: %d@%s\r\n", __LINE__, __FILE__);
            BLE_FREE_FUNCTION(command_result);
          }
          else
          {
            sprintf((char *)command_result->string_value, "%s", (char *)new_string);
            BLE_MANAGER_PRINTF("Called Custom String Command <%s>\r\n", loc_last_custom_command->command_name);
            BLE_MANAGER_PRINTF("\tString=<%s>\r\n", new_string);
          }
        }
        break;
      default:
        break;
    }
  }
  json_value_free(temp_json);

  if (valid_custom_command == 0U)
  {
    BLE_MANAGER_PRINTF("Error: Custom Command Not Valid\r\n");
  }

  return command_result;
}

/**
  * @brief  Clear the Custom Command List
  * @param  ble_ext_custom_command_t **loc_custom_commands Custom Commands Table
  * @param  ble_ext_custom_command_t **loc_last_custom_command Pointer to last Custom Command
  * @retval None
  */
void generic_clear_custom_commands_list(ble_ext_custom_command_t **loc_custom_commands,
                                        ble_ext_custom_command_t **loc_last_custom_command)
{
  if ((*loc_custom_commands) != NULL)
  {
    if ((*loc_custom_commands)->next_command != NULL)
    {
      clear_single_command((ble_ext_custom_command_t *)(*loc_custom_commands)->next_command);
    }
    BLE_FREE_FUNCTION((*loc_custom_commands));
    *loc_last_custom_command = *loc_custom_commands = NULL;
  }
}

/**
  * @brief  Clear One Single Custom Command
  * @param  ble_ext_custom_command_t *next_command Pointer to the next Custom Commands
  * @retval None
  */
static void clear_single_command(ble_ext_custom_command_t *command)
{
  if (command->next_command != NULL)
  {
    clear_single_command((ble_ext_custom_command_t *)command->next_command);
  }
#if (BLE_DEBUG_LEVEL>1)
  BLE_MANAGER_PRINTF("Deleting Custom Command<%s>\r\n", command->command_name);
#endif /* (BLE_DEBUG_LEVEL>1) */
  BLE_FREE_FUNCTION(command->command_name);
  BLE_FREE_FUNCTION(command);
}

/**
  * @brief  This function add a Custom Command
  * @param  ble_ext_custom_command_t **loc_custom_commands Pointer to Custom Commands Table
  * @param  ble_ext_custom_command_t **loc_last_custom_command Pointer to Pointer to slast Custom Command
  * @param  char *command_name Command Name
  * @param  ble_custom_command_types_t command_type Command Type
  * @param  int32_t default_value (for BLE_CUSTOM_COMMAND_INTEGER, BLE_CUSTOM_COMMAND_ENUM_INTEGER,
  *                                BLE_CUSTOM_COMMAND_ENUM_STRING or BLE_CUSTOM_COMMAND_BOOLEAN)
  * @param  int32_t min  Optional Minim value (BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN if not needed)
  * @param  int32_t max Max value
  * @param  int32_t *valid_values_int array of Valid Integer Values (NULL if not needed)
  * @param  char **valid_values_string array of Valid String Values (NULL if not needed)
  * @param  char *short_desc Optionally Short Description (NULL if not needed)
  * @param  JSON_Array *json_sensor_array
  * @retval uint8_t Valid/NotValid Command (1/0)
  */
uint8_t generic_add_custom_command(ble_ext_custom_command_t **loc_custom_commands,
                                   ble_ext_custom_command_t **loc_last_custom_command,
                                   char *command_name, ble_custom_command_types_t command_type,
                                   int32_t default_value,
                                   int32_t min, int32_t max, int32_t *valid_values_int,
                                   char **valid_values_string, char *short_desc,
                                   JSON_Array *json_sensor_array)
{
  uint8_t valid = 1U;
  uint8_t index;

  /* check that we are not using a Standard Command */
  for (index = ((uint8_t)EXT_CONFIG_COM_READ_COMMAND);
       ((index < ((uint8_t)EXT_CONFIG_COMMAND_NUMBER)) && (valid == 1U));
       index++)
  {
    if (strncmp(standard_ext_config_commands[index].command_string, command_name, strlen(command_name)) == 0)
    {
      valid = 0U;
    }
  }

  /* If the Command Name is different from one Standard Command Name */
  if (valid)
  {
    JSON_Value *temp_json1;
    JSON_Object *temp_json1_obj;
    temp_json1 = json_value_init_object();
    temp_json1_obj = json_value_get_object(temp_json1);

    /* Add the Command Name */
    json_object_dotset_string(temp_json1_obj, "Name", command_name);

    /* Add the Command Type */
    switch (command_type)
    {
      case BLE_CUSTOM_COMMAND_VOID:
        json_object_dotset_string(temp_json1_obj, "Type", "Void");
        break;
      case BLE_CUSTOM_COMMAND_INTEGER:
        json_object_dotset_string(temp_json1_obj, "Type", "Integer");
        break;
      case BLE_CUSTOM_COMMAND_BOOLEAN:
        json_object_dotset_string(temp_json1_obj, "Type", "Boolean");
        break;
      case  BLE_CUSTOM_COMMAND_STRING:
        json_object_dotset_string(temp_json1_obj, "Type", "String");
        break;
      case  BLE_CUSTOM_COMMAND_ENUM_INTEGER:
        json_object_dotset_string(temp_json1_obj, "Type", "EnumInteger");
        break;
      case  BLE_CUSTOM_COMMAND_ENUM_STRING:
        json_object_dotset_string(temp_json1_obj, "Type", "EnumString");
        break;
    }

    /* Add the Optional Default Value */
    if ((command_type == BLE_CUSTOM_COMMAND_INTEGER) ||
        (command_type == BLE_CUSTOM_COMMAND_BOOLEAN) ||
        (command_type == BLE_CUSTOM_COMMAND_ENUM_INTEGER) ||
        (command_type == BLE_CUSTOM_COMMAND_ENUM_STRING))
    {
      if (default_value != (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN)
      {
        json_object_dotset_number(temp_json1_obj, "DefaultValue", (double)default_value);
      }
    }

    /* Add the Optional Description */
    if (short_desc != NULL)
    {
      json_object_dotset_string(temp_json1_obj, "Desc", short_desc);
    }

    /* Add the Min/Max Value */
    if (min != (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN)
    {
      json_object_dotset_number(temp_json1_obj, "Min", (double)min);
    }
    if (max != (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN)
    {
      json_object_dotset_number(temp_json1_obj, "Max", (double)max);
    }

    /* Add the array of Valid Integer Values */
    if ((valid_values_int != NULL) && (command_type == BLE_CUSTOM_COMMAND_ENUM_INTEGER))
    {
      JSON_Value *temp_json_array = json_value_init_object();
      JSON_Array *json_sensor_array2 = json_value_get_array(temp_json_array);
      int32_t counter = 0;
      /* Allocate */
      json_object_dotset_value(temp_json1_obj, "IntegerValues", json_value_init_array());
      /* Add Elements */
      json_sensor_array2 = json_object_dotget_array(temp_json1_obj, "IntegerValues");
      while (valid_values_int[counter] != (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN)
      {
        json_array_append_number(json_sensor_array2, (double)(valid_values_int[counter]));
        counter++;
      }
      /* Free */
      json_value_free(temp_json_array);
    }

    /* Add the array of Valid String Values */
    if ((valid_values_string != NULL) && (command_type == BLE_CUSTOM_COMMAND_ENUM_STRING))
    {
      JSON_Value *temp_json_array = json_value_init_object();
      JSON_Array *json_sensor_array2 = json_value_get_array(temp_json_array);
      int32_t counter = 0;
      /* Allocate */
      json_object_dotset_value(temp_json1_obj, "StringValues", json_value_init_array());
      /* Add Elements */
      json_sensor_array2 = json_object_dotget_array(temp_json1_obj, "StringValues");
      while (valid_values_string[counter] != NULL)
      {
        json_array_append_string(json_sensor_array2, (char const *)valid_values_string[counter]);
        counter++;
      }
      /* Free */
      json_value_free(temp_json_array);
    }

    json_array_append_value(json_sensor_array, temp_json1);

    /* Allocate a New Custom Command entry */
    if ((*loc_custom_commands) == NULL)
    {
      (*loc_custom_commands) = (ble_ext_custom_command_t *)BLE_MALLOC_FUNCTION(sizeof(ble_ext_custom_command_t));

      if ((*loc_custom_commands) == NULL)
      {
        BLE_MANAGER_PRINTF("Error: Mem alloc error: %d@%s\r\n", __LINE__, __FILE__);
        return 0;
      }

      (*loc_last_custom_command) = (*loc_custom_commands);
    }
    else
    {
      (*loc_last_custom_command)->next_command = (void *) BLE_MALLOC_FUNCTION(sizeof(ble_ext_custom_command_t));
      if ((*loc_custom_commands) == NULL)
      {
        BLE_MANAGER_PRINTF("Error: Mem alloc error %d@%s\r\n", __LINE__, __FILE__);
        return 0;
      }
      (*loc_last_custom_command) = (ble_ext_custom_command_t *)(*loc_last_custom_command)->next_command;
    }
    /* Fill the Custom Command */
    (*loc_last_custom_command)->command_type = command_type;

    /* Alloc the size for commandName */
    (*loc_last_custom_command)->command_name = BLE_MALLOC_FUNCTION(strlen(command_name) + 1U);
    if (((*loc_last_custom_command)->command_name) == NULL)
    {
      BLE_MANAGER_PRINTF("Error: Mem alloc error %d@%s\r\n", __LINE__, __FILE__);
      return 0;
    }
    sprintf((*loc_last_custom_command)->command_name, "%s", command_name);
    (*loc_last_custom_command)->next_command = NULL;
#if (BLE_DEBUG_LEVEL>1)
    BLE_MANAGER_PRINTF("Adding Custom Command<%s>\r\n", (*loc_last_custom_command)->command_name);
#endif /* (BLE_DEBUG_LEVEL>1) */
  }

  return valid;
}

/**
  * @brief  Send a new List of Custom Commands for Extended Configuration
  * @param  None
  * @retval None
  */
void send_new_custom_command_list(void)
{
  JSON_Value *temp_json = json_value_init_object();
  JSON_Object *temp_json_obj = json_value_get_object(temp_json);
  JSON_Array *json_sensor_array;
  char *json_string_command = NULL;
  uint32_t json_size = 0;

  BLE_MANAGER_PRINTF("Command send_new_custom_command_list\r\n");

  json_object_dotset_value(temp_json_obj, "CustomCommands", json_value_init_array());
  json_sensor_array = json_object_dotget_array(temp_json_obj, "CustomCommands");

  /* Filling the array */
  ext_config_read_custom_commands_callback(json_sensor_array);

  /* convert to a json string and write as string */
  json_string_command = json_serialize_to_string(temp_json);
  json_size = json_serialization_size(temp_json);

  ble_ext_configuration_update((uint8_t *) json_string_command, json_size);
  BLE_FREE_FUNCTION(json_string_command);
  json_value_free(temp_json);
}

/**
  * @brief  Send one error message
  * @param  char *message error message
  * @retval None
  */
extern void send_error(char *message)
{
  JSON_Value *temp_json = json_value_init_object();
  JSON_Object *temp_json_obj = json_value_get_object(temp_json);
  char *json_string_command = NULL;
  uint32_t json_size = 0;

  BLE_MANAGER_PRINTF("Command send_error\r\n");

  json_object_dotset_string(temp_json_obj, "Error", message);

  /* convert to a json string and write as string */
  json_string_command = json_serialize_to_string(temp_json);
  json_size = json_serialization_size(temp_json);

  ble_ext_configuration_update((uint8_t *) json_string_command, json_size);
  BLE_FREE_FUNCTION(json_string_command);

}

/**
  * @brief  Send one Info message
  * @param  char *message Info message
  * @retval None
  */
void send_info(char *message)
{
  JSON_Value *temp_json = json_value_init_object();
  JSON_Object *temp_json_obj = json_value_get_object(temp_json);
  char *json_string_command = NULL;
  uint32_t json_size = 0;

  BLE_MANAGER_PRINTF("Command send_info\r\n");

  json_object_dotset_string(temp_json_obj, "Info", message);

  /* convert to a json string and write as string */
  json_string_command = json_serialize_to_string(temp_json);
  json_size = json_serialization_size(temp_json);

  ble_ext_configuration_update((uint8_t *) json_string_command, json_size);
  BLE_FREE_FUNCTION(json_string_command);
}

#endif /* BLE_MANAGER_NO_PARSON */

/**
  * @brief  This function is called when there is a change on the gatt attribute as consequence of
  *         write request for the Term service
  * @param  void *void_char_pointer
  * @param  uint16_t attr_handle Handle of the attribute
  * @param  uint16_t offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
  *                          - Bits 0-14: offset of the reported value inside the attribute.
  *                          - Bit 15: if the entire value of the attribute does not fit inside a single
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void write_request_term(void *void_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                               uint8_t *att_data)
{
  /* By default Answer with the same message received */
  uint32_t send_back_data = 1;

  if (ble_stack_value.enable_console)
  {
    /* Received one write from Client on Terminal characteristc */
    send_back_data = debug_console_parsing(att_data, data_length);
  }

  /* Send it back if it's necessary */
  if (send_back_data)
  {
    term_update(att_data, data_length);
  }
}

/**
  * @brief  Add the HW Features service using a vendor specific profile
  * @param  None
  * @retval ble_status_t Status
  */
static ble_status_t ble_manager_add_features_service(void)
{
  ble_status_t ret;

#if (BLUE_CORE != BLUENRG_MS)
  service_uuid_t service_uuid;
  char_uuid_t char_uuid;
#endif /* (BLUE_CORE != BLUENRG_MS) */

  uint8_t ble_char;
  uint8_t uuid[16];
  uint16_t local_service_handle;
  uint8_t number_custom_ble_chars = used_ble_chars - used_standard_ble_chars;

  COPY_FEATURES_SERVICE_UUID(uuid);
#if (BLUE_CORE == BLUENRG_MS)
  ret = aci_gatt_add_serv(UUID_TYPE_128,
                          uuid,
                          PRIMARY_SERVICE,
                          (1U + (3U * number_custom_ble_chars)),
                          &local_service_handle);
#else /* (BLUE_CORE == BLUENRG_MS) */
  BLE_MEM_CPY(&service_uuid.service_uuid_128, uuid, 16);
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
  ret = aci_gatt_add_service(UUID_TYPE_128,
                             &service_uuid,
                             PRIMARY_SERVICE,
                             (1U + (3U * number_custom_ble_chars)),
                             &local_service_handle);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
  ret = aci_gatt_srv_add_service_nwk(UUID_TYPE_128,
                                     &service_uuid,
                                     PRIMARY_SERVICE,
                                     (1U + (3U * number_custom_ble_chars)),
                                     &local_service_handle);
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#endif /* (BLUE_CORE == BLUENRG_MS) */

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    goto end_label;
  }

  for (ble_char = used_standard_ble_chars; ble_char < used_ble_chars; ble_char++)
  {
    ble_chars_array[ble_char]->service_handle = local_service_handle;
#if (BLUE_CORE != BLUENRG_MS)
    BLE_MEM_CPY(&char_uuid.char_uuid_128, ble_chars_array[ble_char]->uuid, 16);
#endif /* (BLUE_CORE != BLUENRG_MS) */

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
    ret =  aci_gatt_add_char(ble_chars_array[ble_char]->service_handle,
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
    ret =  aci_gatt_srv_add_char_nwk(ble_chars_array[ble_char]->service_handle,
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
                             ble_chars_array[ble_char]->char_uuid_type,
#if (BLUE_CORE == BLUENRG_MS)
                             ble_chars_array[ble_char]->uuid,
#else /* (BLUE_CORE == BLUENRG_MS) */
                             &char_uuid,
#endif /* (BLUE_CORE == BLUENRG_MS) */
                             ble_chars_array[ble_char]->char_value_length,
                             ble_chars_array[ble_char]->char_properties,
                             ble_chars_array[ble_char]->security_permissions,
                             ble_chars_array[ble_char]->gatt_evt_mask,
                             ble_chars_array[ble_char]->enc_key_size,
                             ble_chars_array[ble_char]->is_variable,
                             &(ble_chars_array[ble_char]->attr_handle));

    if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
    {
      goto end_label;
    }
  }

end_label:
  return ret;
}

#ifdef ACC_BLUENRG_CONGESTION
static int32_t breath = 0;

/**
  * @brief  Update the value of a characteristic avoiding (for a short time) to
  *         send the next updates if an error in the previous sending has
  *         occurred.
  * @param  ble_char_pointer pointer to the ble_char_object_t for the current ble char
  * @param  char_val_offset The offset of the characteristic
  * @param  char_value_len The length of the characteristic
  * @param  char_value The pointer to the characteristic
  * @retval ble_status_t Status
  */
ble_status_t safe_aci_gatt_update_char_value(ble_char_object_t *ble_char_pointer,
                                             uint8_t char_val_offset,
                                             uint8_t char_value_len,
                                             uint8_t *char_value)
{
  ble_status_t ret = BLE_STATUS_INSUFFICIENT_RESOURCES;
  if (breath == 0)
  {
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
    ret = aci_gatt_update_char_value(ble_char_pointer->service_handle,
                                     ble_char_pointer->attr_handle,
                                     char_val_offset,
                                     char_value_len,
                                     char_value);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#if (BLUE_CORE == STM32WB05N)
    ret = aci_gatt_srv_notify(global_connection_handle,
                              0x0004,
                              ble_char_pointer->attr_handle + 1,
                              GATT_NOTIFICATION,
                              char_value_len,
                              char_value);
#else /* (BLUE_CORE == STM32WB05N) */
    ret = aci_gatt_srv_notify(global_connection_handle,
                              ble_char_pointer->attr_handle + 1,
                              GATT_NOTIFICATION,
                              char_value_len,
                              char_value);
#endif /* (BLUE_CORE == STM32WB05N) */
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

    if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
    {
#if (BLE_DEBUG_LEVEL>2)
      BLE_MANAGER_PRINTF("Error: Updating Char handle=%x ret=%x\r\n", ble_char_pointer->attr_handle, ret);
#endif /* (BLE_DEBUG_LEVEL>2) */
      if (ret == (ble_status_t)BLE_STATUS_INSUFFICIENT_RESOURCES)
      {
#if (BLE_DEBUG_LEVEL>2)
        BLE_MANAGER_PRINTF("Char handle=%x insufficient resources\r\n", ble_char_pointer->attr_handle);
#endif /* (BLE_DEBUG_LEVEL>2) */
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
  * @retval ble_ext_config_command_t
  */
static ble_ext_config_command_t ble_ext_config_extract_command_type(uint8_t *hs_command_buffer)
{
  ble_ext_config_command_t return_code = EXT_CONFIG_COM_NOT_VALID;

  /* Parse the Json for taking the pointer to the command_name */
  JSON_Value *temp_json = json_parse_string((char *) hs_command_buffer);
  JSON_Object *json_Parse_handler = json_value_get_object(temp_json);

  if (json_object_dothas_value(json_Parse_handler, "command"))
  {
    uint8_t search_command = (uint8_t)EXT_CONFIG_COM_READ_COMMAND;
    /* Search the Command */
    while ((return_code == EXT_CONFIG_COM_NOT_VALID) && (search_command < ((uint8_t)EXT_CONFIG_COMMAND_NUMBER)))
    {
      if (strncmp(json_object_dotget_string(json_Parse_handler, "command"),
                  standard_ext_config_commands[search_command].command_string,
                  strlen(json_object_dotget_string(json_Parse_handler, "command"))) == 0)
      {
        return_code = standard_ext_config_commands[search_command].command_type;
      }
      search_command++;
    }
  }

  json_value_free(temp_json);
  return return_code;
}

/**
  * @brief Parse Configuration Command Type
  * @param  uint8_t *hs_command_buffer
  * @retval ble_custom_commad_result_t *command_result
  */
ble_custom_commad_result_t *ask_generic_custom_commands(uint8_t *hs_command_buffer)
{
  ble_custom_commad_result_t *command_result = NULL;

  /* Parse the Json for taking the pointer to the command_name */
  JSON_Value *temp_json = json_parse_string((char *) hs_command_buffer);
  JSON_Object *json_Parse_handler = json_value_get_object(temp_json);

  if (json_object_dothas_value(json_Parse_handler, "command"))
  {
    if (strncmp(json_object_dotget_string(json_Parse_handler, "command"),
                standard_ext_config_commands[EXT_CONFIG_COM_READ_CUSTOM_COMMAND].command_string,
                strlen(json_object_dotget_string(json_Parse_handler, "command"))) == 0)
    {
      /* The User has asked a List of Custom Command */
      command_result = (ble_custom_commad_result_t *) BLE_MALLOC_FUNCTION(sizeof(ble_custom_commad_result_t));
      if (command_result == NULL)
      {
        BLE_MANAGER_PRINTF("Error: Mem alloc error: %d@%s\r\n", __LINE__, __FILE__);
      }
      else
      {
        command_result->command_type = BLE_CUSTOM_COMMAND_VOID;

        command_result->command_name = BLE_MALLOC_FUNCTION(strlen(BLE_MANAGER_READ_CUSTOM_COMMAND) + 1U);
        if ((command_result->command_name) == NULL)
        {
          BLE_MANAGER_PRINTF("Error: Mem alloc error: %d@%s\r\n", __LINE__, __FILE__);
          BLE_FREE_FUNCTION(command_result);
          command_result = NULL;
        }
        else
        {
          sprintf((char *) command_result->command_name, "%s", BLE_MANAGER_READ_CUSTOM_COMMAND);
        }
      }
    }
  }
  json_value_free(temp_json);
  return command_result;
}

/**
  * @brief  Update Extended Configuration characteristic value (when length is <=MaxBLECharLen)
  * @param  uint8_t *data string to write
  * @param  uint8_t length length of string to write
  * @retval ble_status_t      Status
 -*/
static ble_status_t ble_update_ext_conf(uint8_t *data, uint8_t length)
{
  ble_status_t ret;

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_ext_config, 0, length, data);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Bat Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
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
  * @param  uint32_t length length of string to write
  * @retval ble_status_t      Status
  */
ble_status_t ble_ext_configuration_update(uint8_t *data, uint32_t length)
{
  uint32_t tot_len;
  uint32_t index;
  uint8_t *json_string_command_w_tp;
  uint32_t length_w_tp;
  uint32_t len;

  if ((length % 19U) == 0U)
  {
    length_w_tp = (length / 19U) + length;
  }
  else
  {
    length_w_tp = (length / 19U) + 1U + length;
  }

  json_string_command_w_tp = BLE_MALLOC_FUNCTION(sizeof(uint8_t) * length_w_tp);

  if (json_string_command_w_tp == NULL)
  {
    BLE_MANAGER_PRINTF("Error: Mem alloc error [%lu]: %d@%s\r\n", (long)length, __LINE__, __FILE__);
    return BLE_STATUS_ERROR;
  }
  else
  {
    tot_len = ble_command_tp_encapsulate(json_string_command_w_tp, data, length, 20);

    /* Data are sent as notifications*/
    index = 0;
    while (index < tot_len)
    {
      len = MIN(20U, (tot_len - index));
      if (ble_update_ext_conf(json_string_command_w_tp + index, (uint8_t)len) != (ble_status_t)BLE_STATUS_SUCCESS)
      {
        return BLE_STATUS_ERROR;
      }
      BLE_MANAGER_DELAY(20);
      index += len;
    }
    BLE_FREE_FUNCTION(json_string_command_w_tp);
    return BLE_STATUS_SUCCESS;
  }
}
#endif /* BLE_MANAGER_NO_PARSON */

/**
  * @brief Each time BLE FW stack raises the error code @ref ble_status_insufficient_resources (0x64),
the @ref aci_gatt_tx_pool_available_event event is generated as soon as the available buffer size
is greater than maximum ATT MTU (on stack versions below v2.1 this event is generated when at least 2 packets
with MTU of 23 bytes are available).
  * @param connection_handle Connection handle related to the request
  * @param Available_Buffers Not used.
  * @retval None
  */
void aci_gatt_tx_pool_available_event(uint16_t connection_handle,
                                      uint16_t Available_Buffers)
{
#if (BLE_DEBUG_LEVEL>1)
  BLE_MANAGER_PRINTF("aci_gatt_tx_pool_available_event\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */

#ifdef ACC_BLUENRG_CONGESTION
  breath = 0;
#endif /* ACC_BLUENRG_CONGESTION */

  aci_gatt_tx_pool_available_event_function();
}

/**
  * @brief  Update the value of a characteristic
  * @param  ble_char_pointer pointer to the ble_char_object_t for the current ble char
  * @param  char_val_offset The offset of the characteristic
  * @param  char_value_len The length of the characteristic
  * @param  char_value The pointer to the characteristic
  * @retval ble_status_t Status
  */
ble_status_t aci_gatt_update_char_value_wrapper(ble_char_object_t *ble_char_pointer,
                                                uint8_t char_val_offset,
                                                uint8_t char_value_len,
                                                uint8_t *char_value)
{
  ble_status_t ret = BLE_STATUS_INSUFFICIENT_RESOURCES;
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
  ret = aci_gatt_update_char_value(ble_char_pointer->service_handle,
                                   ble_char_pointer->attr_handle,
                                   char_val_offset,
                                   char_value_len,
                                   char_value);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#if (BLUE_CORE == STM32WB05N)
  ret = aci_gatt_srv_notify(global_connection_handle,
                            0x0004,
                            ble_char_pointer->attr_handle + 1,
                            GATT_NOTIFICATION,
                            char_value_len,
                            char_value);
#else /* (BLUE_CORE == STM32WB05N) */
  ret = aci_gatt_srv_notify(global_connection_handle,
                            ble_char_pointer->attr_handle + 1,
                            GATT_NOTIFICATION,
                            char_value_len,
                            char_value);
#endif /* (BLUE_CORE == STM32WB05N) */
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
#if (BLE_DEBUG_LEVEL>2)
    BLE_MANAGER_PRINTF("Error: Updating Char handle=%x ret=%x\r\n", ble_char_pointer->attr_handle, ret);
#endif /* (BLE_DEBUG_LEVEL>2) */
  }
  return ret;
}

/**
  * @brief  Update Stderr characteristic value
  * @param  uint8_t *data string to write
  * @param  uint8_t length length of string to write
  * @retval ble_status_t      Status
  */
ble_status_t std_err_update(uint8_t *data, uint8_t length)
{
  uint8_t offset;
  uint8_t data_to_send;
  /* Split the code in Chunks */
  /* First Chunk */
  data_to_send = (length > max_ble_char_std_err_len) ?  max_ble_char_std_err_len : length;

  /* keep a copy */
  memcpy(last_std_err_buffer, data, data_to_send);
  last_std_err_len = data_to_send;

  if (update_term_std_err(data, data_to_send) != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    return BLE_STATUS_ERROR;
  }

  /* Following Chunks if necessary */
  offset = max_ble_char_std_err_len;
  for (; offset < length; offset += max_ble_char_std_err_len)
  {
    /* Add a Delay respect previous chunk */
    BLE_MANAGER_DELAY(20);

    data_to_send = (length - offset);
    data_to_send = (data_to_send > max_ble_char_std_err_len) ?  max_ble_char_std_err_len : data_to_send;

    /* keep a copy */
    memcpy(last_std_err_buffer, data + offset, data_to_send);
    last_std_err_len = data_to_send;

    if (update_term_std_err(data + offset, data_to_send) != (ble_status_t)BLE_STATUS_SUCCESS)
    {
      return BLE_STATUS_ERROR;
    }
  }
  return BLE_STATUS_SUCCESS;
}

/**
  * @brief
  * @param  uint8_t* buffer
  * @param  uint8_t len
  * @retval ble_status_t   Status
  */
ble_status_t ble_std_out_send_buffer(uint8_t *buffer, uint8_t len)
{
  ble_status_t ret;

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
  ret = aci_gatt_update_char_value(ble_char_std_out.service_handle, ble_char_std_out.attr_handle, 0, len, buffer);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#if (BLUE_CORE == STM32WB05N)
  ret = aci_gatt_srv_notify(global_connection_handle, 0x0004,
                            ble_char_std_out.attr_handle + 1, GATT_NOTIFICATION, len, buffer);
#else /* (BLUE_CORE == STM32WB05N) */
  ret = aci_gatt_srv_notify(global_connection_handle, ble_char_std_out.attr_handle + 1, GATT_NOTIFICATION, len, buffer);
#endif /* (BLUE_CORE == STM32WB05N) */
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

  return ret;
}

/**
  * @brief  Update Terminal characteristic value
  * @param  uint8_t *data string to write
  * @param  uint8_t length length of string to write
  * @retval ble_status_t      Status
  */
ble_status_t term_update(uint8_t *data, uint8_t length)
{
  uint8_t   offset;
  uint8_t   data_to_send;

  /* Split the code in Chunks */
  /* First Chunk */
  data_to_send = (length > max_ble_char_std_out_len) ?  max_ble_char_std_out_len : length;

  /* keep a copy */
  memcpy(last_term_buffer, data, data_to_send);
  last_term_len = data_to_send;

  if (update_term_std_out(data, data_to_send) != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    return BLE_STATUS_ERROR;
  }

  /* Following Chunks if necessary */
  offset = max_ble_char_std_out_len;
  for (; offset < length; offset += max_ble_char_std_out_len)
  {

    /* Add a Delay respect previous chunk */
    BLE_MANAGER_DELAY(20);

    data_to_send = (length - offset);
    data_to_send = (data_to_send > max_ble_char_std_out_len) ?  max_ble_char_std_out_len : data_to_send;

    /* keep a copy */
    memcpy(last_term_buffer, data + offset, data_to_send);
    last_term_len = data_to_send;

    if (update_term_std_out(data + offset, data_to_send) != (ble_status_t)BLE_STATUS_SUCCESS)
    {
      return BLE_STATUS_ERROR;
    }
  }
  return BLE_STATUS_SUCCESS;
}

/* @brief  Send a BLE notification for answering to a configuration command for Accelerometer events
* @param  uint32_t feature Feature type
* @param  uint8_t Command Replay to this Command
* @param  uint8_t data result to send back
* @retval ble_status_t Status
*/
ble_status_t config_update(uint32_t feature, uint8_t command, uint8_t data)
{
  uint8_t buff[2 + 4 + 1 + 1];

  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  STORE_BE_32(buff + 2, feature);
  buff[6] = command;
  buff[7] = data;

  if (ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_config, 0, 8, buff) != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("Error: Updating Configuration Char\r\n");
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;

}

/* @brief  Send a BLE notification for answering to a configuration command for Accelerometer events
* @param  uint32_t feature Feature type
* @param  uint8_t command Replay to this Command
* @param  uint32_t data result to send back
* @retval ble_status_t Status
*/
ble_status_t config_update_32(uint32_t feature, uint8_t command, uint32_t data)
{
  uint8_t buff[2 + 4 + 1 + 4];

  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  STORE_BE_32(buff + 2, feature);
  buff[6] = command;
  buff[7]  = (uint8_t)((data) & 0xFFU);
  buff[8]  = (uint8_t)((data >>  8U) & 0xFFU);
  buff[9]  = (uint8_t)((data >> 16U) & 0xFFU);
  buff[10] = (uint8_t)((data >> 24U) & 0xFFU);

  if (ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_config, 0, 11, buff) != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("Error: Updating Configuration Char\r\n");
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}


#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
/**
  * @brief  Update the Advertise Data
  * @param  None
  * @retval None
  */
void update_adv_data()
{
  /* Filling Manufacter Advertise data */
  manuf_data[0 ] = 8U;
  manuf_data[1 ] = 0x09U;
  manuf_data[2 ] = (uint8_t)ble_stack_value.board_name[0];/* Complete Name */
  manuf_data[3 ] = (uint8_t)ble_stack_value.board_name[1];
  manuf_data[4 ] = (uint8_t)ble_stack_value.board_name[2];
  manuf_data[5 ] = (uint8_t)ble_stack_value.board_name[3];
  manuf_data[6 ] = (uint8_t)ble_stack_value.board_name[4];
  manuf_data[7 ] = (uint8_t)ble_stack_value.board_name[5];
  manuf_data[8 ] = (uint8_t)ble_stack_value.board_name[6];
  manuf_data[9 ] = 15U;
  manuf_data[10] = 0xFFU;
  manuf_data[11] = 0x30U;/* STM Manufacter AD */
  manuf_data[12] = 0x00U;
#ifdef BLE_MANAGER_SDKV2
  manuf_data[13] = 0x02U;
#else /* BLE_MANAGER_SDKV2 */
  manuf_data[13] = 0x01U;
#endif /* BLE_MANAGER_SDKV2 */
  manuf_data[14] = ble_stack_value.board_id; /* BoardType */
  manuf_data[15] = 0x00U;
  manuf_data[16] = 0x00U;
  manuf_data[17] = 0x00U;
  manuf_data[18] = 0x00U;
  manuf_data[19] = ble_stack_value.ble_mac_address[5]; /* BLE MAC start */
  manuf_data[20] = ble_stack_value.ble_mac_address[4];
  manuf_data[21] = ble_stack_value.ble_mac_address[3];
  manuf_data[22] = ble_stack_value.ble_mac_address[2];
  manuf_data[23] = ble_stack_value.ble_mac_address[1];
  manuf_data[24] = ble_stack_value.ble_mac_address[0]; /* BLE MAC stop */

  /* Set the Custom BLE Advertise Data */
  ble_set_custom_advertise_data(manuf_data);

  set_connectable_function(manuf_data);

  /* Send Advertising data */
  ble_status_t ret_status = aci_gap_update_adv_data(BLE_MANAGER_ADVERTISE_DATA_LENGHT, manuf_data);
  if (ret_status != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("Error: aci_gap_update_adv_data [%x]\r\n", ret_status);
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gap_update_adv_data OK\r\n");
  }
}

/**
  * @brief  Puts the device in connectable mode.
  * @param  None
  * @retval None
  */
void set_connectable_ble(void)
{
#if (BLUE_CORE == BLUENRG_MS)
  char local_name[8] = {AD_TYPE_COMPLETE_LOCAL_NAME,
#else /* (BLUE_CORE == BLUENRG_MS) */
  uint8_t local_name[8] = {AD_TYPE_COMPLETE_LOCAL_NAME,
#endif /* (BLUE_CORE == BLUENRG_MS) */
                        ble_stack_value.board_name[0],
                        ble_stack_value.board_name[1],
                        ble_stack_value.board_name[2],
                        ble_stack_value.board_name[3],
                        ble_stack_value.board_name[4],
                        ble_stack_value.board_name[5],
                        ble_stack_value.board_name[6]
                       };
  ble_status_t ret_status = BLE_STATUS_SUCCESS;

  /* disable scan response */
#if (BLUE_CORE == BLUENRG_MS)
  ret_status = HCI_LE_SET_SCAN_RESPONSE_DATA(0U, NULL);
#else /* (BLUE_CORE == BLUENRG_MS) */
  ret_status = hci_le_set_scan_response_data(0U, NULL);
#endif /* (BLUE_CORE == BLUENRG_MS) */
  if (ret_status != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("Error: HCI_LE_SET_SCAN_RESPONSE_DATA [%x]\r\n", ret_status);
    goto end_label;
  }

  /* Set the board discoverable */
  if (ble_stack_value.advertising_filter == ((uint8_t)NO_WHITE_LIST_USE))
  {
    ret_status = aci_gap_set_discoverable(ADV_IND, ble_stack_value.adv_interval_min, ble_stack_value.adv_interval_max,
                                          ble_stack_value.own_address_type,
                                          ble_stack_value.advertising_filter,
                                          (uint8_t)(sizeof(local_name)), local_name, 0, NULL, 0, 0);
    if (ret_status != (ble_status_t)BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("Error: aci_gap_set_discoverable [%x] Filter=%x\r\n",
                         ret_status, ble_stack_value.advertising_filter);
      goto end_label;
    }
    else
    {
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("aci_gap_set_discoverable OK Filter=%x\r\n", ble_stack_value.advertising_filter);
#endif /* (BLE_DEBUG_LEVEL>1) */
    }
  }
  else
  {
    /* Advertising filter is enabled: enter in undirected connectable mode
     in order to use the advertising filter on bonded device */
#if (BLUE_CORE == BLUENRG_MS)
    ret_status = aci_gap_set_undirected_connectable(ble_stack_value.own_address_type,
                                                    ble_stack_value.advertising_filter);
#else /* (BLUE_CORE == BLUENRG_MS) */
    ret_status = aci_gap_set_undirected_connectable(0,
                                                    0,
                                                    ble_stack_value.own_address_type,
                                                    ble_stack_value.advertising_filter);
#endif /* (BLUE_CORE == BLUENRG_MS) */
    if (ret_status != (ble_status_t)BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("Error: aci_gap_set_undirected_connectable [%x] Filter=%x\r\n", ret_status,
                         ble_stack_value.advertising_filter);
      goto end_label;
    }
    else
    {
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("aci_gap_set_undirected_connectable OK Filter=%x\r\n", ble_stack_value.advertising_filter);
#endif /* (BLE_DEBUG_LEVEL>1) */
    }
  }

  update_adv_data();

end_label:
  return;
}
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#if (BLUE_CORE == STM32WB05N)
/**
  * @brief  Puts the device in connectable mode.
  * @param  None
  * @retval None
  */
void set_connectable_ble(void)
{
  ble_status_t ret;
  Advertising_Set_Parameters_t advertising_set_parameters[1];

  manuf_data[0 ] = 0x02;
  manuf_data[1 ] = AD_TYPE_FLAGS;
  manuf_data[2 ] = FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED;
  manuf_data[3 ] = 8;
  manuf_data[4 ] = 0x09;
  manuf_data[5 ] = ble_stack_value.board_name[0]; /* Complete Name */
  manuf_data[6 ] = ble_stack_value.board_name[1];
  manuf_data[7 ] = ble_stack_value.board_name[2];
  manuf_data[8 ] = ble_stack_value.board_name[3];
  manuf_data[9 ] = ble_stack_value.board_name[4];
  manuf_data[10] = ble_stack_value.board_name[5];
  manuf_data[11] = ble_stack_value.board_name[6];
  manuf_data[12] = 15;
  manuf_data[13] = 0xFF;
  manuf_data[14] = 0x30;
  manuf_data[15] = 0x00; /* STM Manufacter AD */
  manuf_data[16] = 0x02; /* SDK version */
  manuf_data[17] = ble_stack_value.board_id; /* BoardType*/
  manuf_data[18] = 0x00; /* Fw ID*/
  manuf_data[19] = 0x00; /* Second Custom Byte -> Not Used */
  manuf_data[20] = 0x00; /* Third  Custom Byte -> Not Used */
  manuf_data[21] = 0x00; /* Fourth Custom Byte -> Not Used */
  manuf_data[22] = ble_stack_value.ble_mac_address[5]; /* BLE MAC start - MSB first - */
  manuf_data[23] = ble_stack_value.ble_mac_address[4];
  manuf_data[24] = ble_stack_value.ble_mac_address[3];
  manuf_data[25] = ble_stack_value.ble_mac_address[2];
  manuf_data[26] = ble_stack_value.ble_mac_address[1];
  manuf_data[27] = ble_stack_value.ble_mac_address[0]; /* BLE MAC stop */

  /* Set the Custom BLE Advertise Data */
  ble_set_custom_advertise_data(manuf_data);

  set_connectable_function(manuf_data);

  ret = aci_gap_set_advertising_configuration(0, GAP_MODE_GENERAL_DISCOVERABLE,
                                              HCI_ADV_EVENT_PROP_CONNECTABLE |
                                              HCI_ADV_EVENT_PROP_SCANNABLE |
                                              HCI_ADV_EVENT_PROP_LEGACY,
                                              ADV_INTERV_MIN,
                                              ADV_INTERV_MAX,
                                              HCI_ADV_CH_ALL,
                                              HCI_ADDR_STATIC_RANDOM_ADDR, NULL,
                                              HCI_INIT_FILTER_ACCEPT_LIST_NONE,
                                              0, /* 0 dBm */
                                              HCI_ADV_PHY_LE_1M, /* Primary advertising PHY */
                                              0, /* 0 skips */
                                              HCI_ADV_PHY_LE_1M, /* Secondary advertising PHY.
                                                                    Not used with legacy advertising. */
                                              0, /* SID */
                                              0 /* No scan request notifications */);

  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_configuration failed: 0x%02x\r\n", ret);
    goto end_label;
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_configuration\r\n");
  }

  ret = aci_gap_set_advertising_data_nwk(0, ADV_COMPLETE_DATA, BLE_MANAGER_ADVERTISE_DATA_LENGHT, manuf_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_data_nwk failed: 0x%02x\r\n", ret);
    goto end_label;
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_data_nwk\r\n");
  }

  advertising_set_parameters[0].Advertising_Handle = 0;
  advertising_set_parameters[0].Duration = 0;
  advertising_set_parameters[0].Max_Extended_Advertising_Events = 0;

  /* enable advertising */
  ret = aci_gap_set_advertising_enable(ENABLE, 1, advertising_set_parameters);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_enable failed: 0x%02x\r\n", ret);
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_enable\r\n");
  }

end_label:
  return;
}

/**
  * @brief  Update Advertise Data
  * @param  None
  * @retval None
  */

void update_adv_data()
{
  ble_status_t ret;
  /* Set the Custom BLE Advertise Data */
  ble_set_custom_advertise_data(manuf_data);

  ret = aci_gap_set_advertising_data_nwk(0, ADV_COMPLETE_DATA, BLE_MANAGER_ADVERTISE_DATA_LENGHT, manuf_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_data_nwk failed: 0x%02x\r\n", ret);
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_data_nwk\r\n");
  }

  return;
}
#else /* (BLUE_CORE == STM32WB05N) */
/**
  * @brief  Puts the device in connectable mode.
  * @param  None
  * @retval None
  */
void set_connectable_ble(void)
{
  ble_status_t ret;
  advertising_set_parameters_t advertising_set_parameters[1];

  manuf_data[0 ] = 0x02;
  manuf_data[1 ] = AD_TYPE_FLAGS;
  manuf_data[2 ] = FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED;
  manuf_data[3 ] = 8;
  manuf_data[4 ] = 0x09;
  manuf_data[5 ] = ble_stack_value.board_name[0]; /* Complete Name */
  manuf_data[6 ] = ble_stack_value.board_name[1];
  manuf_data[7 ] = ble_stack_value.board_name[2];
  manuf_data[8 ] = ble_stack_value.board_name[3];
  manuf_data[9 ] = ble_stack_value.board_name[4];
  manuf_data[10] = ble_stack_value.board_name[5];
  manuf_data[11] = ble_stack_value.board_name[6];
  manuf_data[12] = 15;
  manuf_data[13] = 0xFF;
  manuf_data[14] = 0x30;
  manuf_data[15] = 0x00; /* STM Manufacter AD */
  manuf_data[16] = 0x02; /* SDK version */
  manuf_data[17] = ble_stack_value.board_id; /* BoardType*/
  manuf_data[18] = 0x00; /* Fw ID*/
  manuf_data[19] = 0x00; /* Second Custom Byte -> Not Used */
  manuf_data[20] = 0x00; /* Third  Custom Byte -> Not Used */
  manuf_data[21] = 0x00; /* Fourth Custom Byte -> Not Used */
  manuf_data[22] = ble_stack_value.ble_mac_address[5]; /* BLE MAC start - MSB first - */
  manuf_data[23] = ble_stack_value.ble_mac_address[4];
  manuf_data[24] = ble_stack_value.ble_mac_address[3];
  manuf_data[25] = ble_stack_value.ble_mac_address[2];
  manuf_data[26] = ble_stack_value.ble_mac_address[1];
  manuf_data[27] = ble_stack_value.ble_mac_address[0]; /* BLE MAC stop */

  /* Set the Custom BLE Advertise Data */
  ble_set_custom_advertise_data(manuf_data);

  set_connectable_function(manuf_data);

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
                                              LE_1M_PHY, /* Secondary advertising PHY. Not used with legacy adv. */
                                              0, /* SID */
                                              0 /* No scan request notifications */);

  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_configuration failed: 0x%02x\r\n", ret);
    goto end_label;
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_configuration\r\n");
  }

  ret = aci_gap_set_advertising_data_nwk(0, ADV_COMPLETE_DATA, BLE_MANAGER_ADVERTISE_DATA_LENGHT, manuf_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_data_nwk failed: 0x%02x\r\n", ret);
    goto end_label;
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_data_nwk\r\n");
  }

  advertising_set_parameters[0].advertising_handle = 0;
  advertising_set_parameters[0].duration = 0;
  advertising_set_parameters[0].max_extended_advertising_events = 0;

  /* enable advertising */
  ret = aci_gap_set_advertising_enable(ENABLE, 1, advertising_set_parameters);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_enable failed: 0x%02x\r\n", ret);
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_enable\r\n");
  }

end_label:
  return;
}

/**
  * @brief  Update Advertise Data
  * @param  None
  * @retval None
  */

void update_adv_data()
{
  ble_status_t ret;
  /* Set the Custom BLE Advertise Data */
  ble_set_custom_advertise_data(manuf_data);

  ret = aci_gap_set_advertising_data_nwk(0, ADV_COMPLETE_DATA, BLE_MANAGER_ADVERTISE_DATA_LENGHT, manuf_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_data_nwk failed: 0x%02x\r\n", ret);
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_data_nwk\r\n");
  }

  return;
}
#endif /* (BLUE_CORE == STM32WB05N) */
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
/**
  * @brief  Exits the device from connectable mode.
  * @param  None
  * @retval None
  */
void set_not_connectable_ble(void)
{
  aci_gap_set_non_discoverable();
}
#else  /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#if (BLUE_CORE == STM32WB05N)
void set_not_connectable_ble(void)
{
  ble_status_t ret;
  Advertising_Set_Parameters_t advertising_set_parameters[1];
  advertising_set_parameters[0].Advertising_Handle = 0;
  advertising_set_parameters[0].Duration = 0;
  advertising_set_parameters[0].Max_Extended_Advertising_Events = 0;

  /* Disable advertising */
  ret = aci_gap_set_advertising_enable(DISABLE, 1, advertising_set_parameters);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_enable failed: 0x%02x\r\n", ret);
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_enable\r\n");
  }
}
#else /* (BLUE_CORE == STM32WB05N) */
void set_not_connectable_ble(void)
{
  ble_status_t ret;
  advertising_set_parameters_t advertising_set_parameters[1];
  advertising_set_parameters[0].advertising_handle = 0;
  advertising_set_parameters[0].duration = 0;
  advertising_set_parameters[0].max_extended_advertising_events = 0;

  /* Disable advertising */
  ret = aci_gap_set_advertising_enable(DISABLE, 1, advertising_set_parameters);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_enable failed: 0x%02x\r\n", ret);
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gap_set_advertising_enable\r\n");
  }
}
#endif /* (BLUE_CORE == STM32WB05N) */
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

/**
  * @brief  Added BLE service
  * @param  ble_char_object_t *ble_char: Data structure pointer for BLE service
  * @retval 1 in case of success
  */
int32_t ble_manager_add_char(ble_char_object_t *ble_char)
{
  int32_t ret_value = 0;

  if (ble_char != NULL)
  {
    if (used_ble_chars < (uint8_t)BLE_MANAGER_MAX_ALLOCABLE_CHARS)
    {
      ble_chars_array[used_ble_chars] = ble_char;
      used_ble_chars++;
      ret_value = 1;
    }
  }

  return ret_value;
}

/**
  * @brief  Init Ble Manager
  * @param  None
  * @retval ble_status_t Status
  */
ble_status_t init_ble_manager(void)
{
  ble_status_t ret = BLE_STATUS_SUCCESS;

  if (ble_stack_value.board_id == 0U)
  {
    BLE_MANAGER_PRINTF("Error ble_stack_value.board_id Not Defined\r\n");
    return BLE_ERROR_UNSPECIFIED;
  }

  ble_conf_service = BLE_SERV_NOT_ENABLE;
  ble_std_term_service = BLE_SERV_NOT_ENABLE;
  ble_std_err_service = BLE_SERV_NOT_ENABLE;
  ble_ext_conf_service = BLE_SERV_NOT_ENABLE;

  used_ble_chars = 0;
  used_standard_ble_chars = 0;
  global_connection_handle = 0;
  set_connectable = FALSE;
  max_ble_char_std_out_len = DEFAULT_MAX_STDOUT_CHAR_LEN;
  max_ble_char_std_err_len = DEFAULT_MAX_STDERR_CHAR_LEN;

#if (BLUE_CORE != BLUE_WB)
  /* BLE stack initialization */
  ret = init_ble_manager_ble_stack();
#endif /* (BLUE_CORE != BLUE_WB) */

#ifndef BLE_MANAGER_NO_PARSON
  CLEAR_CUSTOM_COMMANDS_LIST();
#endif /* BLE_MANAGER_NO_PARSON */

  if (ret == (ble_status_t)BLE_STATUS_SUCCESS)
  {
    /* Ble Manager services initialization */
    ret = init_ble_manager_services();
  }

  set_connectable = TRUE;

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
void hci_event_cb(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt *)hci_pckt->data;

  if (hci_pckt->type != (uint8_t)HCI_EVENT_PKT)
  {
    return;
  }

  switch (event_pckt->evt)
  {

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

      switch (evt->subevent)
      {
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
          evt_le_connection_update_complete *con_update = (evt_le_connection_update_complete *) evt->data;
#if (BLE_DEBUG_LEVEL>1)
          BLE_MANAGER_PRINTF("EVT_LE_CONN_UPDATE_COMPLETE status=%d\r\n", con_update->status);
#else /* (BLE_DEBUG_LEVEL>1) */
          BLE_UNUSED(con_update);
#endif /* (BLE_DEBUG_LEVEL>1) */
          break;
        }
      }
      break;
    }
    case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void *)event_pckt->data;

      switch (blue_evt->ecode)
      {
        case EVT_BLUE_GATT_READ_PERMIT_REQ:
        {
          evt_gatt_read_permit_req *pr = (void *)blue_evt->data;
          aci_gatt_read_permit_req_event(pr->conn_handle,
                                         pr->attr_handle,
                                         pr->offset);
          break;
        }
        case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
        {
#if (BLE_DEBUG_LEVEL>2)
          BLE_MANAGER_PRINTF("EVT_BLUE_GATT_ATTRIBUTE_MODIFIED\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */
          evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1 *)blue_evt->data;
          aci_gatt_attribute_modified_event(evt->conn_handle,
                                            evt->attr_handle,
                                            evt->offset,
                                            evt->data_length,
                                            evt->att_data);
          break;
        }
        case EVT_BLUE_GAP_AUTHORIZATION_REQUEST:
#if (BLE_DEBUG_LEVEL>2)
          BLE_MANAGER_PRINTF("EVT_BLUE_GAP_AUTHORIZATION_REQUEST\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */
          break;
        case EVT_BLUE_GAP_PASS_KEY_REQUEST:
        {
#if (BLE_DEBUG_LEVEL>2)
          BLE_MANAGER_PRINTF("EVT_BLUE_GAP_PASS_KEY_REQUEST\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */

          evt_gap_pass_key_req *kr = (void *)blue_evt->data;
          aci_gap_pass_key_req_event(kr->conn_handle);
          break;
        }
        case EVT_BLUE_GAP_PAIRING_CMPLT:
        {
          evt_gap_pairing_cmplt *pairing = (evt_gap_pairing_cmplt *)blue_evt->data;
#if (BLE_DEBUG_LEVEL>2)
          BLE_MANAGER_PRINTF("EVT_BLUE_GAP_PAIRING_CMPLT\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */
          aci_gap_pairing_complete_event(pairing->conn_handle, pairing->status, 0);
          break;
        }
        case EVT_BLUE_GAP_SLAVE_SECURITY_INITIATED:
          BLE_MANAGER_PRINTF("EVT_BLUE_GAP_SLAVE_SECURITY_INITIATED\r\n");
          break;
        case EVT_BLUE_GAP_BOND_LOST:
        {
          ble_status_t ret;
          evt_gatt_procedure_timeout *evt = (evt_gatt_procedure_timeout *)blue_evt->data;
#if (BLE_DEBUG_LEVEL>2)
          BLE_MANAGER_PRINTF("EVT_BLUE_GAP_BOND_LOST\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */
          ret = aci_gap_allow_rebond_IDB05A1(evt->conn_handle);
          if (ret != (uint8_t)BLE_STATUS_SUCCESS)
          {
            BLE_MANAGER_PRINTF("\r\nGAP allow Rebound failed\r\n");
          }
          break;
        }
        case EVT_BLUE_ATT_EXCHANGE_MTU_RESP:
        {
#if (BLE_DEBUG_LEVEL>2)
          BLE_MANAGER_PRINTF("EVT_BLUE_ATT_EXCHANGE_MTU_RESP\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */
          evt_att_exchange_mtu_resp *mtu = (evt_att_exchange_mtu_resp *)blue_evt->data;
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
      BLE_UNUSED(encrypt);
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("EVT_ENCRYPT_CHANGE\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
      break;
    }
  }
}

/** @brief Initialize the BLE Stack
  * @param None
  * @retval ble_status_t
  */
static ble_status_t init_ble_manager_ble_stack(void)
{
  uint16_t local_service_handle;
  uint16_t dev_name_char_handle;
  uint16_t appearance_char_handle;
  ble_status_t ret;
  uint8_t data_len_out;
  uint8_t random_number[8];

  /* Initialize the BlueNRG HCI */
  hci_init(hci_event_cb, NULL);

  /* we will let the BLE chip to use its Random MAC address */
  ret = aci_hal_read_config_data(CONFIG_DATA_RANDOM_ADDRESS, 6, &data_len_out, ble_stack_value.ble_mac_address);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nReading  Random BD_ADDR failed\r\n");
    goto fail;
  }

  if ((ble_stack_value.ble_mac_address[5] & 0xC0U) != 0xC0U)
  {
    BLE_MANAGER_PRINTF("\r\nStatic Random address not well formed.\r\n");
    goto fail;
  }

  /* Generate Random Key at every boot */
  if (ble_stack_value.enable_random_secure_pin)
  {
    ble_stack_value.secure_pin = STARTING_NUMBER_SECURE_PIN_GENERATION;

    /* get a random number from BlueNRG-1 */
    if (hci_le_rand(random_number) != BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("hci_le_rand() call failed\r\n");
    }

    /* setup random_key with random number */
    for (uint8_t i = 0; i < 8U; i++)
    {
      ble_stack_value.secure_pin += (435U * ((uint32_t)random_number[i]));
    }

    /* Control section because we need 6 digits */
    if (ble_stack_value.secure_pin < 99999U)
    {
      ble_stack_value.secure_pin += 100000U;
    }
  }

  ret = aci_hal_write_config_data(ble_stack_value.config_value_offsets,
                                  ble_stack_value.config_value_length,
                                  ble_stack_value.ble_mac_address);

  /* Sw reset of the device */
  hci_reset();

  ret = aci_gatt_init();
  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nGATT_Init failed\r\n");
    goto fail;
  }

  aci_gap_clear_security_database();

  ret = aci_gap_init_IDB05A1(ble_stack_value.gap_roles,
                             0,
                             (uint8_t) strlen(ble_stack_value.board_name),
                             &local_service_handle,
                             &dev_name_char_handle,
                             &appearance_char_handle);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nGAP_Init failed\r\n");
    goto fail;
  }

  ret = aci_gatt_update_char_value(local_service_handle, dev_name_char_handle, 0,
                                   (uint8_t) strlen(ble_stack_value.board_name), (uint8_t *)ble_stack_value.board_name);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\naci_gatt_update_char_value failed\r\n");
    goto fail;
  }

  if (ble_stack_value.enable_secure_connection)
  {
    /* Set the I/O capability  Otherwise the Smartphone will propose a Pin
    * that will be acepted without any control */
    if (aci_gap_set_io_capability(ble_stack_value.io_capabilities) == (ble_status_t)BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("I/O Capability Configurated\r\n");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Setting I/O Capability\r\n");
    }

    if (ble_stack_value.enable_random_secure_pin)
    {
      ret = aci_gap_set_auth_requirement(ble_stack_value.mitm_protection_requirements,
                                         ble_stack_value.out_of_band_enable_data,
                                         NULL,
                                         7,
                                         16,
                                         DONOT_USE_FIXED_PIN_FOR_PAIRING,
                                         ble_stack_value.secure_pin,
                                         ble_stack_value.authentication_requirements);
    }
    else
    {
      ret = aci_gap_set_auth_requirement(ble_stack_value.mitm_protection_requirements,
                                         ble_stack_value.out_of_band_enable_data,
                                         NULL,
                                         7,
                                         16,
                                         USE_FIXED_PIN_FOR_PAIRING,
                                         ble_stack_value.secure_pin,
                                         ble_stack_value.authentication_requirements);
    }

  }
  else
  {
    ret = aci_gap_set_auth_requirement(ble_stack_value.mitm_protection_requirements,
                                       ble_stack_value.out_of_band_enable_data,
                                       NULL,
                                       7,
                                       16,
                                       USE_FIXED_PIN_FOR_PAIRING,
                                       ble_stack_value.secure_pin,
                                       ble_stack_value.authentication_requirements);
  }

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nGAP setting Authentication failed\r\n");
    goto fail;
  }

  BLE_MANAGER_PRINTF("\r\nSERVER: BLE Stack Initialized \r\n"
                     "\t\tBoardName= %s\r\n"
                     "\t\tBoardMAC = %x:%x:%x:%x:%x:%x\r\n",
                     ble_stack_value.board_name,
                     ble_stack_value.ble_mac_address[5],
                     ble_stack_value.ble_mac_address[4],
                     ble_stack_value.ble_mac_address[3],
                     ble_stack_value.ble_mac_address[2],
                     ble_stack_value.ble_mac_address[1],
                     ble_stack_value.ble_mac_address[0]);

  if (ble_stack_value.enable_secure_connection)
  {
    BLE_MANAGER_PRINTF("\t-->ONLY SECURE CONNECTION<--\r\n");

    if (ble_stack_value.enable_random_secure_pin)
    {
      BLE_MANAGER_PRINTF("\t\tRandom Key = %ld\r\n", ble_stack_value.secure_pin);
    }
    else
    {
      BLE_MANAGER_PRINTF("\t\tFixed  Key = %ld\r\n", ble_stack_value.secure_pin);
    }
  }

  /* Set output power level */
  aci_hal_set_tx_power_level(ble_stack_value.enable_high_power_mode,
                             ble_stack_value.power_amplifier_output_level);

fail:
  return ret;
}
#else /* (BLUE_CORE == BLUENRG_MS) */

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
#if (BLUE_CORE != BLUE_WB)
/** @brief HCI Transport layer user function
  * @param void *pData pointer to HCI event data
  * @retval None
  */
static void app_user_evt_rx(void *pData)
{
  uint32_t i;

  hci_spi_pckt *hci_pckt = (hci_spi_pckt *)pData;

  if (hci_pckt->type == (uint8_t)HCI_EVENT_PKT)
  {
    hci_event_pckt *event_pckt = (hci_event_pckt *)hci_pckt->data;

    if (event_pckt->evt == (uint8_t)EVT_LE_META_EVENT)
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;

      for (i = 0; i < (sizeof(hci_le_meta_events_table) / sizeof(hci_le_meta_events_table_type_t)); i++)
      {
        if (evt->subevent == hci_le_meta_events_table[i].evt_code)
        {
          hci_le_meta_events_table[i].process((void *)evt->data);
        }
      }
    }
    else if (event_pckt->evt == (uint8_t)EVT_VENDOR)
    {
      evt_blue_aci *blue_evt = (void *)event_pckt->data;

      for (i = 0; i < (sizeof(hci_vendor_specific_events_table) / sizeof(hci_vendor_specific_events_table_type_t)); i++)
      {
        if (blue_evt->ecode == hci_vendor_specific_events_table[i].evt_code)
        {
          hci_vendor_specific_events_table[i].process((void *)blue_evt->data);
        }
      }
    }
    else
    {
      for (i = 0; i < (sizeof(hci_events_table) / sizeof(hci_events_table_type_t)); i++)
      {
        if (event_pckt->evt == hci_events_table[i].evt_code)
        {
          hci_events_table[i].process((void *)event_pckt->data);
        }
      }
    }
  }
}
#endif /* (BLUE_CORE != BLUE_WB) */

#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#if (BLUE_CORE == STM32WB05N)
/**
  * @brief  Callback processing the ACI events
  * @note   Inside this function each event must be identified and correctly
  *         parsed
  * @param  void* Pointer to the ACI packet
  * @retval None
  */
static void app_user_evt_rx(void *pData)
{
  uint32_t i;

  hci_spi_pckt *hci_pckt = (hci_spi_pckt *)pData;

  if (hci_pckt->type == HCI_EVENT_PKT || hci_pckt->type == HCI_EVENT_EXT_PKT)
  {
    void *data;
    hci_event_pckt *event_pckt = (hci_event_pckt *)hci_pckt->data;

    if (hci_pckt->type == HCI_EVENT_PKT)
    {
      data = event_pckt->data;
    }
    else
    {
      hci_event_ext_pckt *event_pckt = (hci_event_ext_pckt *)hci_pckt->data;
      data = event_pckt->data;
    }

    if (event_pckt->evt == EVT_LE_META_EVENT)
    {
      evt_le_meta_event *evt = data;

      for (i = 0; i < (sizeof(hci_le_meta_events_table) / sizeof(hci_le_meta_events_table_type)); i++)
      {
        if (evt->subevent == hci_le_meta_events_table[i].evt_code)
        {
          hci_le_meta_events_table[i].process((void *)evt->data);
          break;
        }
      }
    }
    else if (event_pckt->evt == EVT_VENDOR)
    {
      evt_blue_aci *blue_evt = data;

      for (i = 0; i < (sizeof(hci_vendor_specific_events_table) / sizeof(hci_vendor_specific_events_table_type)); i++)
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
      for (i = 0; i < (sizeof(hci_events_table) / sizeof(hci_events_table_type)); i++)
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
#else /* (BLUE_CORE == STM32WB05N) */
/**
  * @brief  Callback processing the ACI events
  * @note   Inside this function each event must be identified and correctly
  *         parsed
  * @param  void* Pointer to the ACI packet
  * @retval None
  */
static void app_user_evt_rx(void *pData)
{
  uint32_t i;

  hci_spi_pckt *hci_pckt = (hci_spi_pckt *)pData;

  if (hci_pckt->type == HCI_EVENT_PKT || hci_pckt->type == HCI_EVENT_EXT_PKT)
  {
    void *data;
    hci_event_pckt *event_pckt = (hci_event_pckt *)hci_pckt->data;

    if (hci_pckt->type == HCI_EVENT_PKT)
    {
      data = event_pckt->data;
    }
    else
    {
      hci_event_ext_pckt *event_pckt = (hci_event_ext_pckt *)hci_pckt->data;
      data = event_pckt->data;
    }

    if (event_pckt->evt == EVT_LE_META_EVENT)
    {
      evt_le_meta_event *evt = data;

      for (i = 0; i < (sizeof(hci_le_meta_events_table) / sizeof(hci_le_meta_events_table_type_t)); i++)
      {
        if (evt->subevent == hci_le_meta_events_table[i].evt_code)
        {
          hci_le_meta_events_table[i].process((void *)evt->data);
          break;
        }
      }
    }
    else if (event_pckt->evt == EVT_VENDOR)
    {
      evt_blue_aci *blue_evt = data;

      for (i = 0; i < (sizeof(hci_vendor_specific_events_table) / sizeof(hci_vendor_specific_events_table_type_t)); i++)
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
      for (i = 0; i < (sizeof(hci_events_table) / sizeof(hci_events_table_type_t)); i++)
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
#endif /* (BLUE_CORE == STM32WB05N) */
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

#if ((BLUE_CORE != BLUE_WB)  && (BLUE_CORE != STM32WB05N))

/** @brief Initialize the BLE Stack
  * @param None
  * @retval ble_status_t
  */
static ble_status_t init_ble_manager_ble_stack(void)
{
  uint16_t local_service_handle;
  uint16_t dev_name_char_handle;
  uint16_t appearance_char_handle;
  ble_status_t ret;
  uint8_t data_len_out;
  uint8_t random_number[8];
  uint8_t  hw_version;
  uint16_t fw_version;

  /* Initialize the BlueNRG HCI */
  /*  hci_init(app_user_evt_rx, NULL); */

#if ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06))
  init_ble_int_for_blue_nrglp();
#endif /* ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06)) */

  /* Initialize the BlueNRG HCI */
  hci_init(app_user_evt_rx, NULL);

  /* Sw reset of the device */
  hci_reset();

  /* Wait some time for the BlueNRG to be fully operational */
#ifndef  BLE_INITIAL_DELAY
  BLE_MANAGER_DELAY(2000);
#else /* BLE_INITIAL_DELAY */
  BLE_INITIAL_DELAY(2000);
#endif /* BLE_INITIAL_DELAY */

  /* get the BlueNRG HW and FW versions */
  get_blue_nrg_version(&hw_version, &fw_version);

  /* we will let the BLE chip to use its Random MAC address */
#define CONFIG_DATA_RANDOM_ADDRESS          (0x80) /**< Stored static random address. Read-only. */
  ret = aci_hal_read_config_data(CONFIG_DATA_RANDOM_ADDRESS, &data_len_out, ble_stack_value.ble_mac_address);

  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nReading  Random BD_ADDR failed\r\n");
    goto fail;
  }

  /* Check Random MAC */
  if ((ble_stack_value.ble_mac_address[5] & 0xC0U) != 0xC0U)
  {
    BLE_MANAGER_PRINTF("\tStatic Random address not well formed\r\n");
    goto fail;
  }

  /* Generate Random Key at every boot */
  if (ble_stack_value.enable_random_secure_pin)
  {
    ble_stack_value.secure_pin = STARTING_NUMBER_SECURE_PIN_GENERATION;

    /* get a random number from BlueNRG-1 */
    if (hci_le_rand(random_number) != BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("hci_le_rand() call failed\r\n");
    }

    /* setup random_key with random number */
    for (uint8_t i = 0; i < 8U; i++)
    {
      ble_stack_value.secure_pin += (435U * ((uint32_t)random_number[i]));
    }

    /* Control section because we need 6 digits */
    if (ble_stack_value.secure_pin < 99999U)
    {
      ble_stack_value.secure_pin += 100000U;
    }
  }

  ret = aci_hal_write_config_data(ble_stack_value.config_value_offsets,
                                  ble_stack_value.config_value_length,
                                  ble_stack_value.ble_mac_address);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nSetting Public BD_ADDR failed\r\n");
    goto fail;
  }

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06))
  ret = aci_gatt_init();
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06)) */
  ret = aci_gatt_srv_init();
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06)) */
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nGATT_Init failed\r\n");
    goto fail;
  }

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06))
  ret = aci_gap_init(ble_stack_value.gap_roles,
                     0,
                     (uint8_t) strlen(ble_stack_value.board_name),
                     &local_service_handle,
                     &dev_name_char_handle,
                     &appearance_char_handle);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06)) */
  ret = aci_gap_init(ble_stack_value.gap_roles,
                     0x00,
                     (uint8_t) strlen(ble_stack_value.board_name),
                     STATIC_RANDOM_ADDR,
                     &local_service_handle,
                     &dev_name_char_handle,
                     &appearance_char_handle);
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06)) */


  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nGAP_Init failed\r\n");
    goto fail;
  }

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06))
  ret = aci_gatt_update_char_value(local_service_handle, dev_name_char_handle, 0,
                                   (uint8_t) strlen(ble_stack_value.board_name), (uint8_t *)ble_stack_value.board_name);

  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\naci_gatt_update_char_value failed\r\n");
    goto fail;
  }
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06)) */
  ret = aci_gatt_srv_write_handle_value_nwk(dev_name_char_handle + 1,
                                            0,
                                            (uint8_t) strlen(ble_stack_value.board_name),
                                            (uint8_t *) ble_stack_value.board_name);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\taci_gatt_srv_write_handle_value_nwk failed: 0x%02x\r\n", ret);
    goto fail;
  }
  else
  {
    BLE_MANAGER_PRINTF("\taci_gatt_srv_write_handle_value_nwk\r\n");
  }
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06)) */

  if (ble_stack_value.enable_secure_connection)
  {
    /* Set the I/O capability  Otherwise the Smartphone will propose a Pin
    * that will be acepted without any control */
    if (aci_gap_set_io_capability(ble_stack_value.io_capabilities) == BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("I/O Capability Configurated\r\n");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Setting I/O Capability\r\n");
    }

    if (ble_stack_value.enable_random_secure_pin)
    {
      ret = aci_gap_set_authentication_requirement(ble_stack_value.authentication_requirements,
                                                   ble_stack_value.mitm_protection_requirements,
                                                   ble_stack_value.secure_connection_support_option_code,
                                                   ble_stack_value.secure_connection_keypress_notification,
                                                   7,
                                                   16,
                                                   DONOT_USE_FIXED_PIN_FOR_PAIRING,
                                                   ble_stack_value.secure_pin
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06))
                                                   , 0x01);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06)) */
                                                  );
#endif /* (BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) */
    }
    else
    {
      ret = aci_gap_set_authentication_requirement(ble_stack_value.authentication_requirements,
                                                   ble_stack_value.mitm_protection_requirements,
                                                   ble_stack_value.secure_connection_support_option_code,
                                                   ble_stack_value.secure_connection_keypress_notification,
                                                   7,
                                                   16,
                                                   USE_FIXED_PIN_FOR_PAIRING,
                                                   ble_stack_value.secure_pin
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06))
                                                   , 0x01);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06)) */
                                                  );
#endif /* (BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) */
    }

  }
  else
  {
    ret = aci_gap_set_authentication_requirement(ble_stack_value.authentication_requirements,
                                                 ble_stack_value.mitm_protection_requirements,
                                                 ble_stack_value.secure_connection_support_option_code,
                                                 ble_stack_value.secure_connection_keypress_notification,
                                                 7,
                                                 16,
                                                 USE_FIXED_PIN_FOR_PAIRING,
                                                 ble_stack_value.secure_pin
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06))
                                                 , 0x01);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06)) */
                                                );
#endif /* (BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06)*/
  }

  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nGAP setting Authentication failed\r\n");
    goto fail;
  }

  BLE_MANAGER_PRINTF("\r\nSERVER: BLE Stack Initialized \r\n"
                     "\t\tBoardName= %s\r\n"
                     "\t\tBoardMAC = %x:%x:%x:%x:%x:%x\r\n",
                     ble_stack_value.board_name,
                     ble_stack_value.ble_mac_address[5],
                     ble_stack_value.ble_mac_address[4],
                     ble_stack_value.ble_mac_address[3],
                     ble_stack_value.ble_mac_address[2],
                     ble_stack_value.ble_mac_address[1],
                     ble_stack_value.ble_mac_address[0]);

#if (BLUE_CORE == BLUENRG_1_2)
  BLE_MANAGER_PRINTF("\t\tBlueNRG-2 HW ver%d.%d\r\n", ((hw_version >> 4) & 0x0FU), (hw_version & 0x0FU));
  BLE_MANAGER_PRINTF("\t\tBlueNRG-2 FW ver%d.%d.%c\r\n\r\n", (fw_version >> 8) & 0xFU, (fw_version >> 4) & 0xFU,
                     ('a' + (fw_version & 0xFU)));
#elif (BLUE_CORE == BLUENRG_MS)
  BLE_MANAGER_PRINTF("\t\tBlueNRG-MS HW ver%d.%d\r\n", ((hw_version >> 4) & 0x0FU), (hw_version & 0x0FU));
  BLE_MANAGER_PRINTF("\t\tBlueNRG-MS FW ver%d.%d.%c\r\n\r\n", (fw_version >> 8) & 0xFU, (fw_version >> 4) & 0xFU,
                     ('a' + (fw_version & 0xFU)));
#elif (BLUE_CORE == BLUENRG_LP)
  BLE_MANAGER_PRINTF("\t\tBlueNRG-LP HW ver%d.%d\r\n", ((hw_version >> 4) & 0x0FU), (hw_version & 0x0FU));
  BLE_MANAGER_PRINTF("\t\tBlueNRG-LP FW ver%d.%d.%c\r\n\r\n", (fw_version >> 8) & 0xFU, (fw_version >> 4) & 0xFU,
                     ('a' + (fw_version & 0xFU)));
#elif (BLUE_CORE == STM32WB07_06)
  BLE_MANAGER_PRINTF("\t\tSTM32WB07_06 HW ver%d.%d\r\n", ((hw_version >> 4) & 0x0FU), (hw_version & 0x0FU));
  BLE_MANAGER_PRINTF("\t\tSTM32WB07_06 FW ver%d.%d.%c\r\n\r\n", (fw_version >> 8) & 0xFU, (fw_version >> 4) & 0xFU,
                     ('a' + (fw_version & 0xFU)));
#endif /* (BLUE_CORE += BLUENRG_1_2) */

  if (ble_stack_value.enable_secure_connection)
  {
    BLE_MANAGER_PRINTF("\t-->ONLY SECURE CONNECTION<--\r\n");

    if (ble_stack_value.enable_random_secure_pin)
    {
      BLE_MANAGER_PRINTF("\t\tRandom Key = %lu\r\n", ble_stack_value.secure_pin);
    }
    else
    {
      BLE_MANAGER_PRINTF("\t\tFixed  Key = %lu\r\n", ble_stack_value.secure_pin);
    }
  }

  /* Set output power level */
  aci_hal_set_tx_power_level(ble_stack_value.enable_high_power_mode,
                             ble_stack_value.power_amplifier_output_level);
#if ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06))
  ret = hci_le_write_suggested_default_data_length(247, (247 + 14) * 8);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\thci_le_write_suggested_default_data_length failed: 0x%02x\r\n", ret);
  }
  else
  {
    BLE_MANAGER_PRINTF("\thci_le_write_suggested_default_data_length\r\n");
  }
#endif /* (BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06) */

fail:
  return ret;
}
#endif /* ((BLUE_CORE != BLUE_WB)  && (BLUE_CORE != STM32WB05N)) */

#if (BLUE_CORE == STM32WB05N)

/** @brief Initialize the BLE Stack
  * @param None
  * @retval ble_status_t
  */
static ble_status_t init_ble_manager_ble_stack(void)
{
  uint16_t service_handle;
  uint16_t dev_name_char_handle = 0;
  uint16_t appearance_char_handle;
  uint16_t Periph_Pref_Conn_Param_Char_Handle;
  ble_status_t ret;
  uint8_t data_len_out;
  uint8_t random_number[8];
  uint8_t  hw_version;
  uint16_t fw_version;

  /* Initialize the BlueNRG HCI */
  /*  hci_init(app_user_evt_rx, NULL); */

  init_ble_int_for_stm32wb05n();

  /* Initialize the BlueNRG HCI */
  hci_init(app_user_evt_rx, NULL);

  /* Sw reset of the device */
  hci_reset();

  /* Wait some time for the BlueNRG to be fully operational */
#ifndef  BLE_INITIAL_DELAY
  BLE_MANAGER_DELAY(2000);
#else /* BLE_INITIAL_DELAY */
  BLE_INITIAL_DELAY(2000);
#endif /* BLE_INITIAL_DELAY */

  /* get the BlueNRG HW and FW versions */
  get_blue_nrg_version(&hw_version, &fw_version);

  /* we will let the BLE chip to use its Random MAC address */
#define CONFIG_DATA_RANDOM_ADDRESS          (0x80) /**< Stored static random address. Read-only. */
  ret = aci_hal_read_config_data(CONFIG_DATA_RANDOM_ADDRESS, &data_len_out, ble_stack_value.ble_mac_address);

  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nReading  Random BD_ADDR failed\r\n");
    goto fail;
  }

  /* Check Random MAC */
  if ((ble_stack_value.ble_mac_address[5] & 0xC0U) != 0xC0U)
  {
    BLE_MANAGER_PRINTF("\tStatic Random address not well formed\r\n");
    goto fail;
  }

  /* Generate Random Key at every boot */
  if (ble_stack_value.enable_random_secure_pin)
  {
    ble_stack_value.secure_pin = STARTING_NUMBER_SECURE_PIN_GENERATION;

    /* get a random number from BlueNRG-1 */
    if (hci_le_rand(random_number) != BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("hci_le_rand() call failed\r\n");
    }

    /* setup random_key with random number */
    for (uint8_t i = 0; i < 8U; i++)
    {
      ble_stack_value.secure_pin += (435U * ((uint32_t)random_number[i]));
    }

    /* Control section because we need 6 digits */
    if (ble_stack_value.secure_pin < 99999U)
    {
      ble_stack_value.secure_pin += 100000U;
    }
  }

  ret = aci_hal_write_config_data(ble_stack_value.config_value_offsets,
                                  ble_stack_value.config_value_length,
                                  ble_stack_value.ble_mac_address);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nSetting Public BD_ADDR failed\r\n");
    goto fail;
  }

  ret = aci_gatt_srv_profile_init(GATT_INIT_SERVICE_CHANGED_BIT, &service_handle);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nGATT_Init failed\r\n");
    goto fail;
  }

  ret = aci_gap_init(PRIVACY_DISABLED,
                     HCI_ADDR_PUBLIC);

  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nGAP_Init failed\r\n");
    goto fail;
  }
  
  /* Wait some time for the BlueNRG to be fully operational after gap init */
#ifndef  BLE_INITIAL_DELAY
  BLE_MANAGER_DELAY(2000);
#else /* BLE_INITIAL_DELAY */
  BLE_INITIAL_DELAY(2000);
#endif /* BLE_INITIAL_DELAY */

  ret = aci_gap_profile_init(ble_stack_value.gap_roles,
                             PRIVACY_DISABLED,
                             &dev_name_char_handle,
                             &appearance_char_handle,
                             &Periph_Pref_Conn_Param_Char_Handle);

  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\aci_gap_profile_init failed\r\n");
    goto fail;
  }

  ret = aci_gatt_srv_write_handle_value_nwk(dev_name_char_handle + 1,
                                            0,
                                            (uint8_t) strlen(ble_stack_value.board_name),
                                            (uint8_t *) ble_stack_value.board_name);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\taci_gatt_srv_write_handle_value_nwk failed: 0x%02x\r\n", ret);
    goto fail;
  }
  else
  {
    BLE_MANAGER_PRINTF("\taci_gatt_srv_write_handle_value_nwk\r\n");
  }

  if (ble_stack_value.enable_secure_connection)
  {
    /* Set the I/O capability  Otherwise the Smartphone will propose a Pin
    * that will be acepted without any control */
    if (aci_gap_set_io_capability(ble_stack_value.io_capabilities) == BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("I/O Capability Configurated\r\n");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Setting I/O Capability\r\n");
    }

    if (ble_stack_value.enable_random_secure_pin)
    {
      ret = aci_gap_set_security_requirements(ble_stack_value.authentication_requirements,
                                              ble_stack_value.mitm_protection_requirements,
                                              ble_stack_value.secure_connection_support_option_code,
                                              ble_stack_value.secure_connection_keypress_notification,
                                              7,
                                              16,
                                              GAP_PAIRING_RESP_NONE
                                             );
    }
    else
    {
      ret = aci_gap_set_security_requirements(ble_stack_value.authentication_requirements,
                                              ble_stack_value.mitm_protection_requirements,
                                              ble_stack_value.secure_connection_support_option_code,
                                              ble_stack_value.secure_connection_keypress_notification,
                                              7,
                                              16,
                                              GAP_PAIRING_RESP_NONE
                                             );
    }

  }
  else
  {
    /* Set the I/O capability  Otherwise the Smartphone will propose a Pin
    * that will be acepted without any control */
    if (aci_gap_set_io_capability(ble_stack_value.io_capabilities) == BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("I/O Capability Configurated\r\n");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Setting I/O Capability\r\n");
    }

    ret = aci_gap_set_security_requirements(ble_stack_value.authentication_requirements,
                                            ble_stack_value.mitm_protection_requirements,
                                            ble_stack_value.secure_connection_support_option_code,
                                            ble_stack_value.secure_connection_keypress_notification,
                                            7,
                                            16,
                                            GAP_PAIRING_RESP_NONE
                                           );
  }

  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\r\nGAP setting Authentication failed\r\n");
    goto fail;
  }

  BLE_MANAGER_PRINTF("\r\nSERVER: BLE Stack Initialized \r\n"
                     "\t\tBoardName= %s\r\n"
                     "\t\tBoardMAC = %x:%x:%x:%x:%x:%x\r\n",
                     ble_stack_value.board_name,
                     ble_stack_value.ble_mac_address[5],
                     ble_stack_value.ble_mac_address[4],
                     ble_stack_value.ble_mac_address[3],
                     ble_stack_value.ble_mac_address[2],
                     ble_stack_value.ble_mac_address[1],
                     ble_stack_value.ble_mac_address[0]);

  BLE_MANAGER_PRINTF("\t\tSTM32WB05N HW ver%d.%d\r\n", ((hw_version >> 4) & 0x0FU), (hw_version & 0x0FU));
  BLE_MANAGER_PRINTF("\t\tSTM32WB05N FW ver%d.%d.%c\r\n\r\n", (fw_version >> 8) & 0xFU, (fw_version >> 4) & 0xFU,
                     ('a' + (fw_version & 0xFU)));


  if (ble_stack_value.enable_secure_connection)
  {
    BLE_MANAGER_PRINTF("\t-->ONLY SECURE CONNECTION<--\r\n");

    if (ble_stack_value.enable_random_secure_pin)
    {
      BLE_MANAGER_PRINTF("\t\tRandom Key = %lu\r\n", (long)ble_stack_value.secure_pin);
    }
    else
    {
      BLE_MANAGER_PRINTF("\t\tFixed  Key = %lu\r\n", (long)ble_stack_value.secure_pin);
    }
  }

  /* Set output power level */
  aci_hal_set_tx_power_level(ble_stack_value.enable_high_power_mode,
                             ble_stack_value.power_amplifier_output_level);

  /* Wait some time for the BlueNRG to be fully operational after set output power level */
#ifndef  BLE_INITIAL_DELAY
  BLE_MANAGER_DELAY(2000);
#else /* BLE_INITIAL_DELAY */
  BLE_INITIAL_DELAY(2000);
#endif /* BLE_INITIAL_DELAY */
  
  ret = hci_le_write_suggested_default_data_length(247, (247 + 14) * 8);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("\thci_le_write_suggested_default_data_length failed: 0x%02x\r\n", ret);
  }
  else
  {
    BLE_MANAGER_PRINTF("\thci_le_write_suggested_default_data_length\r\n");
  }

fail:
  return ret;
}
#endif /* (BLUE_CORE == STM32WB05N) */
/**
  * @brief  Get hardware and firmware version
  *
  * @param  Hardware version
  * @param  Firmware version
  * @retval Status
  */
uint8_t get_blue_nrg_version(uint8_t *hw_version, uint16_t *fw_version)
{
  ble_status_t status;
  uint8_t hci_version;
  uint8_t lmp_pal_version;
  uint16_t hci_revision;
  uint16_t manufacturer_name;
  uint16_t lmp_pal_subversion;

  status = hci_read_local_version_information(&hci_version, &hci_revision, &lmp_pal_version,
                                              &manufacturer_name, &lmp_pal_subversion);

  if (status == BLE_STATUS_SUCCESS)
  {
    *hw_version = (uint8_t)(hci_revision >> 8);
    *fw_version = (hci_revision & 0xFFU) << 8;              /* Major Version Number */
    *fw_version |= ((lmp_pal_subversion >> 4) & 0xFU) << 4; /* Minor Version Number */
    *fw_version |= lmp_pal_subversion & 0xFU;               /* Patch Version Number */
  }
  return status;
}
#endif /* (BLUE_CORE == BLUENRG_MS) */

/**
  * @brief  Init Ble Manager Services
  * @param  None
  * @retval ble_status_t Status
  */
static ble_status_t init_ble_manager_services(void)
{
  ble_status_t status = BLE_ERROR_UNSPECIFIED;
  ble_char_object_t *ble_char_pointer;

#ifdef BLE_MANAGER_SDKV2
  BLE_MANAGER_PRINTF("BlueST-SDK V2\r\n");
#else /* BLE_MANAGER_SDKV2 */
  BLE_MANAGER_PRINTF("BlueST-SDK V1\r\n");
#endif /* BLE_MANAGER_SDKV2 */

  if (ble_stack_value.enable_config)
  {
    ble_char_pointer = &ble_char_config;
    memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
    ble_char_pointer->attr_mod_request_cb = attr_mod_request_config;
    ble_char_pointer->write_request_cb = write_request_config;
    COPY_CONFIG_CHAR_UUID((ble_char_pointer->uuid));
    ble_char_pointer->char_uuid_type = UUID_TYPE_128;
    ble_char_pointer->char_value_length = 20;
    ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP);
    ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
    ble_char_pointer->gatt_evt_mask = ((uint8_t)GATT_NOTIFY_ATTRIBUTE_WRITE) |
                                      ((uint8_t)GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP);
    ble_char_pointer->enc_key_size = 16;
    ble_char_pointer->is_variable = 1;
    ble_manager_add_char(ble_char_pointer);

    status = ble_manager_add_config_service();
    if (status == (ble_status_t)BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("Config  Service added successfully\r\n");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: while adding Config Service\r\n");
    }
  }

  if (ble_stack_value.enable_console)
  {
    ble_char_pointer = &ble_char_std_out;
    memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
    ble_char_pointer->attr_mod_request_cb = attr_mod_request_term;
    ble_char_pointer->write_request_cb = write_request_term;
    ble_char_pointer->read_request_cb = read_request_term;
    COPY_TERM_CHAR_UUID((ble_char_pointer->uuid));
    ble_char_pointer->char_uuid_type = UUID_TYPE_128;
    ble_char_pointer->char_value_length = DEFAULT_MAX_STDOUT_CHAR_LEN;
    ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) |
                                        ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP) |
                                        ((uint8_t)CHAR_PROP_WRITE) |
                                        ((uint8_t)CHAR_PROP_READ);
    ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
    ble_char_pointer->gatt_evt_mask = ((uint8_t)GATT_NOTIFY_ATTRIBUTE_WRITE) |
                                      ((uint8_t)GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP);
    ble_char_pointer->enc_key_size = 16;
    ble_char_pointer->is_variable = 1;
    ble_manager_add_char(ble_char_pointer);

    ble_char_pointer = &ble_char_std_err;
    memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
    ble_char_pointer->attr_mod_request_cb = attr_mod_request_std_err;
    ble_char_pointer->read_request_cb = read_request_std_err;
    COPY_STDERR_CHAR_UUID((ble_char_pointer->uuid));
    ble_char_pointer->char_uuid_type = UUID_TYPE_128;
    ble_char_pointer->char_value_length = DEFAULT_MAX_STDERR_CHAR_LEN;
    ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_READ);
    ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
    ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
    ble_char_pointer->enc_key_size = 16;
    ble_char_pointer->is_variable = 1;
    ble_manager_add_char(ble_char_pointer);

    status = ble_manager_add_console_service();
    if (status == (ble_status_t)BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("Console Service added successfully\r\n");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: while adding Console Service\r\n");
    }
  }

  used_standard_ble_chars = used_ble_chars;

#ifndef BLE_MANAGER_NO_PARSON
  /* Extended Configuration characteristic value */
  if (ble_stack_value.enable_ext_config)
  {
    ble_char_pointer = &ble_char_ext_config;
    memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
    ble_char_pointer->attr_mod_request_cb = attr_mod_request_ext_config;
    ble_char_pointer->write_request_cb = write_request_ext_config;
    COPY_EXT_CONFIG_CHAR_UUID((ble_char_pointer->uuid));
    ble_char_pointer->char_uuid_type = UUID_TYPE_128;
    ble_char_pointer->char_value_length = 20;
    ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP);
    ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
    ble_char_pointer->gatt_evt_mask = ((uint8_t)GATT_NOTIFY_ATTRIBUTE_WRITE) |
                                      ((uint8_t)GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP);
    ble_char_pointer->enc_key_size = 16;
    ble_char_pointer->is_variable = 1;
    ble_manager_add_char(ble_char_pointer);
  }
#endif /* BLE_MANAGER_NO_PARSON */

  /* Set Custom Configuration and Services */
  ble_init_custom_service();

  if ((used_ble_chars - used_standard_ble_chars) > 0U)
  {
    status = ble_manager_add_features_service();
    if (status == (ble_status_t)BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("Features Service added successfully (Status= 0x%x)\r\n", status);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: while adding Features Service (Status= 0x%x)\r\n", status);
    }
  }

  return status;
}

#ifndef BLE_MANAGER_NO_PARSON
/**
  * @brief  This function is called to parse a BLE_COMM_TP packet.
  * @param  buffer_out: pointer to the output buffer.
  * @param  buffer_in: pointer to the input data.
  * @param  len: buffer in length
  * @retval Buffer out length.
  */
uint32_t ble_command_tp_parse(uint8_t **buffer_out, uint8_t *buffer_in, uint32_t len)
{
  uint32_t buff_out_len = 0;
  ble_comm_tp_packet_t packet_type;

  packet_type = (ble_comm_tp_packet_t) buffer_in[0];

  switch (status_ble_parse)
  {
    case BLE_COMM_TP_WAIT_START:
      if (packet_type == BLE_COMM_TP_START_PACKET)
      {
        /*First part of an BLE Command packet*/
        /*packet is enqueued*/
        uint32_t message_length = buffer_in[1];
        message_length = message_length << 8;
        message_length |= buffer_in[2];

        /*
                To check
                if (*buffer_out != NULL)
                {
                  BLE_FREE_FUNCTION(*buffer_out);
                }
        */

        *buffer_out = (uint8_t *)BLE_MALLOC_FUNCTION((message_length) * sizeof(uint8_t));

        if (*buffer_out == NULL)
        {
          BLE_MANAGER_PRINTF("Error: Mem alloc error [%ld]: %d@%s\r\n", (long)message_length, __LINE__, __FILE__);
        }

        memcpy(*buffer_out + tot_len_ble_parse, (uint8_t *) &buffer_in[3], (len - 3U));


        tot_len_ble_parse += len - 3U;
        status_ble_parse = BLE_COMM_TP_WAIT_END;
        buff_out_len = 0;
      }
      else if (packet_type == BLE_COMM_TP_START_LONG_PACKET)
      {
        /*First part of an BLE Command packet*/
        /*packet is enqueued*/
        uint32_t message_length = buffer_in[1];
        message_length = message_length << 8;
        message_length |= buffer_in[2];
        message_length = message_length << 8;
        message_length |= buffer_in[3];
        message_length = message_length << 8;
        message_length |= buffer_in[4];

        /*
                To check
                if (*buffer_out != NULL)
                {
                  BLE_FREE_FUNCTION(*buffer_out);
                }
        */

        *buffer_out = (uint8_t *)BLE_MALLOC_FUNCTION((message_length) * sizeof(uint8_t));

        if (*buffer_out == NULL)
        {
          BLE_MANAGER_PRINTF("Error: Mem alloc error [%ld]: %d@%s\r\n", (long)message_length, __LINE__, __FILE__);
        }

        memcpy(*buffer_out + tot_len_ble_parse, (uint8_t *) &buffer_in[5], (len - 5U));


        tot_len_ble_parse += len - 5U;
        status_ble_parse = BLE_COMM_TP_WAIT_END;
        buff_out_len = 0;
      }
      else if (packet_type == BLE_COMM_TP_START_END_PACKET)
      {
        /*Final part of an BLE Command packet*/
        /*packet is enqueued*/
        uint32_t message_length = buffer_in[1];
        message_length = message_length << 8;
        message_length |= buffer_in[2];

        *buffer_out = (uint8_t *)BLE_MALLOC_FUNCTION((message_length) * sizeof(uint8_t));
        if (*buffer_out == NULL)
        {
          BLE_MANAGER_PRINTF("Error: Mem alloc error [%ld]: %d@%s\r\n", (long)message_length, __LINE__, __FILE__);
        }

        memcpy(*buffer_out + tot_len_ble_parse, (uint8_t *) &buffer_in[3], (len - 3U));


        tot_len_ble_parse += len - 3U;
        /*number of bytes of the output packet*/
        buff_out_len = tot_len_ble_parse;
        /*total length set to zero*/
        tot_len_ble_parse = 0;
        /*reset status_ble_parse*/
        status_ble_parse = BLE_COMM_TP_WAIT_START;
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

        memcpy(*buffer_out + tot_len_ble_parse, (uint8_t *) &buffer_in[1], (len - 1U));

        tot_len_ble_parse += len - 1U;

        buff_out_len = 0;
      }
      else if (packet_type == BLE_COMM_TP_END_PACKET)
      {
        /*Final part of an BLE Command packet*/
        /*packet is enqueued*/
        memcpy(*buffer_out + tot_len_ble_parse, (uint8_t *) &buffer_in[1], (len - 1U));

        tot_len_ble_parse += len - 1U;
        /*number of bytes of the output packet*/
        buff_out_len = tot_len_ble_parse;
        /*total length set to zero*/
        tot_len_ble_parse = 0;
        /*reset status_ble_parse*/
        status_ble_parse = BLE_COMM_TP_WAIT_START;
      }
      else
      {
        /*reset status_ble_parse*/
        status_ble_parse = BLE_COMM_TP_WAIT_START;
        /*total length set to zero*/
        tot_len_ble_parse = 0;

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
  * @param  byte_packet_size: Packet Size in Bytes
  * @retval Buffer out length.
  */
uint32_t ble_command_tp_encapsulate(uint8_t *buffer_out, uint8_t *buffer_in, uint32_t len, uint32_t byte_packet_size)
{
  uint32_t size = 0;
  uint32_t tot_size = 0;
  uint32_t counter = 0;
  ble_comm_tp_packet_t packet_type = BLE_COMM_TP_START_PACKET;
  uint32_t byte_packet_size_minus1 = byte_packet_size - 1U;

  /* One byte header is added to each BLE packet */
  while (counter < len)
  {
    size = MIN(byte_packet_size_minus1, (len - counter));

    if ((len - counter) <= byte_packet_size_minus1)
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
        buffer_out[tot_size] = ((uint8_t)(BLE_COMM_TP_START_PACKET));
        tot_size++;
        packet_type = BLE_COMM_TP_MIDDLE_PACKET;
        break;
      case BLE_COMM_TP_START_END_PACKET:
        /*First and last part of an BLE Command packet*/
        buffer_out[tot_size] = ((uint8_t)(BLE_COMM_TP_START_END_PACKET));
        tot_size++;
        packet_type = BLE_COMM_TP_START_PACKET;
        break;
      case BLE_COMM_TP_MIDDLE_PACKET:
        /*Central part of an BLE Command packet*/
        buffer_out[tot_size] = ((uint8_t)(BLE_COMM_TP_MIDDLE_PACKET));
        tot_size++;
        break;
      case BLE_COMM_TP_END_PACKET:
        /*Last part of an BLE Command packet*/
        buffer_out[tot_size] = ((uint8_t)(BLE_COMM_TP_END_PACKET));
        tot_size++;
        packet_type = BLE_COMM_TP_START_PACKET;
        break;
      case BLE_COMM_TP_START_LONG_PACKET:
        break;
    }

    /*Input data is incapsulated*/
    memcpy((uint8_t *) &buffer_out[tot_size], (uint8_t *) &buffer_in[counter], size);

    /*length variables update*/
    counter += size;
    tot_size += size;
  }
  return tot_size;
}
#endif /* BLE_MANAGER_NO_PARSON */

#if ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06))
__weak void init_ble_int_for_blue_nrglp(void)
{
  /* NOTE: This function Should not be modified, .
           The init_ble_int_for_blue_nrglp could be implemented in the user file
   */
}
#endif /* ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06)) */

#if (BLUE_CORE == STM32WB05N)
__weak void init_ble_int_for_stm32wb05n(void)
{
  /* NOTE: This function Should not be modified, .
           The init_ble_int_for_stm32wb05n could be implemented in the user file
   */
}
#endif /* (BLUE_CORE == STM32WB05N) */

/* ***************** BlueNRG-1 Stack Callbacks ********************************/

/*******************************************************************************
  * Function Name  : hci_le_connection_complete_event.
  * Description    : This event indicates that a new connection has been created.
  * Input          : See file bluenrg1_events.h
  * Output         : See file bluenrg1_events.h
  * Return         : See file bluenrg1_events.h
  *******************************************************************************/
void hci_le_connection_complete_event(uint8_t status,
                                      uint16_t connection_handle,
                                      uint8_t role,
                                      uint8_t peer_address_type,
                                      uint8_t peer_address[6],
                                      uint16_t conn_interval,
                                      uint16_t conn_latency,
                                      uint16_t supervision_timeout,
                                      uint8_t master_clock_accuracy)
{
  global_connection_handle = connection_handle;

  BLE_MANAGER_PRINTF(">>>>>>CONNECTED %x:%x:%x:%x:%x:%x\r\n", peer_address[5], peer_address[4], peer_address[3],
                     peer_address[2], peer_address[1], peer_address[0]);

#if (BLUE_CORE != BLUENRG_MS)
  if (ble_stack_value.enable_secure_connection)
  {
    ble_status_t ret_status;
    /* Check if the device is already bonded */
    ret_status = aci_gap_is_device_bonded(peer_address_type, peer_address);
    if (ret_status != (ble_status_t)BLE_STATUS_SUCCESS)
    {
#if (BLUE_CORE == STM32WB05N)
      /* Send a slave security request to the master */
      ret_status = aci_gap_set_security(GAP_SECURITY_LEVEL_1, connection_handle, 0);
#else /* (BLUE_CORE == STM32WB05N) */
      /* Send a slave security request to the master */
      ret_status = aci_gap_slave_security_req(connection_handle);
#endif /* (BLUE_CORE == STM32WB05N) */
      if (ret_status != (ble_status_t)BLE_STATUS_SUCCESS)
      {
        BLE_MANAGER_PRINTF("Error: GAP Slave secury request failed %d\r\n", ret_status);
      }
      else
      {
#if (BLE_DEBUG_LEVEL>1)
        BLE_MANAGER_PRINTF("GAP Slave secury request Done\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
      }
    }
    else
    {
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("Device already bounded\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
    }
  }
#endif /* (BLUE_CORE != BLUENRG_MS) */

  /* Start one Exchange configuration for understaning the maximum ATT_MTU */
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))

#if (BLUE_CORE == BLUENRG_MS)
  ACI_GATT_EXCHANGE_CONFIG(global_connection_handle);
#else /* (BLUE_CORE == BLUENRG_MS) */
  aci_gatt_exchange_config(global_connection_handle);
#endif /* (BLUE_CORE == BLUENRG_MS) */

#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
  aci_gatt_clt_exchange_config(connection_handle);
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

#if (BLUE_CORE == BLUENRG_MS)
  connection_completed_function(global_connection_handle, peer_address);
#elif (BLUE_CORE == BLUE_WB)
  connection_completed_function(global_connection_handle);
#else /* (BLUE_CORE == BLUENRG_MS) */
  connection_completed_function(global_connection_handle, peer_address_type, peer_address);
#endif /* (BLUE_CORE == BLUENRG_MS) */

}/* end hci_le_connection_complete_event() */

/*******************************************************************************
  * Function Name  : hci_disconnection_complete_event.
  * Description    : This event occurs when a connection is terminated.
  * Input          : See file bluenrg1_events.h
  * Output         : See file bluenrg1_events.h
  * Return         : See file bluenrg1_events.h
  *******************************************************************************/
void hci_disconnection_complete_event(uint8_t status,
                                      uint16_t connection_handle,
                                      uint8_t reason)
{
  /* No Device Connected */
  global_connection_handle = 0;

#ifndef BLE_MANAGER_NO_PARSON
  /* Reset the BLE Parse State */
  tot_len_ble_parse = 0;
  status_ble_parse = BLE_COMM_TP_WAIT_START;
#endif /* BLE_MANAGER_NO_PARSON */

  BLE_MANAGER_PRINTF("<<<<<<DISCONNECTED\r\n");

  /* Make the device connectable again. */
  set_connectable = TRUE;

  disconnection_completed_function();

}/* end hci_disconnection_complete_event() */

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
/*******************************************************************************
  * Function Name  : aci_gatt_read_permit_req_event.
  * Description    : This event is given when a read request is received
  *                  by the server from the client.
  * Input          : See file bluenrg1_events.h
  * Output         : See file bluenrg1_events.h
  * Return         : See file bluenrg1_events.h
  *******************************************************************************/
void aci_gatt_read_permit_req_event(uint16_t connection_handle,
                                    uint16_t attribute_handle,
                                    uint16_t offset)
{
  uint32_t found_handle = 0;
  uint8_t registered_handle;

  /* Search inside all the registered handles */
  for (registered_handle = 0; ((registered_handle < used_ble_chars) && (found_handle == 0U)); registered_handle++)
  {
    if (ble_chars_array[registered_handle]->read_request_cb != NULL)
    {
      if (attribute_handle == (ble_chars_array[registered_handle]->attr_handle + 1U))
      {
        ble_chars_array[registered_handle]->read_request_cb(ble_chars_array[registered_handle], attribute_handle);
      }
    }
  }

  if (global_connection_handle != 0U)
  {
    aci_gatt_allow_read(global_connection_handle);
  }
}
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
void aci_gatt_srv_authorize_nwk_event(uint16_t connection_handle,
#if (BLUE_CORE == STM32WB05N)
                                      uint16_t CID,
#endif /* (BLUE_CORE == STM32WB05N) */
                                      uint16_t attr_handle,
                                      uint8_t operation_type,
                                      uint16_t attr_val_offset,
                                      uint8_t data_length,
                                      uint8_t data[])
{
  if (operation_type == 0) /* Read */
  {
    uint32_t found_handle = 0;
    uint8_t registered_handle;

    /* Search inside all the registered handles */
    for (registered_handle = 0; ((registered_handle < used_ble_chars) && (found_handle == 0U)); registered_handle++)
    {
      if (ble_chars_array[registered_handle]->read_request_cb != NULL)
      {
        if (attr_handle == (ble_chars_array[registered_handle]->attr_handle + 1U))
        {
          ble_chars_array[registered_handle]->read_request_cb(ble_chars_array[registered_handle],
                                                              attr_handle, connection_handle,
                                                              operation_type,
                                                              attr_val_offset,
                                                              data_length, data);
        }
      }
    }
  }
}
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

/*******************************************************************************
  * Function Name  : aci_gatt_attribute_modified_event.
  * Description    : This event is given when an attribute change his value.
  * Input          : See file bluenrg1_events.h
  * Output         : See file bluenrg1_events.h
  * Return         : See file bluenrg1_events.h
  *******************************************************************************/
void aci_gatt_attribute_modified_event(uint16_t connection_handle,
                                       uint16_t attr_handle,
                                       uint16_t offset,
                                       uint16_t attr_data_length,
                                       uint8_t attr_data[])
{
  uint32_t found_handle = 0;
  uint8_t registered_handle;

  if (attr_handle == ((uint16_t)(0x0002 + 2)))
  {
    BLE_MANAGER_PRINTF("Notification on Service Change Characteristic\r\n");
    found_handle = 1;
    if (ble_stack_value.force_rescan)
    {
      /* Force one UUID rescan */
      ble_status_t ret = BLE_STATUS_INSUFFICIENT_RESOURCES;
      uint8_t buff[4];

      /* Delete all the Handles from 0x0001 to 0xFFFF */
      STORE_LE_16(buff, 0x0001U);
      STORE_LE_16(buff + 2, 0xFFFFU);

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
      ret = aci_gatt_update_char_value(0x0001, 0x0002, 0, 4, buff);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#if (BLUE_CORE == STM32WB05N)
      ret = aci_gatt_srv_notify(connection_handle, 0x0004, 0x0002 + 1, GATT_INDICATION, 4, buff);
#else /* (BLUE_CORE == STM32WB05N) */
      ret = aci_gatt_srv_notify(connection_handle, 0x0002 + 1, GATT_INDICATION, 4, buff);
#endif /* (BLUE_CORE == STM32WB05N) */
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

      if (ret == (ble_status_t)BLE_STATUS_SUCCESS)
      {
        BLE_MANAGER_PRINTF("\r\nUUID Rescan Forced\r\n");
      }
      else
      {
        BLE_MANAGER_PRINTF("\r\nError: Problem forcing UUID Rescan\r\n");
      }
    }
  }

  /* Search inside all the registered handles */
  for (registered_handle = 0; ((registered_handle < used_ble_chars) && (found_handle == 0U)); registered_handle++)
  {
    /* Notification */
    if (ble_chars_array[registered_handle]->attr_mod_request_cb != NULL)
    {
      if (attr_handle == (ble_chars_array[registered_handle]->attr_handle + 2U))
      {
        found_handle = 1U;
        ble_chars_array[registered_handle]->attr_mod_request_cb(ble_chars_array[registered_handle], attr_handle, offset,
                                                                attr_data_length, attr_data);
      }
    }

    /* Write */
    if (found_handle == 0U)
    {
      if (ble_chars_array[registered_handle]->write_request_cb != NULL)
      {
        if (attr_handle == (ble_chars_array[registered_handle]->attr_handle + 1U))
        {
          found_handle = 1U;
          ble_chars_array[registered_handle]->write_request_cb(ble_chars_array[registered_handle], attr_handle, offset,
                                                               attr_data_length, attr_data);
        }
      }
    }
  }

  if (found_handle == 0U)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Notification UNKNOWN handle\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Notification UNKNOWN handle =%d\r\n", attr_handle);
    }
  }
}

#if ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06) || (BLUE_CORE == STM32WB05N))
/**
  * @brief  This event indicates that a new connection has been created
  *
  * @param  See file bluenrg_lp_events.h
  * @retval See file bluenrg_lp_events.h
  */
void hci_le_enhanced_connection_complete_event(uint8_t status,
                                               uint16_t connection_handle,
                                               uint8_t role,
                                               uint8_t peer_address_type,
                                               uint8_t peer_address[6],
                                               uint8_t Local_Resolvable_Private_Address[6],
                                               uint8_t Peer_Resolvable_Private_Address[6],
                                               uint16_t conn_interval,
                                               uint16_t conn_latency,
                                               uint16_t supervision_timeout,
                                               uint8_t master_clock_accuracy)
{

  hci_le_connection_complete_event(status,
                                   connection_handle,
                                   role,
                                   peer_address_type,
                                   peer_address,
                                   conn_interval,
                                   conn_latency,
                                   supervision_timeout,
                                   master_clock_accuracy);
}


/**
  * @brief  This event is given when an attribute changes his value
  * @param  See file bluenrg_lp_events.h
  * @retval See file bluenrg_lp_events.h
  */
void aci_gatt_srv_attribute_modified_event(uint16_t connection_handle,
#if (BLUE_CORE == STM32WB05N)
                                           uint16_t CID,
#endif /* (BLUE_CORE == STM32WB05N) */
                                           uint16_t attr_handle,
                                           uint16_t attr_data_length,
                                           uint8_t attr_data[])
{
  aci_gatt_attribute_modified_event(connection_handle,
                                    attr_handle,
                                    0,
                                    attr_data_length,
                                    attr_data);
}
#endif /* ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06) || (BLUE_CORE == STM32WB05N)) */

/*******************************************************************************
  * Function Name  : aci_att_exchange_mtu_resp_event
  * Description    : This event is generated in response to an Exchange MTU request
  * Input          : See file bluenrg1_events.h
  * Output         : See file bluenrg1_events.h
  * Return         : See file bluenrg1_events.h
  *******************************************************************************/
void aci_att_exchange_mtu_resp_event(uint16_t connection_handle,
                                     uint16_t server_rx_mtu)
{
  if ((server_rx_mtu - 3U) < max_ble_char_std_out_len)
  {
    max_ble_char_std_out_len = (uint8_t)(server_rx_mtu - 3U);
  }

  if ((server_rx_mtu - 3U) < max_ble_char_std_err_len)
  {
    max_ble_char_std_err_len = (uint8_t)(server_rx_mtu - 3U);
  }

#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("aci_att_exchange_mtu_resp_event server_rx_mtu=%d\r\n", server_rx_mtu);
#endif /* (BLE_DEBUG_LEVEL>2) */

  mtu_exchange_resp_event_function(server_rx_mtu - 3U);
}

void aci_gap_pass_key_req_event(uint16_t connection_handle)
{
  ble_status_t status;
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("aci_gap_pass_key_req_event [Requested pass_wd=%ld]\r\n", (long)ble_stack_value.secure_pin);
#endif /* (BLE_DEBUG_LEVEL>2) */

#if (BLUE_CORE == BLUENRG_MS)
  status = ACI_GAP_PASS_KEY_RESP(global_connection_handle, ble_stack_value.secure_pin);
#else /* (BLUE_CORE == BLUENRG_MS) */
#if (BLUE_CORE == STM32WB05N)
  status = aci_gap_passkey_resp(global_connection_handle, ble_stack_value.secure_pin);
#else /* (BLUE_CORE == STM32WB05N) */
  status = aci_gap_pass_key_resp(global_connection_handle, ble_stack_value.secure_pin);
#endif /* (BLUE_CORE == STM32WB05N) */
#endif /* (BLUE_CORE == BLUENRG_MS) */
  if (status != (uint8_t)BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("Error: aci_gap_pass_key_resp failed:0x%02x\r\n", status);
#if (BLE_DEBUG_LEVEL>1)
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gap_pass_key_resp OK\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
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
void aci_gap_pairing_complete_event(uint16_t connection_handle, uint8_t status, uint8_t reason)
{
  char *status_string[] =
  {
    /* 0x00 */ "Success",
    /* 0x01 */ "Timeout",
    /* 0x02 */ "Pairing Failed",
    /* 0x03 */ "Encryption failed, LTK missing on local device",
    /* 0x04 */ "Encryption failed, LTK missing on peer device",
    /* 0x05 */ "Encryption not supported by remote device"
  };

  char *reason_string[] =
  {
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

  switch (status)
  {
    case 0x00: /* Success */
      BLE_MANAGER_PRINTF("aci_gap_pairing_complete_event %s\r\n", status_string[status]);
#if (BLUE_CORE != BLUENRG_MS)
      update_white_list();
#endif /* (BLUE_CORE != BLUENRG_MS) */
      BLE_MANAGER_DELAY(100);
      break;
    case 0x02: /* Pairing Failed */
      BLE_MANAGER_PRINTF("aci_gap_pairing_complete_event failed:\r\n\tstatus= %s\r\n\treason= %s\r\n",
                         status_string[status],
                         reason_string[reason]);
#if 0
      /* Not sure... */
      BLE_MANAGER_PRINTF("Issue one aci_gap_terminate\r\n");
      /*
       0x05: Authentication Failure
       0x13: Remote User Terminated Connection
       0x14: Remote Device Terminated Connection due to Low Resources
       0x15: Remote Device Terminated Connection due to Power Off
       0x1A: Unsupported Remote Feature
       0x3B: Unacceptable Connection Parameters
      */
      aci_gap_terminate(connection_handle, 0x05);
#endif /* 0 */
      break;
    case 0x01: /* Timeout */
    case 0x03: /* Encryption failed, LTK missing on local device */
    case 0x04: /* Encryption failed, LTK missing on peer device */
    case 0x05: /* Encryption not supported by remote device */
      BLE_MANAGER_PRINTF("aci_gap_pairing_complete_event failed:\r\n\tstatus= %s\r\n\treason= %s\r\n",
                         status_string[status],
                         reason_string[reason]);
      break;
  }

  BLE_UNUSED(status_string);
  BLE_UNUSED(reason_string);

  pairing_completed_function(status);
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
void aci_l2cap_connection_update_resp_event(uint16_t connection_handle,
                                            uint16_t result)
{
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("aci_l2cap_connection_update_resp_event Result=%d\r\n", result);
#endif /* (BLE_DEBUG_LEVEL>2) */
}

/*******************************************************************************
  * Function Name  : hci_le_connection_update_complete_event
  * Description    : It indicates that the Controller process to update the connection has completed
  * Input          : See file bluenrg1_events.h
  * Output         : See file bluenrg1_events.h
  * Return         : See file bluenrg1_events.h
  *******************************************************************************/
void hci_le_connection_update_complete_event(uint8_t status,
                                             uint16_t connection_handle,
                                             uint16_t conn_interval,
                                             uint16_t conn_latency,
                                             uint16_t supervision_timeout)
{
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("hci_le_connection_update_complete_event:\r\n");
  BLE_MANAGER_PRINTF("\tStatus=%d\r\n", status);
  BLE_MANAGER_PRINTF("\tConn_Interval=%d\r\n", conn_interval);
  BLE_MANAGER_PRINTF("\tConn_Latency=%d\r\n", conn_latency);
  BLE_MANAGER_PRINTF("\tSupervision_Timeout=%d\r\n", supervision_timeout);
#endif /* (BLE_DEBUG_LEVEL>2) */
}

/*******************************************************************************
  * Function Name  : aci_gatt_proc_complete_event
  * Description    : This event is generated when a GATT client procedure completes
  *                  either with error or successfully
  * Input          : See file bluenrg1_events.h
  * Output         : See file bluenrg1_events.h
  * Return         : See file bluenrg1_events.h
  *******************************************************************************/
void aci_gatt_proc_complete_event(uint16_t connection_handle,
                                  uint8_t error_code)
{
  if (error_code != 0U)
  {
    BLE_MANAGER_PRINTF("Error: aci_gatt_proc_complete_event Error Code=%d\r\n", error_code);
#if (BLE_DEBUG_LEVEL>2)
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gatt_proc_complete_event Success\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */
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
void hci_le_data_length_change_event(uint16_t connection_handle,
                                     uint16_t max_tx_octets,
                                     uint16_t max_tx_time,
                                     uint16_t max_rx_octets,
                                     uint16_t max_rx_time)
{
#if ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06) || (BLUE_CORE == STM32WB05N))
  ble_status_t ret_status;
  int32_t max_retry_number = 10;
  int32_t retry_number = 0;
#endif /* ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06) || (BLUE_CORE == STM32WB05N)) */
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("hci_le_data_length_change_event\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */

#if ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06) || (BLUE_CORE == STM32WB05N))
  do
  {
    BLE_MANAGER_DELAY(200);
    ret_status = aci_gatt_clt_exchange_config(connection_handle);
    if (ret_status != BLE_STATUS_SUCCESS)
    {
      BLE_MANAGER_PRINTF("Error: ACI GATT Exchange Config Failed (0x%x)\r\n", ret_status);
      retry_number++;
    }
    else
    {
      BLE_MANAGER_PRINTF("ACI GATT Exchange Config Done\r\n");
    }
  } while ((ret_status != BLE_STATUS_SUCCESS) & (retry_number < max_retry_number));

#endif /* ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06) || (BLUE_CORE == STM32WB05N)) */
}

#if ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06) || (BLUE_CORE == STM32WB05N))
/**
  * @brief This event is generated when a GATT client procedure completes either
  *        with error or successfully.
  * @param connection_handle Connection handle related to the response
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
  - 0x1B: SCO offset Rejected
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

void aci_gatt_clt_proc_complete_event(uint16_t connection_handle,
#if (BLUE_CORE == STM32WB05N)
                                      uint16_t CID,
#endif /* (BLUE_CORE == STM32WB05N) */
                                      uint8_t error_code)
{
  if (error_code != 0x0)
  {
    BLE_MANAGER_PRINTF("aci_gatt_clt_proc_complete_event Error Code=0x%x\r\n", error_code);
  }
  else
  {
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
  * @param status For standard error codes see Bluetooth specification, Vol. 2,
  *        part D. For proprietary error code refer to Error codes section
  * @param connection_handle connection_handle to be used to identify a
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
void hci_le_phy_update_complete_event(uint8_t status,
                                      uint16_t connection_handle,
                                      uint8_t TX_PHY,
                                      uint8_t RX_PHY)
{

  BLE_MANAGER_PRINTF("hci_le_phy_update_complete_event Status=0x%x\r\n", status);
  BLE_MANAGER_PRINTF("\tTX_PHY=0x%x\r\n", TX_PHY);
  BLE_MANAGER_PRINTF("\tRX_PHY=0x%x\r\n", RX_PHY);
}
#endif /* ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06) || (BLUE_CORE == STM32WB05N)) */

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
/**
  * @brief This event is generated when an indication is received from the server.
  * @param connection_handle Connection handle related to the response
  * @param attribute_handle The handle of the attribute
  * @param attribute_value_length Length of attribute_value in octets
  * @param attribute_value The current value of the attribute
  * @retval None
  */
void aci_gatt_indication_event(uint16_t connection_handle,
                               uint16_t attribute_handle,
                               uint8_t attribute_value_length,
                               uint8_t attribute_value[])
{
  ble_status_t ret_status;

  /* This callback should be called when we connect the .box also to something
  * that could work also like server mode.
  * In our case we don't need to do nothing when we receive this indication,
  * except it's confirmation
  */

  BLE_MANAGER_PRINTF("aci_gatt_indication_event\r\n");
#ifdef BLE_MANAGER_DEBUG
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("aci_gatt_indication_event:\r\n");
  BLE_MANAGER_PRINTF("\tConnection_Handle=0x%x\r\n", connection_handle);
  BLE_MANAGER_PRINTF("\tAttribute_Handle=0x%x\r\n", attribute_handle);
  if (attribute_value_length == 4U)
  {
    uint16_t start_handle = 0;
    uint16_t stop_handle  = 0;
    /* Should be the range of Handles */
    start_handle = (((uint16_t) attribute_value[1]) << 8);
    start_handle = start_handle  | ((uint16_t)attribute_value[0]);
    stop_handle  = (((uint16_t) attribute_value[3]) << 8);
    stop_handle = stop_handle | ((uint16_t)attribute_value[2]);
    BLE_MANAGER_PRINTF("\tFrom Handles =0x%x to 0x%x\r\n", start_handle, stop_handle);
  }
#endif /* (BLE_DEBUG_LEVEL>2) */
  BLE_MANAGER_PRINTF("Nothing to do except send confirmation\r\n");
#endif /* BLE_MANAGER_DEBUG */

  ret_status = aci_gatt_confirm_indication(connection_handle);

  if (ret_status != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("Error: aci_gatt_confirm_indicationt failed %d\r\n", ret_status);
#if (BLE_DEBUG_LEVEL>2)
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gatt_confirm_indication Done\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */
  }
}

#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#if (BLUE_CORE == STM32WB05N)
/**
  * @brief This event is generated when an indication is received from the server.
  * @param connection_handle Connection handle related to the response
  * @param attribute_handle The handle of the attribute
  * @param attribute_value_length Length of attribute_value in octets
  * @param attribute_value The current value of the attribute
  * @retval None
  */
void aci_gatt_clt_indication_event(uint16_t connection_handle,
                                   uint16_t CID,
                                   uint16_t attribute_handle,
                                   uint16_t attribute_value_length,
                                   uint8_t attribute_value[])
{
  ble_status_t ret_status;

  /* This callback should be called when we connect the .box also to something
  * that could work also like server mode.
  * In our case we don't need to do nothing when we receive this indication,
  * except it's confirmation
  */
#ifdef BLE_MANAGER_DEBUG

  BLE_MANAGER_PRINTF("aci_gatt_clt_indication_event:\r\n");

#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("\tConnection_Handle=0x%x\r\n", connection_handle);
  BLE_MANAGER_PRINTF("\tAttribute_Handle=0x%x\r\n", attribute_handle);
  if (attribute_value_length == 4U)
  {
    uint16_t start_handle = 0;
    uint16_t stop_handle  = 0;
    /* Should be the range of Handles */
    start_handle = (((uint16_t) attribute_value[1]) << 8);
    start_handle = start_handle  | ((uint16_t)attribute_value[0]);
    stop_handle  = (((uint16_t) attribute_value[3]) << 8);
    stop_handle = stop_handle | ((uint16_t)attribute_value[2]);
    BLE_MANAGER_PRINTF("\tFrom Handles =0x%x to 0x%x\r\n", start_handle, stop_handle);
  }
#endif /* (BLE_DEBUG_LEVEL>2) */
  BLE_MANAGER_PRINTF("Nothing to do except send confirmation\r\n");
#endif /* BLE_MANAGER_DEBUG */

  ret_status = aci_gatt_clt_confirm_indication(connection_handle, 0x0004);

  if (ret_status != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("Error: aci_gatt_clt_indication_event failed %d\r\n", ret_status);
#if (BLE_DEBUG_LEVEL>2)
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gatt_confirm_indication Done\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */
  }
}
#else /* (BLUE_CORE == STM32WB05N) */
/**
  * @brief This event is generated when an indication is received from the server.
  * @param connection_handle Connection handle related to the response
  * @param attribute_handle The handle of the attribute
  * @param attribute_value_length Length of attribute_value in octets
  * @param attribute_value The current value of the attribute
  * @retval None
  */
void aci_gatt_clt_indication_event(uint16_t connection_handle,
                                   uint16_t attribute_handle,
                                   uint16_t attribute_value_length,
                                   uint8_t attribute_value[])
{
  ble_status_t ret_status;

  /* This callback should be called when we connect the .box also to something
  * that could work also like server mode.
  * In our case we don't need to do nothing when we receive this indication,
  * except it's confirmation
  */
#ifdef BLE_MANAGER_DEBUG

  BLE_MANAGER_PRINTF("aci_gatt_clt_indication_event:\r\n");

#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("\tConnection_Handle=0x%x\r\n", connection_handle);
  BLE_MANAGER_PRINTF("\tAttribute_Handle=0x%x\r\n", attribute_handle);
  if (attribute_value_length == 4U)
  {
    uint16_t start_handle = 0;
    uint16_t stop_handle  = 0;
    /* Should be the range of Handles */
    start_handle = (((uint16_t) attribute_value[1]) << 8);
    start_handle = start_handle  | ((uint16_t)attribute_value[0]);
    stop_handle  = (((uint16_t) attribute_value[3]) << 8);
    stop_handle = stop_handle | ((uint16_t)attribute_value[2]);
    BLE_MANAGER_PRINTF("\tFrom Handles =0x%x to 0x%x\r\n", start_handle, stop_handle);
  }
#endif /* (BLE_DEBUG_LEVEL>2) */
  BLE_MANAGER_PRINTF("Nothing to do except send confirmation\r\n");
#endif /* BLE_MANAGER_DEBUG */

  ret_status = aci_gatt_clt_confirm_indication(connection_handle);

  if (ret_status != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("Error: aci_gatt_clt_indication_event failed %d\r\n", ret_status);
#if (BLE_DEBUG_LEVEL>2)
  }
  else
  {
    BLE_MANAGER_PRINTF("aci_gatt_confirm_indication Done\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */
  }
}
#endif /* (BLUE_CORE == STM32WB05N) */
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

#if (BLUE_CORE != STM32WB05N)
/**
  * @brief This event is generated by the client/server to the application on a GATT timeout (30 seconds).
  *        This is a critical event that should not happen during normal operating conditions.
  *        It is an indication of either a major disruption in the communication link or a mistake in the application
  *        which does not provide a reply to GATT procedures. After this event, the GATT channel is closed and no more
  *        GATT communication can be performed. The applications is expected to issue an
  *        @ref aci_gap_terminate to disconnect from the peer device. It is important to leave an 100 ms blank
  *             window before sending the
  *        @ref aci_gap_terminate, since immediately after this event, system could save important information
  *             in non volatile memory.
  * @param connection_handle Connection handle on which the GATT procedure has timed out
  * @retval None
  */
void aci_gatt_proc_timeout_event(uint16_t connection_handle)
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
  aci_gap_terminate(connection_handle, 0x13);
}
#else /* (BLUE_CORE != STM32WB05N) */
/**
  * @brief This event is generated by the client/server to the application on a GATT timeout (30 seconds).
  *        This is a critical event that should not happen during normal operating conditions.
  *        It is an indication of either a major disruption in the communication link or a mistake in the application
  *        which does not provide a reply to GATT procedures. After this event, the GATT channel is closed and no more
  *        GATT communication can be performed. The applications is expected to issue an
  *        @ref aci_gap_terminate to disconnect from the peer device. It is important to leave an 100 ms blank
  *             window before sending the
  *        @ref aci_gap_terminate, since immediately after this event, system could save important information
  *             in non volatile memory.
  * @param connection_handle Connection handle on which the GATT procedure has timed out
  * @retval None
  */
void aci_gatt_proc_timeout_event(uint16_t connection_handle, uint16_t CID)
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
  aci_gap_terminate(connection_handle, 0x13);
}
#endif /* (BLUE_CORE != STM32WB05N) */

/**
  * @brief The Hardware Error event is used to indicate some implementation specific type of hardware failure for the
  *        controller. This event is used to notify the Host that a hardware failure has occurred in the Controller.
  * @param hardware_code Hardware Error Event code.
  *        Error code 0x01 and 0x02 are errors generally caused by hardware issue on the PCB;
  *        another possible cause is a slow crystal startup.
  *        In the latter case, the HS_STARTUP_TIME in the device configuration needs to be tuned.
  *        Error code 0x03 indicates an internal error of the protocol stack.
  *        After this event is recommended to force device reset.
  *        Values:
  *        - 0x01: Radio state error
  *        - 0x02: Timer overrun error
  *        - 0x03: Internal queue overflow error
  * @retval None
  */
void hci_hardware_error_event(uint8_t hardware_code)
{
  hardware_error_event_handler_function(hardware_code);
}

#if (BLUE_CORE != STM32WB05N)
/*******************************************************************************
  * Function Name  : aci_gap_bond_lost_event
  * Description    : This event is generated on the slave when a
  *                  ACI_GAP_SLAVE_SECURITY_REQUEST is called to reestablish the bond.
  * Input          : See file bluenrg1_events.h
  * Output         : See file bluenrg1_events.h
  * Return         : See file bluenrg1_events.h
  *******************************************************************************/
void aci_gap_bond_lost_event(void)
{
  aci_gap_allow_rebond(global_connection_handle);
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("aci_gap_allow_rebond()\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */
}
#endif /* (BLUE_CORE != STM32WB05N) */
/**
  * @brief  This function Updates the White list for BLE Connection
  * @param None
  * @retval None
  */
static void update_white_list(void)
{
  ble_status_t ret_status;
  uint8_t num_of_addresses;

  bonded_device_entry_t bonded_device_entry[BLE_MANAGER_MAX_BONDED_DEVICES];

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
  ret_status =  aci_gap_get_bonded_devices(&num_of_addresses, bonded_device_entry);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
  ret_status =  aci_gap_get_bonded_devices(0, BLE_MANAGER_MAX_BONDED_DEVICES, &num_of_addresses, bonded_device_entry);
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

  if (ret_status == BLE_STATUS_SUCCESS)
  {
    if (num_of_addresses > 0U)
    {
#if (BLE_DEBUG_LEVEL>2)
      BLE_MANAGER_PRINTF("Bonded with %d Device(s): \r\n", num_of_addresses);
#endif /* (BLE_DEBUG_LEVEL>2) */

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
      ret_status = aci_gap_configure_whitelist();
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#if (BLUE_CORE == STM32WB05N)
      ret_status = aci_gap_configure_filter_accept_and_resolving_list(0x01 /* White List */);
#else /* (BLUE_CORE == STM32WB05N) */
      ret_status = aci_gap_configure_white_and_resolving_list(0x01 /* White List */);
#endif /* (BLUE_CORE == STM32WB05N) */
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
      if (ret_status != BLE_STATUS_SUCCESS)
      {
        BLE_MANAGER_PRINTF("Error: aci_gap_configure_whitelist() failed:0x%02x\r\n", ret_status);
#if (BLE_DEBUG_LEVEL>2)
      }
      else
      {
        BLE_MANAGER_PRINTF("aci_gap_configure_whitelist --> SUCCESS\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */
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
void aci_gap_numeric_comparison_value_event(uint16_t connection_handle, uint32_t numeric_value)
{
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("aci_gap_numeric_comparison_value_event Numeric Value=%ld\r\n", (long)numeric_value);
#endif /* (BLE_DEBUG_LEVEL>2) */

  /* Confirm Yes... without control of Numeric Value received from Master */
  aci_gap_numeric_comparison_value_confirm_yesno(connection_handle, 0x01);
}

/*******************************************************************************
  * Function Name  : hci_encryption_change_event
  * Description    : It is used to indicate that the change of the encryption
  *                  mode has been completed
  * Input          : See file bluenrg1_events.h
  * Output         : See file bluenrg1_events.h
  * Return         : See file bluenrg1_events.h
  *******************************************************************************/
void hci_encryption_change_event(uint8_t status, uint16_t connection_handle, uint8_t encryption_enabled)
{
#if (BLE_DEBUG_LEVEL>2)
  BLE_MANAGER_PRINTF("hci_encryption_change_event\r\n");
#endif /* (BLE_DEBUG_LEVEL>2) */
}
#endif /* (BLUE_CORE != BLUENRG_MS) */

/*************************** Bluetooth Communication **************************/
#if (BLUE_CORE == BLUENRG_MS)
/**
  * @brief  This function is called when there is a LE Connection Complete event.
  * @param  uint16_t connection_handle
  * @param  uint8_t addr[6]
  * @retval None
  */
__weak void connection_completed_function(uint16_t connection_handle, uint8_t addr[6])
#elif (BLUE_CORE == BLUE_WB)
/**
  * @brief  This function is called when there is a LE Connection Complete event.
  * @param  uint16_t connection_handle
  * @retval None
  */
__weak void connection_completed_function(uint16_t connection_handle)
#else /* (BLUE_CORE == BLUENRG_MS) */
/**
  * @brief  This function is called when there is a LE Connection Complete event.
  * @param  uint16_t connection_handle
  * @param  uint8_t address_type
  * @param  uint8_t addr[6]
  * @retval None
  */
__weak void connection_completed_function(uint16_t connection_handle, uint8_t address_type, uint8_t addr[6])
#endif /* (BLUE_CORE == BLUENRG_MS) */
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(connection_handle);
#if (BLUE_CORE == BLUENRG_MS)
  BLE_UNUSED(addr);
#else /* (BLUE_CORE == BLUENRG_MS) */
  BLE_UNUSED(address_type);
  BLE_UNUSED(addr);
#endif /* (BLUE_CORE == BLUENRG_MS) */

  custom_command_page_level = 0;

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ConnectionCompletedFunction could be implemented in the user file
   */

  BLE_MANAGER_PRINTF("Call to ConnectionCompletedFunction (It is a weak function)\r\n");
  BLE_MANAGER_DELAY(100);
}

/**
  * @brief  This function is called when the peer device get disconnected.
  * @param  None
  * @retval None
  */
__weak void disconnection_completed_function(void)
{
  BLE_MANAGER_PRINTF("Call to disconnection_completed_function (It is a weak function)\r\n");
  BLE_MANAGER_DELAY(100);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the DisconnectionCompletedFunction could be implemented in the user file
   */
}

/**
  * @brief  This function is called when the pairing process has completed successfully
  *         or a pairing procedure timeout has occurred or the pairing has failed.
  * @param  uint8_t pairing_status
  * @retval None
  */
__weak void pairing_completed_function(uint8_t pairing_status)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(pairing_status);

  BLE_MANAGER_PRINTF("Call to pairing_completed_function (It is a weak function)\r\n");
  BLE_MANAGER_DELAY(100);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the pairing_completed_function could be implemented in the user file
   */
}

/**
  * @brief  This function is called when the device is put in connectable mode.
  * @param  uint8_t *manuf_data Filling Manufacter Advertise data
  * @retval None
  */
__weak void set_connectable_function(uint8_t *manuf_data)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(manuf_data);

  BLE_MANAGER_PRINTF("Call to set_connectable_function (It is a weak function)\r\n");
  BLE_MANAGER_DELAY(100);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the set_connectable_function could be implemented in the user file
   */
}

__weak void mtu_exchange_resp_event_function(uint16_t server_rx_mtu)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(server_rx_mtu);

  BLE_MANAGER_PRINTF("Call to mtu_exchange_resp_event_function (It is a weak function)\r\n");
  BLE_MANAGER_DELAY(100);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the mtu_exchange_resp_event_function could be implemented in the user file
   */
}

/**
  * @brief  This function is called when bluetooth congestion buffer occurs
  *         or a pairing procedure timeout has occurred or the pairing has failed.
  * @param  None
  * @retval None
  */
__weak void aci_gatt_tx_pool_available_event_function(void)
{
  BLE_MANAGER_PRINTF("Call to aci_gatt_tx_pool_available_event_function (It is a weak function)\r\n");
  BLE_MANAGER_DELAY(100);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the aci_gatt_tx_pool_available_event_function could be implemented in the user file
   */
}

/**
  * @brief  This event is used to notify the Host that a hardware failure has occurred in the Controller.
  * @param  uint8_t hardware_code Hardware Error event code.
  * @retval None
  */
__weak void hardware_error_event_handler_function(uint8_t hardware_code)
{
#if (BLE_DEBUG_LEVEL>2)
  switch (hardware_code)
  {
    case 0:
      BLE_MANAGER_PRINTF("\r\n-->hci_hardware_error_event hardware_code=\r\n\tNaN<--\r\n");
      break;
    case 1:
      BLE_MANAGER_PRINTF("\r\n-->hci_hardware_error_event hardware_code=\r\n\tRadio state error<--\r\n");
      break;
    case 2:
      BLE_MANAGER_PRINTF("\r\n-->hci_hardware_error_event hardware_code=\r\n\tTimer overrun error<--\r\n");
      break;
    case 3:
      BLE_MANAGER_PRINTF("\r\n-->hci_hardware_error_event hardware_code=\r\n\tInternal queue overflow error<--\r\n");
      break;
  }
#endif /* (BLE_DEBUG_LEVEL>2) */

  BLE_MANAGER_PRINTF("Call to hardware_error_event_handler_function (It is a weak function)\r\n");

  BLE_MANAGER_DELAY(1000);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the hardware_error_event_handler_function could be implemented in the user file
   */
}

/******************************** Debug Console *******************************/
/**
  * @brief  This function makes the parsing of the Debug Console
  * @param  uint8_t *att_data attribute data
  * @param  uint8_t data_length length of the data
  * @retval uint32_t send_back_data true/false
  */
__weak uint32_t debug_console_parsing(uint8_t *att_data, uint8_t data_length)
{
  /* By default answer with the same message received */
  uint32_t send_back_data = 1;

  /* Help Command */
  if ((strncmp("help", (char *)(att_data), 4)) == 0)
  {
    /* Print Legend */
    send_back_data = 0;

    bytes_to_write = sprintf((char *)buffer_to_write, "Command:\r\n"
                             "info-> System Info\r\n"
                             "uid-> STM32 UID value\r\n");
    term_update(buffer_to_write, bytes_to_write);
  }
  else if ((strncmp("info", (char *)(att_data), 4)) == 0)
  {
    send_back_data = 0;

    bytes_to_write = sprintf((char *)buffer_to_write, "\r\nSTMicroelectronics %s:\r\n"
                             "\tVersion %c.%c.%c\r\n"
                             "\tSTM32L4xx MCU Family Name"
                             "\r\n",
                             BLE_FW_PACKAGENAME,
                             BLE_VERSION_FW_MAJOR, BLE_VERSION_FW_MINOR, BLE_VERSION_FW_PATCH);

    term_update(buffer_to_write, bytes_to_write);

    bytes_to_write = sprintf((char *)buffer_to_write,
                             "\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
                             " (IAR)\r\n",
#elif defined (__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* For ARM Compiler 5 and 6 */
                             " (KEIL)\r\n",
#elif defined (__GNUC__)
                             " (STM32CubeIDE)\r\n",
#endif /* IDE cases */
                             __DATE__, __TIME__);

    term_update(buffer_to_write, bytes_to_write);
  }
  else if ((((char)att_data[0] == 'u') == 0) && (((char)att_data[1] == 'i') == 0) && (((char)att_data[2] == 'd') == 0))
  {
    /* Write back the STM32 UID */
    uint8_t *uid = (uint8_t *)BLE_STM32_UUID;
    uint32_t MCU_ID = BLE_STM32_MCU_ID[0] & 0xFFFU;
    bytes_to_write = sprintf((char *)buffer_to_write, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X_%.3lX\r\n",
                             uid[ 3], uid[ 2], uid[ 1], uid[ 0],
                             uid[ 7], uid[ 6], uid[ 5], uid[ 4],
                             uid[11], uid[ 10], uid[9], uid[8],
                             (long)MCU_ID);
    term_update(buffer_to_write, bytes_to_write);
    send_back_data = 0;
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the DebugConsoleParsing could be implemented in the user file
   */

  BLE_MANAGER_PRINTF("Call to debug_console_parsing (It is a weak function)\r\n");

  return send_back_data;
}

/********************************* Config Char ********************************/
/**
  * @brief  This function is called when there is a change on the gatt attribute.
  * @param  None
  * @retval None
  */
__weak void attr_mod_config_function(uint8_t *att_data, uint8_t data_length)
{
  BLE_MANAGER_PRINTF("Call to attr_mod_config_function (It is a weak function)\r\n");
  BLE_MANAGER_DELAY(100);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the attr_mod_config_function could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Config write request.
  * @param uint8_t *att_data attribute data
  * @param uint8_t data_length length of the data
  * @retval None
  */
__weak void write_request_config_function(uint8_t *att_data, uint8_t data_length)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(att_data);
  BLE_UNUSED(data_length);

  BLE_MANAGER_PRINTF("Call to write_request_config_function (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the write_request_config_function could be implemented in the user file
   */
}

#ifndef BLE_MANAGER_NO_PARSON
/************************************************************************************
  * Callback functions to manage the extended configuration characteristic commands *
  ***********************************************************************************/
/**
  * @brief  Callback Function for answering to the UID command
  * @param  uint8_t **uid STM32 UID Return value
  * @retval None
  */
__weak void ext_ext_config_uid_command_callback(uint8_t **uid)
{
#ifdef BLE_STM32_UUID
  *uid = (uint8_t *)BLE_STM32_UUID;
#endif /* BLE_STM32_UUID */

  BLE_MANAGER_PRINTF("Call to ext_ext_config_uid_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtExtConfigUidCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for answering to VersionFw command
  * @param  uint8_t *answer Return String
  * @retval None
  */
__weak void ext_config_version_fw_command_callback(uint8_t *answer)
{
  sprintf((char *)answer, "%s_%s_%c.%c.%c",
          BLE_STM32_MICRO,
          BLE_FW_PACKAGENAME,
          BLE_VERSION_FW_MAJOR,
          BLE_VERSION_FW_MINOR,
          BLE_VERSION_FW_PATCH);

  BLE_MANAGER_PRINTF("Call to ext_config_version_fw_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigVersionFwCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for answering to Info command
  * @param  uint8_t *answer Return String
  * @retval None
  */
__weak void ext_config_info_command_callback(uint8_t *answer)
{
  sprintf((char *)answer, "STMicroelectronics %s:\n"
          "Version %c.%c.%c\n"
          "Compiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
          " (IAR)",
#elif defined (__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* For ARM Compiler 5 and 6 */
          " (KEIL)",
#elif defined (__GNUC__)
          " (STM32CubeIDE)",
#endif /* IDE cases */
          BLE_FW_PACKAGENAME,
          BLE_VERSION_FW_MAJOR,
          BLE_VERSION_FW_MINOR,
          BLE_VERSION_FW_PATCH,
          __DATE__, __TIME__);

  BLE_MANAGER_PRINTF("Call to ext_config_info_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigInfoCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for answering to Help command
  * @param  uint8_t *answer Return String
  * @retval None
  */
__weak void ext_config_help_command_callback(uint8_t *answer)
{
  sprintf((char *)answer, "List of available command:\n"
          "1) Board Report\n"
          "- STM32 UID\n"
          "- Version Firmware\n"
          "- Info\n"
          "- Help\n\n");

  BLE_MANAGER_PRINTF("Call to ext_config_help_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigHelpCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for answering to PowerStatus command
  * @param  uint8_t *answer Return String
  * @retval None
  */
__weak void ext_config_power_status_command_callback(uint8_t *answer)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(answer);

  sprintf((char *)answer, "Weak function: \n"
          "To be implemented it in the user file for managing the received command\n\n");

  BLE_MANAGER_PRINTF("Call to ext_config_power_status_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigPowerStatusCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the ChangePin command
  * @param  uint32_t new_pin
  * @retval None
  */
__weak void ext_config_change_pin_command_callback(uint32_t new_pin)
{
  BLE_MANAGER_PRINTF("New Board Pin= <%ld>\r\n", (long)new_pin);
  ble_stack_value.secure_pin = new_pin;

  BLE_MANAGER_PRINTF("Call to ext_config_change_pin_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigChangePinCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the ClearDB command
  * @param  None
  * @retval None
  */
__weak void ext_config_clear_db_command_callback(void)
{
  BLE_MANAGER_PRINTF("Call to ext_config_clear_db_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ext_config_clear_db_command_callback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the ReadCert command
  * @param  uint8_t *certificate Certificate to register
  * @retval None
  */
__weak void ext_config_read_cert_command_callback(uint8_t *certificate)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(certificate);

  BLE_MANAGER_PRINTF("Call to ext_config_read_cert_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigReadCertCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the SetCert command
  * @param  uint8_t *certificate registered certificate
  * @retval None
  */
__weak void ext_config_set_cert_command_callback(uint8_t *certificate)
{
  BLE_MANAGER_PRINTF("Certificate From Dashboard= <%s>\r\n", certificate);

  BLE_MANAGER_PRINTF("Call to ext_config_set_cert_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigSetCertCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the DFU command
  * @param  None
  * @retval None
  */
__weak void ext_config_reboot_on_dfu_mode_command_callback(void)
{
  BLE_MANAGER_PRINTF("Call to ext_config_reboot_on_dfu_mode_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigRebootOnDFUModeCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the PowerOff command
  * @param  None
  * @retval None
  */
__weak void ext_config_power_off_command_callback(void)
{
  BLE_MANAGER_PRINTF("Call to ext_config_power_off_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ext_config_power_off_command_callback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for answering to ReadBanksFwId command
  * @param  uint8_t *cur_bank Number Current Bank
  * @param  uint16_t *fw_id1 Bank1 Firmware Id
  * @param  uint16_t *fw_id2 Bank2 Firmware Id
  * @retval None
  */
__weak void ext_config_read_banks_fw_id_command_callback(uint8_t *cur_bank, uint16_t *fw_id1, uint16_t *fw_id2)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(cur_bank);
  BLE_UNUSED(fw_id1);
  BLE_UNUSED(fw_id2);

  BLE_MANAGER_PRINTF("Call to ext_config_read_banks_fw_id_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigReadBanksFwIdCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for answering to BanksSwap command
  * @param  None
  * @retval None
  */
__weak void ext_config_banks_swap_command_callback(void)
{

  BLE_MANAGER_PRINTF("Call to ext_config_banks_swap_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigReadBanksFwIdCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the SetName command
  * @param  uint8_t *new_name
  * @retval None
  */
__weak void ext_config_set_name_command_callback(uint8_t *new_name)
{
  BLE_MANAGER_PRINTF("New Board Name = <%s>\r\n", new_name);
  /* Change the Board Name */
  sprintf(ble_stack_value.board_name, "%s", new_name);

  BLE_MANAGER_PRINTF("Call to ext_config_set_name_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigSetNameCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Custom commands definition
  * @param  JSON_Array *json_command_array
  * @retval None
  */
__weak void ext_config_read_custom_commands_callback(JSON_Array *json_command_array)
{
  /* Clear the previous Costom Command List */
  CLEAR_CUSTOM_COMMANDS_LIST();

  if (custom_command_page_level == 0U)
  {

    /* Add all the custom Commands */
    if (ADD_CUSTOM_COMMAND("IntValue1", /* Name */
                           BLE_CUSTOM_COMMAND_INTEGER, /* Type */
                           100, /* Default Value */
                           -100, /* MIN */
                           200,  /* MAX */
                           NULL, /* Enum Int */
                           NULL, /* Enum String */
                           NULL, /* Description */
                           json_command_array))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "IntValue1");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "IntValue1");
      return;
    }

    if (ADD_CUSTOM_COMMAND("IntValue2", /* Name */
                           BLE_CUSTOM_COMMAND_INTEGER, /* Type */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                           10, /* MIN */
                           3000,  /* MAX */
                           NULL, /* Enum Int */
                           NULL, /* Enum String */
                           NULL, /* Description */
                           json_command_array))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "IntValue2");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "IntValue2");
      return;
    }

    if (ADD_CUSTOM_COMMAND("VoidCommand", /* Name */
                           BLE_CUSTOM_COMMAND_VOID, /* Type */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  /* MAX */
                           NULL, /* Enum Int */
                           NULL, /* Enum String */
                           "Example Void Command", /* Description */
                           json_command_array))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "Command1");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "Command1");
      return;
    }

    if (ADD_CUSTOM_COMMAND("StringValue1", /* Name */
                           BLE_CUSTOM_COMMAND_STRING, /* Type */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                           20,  /* MAX */
                           NULL, /* Enum Int */
                           NULL, /* Enum String */
                           NULL, /* Description */
                           json_command_array))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "StringValue1");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "StringValue1");
      return;
    }

    if (ADD_CUSTOM_COMMAND("BooleanValue", /* Name */
                           BLE_CUSTOM_COMMAND_BOOLEAN, /* Type */
                           1, /* Default Value */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  /* MAX */
                           NULL, /* Enum Int */
                           NULL, /* Enum String */
                           "Example for Boolean", /* Description */
                           json_command_array))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "BooleanValue");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "BooleanValue");
      return;
    }

    if (ADD_CUSTOM_COMMAND("StringValue2", /* Name */
                           BLE_CUSTOM_COMMAND_STRING, /* Type */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                           4, /* MIN */
                           10,  /* MAX */
                           NULL, /* Enum Int */
                           NULL, /* Enum String */
                           "It's possible to add a  very very very very very very long description", /* Description */
                           json_command_array))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "StringValue2");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "StringValue2");
      return;
    }

    /* Example of Enum String Custom Command */
    {
      /* The Last value should be NULL */
      char *ValidStringValues[] = {"Ciao", "Buona", "Giornata", NULL};
      if (ADD_CUSTOM_COMMAND("StringEnum", /* Name */
                             BLE_CUSTOM_COMMAND_ENUM_STRING, /* Type */
                             1, /* Default Value */
                             (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                             (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  /* MAX */
                             NULL, /* Enum Int */
                             (void *)ValidStringValues, /* Enum String */
                             "Example of Enum String", /* Description */
                             json_command_array))
      {
        BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "StringEnum");
      }
      else
      {
        BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "StringEnum");
        return;
      }
    }

    /* Example of Enum Int Custom Command */
    {
      /* The Last value should be BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN */
      int32_t ValidIntValues[] = {-1, 12, 123, 321, (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN};
      if (ADD_CUSTOM_COMMAND("IntEnum", /* Name */
                             BLE_CUSTOM_COMMAND_ENUM_INTEGER, /* Type */
                             3, /* Default Value */
                             (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                             (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  /* MAX */
                             (void *) ValidIntValues, /* Enum Int */
                             NULL, /* Enum String */
                             "Example of Enum Integer", /* Description */
                             json_command_array))
      {
        BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "IntEnum");
      }
      else
      {
        BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "IntEnum");
        return;
      }
    }

    if (ADD_CUSTOM_COMMAND("ChangeCustomCommand", /* Name */
                           BLE_CUSTOM_COMMAND_VOID, /* Type */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  /* MAX */
                           NULL, /* Enum Int */
                           NULL, /* Enum String */
                           "Change the Custom Commands", /* Description */
                           json_command_array))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "Command1");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "Command1");
      return;
    }

    /* Just one Example of one Invalid Command */
    BLE_MANAGER_PRINTF("Example of trying to add one Invalid Custom Command\r\n");
    if (ADD_CUSTOM_COMMAND("ReadCert", /* Name */
                           BLE_CUSTOM_COMMAND_STRING, /* Type */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                           4, /* MIN */
                           10,  /* MAX */
                           NULL, /* Enum Int */
                           NULL, /* Enum String */
                           "Invalid Command...", /* Description */
                           json_command_array))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "ReadCert");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "ReadCert");
      return; /* not mandatory... it's the last one */
    }
  }
  else if (custom_command_page_level == 1U)
  {
    if (ADD_CUSTOM_COMMAND("ComeBackCustomCommand", /* Name */
                           BLE_CUSTOM_COMMAND_VOID, /* Type */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  /* MAX */
                           NULL, /* Enum Int */
                           NULL, /* Enum String */
                           "Come back to previous Custom Commands", /* Description */
                           json_command_array))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "Command1");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "Command1");
      return;
    }

    if (ADD_CUSTOM_COMMAND("StringValueNewLevel", /* Name */
                           BLE_CUSTOM_COMMAND_STRING, /* Type */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                           (int32_t)BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                           20,  /* MAX */
                           NULL, /* Enum Int */
                           NULL, /* Enum String */
                           NULL, /* Description */
                           json_command_array))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "StringValueNewLevel");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "StringValue1");
      return;
    }
  }

  BLE_MANAGER_PRINTF("Call to ext_config_read_custom_commands_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigReadCustomCommandsCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the custom command
  * @param  ble_custom_commad_result_t *custom_command:
  * @param                            uint8_t *CommandName: Name of the command
  * @param                            custom_command->CommandType: Type of the command
  * @param                            int32_t IntValue:    Integer or boolean parameter
  * @param                            uint8_t *StringValue: String parameter
  * @retval None
  */
__weak void  ext_config_custom_command_callback(ble_custom_commad_result_t *custom_command)
{
  BLE_MANAGER_PRINTF("Received Custom Command:\r\n");
  BLE_MANAGER_PRINTF("\tCommand Name: <%s>\r\n", custom_command->command_name);
  BLE_MANAGER_PRINTF("\tCommand Type: <%d>\r\n", custom_command->command_type);

  switch (custom_command->command_type)
  {
    case BLE_CUSTOM_COMMAND_VOID:
      if ((strncmp((char *)custom_command->command_name, "BleManagerReset", 15) == 0))
      {

        /* Sample code to reset BLE_Manager */
        /*
              aci_gap_terminate(CurrentConnectionHandle, 0x13);
              HAL_Delay(5000);
              needToResetBLE=1;
        */
      }
      break;
    case BLE_CUSTOM_COMMAND_INTEGER:
      BLE_MANAGER_PRINTF("\tInt    Value: <%ld>\r\n", (long)custom_command->int_value);
      break;
    case BLE_CUSTOM_COMMAND_ENUM_INTEGER:
      BLE_MANAGER_PRINTF("\tInt     Enum: <%ld>\r\n", (long)custom_command->int_value);
      break;
    case BLE_CUSTOM_COMMAND_BOOLEAN:
      BLE_MANAGER_PRINTF("\tInt    Value: <%ld>\r\n", (long)custom_command->int_value);
      break;
    case  BLE_CUSTOM_COMMAND_STRING:
      BLE_MANAGER_PRINTF("\tString Value: <%s>\r\n", custom_command->string_value);
      break;
    case  BLE_CUSTOM_COMMAND_ENUM_STRING:
      BLE_MANAGER_PRINTF("\tString  Enum: <%s>\r\n", custom_command->string_value);
      break;
  }

  if ((strncmp((char *)custom_command->command_name, "ChangeCustomCommand", 20) == 0))
  {
    custom_command_page_level = 1;
    send_new_custom_command_list();
  }
  else if ((strncmp((char *)custom_command->command_name, "ComeBackCustomCommand", 21) == 0))
  {
    custom_command_page_level = 0;
    send_new_custom_command_list();
  }
  else if ((strncmp((char *)custom_command->command_name, "IntValue2", 9) == 0))
  {
    send_error("Example of Error");
  }
  else if ((strncmp((char *)custom_command->command_name, "IntValue1", 9) == 0))
  {
    send_info("Example of Info");
  }

  BLE_MANAGER_PRINTF("Call to ext_config_custom_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigCustomCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the SetDate command
  * @param  uint8_t *new_date
  * @retval None
  */
__weak void ext_config_set_date_command_callback(uint8_t *new_date)
{
  BLE_MANAGER_PRINTF("New Board Date= <%s>\r\n", new_date);

  BLE_MANAGER_PRINTF("Call to ext_config_set_date_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigSetDateCommandCallback could be implemented in the user file
           for managing the received command (Insert the code for changing the RTC Date)
   */
}

/**
  * @brief  Callback Function for managing the SetTime command
  * @param  uint8_t *new_time
  * @retval None
  */
__weak void ext_config_set_time_command_callback(uint8_t *new_time)
{
  BLE_MANAGER_PRINTF("New Board Time= <%s>\r\n", new_time);

  BLE_MANAGER_PRINTF("Call to ext_config_set_time_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigSetTimeCommandCallback could be implemented in the user file
           for managing the received command (Insert the code for changing the RTC Time)
   */
}

/**
  * @brief  Callback Function for managing the SetWiFi command
  * @param  ble_wifi_cred_acc_t new_wifi_cred
  * @retval None
  */
__weak void ext_config_set_wi_fi_command_callback(ble_wifi_cred_acc_t new_wifi_cred)
{
  BLE_MANAGER_PRINTF("new_wifi_cred=\r\n");
  BLE_MANAGER_PRINTF("\tSSID    = <%s>\r\n", new_wifi_cred.ssid);
  BLE_MANAGER_PRINTF("\tPassWd = <%s>\r\n", new_wifi_cred.pass_wd);
  BLE_MANAGER_PRINTF("\tSecurity= <%s>\r\n", new_wifi_cred.security);

  BLE_MANAGER_PRINTF("Call to ext_config_set_wi_fi_command_callback (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigSetTimeCommandCallback could be implemented in the user file
           for managing the received command (Insert the code for changing the Wi-Fi Credential)
   */
}
#endif /* BLE_MANAGER_NO_PARSON */
