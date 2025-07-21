/**
  ******************************************************************************
  * @file    ble_manager.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   BLE Manager services APIs
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BLE_MANAGER_H_
#define _BLE_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>

#include "ble_manager_conf.h"

#if !defined(BLE_UNUSED)
#define BLE_UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#endif /* BLE_UNUSED */

#ifndef BLE_MANAGER_NO_PARSON
#include "parson.h"
#endif /* BLE_MANAGER_NO_PARSON */

/* Bluetooth core supported */
#define BLUENRG_1_2     0x00
#define BLUENRG_MS      0x01
#define BLUENRG_LP      0x02
#define BLUE_WB         0x03
#define STM32WB07_06    0x04
#define STM32WB05N      0x05

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
#elif (BLUE_CORE == STM32WB07_06)
#include "stm32wb07_06_aci.h"
#include "stm32wb07_06_hci_le.h"
#include "stm32wb07_06_events.h"
#include "hci_const.h"
#include "hci_tl.h"
#include "hci.h"
#include "stm32wb07_06_utils.h"
#elif (BLUE_CORE == STM32WB05N)
#include "stm32wb05n_aci.h"
#include "stm32wb05n_hci_le.h"
#include "stm32wb05n_events.h"
#include "hci_const.h"
#include "hci_tl.h"
#include "hci.h"
#include "stm32wb_utils.h"
#else /* BLUENRG_1_2 */
#include "hci.h"
#include "bluenrg1_hal_aci.h"
#include "bluenrg1_gatt_aci.h"
#include "bluenrg1_gap_aci.h"
#include "bluenrg1_hci_le.h"
#include "bluenrg1_l2cap_aci.h"
#include "bluenrg1_events.h"
#endif /* (BLUE_CORE == BLUENRG_MS) */

/* Exported Defines ----------------------------------------------------------*/

/* BLE Manager Version only numbers 0->9 */
#define BLE_MANAGER_VERSION_MAJOR '2'
#define BLE_MANAGER_VERSION_MINOR '0'
#define BLE_MANAGER_VERSION_PATCH '0'

/* Length of AdvData in octets */
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
#define BLE_MANAGER_ADVERTISE_DATA_LENGHT 25
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#define BLE_MANAGER_ADVERTISE_DATA_LENGHT 28
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

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
#define BLE_MANAGER_SENSOR_TILE_BOX_PRO_C_PLATFORM 0x13U
#define BLE_MANAGER_NUCLEO_PLATFORM                0x80U
#define BLE_MANAGER_STM32U5A5ZJ_NUCLEO_PLATFORM    0x7AU
#define BLE_MANAGER_STM32U575ZI_NUCLEO_PLATFORM    0x7BU
#define BLE_MANAGER_STM32F446RE_NUCLEO_PLATFORM    0x7CU
#define BLE_MANAGER_STM32L053R8_NUCLEO_PLATFORM    0x7DU
#define BLE_MANAGER_STM32L476RG_NUCLEO_PLATFORM    0x7EU
#define BLE_MANAGER_STM32F401RE_NUCLEO_PLATFORM    0x7FU
#define BLE_MANAGER_WB05N_PLATFORM                 0x8FU
#define BLE_MANAGER_UNDEF_PLATFORM                 0xFFU

#define BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN 0xDEADBEEF

#define BLE_MANAGER_READ_CUSTOM_COMMAND "ReadCustomCommand"
#ifdef BLE_MANAGER_SDKV2
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
#define BLE_MANAGER_CUSTOM_FIELD1 15
#define BLE_MANAGER_CUSTOM_FIELD2 16
#define BLE_MANAGER_CUSTOM_FIELD3 17
#define BLE_MANAGER_CUSTOM_FIELD4 18
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
#define BLE_MANAGER_CUSTOM_FIELD1 18
#define BLE_MANAGER_CUSTOM_FIELD2 19
#define BLE_MANAGER_CUSTOM_FIELD3 20
#define BLE_MANAGER_CUSTOM_FIELD4 21
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
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
#define HCI_LE_SET_SCAN_RESPONSE_DATA hci_le_set_scan_resp_data
#define ACI_GATT_EXCHANGE_CONFIG      aci_gatt_exchange_configuration
#define ACI_GAP_PASS_KEY_RESP         aci_gap_pass_key_response
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
  uint8_t config_value_offsets;
  uint8_t config_value_length;
  uint8_t gap_roles;
  uint8_t io_capabilities;
  uint8_t authentication_requirements;
  uint8_t mitm_protection_requirements;
#if (BLUE_CORE == BLUENRG_MS)
  uint8_t out_of_band_enable_data;
  uint8_t *oob_data;
#else /* (BLUE_CORE == BLUENRG_MS) */
  uint8_t secure_connection_support_option_code;
  uint8_t secure_connection_keypress_notification;
#endif /* (BLUE_CORE == BLUENRG_MS) */

  /* To set the TX power level of the bluetooth device */
  /* ----------------------
     | 0x00: Normal Power |
     | 0x01: High Power   |
     ---------------------- */
  uint8_t enable_high_power_mode;

  /* Values: 0x00 ... 0x31 - The value depends on the device */
  uint8_t power_amplifier_output_level;

  /* BLE Manager services setting */
  uint8_t enable_config;
  uint8_t enable_console;
#ifndef BLE_MANAGER_NO_PARSON
  uint8_t enable_ext_config;
#endif /* BLE_MANAGER_NO_PARSON */

  /* BLE Board Name */
  char board_name[8];

  /* For enabling the Secure BLE connection */
  uint8_t enable_secure_connection;

  /* Secure Connection PIN */
  uint32_t secure_pin;

  /* For creating a Random Connection PIN */
  uint8_t enable_random_secure_pin;
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
  uint8_t advertising_filter;
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

  /* Set to 1 for forcing a full BLE rescan for the Android/iOS "ST BLE Sensor" application */
  /* with the Secure connection it should not necessary because it will be managed directly by BLE Chip */
  uint8_t force_rescan;

  /* Set to PUBLIC_ADDR or RANDOM_ADDR */
  uint8_t own_address_type;

  /* Bluetooth Board Mac Address */
  uint8_t ble_mac_address[6];

  /* Advertising parameters */
  uint16_t adv_interval_min;
  uint16_t adv_interval_max;

  uint8_t board_id;

} ble_stack_object_t;

typedef struct
{
  /* Enable/Disable Board Report Extended configuration commands */
  uint8_t stm32_uid;
  uint8_t info;
  uint8_t help;
  uint8_t version_fw;
  uint8_t power_status;

  /* Enable/Disable Board Security Extended configuration commands */
  uint8_t change_secure_pin;
  uint8_t clear_secure_data;
  uint8_t read_certificate;
  uint8_t set_certificate;

  /* Enable/Disable Board Control Extended configuration commands */
  uint8_t reboot_on_dfu_mode_command;
  uint8_t power_off;
  uint8_t read_banks;
  uint8_t banks_swap;

  /* Enable/Disable Board Settings Extended configuration commands */
  uint8_t set_name;
  uint8_t read_custom_command;
  uint8_t set_date;
  uint8_t set_time;
  uint8_t set_wifi;
} ble_extended_configuration_object_t;

typedef struct
{
  /* BLE Char Definition */
  uint8_t uuid[16];
  uint8_t char_uuid_type;
#if (BLUE_CORE == BLUENRG_MS)
  uint8_t char_value_length;
#else /* (BLUE_CORE == BLUENRG_MS) */
  uint16_t char_value_length;
#endif /* (BLUE_CORE == BLUENRG_MS) */
  uint8_t char_properties;
  uint8_t security_permissions;
  uint8_t gatt_evt_mask;
  uint8_t enc_key_size;
  uint8_t is_variable;

  /* BLE Attribute handle */
  uint16_t attr_handle;
  /*LE Service handle  */
  uint16_t service_handle;

  /*  Callback function pointers */
  /*  Attribute Modify */
  void (*attr_mod_request_cb)(void *ble_char_pointer,
                              uint16_t attr_handle,
                              uint16_t offset,
                              uint8_t data_length,
                              uint8_t *att_data);
  /* Read Request */
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
  void (*read_request_cb)(void *ble_char_pointer, uint16_t handle);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
  void (*read_request_cb)(void *ble_char_pointer,
                          uint16_t handle,
                          uint16_t connection_handle,
                          uint8_t operation_type,
                          uint16_t attr_val_offset,
                          uint8_t data_length,
                          uint8_t data[]);
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
  /* Write Request */
  void (*write_request_cb)(void *ble_char_pointer,
                           uint16_t attr_handle,
                           uint16_t offset,
                           uint8_t data_length,
                           uint8_t *att_data);
} ble_char_object_t;

/* Enum type for Service Notification Change */
typedef enum
{
  BLE_NOTIFY_NOTHING = 0, /* No Event */
  BLE_NOTIFY_SUB     = 1, /* Subscription Event */
  BLE_NOTIFY_UNSUB   = 2  /* Unsubscription Event */
} ble_notify_event_t;

/* Enum type for Standard Service Enabled or Not */
typedef enum
{
  BLE_SERV_NOT_ENABLE = 0, /* Service Not Enable */
  BLE_SERV_ENABLE    = 1  /* Service Enabled */
} ble_serv_enab_t;

/* Typedef for Wi-Fi credential */
typedef struct
{
  uint8_t *ssid;
  uint8_t *pass_wd;
  uint8_t *security;
} ble_wifi_cred_acc_t;

/* Typedef for Custom Command types */
typedef enum
{
  BLE_CUSTOM_COMMAND_VOID,
  BLE_CUSTOM_COMMAND_INTEGER,
  BLE_CUSTOM_COMMAND_BOOLEAN,
  BLE_CUSTOM_COMMAND_STRING,
  BLE_CUSTOM_COMMAND_ENUM_INTEGER,
  BLE_CUSTOM_COMMAND_ENUM_STRING
} ble_custom_command_types_t;

/* Structure for saving the Custom Commands */
typedef struct
{
  char *command_name;
  ble_custom_command_types_t command_type;
  void *next_command;
} ble_ext_custom_command_t;

/* Typedef for Custom Command */
typedef struct
{
  uint8_t *command_name;
  ble_custom_command_types_t command_type;
  int32_t int_value;
  uint8_t *string_value;
} ble_custom_commad_result_t;

typedef enum
{
  BLE_COMM_TP_START_PACKET = 0x00,
  BLE_COMM_TP_START_END_PACKET = 0x20,
  BLE_COMM_TP_MIDDLE_PACKET = 0x40,
  BLE_COMM_TP_END_PACKET = 0x80,
  BLE_COMM_TP_START_LONG_PACKET = 0x10
} ble_comm_tp_packet_t;

typedef enum
{
  BLE_COMM_TP_WAIT_START = 0,
  BLE_COMM_TP_WAIT_END = 1
} ble_comm_tp_status_t;

/* Exported Variables ------------------------------------------------------- */

extern  ble_serv_enab_t ble_conf_service;
extern  ble_serv_enab_t ble_std_term_service;
extern  ble_serv_enab_t ble_std_err_service;
extern  ble_serv_enab_t ble_ext_conf_service;

extern uint8_t buffer_to_write[256];
extern uint8_t bytes_to_write;
extern uint16_t custom_command_page_level;
extern uint8_t set_connectable;

extern ble_stack_object_t ble_stack_value;
extern ble_extended_configuration_object_t ble_extended_configuration_value;

/* Manufacter Advertise data */
extern uint8_t manuf_data[BLE_MANAGER_ADVERTISE_DATA_LENGHT];

extern uint8_t max_ble_char_std_out_len;
extern uint8_t max_ble_char_std_err_len;

extern ble_ext_custom_command_t *ext_config_custom_commands;
extern ble_ext_custom_command_t *ext_config_last_custom_command;

/* Exported Function prototypes --------------------------------------------- */
#if ((BLUE_CORE == BLUENRG_LP) && (BLUE_CORE == STM32WB07_06))
extern void init_ble_int_for_blue_nrglp(void);
#endif /* (BLUE_CORE == BLUENRG_LP) && (BLUE_CORE == STM32WB07_06)) */
#if (BLUE_CORE == STM32WB05N)
extern void init_ble_int_for_stm32wb05n(void);
#endif /* (BLUE_CORE == STM32WB05N) */
/* Exported functions ------------------------------------------------------- */
#if (BLUE_CORE == BLUE_WB)
extern void hci_le_connection_complete_event(uint8_t status,
                                             uint16_t connection_handle,
                                             uint8_t role,
                                             uint8_t peer_address_type,
                                             uint8_t peer_address[6],
                                             uint16_t conn_interval,
                                             uint16_t conn_latency,
                                             uint16_t supervision_timeout,
                                             uint8_t master_clock_accuracy);

extern void aci_gatt_attribute_modified_event(uint16_t connection_handle,
                                              uint16_t attr_handle,
                                              uint16_t offset,
                                              uint16_t attr_data_length,
                                              uint8_t attr_data[]);

extern void aci_gatt_indication_event(uint16_t connection_handle,
                                      uint16_t attribute_handle,
                                      uint8_t attribute_value_length,
                                      uint8_t attribute_value[]);

#endif /* (BLUE_CORE == BLUE_WB) */

extern ble_status_t std_err_update(uint8_t *data, uint8_t length);
extern ble_status_t term_update(uint8_t *data, uint8_t length);
extern ble_status_t config_update(uint32_t feature, uint8_t command, uint8_t data);
extern ble_status_t config_update_32(uint32_t feature, uint8_t command, uint32_t data);
extern void       set_connectable_ble(void);
extern void update_adv_data(void);
extern void       set_not_connectable_ble(void);

extern ble_status_t init_ble_manager(void);
extern int32_t ble_manager_add_char(ble_char_object_t *ble_char);

extern ble_status_t aci_gatt_update_char_value_wrapper(ble_char_object_t *ble_char_pointer,
                                                       uint8_t char_val_offset,
                                                       uint8_t char_value_len,
                                                       uint8_t *char_value);
extern ble_status_t safe_aci_gatt_update_char_value(ble_char_object_t *ble_char_pointer,
                                                    uint8_t char_val_offset,
                                                    uint8_t char_value_len,
                                                    uint8_t *char_value);

#ifndef BLE_MANAGER_NO_PARSON
/* Add a Custom Command to a Generic Feature */
extern uint8_t generic_add_custom_command(ble_ext_custom_command_t **custom_commands,
                                          ble_ext_custom_command_t **last_custom_command,
                                          char *command_name,
                                          ble_custom_command_types_t command_type,
                                          int32_t default_value,
                                          int32_t min,
                                          int32_t max,
                                          int32_t *valid_values_int,
                                          char **valid_values_string,
                                          char *short_desc,
                                          JSON_Array *json_sensor_array);
/* Little specialization for Ext configuration */
#define ADD_CUSTOM_COMMAND(...) generic_add_custom_command(&ext_config_custom_commands,\
                                                           &ext_config_last_custom_command,\
                                                           __VA_ARGS__)

/* Clear the Custom Commands List for a Generic Feature */
extern void generic_clear_custom_commands_list(ble_ext_custom_command_t **custom_commands,
                                               ble_ext_custom_command_t **last_custom_command);
/* Little specialization for Ext configuration */
#define CLEAR_CUSTOM_COMMANDS_LIST() generic_clear_custom_commands_list(&ext_config_custom_commands,\
                                                                        &ext_config_last_custom_command)

extern void send_new_custom_command_list(void);
extern void send_error(char *message);
extern void send_info(char *message);
#endif /* BLE_MANAGER_NO_PARSON */

extern uint8_t get_blue_nrg_version(uint8_t *hw_version, uint16_t *fw_version);

/**
  * @brief
  * @param  uint8_t* buffer
  * @param  uint8_t len
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_std_out_send_buffer(uint8_t *buffer, uint8_t len);

#ifndef BLE_MANAGER_NO_PARSON
/**
  * @brief  This function is called to parse a BLE_COMM_TP packet.
  * @param  buffer_out: pointer to the output buffer.
  * @param  buffer_in: pointer to the input data.
  * @param  len: buffer in length
  * @retval Buffer out length.
  */
extern uint32_t ble_command_tp_parse(uint8_t **buffer_out, uint8_t *buffer_in, uint32_t len);

/**
  * @brief  This function is called to prepare a BLE_COMM_TP packet.
  * @param  buffer_out: pointer to the buffer used to save BLE_COMM_TP packet.
  * @param  buffer_in: pointer to the input data.
  * @param  len: buffer in length
  * @param  byte_packet_size: Packet Size in Bytes
  * @retval Buffer out length.
  */
extern uint32_t ble_command_tp_encapsulate(uint8_t *buffer_out, uint8_t *buffer_in, uint32_t len,
                                           uint32_t byte_packet_size);

extern ble_status_t ble_ext_configuration_update(uint8_t *data, uint32_t length);

extern ble_custom_commad_result_t *parse_custom_command(ble_ext_custom_command_t *loc_custom_commands,
                                                        uint8_t *hs_command_buffer);
extern ble_custom_commad_result_t *ask_generic_custom_commands(uint8_t *hs_command_buffer);
#endif /* BLE_MANAGER_NO_PARSON */

#ifdef ACC_BLUENRG_CONGESTION
#define ACI_GATT_UPDATE_CHAR_VALUE safe_aci_gatt_update_char_value
#else /* ACC_BLUENRG_CONGESTION */
#define ACI_GATT_UPDATE_CHAR_VALUE aci_gatt_update_char_value_wrapper
#endif /* ACC_BLUENRG_CONGESTION */

/*************************** Bluetooth Communication **************************/
#if (BLUE_CORE == BLUENRG_MS)
extern void connection_completed_function(uint16_t connection_handle, uint8_t addr[6]);
#elif (BLUE_CORE == BLUE_WB)
extern void connection_completed_function(uint16_t connection_handle);
#else /* (BLUE_CORE == BLUENRG_MS) */
extern void connection_completed_function(uint16_t connection_handle, uint8_t address_type, uint8_t addr[6]);
#endif /* (BLUE_CORE == BLUENRG_MS) */

extern void disconnection_completed_function(void);
extern void pairing_completed_function(uint8_t pairing_status);
extern void set_connectable_function(uint8_t *manuf_data);
extern void mtu_exchange_resp_event_function(uint16_t server_rx_mtu);
extern void aci_gatt_tx_pool_available_event_function(void);
extern void hardware_error_event_handler_function(uint8_t hardware_code);

/******************************** Debug Console *******************************/
extern uint32_t debug_console_parsing(uint8_t *att_data, uint8_t data_length);

/********************************* Config Char ********************************/
extern void attr_mod_config_function(uint8_t *att_data, uint8_t data_length);
extern void write_request_config_function(uint8_t *att_data, uint8_t data_length);

#ifndef BLE_MANAGER_NO_PARSON
/***********************************************************************************************
  * Callback functions prototypes to manage the extended configuration characteristic commands *
  **********************************************************************************************/
extern void ext_ext_config_uid_command_callback(uint8_t **uid);
extern void ext_config_version_fw_command_callback(uint8_t *answer);
extern void ext_config_info_command_callback(uint8_t *answer);
extern void ext_config_help_command_callback(uint8_t *answer);
extern void ext_config_power_status_command_callback(uint8_t *answer);

extern void ext_config_change_pin_command_callback(uint32_t new_pin);
extern void ext_config_clear_db_command_callback(void);
extern void ext_config_read_cert_command_callback(uint8_t *certificate);
extern void ext_config_set_cert_command_callback(uint8_t *certificate);

extern void ext_config_reboot_on_dfu_mode_command_callback(void);
extern void ext_config_power_off_command_callback(void);
extern void ext_config_read_banks_fw_id_command_callback(uint8_t *cur_bank, uint16_t *fw_id1, uint16_t *fw_id2);
extern void ext_config_banks_swap_command_callback(void);

extern void ext_config_set_name_command_callback(uint8_t *new_name);
extern void ext_config_read_custom_commands_callback(JSON_Array *json_command_array);
extern void ext_config_custom_command_callback(ble_custom_commad_result_t *custom_command);
extern void ext_config_set_date_command_callback(uint8_t *new_date);
extern void ext_config_set_time_command_callback(uint8_t *new_time);
extern void ext_config_set_wi_fi_command_callback(ble_wifi_cred_acc_t new_wifi_cred);
#endif /* BLE_MANAGER_NO_PARSON */

#ifdef __cplusplus
}
#endif

#include "ble_implementation.h"

#endif /* _BLE_MANAGER_H_ */

