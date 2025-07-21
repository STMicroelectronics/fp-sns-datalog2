/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ble_implementation_template.c
  * @author  System Research & Applications Team - Catania Lab.
  * @brief   BLE Implementation header template file.
  *          This file should be copied to the application folder and renamed
  *          to ble_implementation.c.
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

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "ble_manager.h"
#include "main.h"

__weak void ble_set_custom_advertise_data(uint8_t *manuf_data);
__weak void set_board_name(void);
__weak void enable_extended_configuration_commad(void);

#if (BLUE_CORE != BLUE_WB)
__weak void reset_ble_manager(void);
#endif /* (BLUE_CORE != BLUE_WB) */

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private functions ---------------------------------------------------------*/

/** @brief Initialize the BlueNRG stack and services
  * @param  None
  * @retval None
  */
void bluetooth_init(void)
{
  /* BlueNRG stack setting */
  ble_stack_value.config_value_offsets                    = CONFIG_VALUE_OFFSETS;
  ble_stack_value.config_value_length                     = CONFIG_VALUE_LENGTH;
  ble_stack_value.gap_roles                               = GAP_ROLES;
  ble_stack_value.io_capabilities                         = IO_CAPABILITIES;
  ble_stack_value.authentication_requirements             = AUTHENTICATION_REQUIREMENTS;
  ble_stack_value.mitm_protection_requirements            = MITM_PROTECTION_REQUIREMENTS;
#if (BLUE_CORE != BLUENRG_MS)
  ble_stack_value.secure_connection_support_option_code   = SECURE_CONNECTION_SUPPORT_OPTION_CODE;
  ble_stack_value.secure_connection_keypress_notification = SECURE_CONNECTION_KEYPRESS_NOTIFICATION;
#else /* (BLUE_CORE != BLUENRG_MS)  */
  ble_stack_value.out_of_band_enable_data             = OUT_OF_BAND_ENABLEDATA;
  ble_stack_value.oob_data                            = NULL;
#endif /* (BLUE_CORE != BLUENRG_MS)  */

  /* Use BLE Random Address */
  ble_stack_value.own_address_type = ADDRESS_TYPE;

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
#ifndef BLE_MANAGER_NO_PARSON
  ble_stack_value.enable_ext_config = ENABLE_EXT_CONFIG;
#endif /* BLE_MANAGER_NO_PARSON */

  /* For Enabling the Secure Connection */
  ble_stack_value.enable_secure_connection = ENABLE_SECURE_CONNECTION;
  /* Default Secure PIN */
  ble_stack_value.secure_pin = SECURE_PIN;
  /* For creating a Random Secure PIN */
  ble_stack_value.enable_random_secure_pin = ENABLE_RANDOM_SECURE_PIN;

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
  /* Advertising policy for filtering (white list related) */
  ble_stack_value.advertising_filter = ADVERTISING_FILTER;
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))  */

  /* Used platform */
  ble_stack_value.board_id = BLE_MANAGER_USED_PLATFORM;

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
}

#if ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06))
/**
  * @brief  Enable BlueNRG-LP Interrupt.
  * @param  None
  * @retval None
  */
void init_ble_int_for_blue_nrglp(void)
{
  HAL_EXTI_GetHandle(&H_EXTI, EXTI_LINE);
  HAL_EXTI_RegisterCallback(&H_EXTI, HAL_EXTI_COMMON_CB_ID, hci_tl_lowlevel_isr);
  HAL_NVIC_SetPriority(HCI_TL_SPI_EXTI_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(HCI_TL_SPI_EXTI_IRQn);
}
#endif /* ((BLUE_CORE == BLUENRG_LP) || (BLUE_CORE == STM32WB07_06)) */

/**
  * @brief  Set Board Name.
  * @param  None
  * @retval None
  */
__weak void set_board_name(void)
{
  sprintf(ble_stack_value.board_name, "%s%c%c%c", "BLEM",
          BLE_VERSION_FW_MAJOR,
          BLE_VERSION_FW_MINOR,
          BLE_VERSION_FW_PATCH);
}

/**
  * @brief  Enable/Disable Extended Configuration Command.
  * @param  None
  * @retval None
  */
__weak void enable_extended_configuration_commad(void)
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
  /**
    * User can added here the custom service initialization for the selected BLE features.
    * For example for the environmental features:
  */
  /* Characteristc allocation for audio scene classification features */
  ble_manager_add_char(ble_init_audio_scene_class_service(BLE_ASC_ALG_SCENE_CLASS));
  /* Characteristc allocation for High Speed Data Log features */
  ble_manager_add_char(ble_init_high_speed_data_log_service());
  /* Characteristc allocation for the accelerometer events features */
  ble_manager_add_char(ble_init_acc_env_service());
  /* Characteristc allocation for the audio level features */
  ble_manager_add_char(ble_init_audio_level_service(AUDIO_CHANNELS_NUMBER));
  /* Characteristc allocation for the battery features */
  ble_manager_add_char(ble_init_battery_service());
  /* Characteristc allocation for environmental Bluetooth features */
  ble_manager_add_char(ble_init_env_service(ENABLE_ENV_PRESSURE_DATA,
                                            ENABLE_ENV_HUMIDITY_DATA,
                                            ENABLE_ENV_TEMPERATURE_DATA));
  /* Characteristic allocation for finite state machine features */
  ble_manager_add_char(ble_init_finite_state_machine_service(BLE_FSM_16_REG));
  /* Characteristc allocation for gas concentration features */
  ble_manager_add_char(ble_init_gas_concentration_service());
  /* Characteristc allocation for the Gnss features */
  ble_manager_add_char(ble_init_gnss_service());
  /* Characteristc allocation for inertial features */
  ble_manager_add_char(ble_init_inertial_service(ENABLE_ACC_DATA,
                                                 ENABLE_GYRO_DATA,
                                                 ENABLE_MAG_DATA));
  /* Characteristc allocation for LED Bluetooth feature */
  ble_manager_add_char(ble_init_led_service());
  /* Characteristc allocation for machine learning core features */
  ble_manager_add_char(ble_init_machine_learning_core_service(BLE_MLC_8_REG));
  /* Characteristc allocation for objects detection features */
  ble_manager_add_char(ble_init_objects_detection_service());
  /* Characteristc allocation for piano features */
  ble_manager_add_char(ble_init_piano_service());
  /* Characteristc allocation for Activity Recognition features */
  ble_manager_add_char(ble_init_act_rec_service());
  /* Characteristc allocation for the audio source localization features */
  ble_manager_add_char(ble_init_audio_source_localization_service());
  /* Characteristc allocation for Carry Position features */
  ble_manager_add_char(ble_init_carry_position_service());
  /* Characteristc allocation for the e-compass features */
  ble_manager_add_char(ble_init_e_compass_service());
  /* Characteristc allocation for Fitness Activities features */
  ble_manager_add_char(ble_init_fitness_activities_service());
  /* Characteristc allocation for Gesture Recognition features */
  ble_manager_add_char(ble_init_gesture_recognition_service());
  /* Characteristc allocation for Motion Algorithms features */
  ble_manager_add_char(ble_init_motion_algorithms_service());
  /* Characteristc allocation for Motion Intensity features */
  ble_manager_add_char(ble_init_motion_intensity_service());
  /* Characteristc allocation for Pedometer Algorithm features */
  ble_manager_add_char(ble_init_pedometer_algorithm_service());
  /* Characteristc allocation for sensor fusion Bluetooth features */
  ble_manager_add_char(ble_init_sensor_fusion_service(NUMBER_OF_QUATERNION));
  /* Characteristc allocation for Tilt Sensing features */
  ble_manager_add_char(ble_init_tilt_sensing_service());
  /* Characteristc allocation for the FFT Alarm Subrange Status features */
  ble_manager_add_char(ble_init_fft_alarm_subrange_status_service());
  /* Characteristc allocation for the FFT Amplitude features */
  ble_manager_add_char(ble_init_fft_amplitude_service());
  /* Characteristc allocation for the Time Domain features */
  ble_manager_add_char(ble_init_time_domain_service());
  /* Characteristc allocation for the Time Domain Alarm Acc Peak Status features */
  ble_manager_add_char(ble_init_td_alarm_acc_peak_status_service());;
  /* Characteristc allocation for the Time Domain Alarm Speed RMS Status features */
  ble_manager_add_char(ble_init_td_alarm_speed_rms_status_service());
  /* Characteristc allocation for the General Purpose features */
  ble_manager_add_char(ble_init_general_purpose_service(0x00, GENERAL_PURPOSE_SIZE_1 /* Size */));
  ble_manager_add_char(ble_init_general_purpose_service(0x01, GENERAL_PURPOSE_SIZE_2 /* Size */));
  ble_manager_add_char(ble_init_general_purpose_service(0x02, GENERAL_PURPOSE_SIZE_3 /* Size */));

  /* Characteristc allocation for OTA features */
  ble_manager_add_char(ble_init_ota_service());
  /* Characteristc allocation for SD Data Log features */
  ble_manager_add_char(ble_init_sd_log_service());
  /* Characteristc allocation for PnP Like features */
  ble_manager_add_char(ble_init_pn_p_like_service());
  /* Characteristc allocation for the event counter features */
  ble_manager_add_char(ble_init_event_counter_service());
  /* Characteristc allocation for Json features */
  ble_manager_add_char(ble_init_json_service());
  /* Characteristc allocation for Neai Anomaly Detection features */
  ble_manager_add_char(ble_init_ad_service());
  /* Characteristc allocation for Neai N Class Classification features */
  ble_manager_add_char(ble_init_neai_classification_service());
  /* Characteristc allocation for the normalization features */
  ble_manager_add_char(ble_init_normalization_service());
  /* Characteristc allocation for the Gesture Navigation features */
  ble_manager_add_char(ble_init_gesture_navigation_service());
  /* Characteristc allocation for Binary Content features */
  ble_manager_add_char(ble_init_binary_content_service());
  /* Characteristc allocation for the Qvar features */
  ble_manager_add_char(ble_init_qvar_service());
}

/**
  * @brief  Set Custom Advertize Data.
  * @param  uint8_t *manuf_data: Advertize Data
  * @retval None
  */
__weak void ble_set_custom_advertise_data(uint8_t *manuf_data)
{
#ifndef BLE_MANAGER_SDKV2
  /**
    * For only SDKV1, user can add here the custom advertize data setting for the selected BLE features.
    * For example for the environmental features:
    *
    * ble_set_custom_advertise_data(manuf_data);
    */

#else /* BLE_MANAGER_SDKV2 */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD1] = 0xFF; /* Custom Firmware */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD2] = 0x00;
  manuf_data[BLE_MANAGER_CUSTOM_FIELD3] = 0x00;
  manuf_data[BLE_MANAGER_CUSTOM_FIELD4] = 0x00;
#endif /* BLE_MANAGER_SDKV2 */
}

#if (BLUE_CORE != BLUE_WB)
/**
  * @brief  reset_ble_manager
  * @param  None
  * @retval None
  */
__weak void reset_ble_manager(void)
{
  BLE_MANAGER_PRINTF("\r\nReset BleManager (It is a week function)\r\n\r\n");
}
#endif /* (BLUE_CORE != BLUE_WB) */

