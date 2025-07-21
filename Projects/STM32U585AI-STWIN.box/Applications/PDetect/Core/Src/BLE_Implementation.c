/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ble_implementation.c
  * @author  System Research & Applications Team - Catania Lab.
  * @brief   BLE Implementation header template file.
  *          This file should be copied to the application folder and renamed
  *          to ble_implementation.c.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "ble_manager.h"
#include "OTA.h"
#include "App_model.h"
#include "ble_stream_class.h"
#include "UtilTask.h"
#include "services/SQuery.h"

__weak void ble_set_custom_advertise_data(uint8_t *manuf_data);
__weak void set_board_name(void);
__weak void enable_extended_configuration_command(void);

#if (BLUE_CORE != BLUE_WB)
__weak void reset_ble_manager(void);
#endif /* (BLUE_CORE != BLUE_WB) */

/* USER CODE BEGIN PV */
/* Private Types ------------------------------------------------------------*/

typedef struct _BLE_MLCStream_t
{
  uint8_t mlc_id;
  uint8_t mlc_out[9];
  bool mlc_data_ready;
} BLE_MLCStream_t;

BLE_MLCStream_t ble_mlc_stream;

/* Exported Variables --------------------------------------------------------*/
volatile uint8_t paired = FALSE;
uint32_t SizeOfUpdateBlueFW = 0;
volatile uint32_t NeedToClearSecureDB = 0;
extern AppModel_t app_model;

/* Private variables ---------------------------------------------------------*/
static uint32_t NeedToRebootBoard = 0;
static uint32_t NeedToSwapBanks = 0;
ble_notify_event_t mlc_char_enabled = BLE_NOTIFY_UNSUB;

/* Imported Variables --------------------------------------------------------*/
uint8_t CurrentActiveBank = 0;

/* USER CODE END PV */

/* Private functions ---------------------------------------------------------*/
static uint32_t debug_console_command_parsing(uint8_t *att_data, uint8_t data_length);
static void BLE_SetCustomStreamID(void);
static sys_error_code_t BLE_PostCustomData(uint8_t sId, uint8_t *buf, uint32_t size);
static void BLE_SendCustomData(uint8_t sId);
static void BLE_SendCommand(char *buf, uint32_t size);

/** @brief Initialize the BlueNRG stack and services
  * @param  None
  * @retval None
  */
void bluetooth_init(void)
{
  /* BlueNRG stack setting */
  ble_stack_value.config_value_offsets                    = CONFIG_DATA_PUBADDR_OFFSET;
  ble_stack_value.config_value_length                     = CONFIG_DATA_PUBADDR_LEN;
  ble_stack_value.gap_roles                               = GAP_PERIPHERAL_ROLE;
  ble_stack_value.io_capabilities                         = IO_CAP_DISPLAY_ONLY;
  ble_stack_value.authentication_requirements             = BONDING;
  ble_stack_value.mitm_protection_requirements            = MITM_PROTECTION_REQUIRED;
  ble_stack_value.secure_connection_support_option_code   = SC_IS_SUPPORTED;
  ble_stack_value.secure_connection_keypress_notification = KEYPRESS_IS_NOT_SUPPORTED;

  /* Use BLE Random Address */
  ble_stack_value.own_address_type = RANDOM_ADDR;

  /* Set the BLE Board Name */
  set_board_name();

  /* En_High_Power Enable High Power mode.
     High power mode should be enabled only to reach the maximum output power.
     Values:
     - 0x00: Normal Power
     - 0x01: High Power */
  ble_stack_value.enable_high_power_mode = ENABLE_HIGH_POWER_MODE;

  /* Values: 0x00 ... 0x31 - The value depends on the device */
  ble_stack_value.power_amplifier_output_level = POWER_AMPLIFIER_OUTPUT_LEVEL;

  /* BlueNRG services setting */
  ble_stack_value.enable_config    = ENABLE_CONFIG;
  ble_stack_value.enable_console   = ENABLE_CONSOLE;
  ble_stack_value.enable_ext_config = ENABLE_EXT_CONFIG;

  /* For Enabling the Secure Connection */
  ble_stack_value.enable_secure_connection = ENABLE_SECURE_CONNECTION;
  /* Default Secure PIN */
  ble_stack_value.secure_pin = SECURE_PIN;
  /* For creating a Random Secure PIN */

  ble_stack_value.enable_random_secure_pin = ENABLE_RANDOM_SECURE_PIN;

  /* Advertising policy for filtering (white list related) */
  ble_stack_value.advertising_filter = ADVERTISING_FILTER;
  ble_stack_value.adv_interval_min = ADV_INTERV_MIN;
  ble_stack_value.adv_interval_max = ADV_INTERV_MAX;

  /* Used platform */
  ble_stack_value.board_id = PnPLGetBOARDID();

  if (ble_stack_value.enable_secure_connection)
  {
    /* Using the Secure Connection, the Rescan should be done by BLE chip */
    ble_stack_value.force_rescan = 0;
  }
  else
  {
    ble_stack_value.force_rescan = 1;
  }

  init_ble_manager();

  /* Check if we are running from Bank1 or Bank2 */
  FLASH_OBProgramInitTypeDef    OBInit;
  /* Allow Access to Flash control registers and user Flash */
  HAL_FLASH_Unlock();
  /* Allow Access to option bytes sector */
  HAL_FLASH_OB_Unlock();
  /* Get the Dual boot configuration status */
  HAL_FLASHEx_OBGetConfig(&OBInit);
  if (((OBInit.USERConfig) & (OB_SWAP_BANK_ENABLE)) == OB_SWAP_BANK_ENABLE)
  {
    CurrentActiveBank = 2;
  }
  else
  {
    CurrentActiveBank = 1;
  }
  HAL_FLASH_OB_Lock();
  HAL_FLASH_Lock();

  //Update the Current Fw ID saved in flash if it's neceessary
  UpdateCurrFlashBankFwIdBoardName(BLE_GetFWID(), NULL);

  char mac_string[18];
  sprintf(mac_string, "%02x:%02x:%02x:%02x:%02x:%02x",
          ble_stack_value.ble_mac_address[5],
          ble_stack_value.ble_mac_address[4],
          ble_stack_value.ble_mac_address[3],
          ble_stack_value.ble_mac_address[2],
          ble_stack_value.ble_mac_address[1],
          ble_stack_value.ble_mac_address[0]);
  set_mac_address(mac_string);
}

uint8_t BLE_GetFWID(void)
{
  if (ble_stack_value.board_id == BOARD_ID_BOXA)
  {
    return BLE_FW_ID_DATALOG2_BOXA;
  }
  else
  {
    return BLE_FW_ID_DATALOG2_BOXB;
  }
}

/**
  * @brief  Set Board Name.
  * @param  None
  * @retval None
  */
__weak void set_board_name(void)
{
  uint8_t *board_name = ReadFlashBoardName();
  if (board_name != NULL)
  {
    /* If there is Saved Board Name */
    memcpy(ble_stack_value.board_name, board_name, 8);
  }
  else
  {
    sprintf(ble_stack_value.board_name, "%s", BLE_FW_PACKAGENAME);
  }
}

/**
  * @brief  Enable/Disable Extended Configuration Command.
  * @param  None
  * @retval None
  */
__weak void enable_extended_configuration_command(void)
{
  /* Enable/Disable Board Report Extended configuration commands */
  ble_extended_configuration_value.stm32_uid = ENABLE_STM32_UID_EXT_CONFIG;
  ble_extended_configuration_value.info = ENABLE_INFO_EXT_CONFIG;
  ble_extended_configuration_value.help = ENABLE_HELP_EXT_CONFIG;
  ble_extended_configuration_value.version_fw = ENABLE_VERSION_FW_EXT_CONFIG;
  ble_extended_configuration_value.power_status = ENABLE_POWER_STATUS_EXT_CONFIG;

  /* Enable/Disable Board Security Extended configuration commands */
  ble_extended_configuration_value.change_secure_pin = ENABLE_CHANGE_SECURE_PIN_EXT_CONFIG;
  ble_extended_configuration_value.clear_secure_data = ENABLE_CLEAR_SECURE_DATA_BASE_EXT_CONFIG;
  ble_extended_configuration_value.read_certificate = ENABLE_READ_CERTIFICATE_EXT_CONFIG;
  ble_extended_configuration_value.set_certificate = ENABLE_SET_CERTIFICATE_EXT_CONFIG;

  /* Enable/Disable Board Control Extended configuration commands */
  ble_extended_configuration_value.reboot_on_dfu_mode_command = ENABLE_REBOOT_ON_DFU_MODE_EXT_CONFIG;
  ble_extended_configuration_value.power_off = ENABLE_POWER_OFF_EXT_CONFIG;
  ble_extended_configuration_value.read_banks = ENABLE_READ_BANKS_FW_ID_EXT_CONFIG;
  ble_extended_configuration_value.banks_swap = ENABLE_BANKS_SWAP_EXT_CONFIG;

  /* Enable/Disable Board Settings Extended configuration commands */
  ble_extended_configuration_value.set_name = ENABLE_SET_NAME_EXT_CONFIG;
  ble_extended_configuration_value.read_custom_command = ENABLE_READ_CUSTOM_COMMAND_EXT_CONFIG;
  ble_extended_configuration_value.set_date = ENABLE_SET_DATE_EXT_CONFIG;
  ble_extended_configuration_value.set_time = ENABLE_SET_TIME_EXT_CONFIG;
  ble_extended_configuration_value.set_wifi = ENABLE_SET_WIFI_EXT_CONFIG;
}

/**
  * @brief  Custom Service Initialization.
  * @param  None
  * @retval None
  */
void ble_init_custom_service(void)
{
  /* Service initialization PnPLike feature */
  if (ble_manager_add_char(ble_init_pn_p_like_service()) == 0)
  {
    PRINT_DBG("Error adding PnPLike characteristic\r\n");
  }

  /* Service initialization for Machine Learning Core feature */
  if (ble_manager_add_char(ble_init_machine_learning_core_service(BLE_MLC_8_REG)) == 0)
  {
    PRINT_DBG("Error adding Machine Learning Core characteristic\r\n");
  }

  /* Service initialization for High Speed Datalog Core feature */
  if (ble_manager_add_char(ble_init_high_speed_data_log_service()) == 0)
  {
    PRINT_DBG("Error adding HSD characteristic\r\n");
  }

  /* Init custom ble stream callback */
  ble_stream_SetCustomStreamIDCallback = &BLE_SetCustomStreamID;
  ble_stream_PostCustomDataCallback = &BLE_PostCustomData;
  ble_stream_SendCustomDataCallback = &BLE_SendCustomData;
  ble_stream_SendCommandCallback = &BLE_SendCommand;

  ble_mlc_stream.mlc_data_ready = false;
}

#if (BLUE_CORE != BLUE_WB)
/**
  * @brief  reset_ble_manager
  * @param  None
  * @retval None
  */
__weak void reset_ble_manager(void)
{
  PRINT_DBG("\r\nReset BleManager (It is a week function)\r\n\r\n");
}
#endif /* (BLUE_CORE != BLUE_WB) */


/**
  * @brief  This function makes the parsing of the Debug Console
  * @param  uint8_t *att_data attribute data
  * @param  uint8_t data_length length of the data
  * @retval uint32_t SendBackData true/false
  */
uint32_t debug_console_parsing(uint8_t *att_data, uint8_t data_length)
{
  /* By default Answer with the same message received */
  uint32_t send_back_data = 1;

  if (SizeOfUpdateBlueFW != 0)
  {
    /* Firmware update */
    int8_t ret_value = UpdateFWBlueMS(&SizeOfUpdateBlueFW, att_data, data_length, 1);
    if (ret_value != 0)
    {
      term_update(((uint8_t *)&ret_value), 1);
      if (ret_value == 1)
      {
        /* if OTA checked */
        PRINT_DBG("%s will restart after the disconnection\r\n", BLE_FW_PACKAGENAME);
        HAL_Delay(1000);
        NeedToSwapBanks = 1;

        /* Swap the Flash Banks */
        SwitchBank();
      }
    }
    send_back_data = 0;
  }
  else
  {
    /* Received one write from Client on Terminal characteristic */
    send_back_data = debug_console_command_parsing(att_data, data_length);
  }
  return send_back_data;
}

/**
  * @brief  This function is called when the peer device get disconnected.
  * @param  None
  * @retval None
  */
void disconnection_completed_function(void)
{
  paired = FALSE;
  set_connectable = TRUE;

  /* Reset for any problem during FOTA update */
  SizeOfUpdateBlueFW = 0;

  if (NeedToRebootBoard)
  {
    NeedToRebootBoard = 0;
    HAL_NVIC_SystemReset();
  }

  if (NeedToSwapBanks)
  {
    NeedToSwapBanks = 0;
    SwitchBank();
  }

  HAL_Delay(100);

  streamMsg_t msg;
  msg.messageId = BLE_ISTREAM_MSG_DISCONNECTED;
  ble_stream_msg(&msg);

  set_connectable_ble();
}

/**
  * @brief  This function is called when there is a LE Connection Complete event.
  * @param  None
  * @retval None
  */
void connection_completed_function(uint16_t ConnectionHandle, uint8_t Address_Type, uint8_t Addr[6])
{

  aci_l2cap_connection_parameter_update_req(ConnectionHandle,
                                            6 /* interval_min*/,
                                            6 /* interval_max */,
                                            0   /* slave_latency */,
                                            400 /*timeout_multiplier*/);

  /* Check if the device is already bonded */
  if (aci_gap_is_device_bonded(Address_Type, Addr) == BLE_STATUS_SUCCESS)
  {
    PRINT_DBG("Device already bounded\r\n");
    paired = TRUE;
  }

  streamMsg_t msg;
  msg.messageId = BLE_ISTREAM_MSG_CONNECTED;
  msg.streamID = 0;
  ble_stream_msg(&msg);
}

/**
  * @brief  This function is called when there is a Pairing Complete event.
  * @param  uint8_t PairingStatus
  * @retval None
  */
void pairing_completed_function(uint8_t PairingStatus)
{
  /* Pairing Status:
   * 0x00 -> "Success",
   * 0x01 -> "Timeout",
   * 0x02 -> "Pairing Failed",
   * 0x03 -> "Encryption failed, LTK missing on local device",
   * 0x04 -> "Encryption failed, LTK missing on peer device",
   * 0x05 -> "Encryption not supported by remote device"
   */
  if (PairingStatus == 0x00 /* Success */)
  {
    paired = TRUE;
  }
  else
  {
    paired = FALSE;
  }
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
void notify_event_machine_learning_core(ble_notify_event_t event)
{
  mlc_char_enabled = event;
}

/**
  * @brief  Callback Called after a MTU Exchange Event
  * @param  int32_t MaxCharLength
  * @retval none
  */
void mtu_exchange_resp_event_function(uint16_t MaxCharLength)
{
  if (MaxCharLength < ble_pn_p_like_get_max_char_length())
  {
    ble_pn_p_like_set_max_char_length(MaxCharLength);
    PRINT_DBG("BLE_PnPLikeSetMaxCharLength ->%d\r\n", MaxCharLength);
  }
}

/***********************************************************************************
  * Callback functions to manage the extended configuration characteristic commands *
  ***********************************************************************************/

/**
  * @brief  Callback Function for answering to the UID command
  * @param  uint8_t **UID STM32 UID Return value
  * @retval None
  */
void ext_ext_config_uid_command_callback(uint8_t **UID)
{
  *UID = (uint8_t *) BLE_STM32_UUID;
}

/**
  * @brief  Callback Function for answering to ClearDB Command
  * @param  None
  * @retval None
  */
void ext_config_clear_db_command_callback(void)
{
  NeedToClearSecureDB = 1;
}


/**
  * @brief  Callback Function for answering to Info command
  * @param  uint8_t *Answer Return String
  * @retval None
  */
void ext_config_info_command_callback(uint8_t *Answer)
{
  sprintf((char *)Answer, "\r\nSTMicroelectronics %s:\n"
          "\tVersion %s.%s.%s\n"
          "\tSTM32U585AI-STWIN.box board"
          "\n\t(HAL %ld.%ld.%ld_%ld)\n"
          "\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
          " (IAR)\n"
#elif defined (__CC_ARM)
          " (KEIL)\n"
#elif defined (__GNUC__)
          " (STM32CubeIDE)\n"
#endif
          "\tCurrent Bank =%ld\n",
          BLE_FW_PACKAGENAME,
          FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH,
          (long)(HAL_GetHalVersion() >> 24),
          (long)((HAL_GetHalVersion() >> 16) & 0xFF),
          (long)((HAL_GetHalVersion() >> 8) & 0xFF),
          (long)(HAL_GetHalVersion()      & 0xFF),
          __DATE__, __TIME__,
          (long)CurrentActiveBank);
}

/**
  * @brief  Callback Function for answering to SetName command
  * @param  uint8_t *NewName New Name
  * @retval None
  */
void ext_config_set_name_command_callback(uint8_t *NewName)
{
  PRINT_DBG("Received a new Board's Name=%s\r\n", NewName);
  /* Update the Board's name in flash */
  UpdateCurrFlashBankFwIdBoardName(BLE_GetFWID(), NewName);

  /* Update the Name for BLE Advertise */
  sprintf(ble_stack_value.board_name, "%s", NewName);
}

/**
  * @brief  Callback Function for answering to Help command
  * @param  uint8_t *Answer Return String
  * @retval None
  */
void ext_config_help_command_callback(uint8_t *Answer)
{
  sprintf((char *) Answer, "Help Message.....");
}

/**
  * @brief  Callback Function for answering to VersionFw command
  * @param  uint8_t *Answer Return String
  * @retval None
  */
void ext_config_version_fw_command_callback(uint8_t *Answer)
{
  sprintf((char *)Answer, "%s_%s_%s.%s.%s\r\n",
          "U585",
          BLE_FW_PACKAGENAME,
          FW_VERSION_MAJOR,
          FW_VERSION_MINOR,
          FW_VERSION_PATCH);
}


/**
  * @brief  Callback Function for answering to ReadBanksFwId command
  * @param  uint8_t *CurBank Number Current Bank
  * @param  uint16_t *FwId1 Bank1 Firmware Id
  * @param  uint16_t *FwId2 Bank2 Firmware Id
  * @retval None
  */
void ext_config_read_banks_fw_id_command_callback(uint8_t *CurBank, uint16_t *FwId1, uint16_t *FwId2)
{
  ReadFlashBanksFwId(FwId1, FwId2);
  *CurBank = CurrentActiveBank;
}


/**
  * @brief  Callback Function for answering to BanksSwap command
  * @param  None
  * @retval None
  */
void ext_config_banks_swap_command_callback(void)
{
  uint16_t FwId1, FwId2;

  ReadFlashBanksFwId(&FwId1, &FwId2);
  if (FwId2 != OTA_OTA_FW_ID_NOT_VALID)
  {
    PRINT_DBG("Swapping to Bank%d\n", (CurrentActiveBank == 1) ? 0 : 1);
    PRINT_DBG("%s will restart after the disconnection\r\n", BLE_FW_PACKAGENAME);
    HAL_Delay(1000);
    NeedToSwapBanks = 1;
  }
  else
  {
    PRINT_DBG("Not Valid fw on Bank%d\n\tCommand Rejected\n", (CurrentActiveBank == 1) ? 0 : 1);
    PRINT_DBG("\tLoad a Firmware on Bank%d\n", (CurrentActiveBank == 1) ? 0 : 1);
  }
}


static uint32_t debug_console_command_parsing(uint8_t *att_data, uint8_t data_length)
{
  /* By default answer with the same message received */
  uint32_t send_back_data = 1;

  /* Help Command */
  if (!strncmp("help", (char *)(att_data), 4))
  {
    send_back_data = 0;

    bytes_to_write = sprintf((char *)buffer_to_write,
                             "info\n"
                             "clearDB\n");
    term_update(buffer_to_write, bytes_to_write);
  }
  else if (!strncmp("versionFw", (char *)(att_data), 9))
  {
    send_back_data = 0;

    bytes_to_write = sprintf((char *)buffer_to_write, "%s_%s_%s.%s.%s\r\n",
                             "U585",
                             BLE_FW_PACKAGENAME,
                             FW_VERSION_MAJOR,
                             FW_VERSION_MINOR,
                             FW_VERSION_PATCH);
    term_update(buffer_to_write, bytes_to_write);
  }
  else if (!strncmp("info", (char *)(att_data), 4))
  {
    send_back_data = 0;

    bytes_to_write = sprintf((char *)buffer_to_write, "\r\nSTMicroelectronics %s:\r\n"
                             "\tVersion %c.%c.%c\r\n"
                             "\tSTM32U585AI-STWIN.box board"
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
  else if (!strncmp("upgradeFw", (char *)(att_data), 9))
  {
    send_back_data = 0;

    uint32_t uwCRCValue;
    uint8_t *PointerByte = (uint8_t *) &SizeOfUpdateBlueFW;

    PointerByte[0] = att_data[ 9];
    PointerByte[1] = att_data[10];
    PointerByte[2] = att_data[11];
    PointerByte[3] = att_data[12];

    /* Check the Maximum Possible OTA size */
    if (SizeOfUpdateBlueFW > OTA_MAX_PROG_SIZE)
    {
      BLE_MANAGER_PRINTF("OTA %s SIZE=%ld > %d Max Allowed\r\n", BLE_FW_PACKAGENAME, SizeOfUpdateBlueFW, OTA_MAX_PROG_SIZE);
      /* Answer with a wrong CRC value for signaling the problem to BlueMS application */
      buffer_to_write[0] = att_data[13];
      buffer_to_write[1] = (att_data[14] != 0) ? 0 : 1; /* In order to be sure to have a wrong CRC */
      buffer_to_write[2] = att_data[15];
      buffer_to_write[3] = att_data[16];
      bytes_to_write = 4;
      term_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      PointerByte = (uint8_t *) &uwCRCValue;
      PointerByte[0] = att_data[13];
      PointerByte[1] = att_data[14];
      PointerByte[2] = att_data[15];
      PointerByte[3] = att_data[16];

      BLE_MANAGER_PRINTF("OTA %s SIZE=%ld uwCRCValue=%lx\r\n", BLE_FW_PACKAGENAME, SizeOfUpdateBlueFW, uwCRCValue);

      /* Reset the Flash */
      StartUpdateFWBlueMS(SizeOfUpdateBlueFW, uwCRCValue);

      /* Signal that we are ready sending back the CRV value*/
      buffer_to_write[0] = PointerByte[0];
      buffer_to_write[1] = PointerByte[1];
      buffer_to_write[2] = PointerByte[2];
      buffer_to_write[3] = PointerByte[3];
      bytes_to_write = 4;
      term_update(buffer_to_write, bytes_to_write);
    }
  }
  else if (!strncmp("uid", (char *)(att_data), 3))
  {
    send_back_data = 0;

    /* Write back the STM32 UID */
    uint8_t *uid = (uint8_t *)BLE_STM32_UUID;
    uint32_t MCU_ID = BLE_STM32_MCU_ID[0] & 0xFFFU;
    bytes_to_write = sprintf((char *)buffer_to_write, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X_%.3lX\r\n",
                             uid[ 3], uid[ 2], uid[ 1], uid[ 0],
                             uid[ 7], uid[ 6], uid[ 5], uid[ 4],
                             uid[11], uid[ 10], uid[9], uid[8],
                             (long)MCU_ID);
    term_update(buffer_to_write, bytes_to_write);
  }
  else if (!strncmp("clearDB", (char *)(att_data), 7))
  {
    send_back_data = 0;

    bytes_to_write = sprintf((char *)buffer_to_write, "\nThe Secure database will be cleared\n");
    term_update(buffer_to_write, bytes_to_write);
    NeedToClearSecureDB = 1;
  }
  return send_back_data;
}

/**
  * @brief Assign ID to each stream
  * @param  None
  * @retval None
  */
static void BLE_SetCustomStreamID(void)
{
  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  ble_mlc_stream.mlc_id = SQNextByNameAndType(&querySM, "ism330dhcx", COM_TYPE_MLC);
}

/**
  * @brief Save data in the relevant buffer
  * @param  uint8_t id_stream indicates the correct stream to be filled
  * @param  uint8_t *buf data to be saved
  * @param  uint32_t size number of byte to be saved
  * @retval None
  */
static sys_error_code_t BLE_PostCustomData(uint8_t sId, uint8_t *buf, uint32_t size)
{
  sys_error_code_t res = SYS_NOT_IMPLEMENTED_ERROR_CODE;
  if ((sId == ble_mlc_stream.mlc_id) && (mlc_char_enabled == BLE_NOTIFY_SUB))
  {
    memcpy(ble_mlc_stream.mlc_out, buf, size);
    ble_mlc_stream.mlc_data_ready = true;
    res = SYS_NO_ERROR_CODE;
  }
  return res;
}

/**
  * @brief Send data on the relevant characteristics if possible
  * @param  uint8_t id_stream indicates the correct stream to be sent
  * @retval None
  */
static void BLE_SendCustomData(uint8_t sId)
{
  if (ble_mlc_stream.mlc_data_ready && (mlc_char_enabled == BLE_NOTIFY_SUB))
  {
    ble_machine_learning_core_update((uint8_t *) &ble_mlc_stream.mlc_out, (uint8_t *) &ble_mlc_stream.mlc_out[8]);
    ble_mlc_stream.mlc_data_ready = false;
  }
}

static void BLE_SendCommand(char *buf, uint32_t size)
{
  if (buf != NULL)
  {
    tBleStatus ret;
    uint32_t j = 0, chunk, tot_len;
    uint8_t *buffer_out;
    uint32_t length_wTP;

    int32_t MaxPnPLikeUpdate = ble_pn_p_like_get_max_char_length();
    int32_t MaxPnPLikeUpdateMinus1 = MaxPnPLikeUpdate - 1;

    if ((size % MaxPnPLikeUpdateMinus1) == 0U)
    {
      length_wTP = (size / MaxPnPLikeUpdateMinus1) + size;
    }
    else
    {
      length_wTP = (size / MaxPnPLikeUpdateMinus1) + 1U + size;
    }

    buffer_out = BLE_MALLOC_FUNCTION(sizeof(uint8_t) * length_wTP);

    if (buffer_out == NULL)
    {
      BLE_MANAGER_PRINTF("Error: Mem calloc error [%ld]: %d@%s\r\n", length_wTP, __LINE__, __FILE__);
      // TODO: manage error
      return;
    }
    else
    {
      tot_len = ble_command_tp_encapsulate(buffer_out, (uint8_t *) buf, size, MaxPnPLikeUpdate);

      j = 0;

      /* Data are sent as notifications*/
      while (j < tot_len)
      {

        chunk = MIN(MaxPnPLikeUpdate, tot_len - j);

        ret = ble_pn_p_like_update(&buffer_out[j], chunk);

        if (ret == BLE_STATUS_INSUFFICIENT_RESOURCES)
        {
          ble_sendMSG_wait();
        }
        else if (ret == BLE_STATUS_SUCCESS)
        {
          j += chunk;
        }
      }
    }

    BLE_FREE_FUNCTION(buffer_out);
  }
}

