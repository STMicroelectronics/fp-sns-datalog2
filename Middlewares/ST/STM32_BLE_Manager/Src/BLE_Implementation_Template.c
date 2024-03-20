/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    BLE_Implementation_Template.c
  * @author  System Research & Applications Team - Catania Lab.
  * @brief   BLE Implementation header template file.
  *          This file should be copied to the application folder and renamed
  *          to BLE_Implementation.c.
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

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "BLE_Manager.h"
#include "main.h"

__weak void BLE_SetCustomAdvertiseData(uint8_t *manuf_data);
__weak void DisconnectionCompletedFunction(void);
#ifdef BLUENRG_1_2
__weak void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t Address_Type, uint8_t addr[6]);
#endif /* BLUENRG_1_2 */
#ifdef BLUENRG_MS
__weak void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t addr[6]);
#endif /* BLUENRG_MS */
#ifdef BLUENRG_LP
__weak void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t Address_Type, uint8_t addr[6]);
#endif /* BLUENRG_LP */

__weak void SetBoardName(void);

__weak void AttrModConfigFunction(uint8_t *att_data, uint8_t data_length);
__weak void PairingCompletedFunction(uint8_t PairingStatus);
__weak void SetConnectableFunction(uint8_t *ManufData);
__weak void AciGattTxPoolAvailableEventFunction(void);
__weak void HardwareErrorEventHandlerFunction(uint8_t Hardware_Code);

__weak uint32_t DebugConsoleParsing(uint8_t *att_data, uint8_t data_length);
__weak void WriteRequestConfigFunction(uint8_t *att_data, uint8_t data_length);

#ifdef BLE_MANAGER_USE_PARSON
/***********************************************************************************************
  * Callback functions prototypes to manage the extended configuration characteristic commands *
  **********************************************************************************************/
__weak void ExtExtConfigUidCommandCallback(uint8_t **UID);
__weak void ExtConfigVersionFwCommandCallback(uint8_t *Answer);
__weak void ExtConfigInfoCommandCallback(uint8_t *Answer);
__weak void ExtConfigHelpCommandCallback(uint8_t *Answer);
__weak void ExtConfigPowerStatusCommandCallback(uint8_t *Answer);

__weak void ExtConfigChangePinCommandCallback(uint32_t NewPin);
__weak void ExtConfigClearDBCommandCallback(void);
__weak void ExtConfigReadCertCommandCallback(uint8_t *Certificate);
__weak void ExtConfigSetCertCommandCallback(uint8_t *Certificate);

__weak void ExtConfigRebootOnDFUModeCommandCallback(void);
__weak void ExtConfigPowerOffCommandCallback(void);
__weak void ExtConfigReadBanksFwIdCommandCallback(uint8_t *CurBank, uint16_t *FwId1, uint16_t *FwId2);
__weak void ExtConfigBanksSwapCommandCallback(void);

__weak void ExtConfigSetNameCommandCallback(uint8_t *NewName);
__weak void ExtConfigReadCustomCommandsCallback(JSON_Array *JSON_SensorArray);
__weak void ExtConfigCustomCommandCallback(BLE_CustomCommadResult_t *CustomCommand);
__weak void ExtConfigSetDateCommandCallback(uint8_t *NewDate);
__weak void ExtConfigSetTimeCommandCallback(uint8_t *NewTime);
__weak void ExtConfigSetWiFiCommandCallback(BLE_WiFi_CredAcc_t NewWiFiCred);
__weak void ExtConfigReadSensorConfigCommandCallback(JSON_Array *JSON_SensorArray);
__weak void ExtConfigSetSensorConfigCommandCallback(uint8_t *configuration);
#endif /* BLE_MANAGER_USE_PARSON */

/**************************************************************
  * Callback functions prototypes to manage the notify events *
  *************************************************************/

__weak void NotifyEventAudioSceneClassification(BLE_NotifyEvent_t Event);

__weak void NotifyEventHighSpeedDataLog(BLE_NotifyEvent_t Event);

__weak void NotifyEventAccEvent(BLE_NotifyEvent_t Event);

__weak void NotifyEventAudioLevel(BLE_NotifyEvent_t Event);

__weak void NotifyEventBattery(BLE_NotifyEvent_t Event);

__weak void NotifyEventEnv(BLE_NotifyEvent_t Event);
__weak void ReadRequestEnvFunction(int32_t *Press, uint16_t *Hum, int16_t *Temp1, int16_t *Temp2);

__weak void NotifyEventFiniteStateMachine(BLE_NotifyEvent_t Event);

__weak void NotifyEventGasConcentration(BLE_NotifyEvent_t Event);

__weak void NotifyEventGnss(BLE_NotifyEvent_t Event);

__weak void NotifyEventInertial(BLE_NotifyEvent_t Event);

__weak void NotifyEventLed(BLE_NotifyEvent_t Event);

__weak void NotifyEventMachineLearningCore(BLE_NotifyEvent_t Event);

__weak void NotifyEventObjectsDetection(BLE_NotifyEvent_t Event);

__weak void NotifyEventPiano(BLE_NotifyEvent_t Event);

__weak void NotifyEventActRec(BLE_NotifyEvent_t Event);

__weak void NotifyEventAudioSourceLocalization(BLE_NotifyEvent_t Event);

__weak void NotifyEventCarryPosition(BLE_NotifyEvent_t Event);

__weak void NotifyEventECompass(BLE_NotifyEvent_t Event);

__weak void NotifyEventFitnessActivities(BLE_NotifyEvent_t Event);
__weak void WriteRequestFitnessActivities(uint8_t FitnessActivitie);

__weak void NotifyEventGestureRecognition(BLE_NotifyEvent_t Event);

__weak void NotifyEventMotionAlgorithms(BLE_NotifyEvent_t Event, BLE_MotionAlgorithmsType_t Algorithm);
__weak void WriteRequestMotionAlgorithms(BLE_MotionAlgorithmsType_t Algorithm);

__weak void NotifyEventMotionIntensity(BLE_NotifyEvent_t Event);

__weak void NotifyEventPedometerAlgorithm(BLE_NotifyEvent_t Event);

__weak void NotifyEventSensorFusion(BLE_NotifyEvent_t Event);

__weak void NotifyEventTiltSensing(BLE_NotifyEvent_t Event);

__weak void NotifyEventFFT_AlarmSubrangeStatus(BLE_NotifyEvent_t Event);

__weak void NotifyEventFFT_Amplitude(BLE_NotifyEvent_t Event);

__weak void NotifyEventTimeDomain(BLE_NotifyEvent_t Event);

__weak void NotifyEventTimeDomainAlarmAccPeakStatus(BLE_NotifyEvent_t Event);

__weak void NotifyEventTimeDomainAlarmSpeed_RMS_Status(BLE_NotifyEvent_t Event);

__weak void NotifyEventGeneralPurpose(uint8_t GP_CharNum, BLE_NotifyEvent_t Event);

__weak void NotifyEventSDLog(BLE_NotifyEvent_t Event);
__weak void WriteRequestSDLog(uint8_t *att_data, uint8_t data_length);

#ifdef BLE_MANAGER_USE_PARSON
__weak void NotifyEventPnpLike(BLE_NotifyEvent_t Event);
__weak void WriteRequestPnPLike(uint8_t *received_msg, uint8_t msg_length);
#endif /* BLE_MANAGER_USE_PARSON */

__weak void NotifyEventEventCounter(BLE_NotifyEvent_t Event);

#ifdef BLE_MANAGER_USE_PARSON
__weak void NotifyEventJson(BLE_NotifyEvent_t Event);
__weak void WriteRequestJson(uint8_t *received_msg, uint8_t msg_length);
#endif /* BLE_MANAGER_USE_PARSON */

__weak void NotifyEventNeaiAnomalyDetection(BLE_NotifyEvent_t Event);
__weak void WriteRequestNeaiAnomalyDetection(uint8_t *att_data, uint8_t data_length);

__weak void NotifyEventNeaiNClassClassification(BLE_NotifyEvent_t Event);
__weak void WriteRequestNeaiNClassClassification(uint8_t *att_data, uint8_t data_length);

__weak void NotifyEventNormalization(BLE_NotifyEvent_t Event);

__weak void NotifyEventGestureNavigation(BLE_NotifyEvent_t Event);

__weak void NotifyEventBinaryContent(BLE_NotifyEvent_t Event);
__weak void WriteRequestBinaryContent(uint8_t *received_msg, uint32_t msg_length);

__weak void NotifyEventQVAR(BLE_NotifyEvent_t Event);

/* Private variables ------------------------------------------------------------*/
static uint16_t CustomCommandPageLevel = 0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private functions ---------------------------------------------------------*/

/** @brief Initialize the BlueNRG stack and services
  * @param  None
  * @retval None
  */
void BluetoothInit(void)
{
  /* BlueNRG stack setting */
  BLE_StackValue.ConfigValueOffsets                   = CONFIG_VALUE_OFFSETS;
  BLE_StackValue.ConfigValuelength                    = CONFIG_VALUE_LENGTH;
  BLE_StackValue.GAP_Roles                            = GAP_ROLES;
  BLE_StackValue.IO_capabilities                      = IO_CAPABILITIES;
  BLE_StackValue.AuthenticationRequirements           = BONDING;
  BLE_StackValue.MITM_ProtectionRequirements          = AUTHENTICATION_REQUIREMENTS;
#ifndef BLUENRG_MS
  BLE_StackValue.SecureConnectionSupportOptionCode    = SECURE_CONNECTION_SUPPORT_OPTION_CODE;
  BLE_StackValue.SecureConnectionKeypressNotification = SECURE_CONNECTION_KEYPRESS_NOTIFICATION;
#else /* BLUENRG_MS */
  BLE_StackValue.Out_Of_Band_EnableData               = OOB_AUTH_DATA_ABSENT;
  BLE_StackValue.oob_data                             = NULL;
#endif /* BLUENRG_MS */

  /* Use BLE Random Address */
  BLE_StackValue.OwnAddressType = ADDRESS_TYPE;

  /* Set the BLE Board Name */
  SetBoardName();

  /* En_High_Power Enable High Power mode.
     High power mode should be enabled only to reach the maximum output power.
     Values:
     - 0x00: Normal Power
     - 0x01: High Power */
  BLE_StackValue.EnableHighPowerMode = ENABLE_HIGH_POWER_MODE;

  /* Values: 0x00 ... 0x31 - The value depends on the device */
  BLE_StackValue.PowerAmplifierOutputLevel = POWER_AMPLIFIER_OUTPUT_LEVEL;

  /* BlueNRG services setting */
  BLE_StackValue.EnableConfig    = ENABLE_CONFIG;
  BLE_StackValue.EnableConsole   = ENABLE_CONSOLE;
#ifdef BLE_MANAGER_USE_PARSON
  BLE_StackValue.EnableExtConfig = ENABLE_EXT_CONFIG;
#endif /* BLE_MANAGER_USE_PARSON */

  /* For Enabling the Secure Connection */
  BLE_StackValue.EnableSecureConnection = ENABLE_SECURE_CONNECTION;
  /* Default Secure PIN */
  BLE_StackValue.SecurePIN = SECURE_PIN;
  /* For creating a Random Secure PIN */
  BLE_StackValue.EnableRandomSecurePIN = ENABLE_RANDOM_SECURE_PIN;

  /* Advertising policy for filtering (white list related) */
  BLE_StackValue.AdvertisingFilter = ADVERTISING_FILTER;

  /* Used platform */
  BLE_StackValue.BoardId = BLE_MANAGER_USED_PLATFORM;

  if (BLE_StackValue.EnableSecureConnection)
  {
    /* Using the Secure Connection, the Rescan should be done by BLE chip */
    BLE_StackValue.ForceRescan = 0;
  }
  else
  {
    BLE_StackValue.ForceRescan = 1;
  }

  InitBleManager();
}

/**
  * @brief  Set Board Name.
  * @param  None
  * @retval None
  */
__weak void SetBoardName(void)
{
  sprintf(BLE_StackValue.BoardName, "%s%c%c%c", "BLEM",
          BLE_VERSION_FW_MAJOR,
          BLE_VERSION_FW_MINOR,
          BLE_VERSION_FW_PATCH);
}

/**
  * @brief  Custom Service Initialization.
  * @param  None
  * @retval None
  */
void BLE_InitCustomService(void)
{

  /* Define Custom Function for Connection Completed */
  CustomConnectionCompleted = ConnectionCompletedFunction;

  /* Define Custom Function for Disconnection Completed */
  CustomDisconnectionCompleted = DisconnectionCompletedFunction;

  /* Define Custom Function for Attribute Modify Config */
  CustomAttrModConfigCallback = AttrModConfigFunction;

  /* Define Custom Function for Pairing Completed */
  CustomPairingCompleted = PairingCompletedFunction;

  /* Define Custom Function for Set Connectable */
  CustomSetConnectable = SetConnectableFunction;

  /* Define Custom Function for Aci Gatt Tx Pool Available Event */
  CustomAciGattTxPoolAvailableEvent = AciGattTxPoolAvailableEventFunction;

  /* Define Custom Function for Hardware Error Event Handler */
  CustomHardwareErrorEventHandler = HardwareErrorEventHandlerFunction;

  /* Define Custom Function for Debug Console Command parsing */
  CustomDebugConsoleParsingCallback = DebugConsoleParsing;

  /* Define Custom Command for Parsing Write on Config Char */
  CustomWriteRequestConfigCallback = WriteRequestConfigFunction;

  /***************************************************************************************
    * Callback functions to manage the notify events and write request for each features *
    **************************************************************************************/

  CustomNotifyEventAudioSceneClass =             NotifyEventAudioSceneClassification;

  CustomNotifyEventHighSpeedDataLog =            NotifyEventHighSpeedDataLog;

  CustomNotifyEventAccEvent =                    NotifyEventAccEvent;

  CustomNotifyEventAudioLevel =                  NotifyEventAudioLevel;

  CustomNotifyEventBattery =                     NotifyEventBattery;

  CustomNotifyEventEnv =                         NotifyEventEnv;

  CustomNotifyEventFiniteStateMachine =          NotifyEventFiniteStateMachine;

  CustomNotifyEventGasConcentration =            NotifyEventGasConcentration;

  CustomNotifyEventGnss =                        NotifyEventGnss;

  CustomNotifyEventInertial =                    NotifyEventInertial;

  CustomNotifyEventLed =                         NotifyEventLed;

  CustomNotifyEventMachineLearningCore =         NotifyEventMachineLearningCore;

  CustomNotifyEventObjectsDetection =            NotifyEventObjectsDetection;

  CustomNotifyEventPiano =                       NotifyEventPiano;

  CustomNotifyEventActRec =                      NotifyEventActRec;

  CustomNotifyEventAudioSourceLocalization =     NotifyEventAudioSourceLocalization;

  CustomNotifyEventCarryPosition =               NotifyEventCarryPosition;

  CustomNotifyECompass =                         NotifyEventECompass;

  CustomNotifyEventFitnessActivities =           NotifyEventFitnessActivities;
  /* Define Custom Function for Write Request Fitness Activities */
  CustomWriteRequestFitnessActivities =          WriteRequestFitnessActivities;

  CustomNotifyEventGestureRecognition =          NotifyEventGestureRecognition;

  CustomNotifyEventMotionAlgorithms =            NotifyEventMotionAlgorithms;
  /* Define Custom Function for Write Request Motion Algorithms */
  CustomWriteRequestMotionAlgorithms = WriteRequestMotionAlgorithms;

  CustomNotifyEventMotionIntensity =             NotifyEventMotionIntensity;

  CustomNotifyEventPedometerAlgorithm =          NotifyEventPedometerAlgorithm;

  CustomNotifyEventSensorFusion =                NotifyEventSensorFusion;

  CustomNotifyEventTiltSensing =                 NotifyEventTiltSensing;

  CustomNotifyEventFFT_AlarmSubrangeStatus =     NotifyEventFFT_AlarmSubrangeStatus;

  CustomNotifyEventFFT_Amplitude =               NotifyEventFFT_Amplitude;

  CustomNotifyEventTimeDomain =                  NotifyEventTimeDomain;

  CustomNotifyEventTD_AlarmAccPeakStatus =       NotifyEventTimeDomainAlarmAccPeakStatus;

  CustomNotifyEventTD_AlarmSpeed_RMS_Status =    NotifyEventTimeDomainAlarmSpeed_RMS_Status;

  CustomNotifyEventGeneralPurpose =              NotifyEventGeneralPurpose;

  CustomNotifyEventSDLog =                       NotifyEventSDLog;
  CustomWriteSDLog =                             WriteRequestSDLog;

#ifdef BLE_MANAGER_USE_PARSON
  CustomNotifyEventPnPLike =                     NotifyEventPnpLike;
  CustomWriteRequestPnPLike =                    WriteRequestPnPLike;
#endif /* BLE_MANAGER_USE_PARSON */

  CustomNotifyEventCounter =                     NotifyEventEventCounter;

#ifdef BLE_MANAGER_USE_PARSON
  CustomNotifyEventJson =                        NotifyEventJson;
  CustomWriteRequestJson =                       WriteRequestJson;
#endif /* BLE_MANAGER_USE_PARSON */

  CustomNotifyEventAD =                          NotifyEventNeaiAnomalyDetection;
  CustomWriteRequestAD =                         WriteRequestNeaiAnomalyDetection;

  CustomNotifyEventNCC =                         NotifyEventNeaiNClassClassification;
  CustomWriteRequestNCC =                        WriteRequestNeaiNClassClassification;

  CustomNotifyEventNormalization =               NotifyEventNormalization;

  CustomNotifyEventGestureNavigation =           NotifyEventGestureNavigation;

  CustomNotifyEventBinaryContent =               NotifyEventBinaryContent;
  /* Define Custom Function for Write Request Binary Content */
  CustomWriteRequestBinaryContent =              WriteRequestBinaryContent;

  CustomNotifyEventQVAR =                        NotifyEventQVAR;

  /************************************************************************************
    * Callback functions to manage the extended configuration characteristic commands *
    ***********************************************************************************/
#ifdef BLE_MANAGER_USE_PARSON
  CustomExtConfigUidCommandCallback  = ExtExtConfigUidCommandCallback;
  CustomExtConfigVersionFwCommandCallback = ExtConfigVersionFwCommandCallback;
  CustomExtConfigInfoCommandCallback = ExtConfigInfoCommandCallback;
  CustomExtConfigHelpCommandCallback = ExtConfigHelpCommandCallback;
  CustomExtConfigPowerStatusCommandCallback = ExtConfigPowerStatusCommandCallback;

  CustomExtConfigChangePinCommandCallback = ExtConfigChangePinCommandCallback;
  CustomExtConfigClearDBCommandCallback = ExtConfigClearDBCommandCallback;
  CustomExtConfigReadCertCommandCallback = ExtConfigReadCertCommandCallback;
  CustomExtConfigSetCertCommandCallback = ExtConfigSetCertCommandCallback;

  CustomExtConfigRebootOnDFUModeCommandCallback = ExtConfigRebootOnDFUModeCommandCallback;
  CustomExtConfigPowerOffCommandCallback = ExtConfigPowerOffCommandCallback;
  CustomExtConfigReadBanksFwIdCommandCallback       = ExtConfigReadBanksFwIdCommandCallback;
  CustomExtConfigBanksSwapCommandCallback = ExtConfigBanksSwapCommandCallback;

  CustomExtConfigSetNameCommandCallback = ExtConfigSetNameCommandCallback;
  CustomExtConfigReadCustomCommandsCallback = ExtConfigReadCustomCommandsCallback;
  CustomExtConfigCustomCommandCallback = ExtConfigCustomCommandCallback;
  CustomExtConfigSetDateCommandCallback = ExtConfigSetDateCommandCallback;
  CustomExtConfigSetTimeCommandCallback = ExtConfigSetTimeCommandCallback;
  CustomExtConfigSetWiFiCommandCallback = ExtConfigSetWiFiCommandCallback;
  CustomExtConfigReadSensorsConfigCommandsCallback = ExtConfigReadSensorConfigCommandCallback;
  CustomExtConfigSetSensorsConfigCommandsCallback =  ExtConfigSetSensorConfigCommandCallback;
#endif /* BLE_MANAGER_USE_PARSON */

  /**
    * For each features, user can assign here the pointer at the function for the read request data.
    * For example for the environmental features:
    *
    * CustomReadRequestEnv = ReadRequestEnvFunction;
    *
    * User can define and insert in the BLE_Implementation.c source code the functions for the read request data
    * ReadRequestEnvFunction function is already defined.
    *
  */

  /* Define Custom Function for Read Request Environmental Data */
  CustomReadRequestEnv = ReadRequestEnvFunction;

  /**
    * User can added here the custom service initialization for the selected BLE features.
    * For example for the environmental features:
    *
    * BLE_InitEnvService(PressEnable,HumEnable,NumTempEnabled)
    * BleManagerAddChar(BleCharPointer= BLE_InitEnvService(1, 1, 1));
  */

  /* Characteristc allocation for audio scene classification features */
  BleManagerAddChar(BLE_InitAudioSceneClassService(BLE_ASC_ALG_SCENE_CLASS));

  /* Characteristc allocation for high speed dataLog features */
  BleManagerAddChar(BLE_InitHighSpeedDataLogService());

  /* Characteristc allocation for accelerometer events features */
  BleManagerAddChar(BLE_InitAccEnvService());

  /* Characteristc allocation for audio level features */
  BleManagerAddChar(BLE_InitAudioLevelService(AUDIO_CHANNELS_NUMBER));

  /* Characteristc allocation for battery features */
  BleManagerAddChar(BLE_InitBatteryService());

  /* Characteristc allocation for environmental features */
  /* BLE_InitEnvService(PressEnable,HumEnable,NumTempEnabled) */
  BleManagerAddChar(BLE_InitEnvService(ENABLE_ENV_PRESSURE_DATA,
                                       ENABLE_ENV_HUMIDITY_DATA,
                                       ENABLE_ENV_TEMPERATURE_DATA));

  /* Characteristic allocation for finite state machine features */
  BleManagerAddChar(BLE_InitFiniteStateMachineService(BLE_FSM_16_REG));

  /* Characteristc allocation for gas concentration features */
  BleManagerAddChar(BLE_InitGasConcentrationService());

  /* Characteristc allocation for Gnss features */
  BleManagerAddChar(BLE_InitGnssService());

  /* Characteristc allocation for inertial features */
  /* BLE_InitInertialService(AccEnable, GyroEnable, MagEnabled) */
  BleManagerAddChar(BLE_InitInertialService(ENABLE_ACC_DATA, ENABLE_GYRO_DATA, ENABLE_MAG_DATA));

  /* Characteristc allocation for the led features */
  BleManagerAddChar(BLE_InitLedService());

  /* Characteristc allocation for machine learning core features */
  BleManagerAddChar(BLE_InitMachineLearningCoreService(BLE_MLC_8_REG));

  /* Characteristc allocation for objects detection features */
  BleManagerAddChar(BLE_InitObjectsDetectionService());

  /* Characteristc allocation for piano features */
  BleManagerAddChar(BLE_InitPianoService());

  /* Characteristc allocation for activity recognition features */
  BleManagerAddChar(BLE_InitActRecService());

  /* Characteristc allocation for audio source localization */
  BleManagerAddChar(BLE_InitAudioSourceLocalizationService());

  /* Characteristc allocation for carry position features */
  BleManagerAddChar(BLE_InitCarryPositionService());

  /* Characteristc allocation for E-Compass features */
  BleManagerAddChar(BLE_InitECompassService());

  /* Characteristc allocation for fitness activities features */
  BleManagerAddChar(BLE_InitFitnessActivitiesService());

  /* Characteristc allocation for gesture recognition features */
  BleManagerAddChar(BLE_InitGestureRecognitionService());

  /* Characteristc allocation for motion algorithms features */
  BleManagerAddChar(BLE_InitMotionAlgorithmsService());

  /* Characteristc allocation for motion intensity features */
  BleManagerAddChar(BLE_InitMotionIntensityService());

  /* Characteristc allocation for pedometer algorithm features */
  BleManagerAddChar(BLE_InitPedometerAlgorithmService());

  /* Characteristc allocation for sensor fusion features */
  BleManagerAddChar(BLE_InitSensorFusionService(NUMBER_OF_QUATERNION));

  /* Characteristc allocation for tilt sensing features */
  BleManagerAddChar(BLE_InitTiltSensingService());

  /* Characteristc allocation for FFT alarm subrange status features */
  BleManagerAddChar(BLE_InitFFT_AlarmSubrangeStatusService());

  /* Characteristc allocation for FFT amplitude features */
  BleManagerAddChar(BLE_InitFFTAmplitudeService());

  /* Characteristc allocation for the time domain features */
  BleManagerAddChar(BLE_InitTimeDomainService());

  /* Characteristc allocation for time domain alarm acc peak status features */
  BleManagerAddChar(BLE_InitTD_AlarmAccPeakStatusService());

  /* Characteristc allocation for time domain alarm speed RMS status features */
  BleManagerAddChar(BLE_InitTD_AlarmSpeed_RMS_StatusService());

  /* Characteristc allocation for general purpose features */
  BleManagerAddChar(BLE_InitGeneralPurposeService(GENERAL_PURPOSE_SIZE_1 /* Size */));

  /* Characteristc allocation for SD Logging features */
  BleManagerAddChar(BLE_InitSDLogService());

  /* Characteristc allocation for SD Logging features */
  BleManagerAddChar(BLE_InitPnPLikeService());

  /* Characteristc allocation for time domain alarm speed RMS status features */
  BleManagerAddChar(BLE_InitEventCounterService());

  /* Characteristc allocation for time domain alarm speed RMS status features */
  BleManagerAddChar(BLE_InitJsonService());

  /* Characteristc allocation for time domain alarm speed RMS status features */
  BleManagerAddChar(BLE_InitADService());

  /* Characteristc allocation for time domain alarm speed RMS status features */
  BleManagerAddChar(BLE_InitNeaiClassificationService());

  /* Characteristc allocation for normalization features */
  BleManagerAddChar(BLE_InitNormalizationService());

  /* Characteristc allocation for gesture navigation features */
  BleManagerAddChar(BLE_InitGestureNavigationService());

  /* Characteristc allocation for binary content features */
  BleManagerAddChar(BLE_InitBinaryContentService());

  /* Characteristc allocation for QVAR features */
  BleManagerAddChar(BLE_InitQVARService());

}

/**
  * @brief  Set Custom Advertize Data.
  * @param  uint8_t *manuf_data: Advertize Data
  * @retval None
  */
__weak void BLE_SetCustomAdvertiseData(uint8_t *manuf_data)
{
#ifndef BLE_MANAGER_SDKV2
  /**
    * For only SDKV1, user can add here the custom advertize data setting for the selected BLE features.
    * For example for the environmental features:
    *
    * BLE_SetCustomEnvAdvertizeData(manuf_data);
    */

#else /* BLE_MANAGER_SDKV2 */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD1] = 0xFF; /* Custom Firmware */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD2] = 0x00;
  manuf_data[BLE_MANAGER_CUSTOM_FIELD3] = 0x00;
  manuf_data[BLE_MANAGER_CUSTOM_FIELD4] = 0x00;
#endif /* BLE_MANAGER_SDKV2 */
}

/**
  * @brief  This function makes the parsing of the Debug Console
  * @param  uint8_t *att_data attribute data
  * @param  uint8_t data_length length of the data
  * @retval uint32_t SendBackData true/false
  */
__weak uint32_t DebugConsoleParsing(uint8_t *att_data, uint8_t data_length)
{
  /* By default Answer with the same message received */
  uint32_t SendBackData = 1;

  /* Help Command */
  if (!strncmp("help", (char *)(att_data), 4))
  {
    /* Print Legend */
    SendBackData = 0;

    BytesToWrite = sprintf((char *)BufferToWrite, "Command:\r\n"
                           "info-> System Info\r\n"
                           "uid-> STM32 UID value\r\n");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else if (!strncmp("info", (char *)(att_data), 4))
  {
    SendBackData = 0;

    BytesToWrite = sprintf((char *)BufferToWrite, "\r\nSTMicroelectronics %s:\r\n"
                           "\tVersion %c.%c.%c\r\n"
                           "\tSTM32L4xx MCU Family Name"
                           "\r\n",
                           BLE_FW_PACKAGENAME,
                           BLE_VERSION_FW_MAJOR, BLE_VERSION_FW_MINOR, BLE_VERSION_FW_PATCH);

    Term_Update(BufferToWrite, BytesToWrite);

    BytesToWrite = sprintf((char *)BufferToWrite, "\t(HAL %ld.%ld.%ld_%ld)\r\n"
                           "\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
                           " (IAR)\r\n",
#elif defined (__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* For ARM Compiler 5 and 6 */
                           " (KEIL)\r\n",
#elif defined (__GNUC__)
                           " (STM32CubeIDE)\r\n",
#endif /* IDE cases */
                           HAL_GetHalVersion() >> 24,
                           (HAL_GetHalVersion() >> 16) & 0xFF,
                           (HAL_GetHalVersion() >> 8) & 0xFF,
                           HAL_GetHalVersion()      & 0xFF,
                           __DATE__, __TIME__);

    Term_Update(BufferToWrite, BytesToWrite);
  }
  else if ((att_data[0] == 'u') & (att_data[1] == 'i') & (att_data[2] == 'd'))
  {
    /* Write back the STM32 UID */
    uint8_t *uid = (uint8_t *)BLE_STM32_UUID;
    uint32_t MCU_ID = BLE_STM32_MCU_ID[0] & 0xFFF;
    BytesToWrite = sprintf((char *)BufferToWrite, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X_%.3lX\r\n",
                           uid[ 3], uid[ 2], uid[ 1], uid[ 0],
                           uid[ 7], uid[ 6], uid[ 5], uid[ 4],
                           uid[11], uid[ 10], uid[9], uid[8],
                           MCU_ID);
    Term_Update(BufferToWrite, BytesToWrite);
    SendBackData = 0;
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the DebugConsoleParsing could be implemented in the user file
   */

  return SendBackData;
}

/**
  * @brief  Callback Function for Environmental read request.
  * @param  int32_t *Press Pressure Value
  * @param  uint16_t *Hum Humidity Value
  * @param  int16_t *Temp1 Temperature Number 1
  * @param  int16_t *Temp2 Temperature Number 2
  * @retval None
  */
__weak void ReadRequestEnvFunction(int32_t *Press, uint16_t *Hum, int16_t *Temp1, int16_t *Temp2)
{
  /* NOTE: Insert here the function to read the environmental data */
}

/**
  * @brief  This function is called when the peer device get disconnected.
  * @param  None
  * @retval None
  */
__weak void DisconnectionCompletedFunction(void)
{
  BLE_MANAGER_PRINTF("Call to DisconnectionCompletedFunction\r\n");
  BLE_MANAGER_DELAY(100);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the DisconnectionCompletedFunction could be implemented in the user file
   */
}

#ifdef BLUENRG_1_2
/**
  * @brief  This function is called when there is a LE Connection Complete event.
  * @param  uint16_t ConnectionHandle
  * @param  uint8_t Address_Type
  * @param  uint8_t addr[6]
  * @retval None
  */
__weak void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t Address_Type, uint8_t addr[6])
#endif /* BLUENRG_1_2 */
#ifdef BLUENRG_MS
/**
  * @brief  This function is called when there is a LE Connection Complete event.
  * @param  uint16_t ConnectionHandle
  * @param  uint8_t addr[6]
  * @retval None
  */
__weak void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t addr[6])
#endif /* BLUENRG_MS */
#ifdef BLUENRG_LP
/**
  * @brief  This function is called when there is a LE Connection Complete event.
  * @param  uint16_t ConnectionHandle
  * @param  uint8_t Address_Type
  * @param  uint8_t addr[6]
  * @retval None
  */
__weak void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t Address_Type, uint8_t addr[6])
#endif /* BLUENRG_LP */
{
  /* Prevent unused argument(s) compilation warning */
#ifndef BLUENRG_MS
  UNUSED(Address_Type);
#endif /* BLUENRG_MS */
  UNUSED(ConnectionHandle);
  UNUSED(addr);

  CustomCommandPageLevel = 0;
  /* NOTE: This function Should not be modified, when the callback is needed,
           the ConnectionCompletedFunction could be implemented in the user file
   */

  BLE_MANAGER_PRINTF("Call to ConnectionCompletedFunction\r\n");
  BLE_MANAGER_DELAY(100);
}

/**
  * @brief  This function is called when there is a change on the gatt attribute.
  * @param  None
  * @retval None
  */
__weak void AttrModConfigFunction(uint8_t *att_data, uint8_t data_length)
{
  BLE_MANAGER_PRINTF("Call to AttrModConfigFunction\r\n");
  BLE_MANAGER_DELAY(100);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the AttrModConfigFunction could be implemented in the user file
   */
}

/**
  * @brief  This function is called when the pairing process has completed successfully
  *         or a pairing procedure timeout has occurred or the pairing has failed.
  * @param  uint8_t PairingStatus
  * @retval None
  */
__weak void PairingCompletedFunction(uint8_t PairingStatus)
{
  BLE_MANAGER_PRINTF("Call to PairingCompletedFunction\r\n");
  BLE_MANAGER_DELAY(100);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the PairingCompletedFunction could be implemented in the user file
   */
}

/**
  * @brief  This function is called when the device is put in connectable mode.
  * @param  uint8_t *ManufData Filling Manufacter Advertise data
  * @retval None
  */
__weak void SetConnectableFunction(uint8_t *ManufData)
{
  BLE_MANAGER_PRINTF("Call to SetConnectableFunction\r\n");
  BLE_MANAGER_DELAY(100);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the SetConnectableFunction could be implemented in the user file
   */
}

/**
  * @brief  This function is called when bluetooth congestion buffer occurs
  *         or a pairing procedure timeout has occurred or the pairing has failed.
  * @param  None
  * @retval None
  */
__weak void AciGattTxPoolAvailableEventFunction(void)
{
  BLE_MANAGER_PRINTF("Call to AciGattTxPoolAvailableEventFunction\r\n");
  BLE_MANAGER_DELAY(100);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the AciGattTxPoolAvailableEventFunction could be implemented in the user file
   */
}

/**
  * @brief  This event is used to notify the Host that a hardware failure has occurred in the Controller.
  * @param  uint8_t Hardware_Code Hardware Error Event code.
  * @retval None
  */
__weak void HardwareErrorEventHandlerFunction(uint8_t Hardware_Code)
{
  BLE_MANAGER_PRINTF("Call to HardwareErrorEventHandlerFunction\r\n");
  BLE_MANAGER_DELAY(100);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the HardwareErrorEventHandlerFunction could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Config write request.
  * @param uint8_t *att_data attribute data
  * @param uint8_t data_length length of the data
  * @retval None
  */
__weak void WriteRequestConfigFunction(uint8_t *att_data, uint8_t data_length)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(att_data);
  UNUSED(data_length);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the WriteRequestConfigFunction could be implemented in the user file
   */
}

/***************************************************
  * Callback functions to manage the notify events *
  **************************************************/

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventAudioSceneClassification(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventAudioSceneClassification could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventHighSpeedDataLog(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventHighSpeedDataLog could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventAccEvent(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventAccEvent could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventAudioLevel(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventAudioLevel could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventBattery(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventBattery could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventEnv(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventEnv could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventFiniteStateMachine(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventFiniteStateMachine could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventGasConcentration(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventGasConcentration could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventGnss(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventGnss could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventInertial(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventInertial could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventLed(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventLed could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventMachineLearningCore(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventMachineLearningCore could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventObjectsDetection(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventObjectsDetection could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventPiano(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventPiano could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventActRec(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventActRec could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventAudioSourceLocalization(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventAudioSourceLocalization could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventCarryPosition(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventCarryPosition could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventECompass(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventECompass could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventFitnessActivities(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventFitnessActivities could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Fitness Activities write request.
  * @param  uint8_t FitnessActivitie
  * @retval None
  */
__weak void WriteRequestFitnessActivities(uint8_t FitnessActivitie)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(FitnessActivitie);
  /* NOTE: Insert here the function to write request for Fitness Activities */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventGestureRecognition(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventGestureRecognition could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @param  BLE_MotionAlgorithmsType_t Algorithm
  * @retval None
  */
__weak void NotifyEventMotionAlgorithms(BLE_NotifyEvent_t Event, BLE_MotionAlgorithmsType_t Algorithm)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  UNUSED(Algorithm);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventMotionAlgorithms could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Motion Algorithms write request.
  * @param  BLE_MotionAlgorithmsType_t Algorithm
  * @retval None
  */
__weak void WriteRequestMotionAlgorithms(BLE_MotionAlgorithmsType_t Algorithm)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Algorithm);
  /* NOTE: Insert here the function to read the environmental data */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventMotionIntensity(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventMotionIntensity could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventPedometerAlgorithm(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventPedometerAlgorithm could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventSensorFusion(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventSensorFusion could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventTiltSensing(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventTiltSensing could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventFFT_AlarmSubrangeStatus(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventFFT_AlarmSubrangeStatus could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventFFT_Amplitude(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventFFT_Amplitude could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventTimeDomain(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventTimeDomain could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventTimeDomainAlarmAccPeakStatus(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventTimeDomainAlarmAccPeakStatus could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventTimeDomainAlarmSpeed_RMS_Status(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventTimeDomainAlarmSpeed_RMS_Status could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventGeneralPurpose(uint8_t GP_CharNum, BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventGeneralPurpose could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventSDLog(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventSDLog could be implemented in the user file
   */
}

/**
  * @brief  This event is given when a write request is received by the server from the client.
  * @param  uint8_t data_length
  * @param  uint8_t *att_data
  * @retval None
  */
__weak void WriteRequestSDLog(uint8_t *att_data, uint8_t data_length)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(data_length);
  UNUSED(att_data);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the WriteRequestSDLog could be implemented in the user file
   */
}

#ifdef BLE_MANAGER_USE_PARSON
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventPnpLike(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventPnpLike could be implemented in the user file
   */
}

/**
  * @brief  This event is given when a write request is received by the server from the client.
  * @param  uint8_t *received_msg
  * @param  uint8_t msg_length
  * @retval None
  */
__weak void WriteRequestPnPLike(uint8_t *received_msg, uint8_t msg_length)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(received_msg);
  UNUSED(msg_length);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the WriteRequestPnPLike could be implemented in the user file
   */
}
#endif /* BLE_MANAGER_USE_PARSON */

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventEventCounter(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventEventCounter could be implemented in the user file
   */
}

#ifdef BLE_MANAGER_USE_PARSON
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventJson(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventJson could be implemented in the user file
   */
}

/**
  * @brief  This event is given when a write request is received by the server from the client.
  * @param  uint8_t *received_msg
  * @param  uint8_t msg_length
  * @retval None
  */
__weak void WriteRequestJson(uint8_t *received_msg, uint8_t msg_length)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(received_msg);
  UNUSED(msg_length);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the WriteRequestJson could be implemented in the user file
   */
}
#endif /* BLE_MANAGER_USE_PARSON */

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventNeaiAnomalyDetection(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventNeaiAnomalyDetection could be implemented in the user file
   */
}

/**
  * @brief  This event is given when a write request is received by the server from the client.
  * @param  uint8_t *att_data
  * @param  uint8_t data_length
  * @retval None
  */
__weak void WriteRequestNeaiAnomalyDetection(uint8_t *att_data, uint8_t data_length)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(att_data);
  UNUSED(data_length);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the WriteRequestNeaiAnomalyDetection could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventNeaiNClassClassification(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventNeaiNClassClassification could be implemented in the user file
   */
}

/**
  * @brief  This event is given when a write request is received by the server from the client.
  * @param  uint8_t *att_data
  * @param  uint8_t data_length
  * @retval None
  */
__weak void WriteRequestNeaiNClassClassification(uint8_t *att_data, uint8_t data_length)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(att_data);
  UNUSED(data_length);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the WriteRequestNeaiNClassClassification could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventNormalization(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventNormalization could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventGestureNavigation(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventGestureNavigation could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventBinaryContent(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventBinaryContent could be implemented in the user file
   */
}

/**
  * @brief  Callback Function for binary content write request.
  * @param  uint8_t *received_msg
  * @param  uint8_t msg_length
  * @retval None
  */
__weak void WriteRequestBinaryContent(uint8_t *received_msg, uint32_t msg_length)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(received_msg);
  UNUSED(msg_length);
  /* NOTE: Insert here the function to write request for Fitness Activities */
}

/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void NotifyEventQVAR(BLE_NotifyEvent_t Event)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Event);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the NotifyEventQVAR could be implemented in the user file
   */
}

/************************************************************************************
  * Callback functions to manage the extended configuration characteristic commands *
  ***********************************************************************************/
#ifdef BLE_MANAGER_USE_PARSON
/**
  * @brief  Callback Function for answering to the UID command
  * @param  uint8_t **UID STM32 UID Return value
  * @retval None
  */
__weak void ExtExtConfigUidCommandCallback(uint8_t **UID)
{
#ifdef BLE_STM32_UUID
  *UID = (uint8_t *)BLE_STM32_UUID;
#endif /* BLE_STM32_UUID */

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtExtConfigUidCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for answering to VersionFw command
  * @param  uint8_t *Answer Return String
  * @retval None
  */
__weak void ExtConfigVersionFwCommandCallback(uint8_t *Answer)
{
  sprintf((char *)Answer, "%s_%s_%c.%c.%c",
          BLE_STM32_MICRO,
          BLE_FW_PACKAGENAME,
          BLE_VERSION_FW_MAJOR,
          BLE_VERSION_FW_MINOR,
          BLE_VERSION_FW_PATCH);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigVersionFwCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for answering to Info command
  * @param  uint8_t *Answer Return String
  * @retval None
  */
__weak void ExtConfigInfoCommandCallback(uint8_t *Answer)
{
  sprintf((char *)Answer, "STMicroelectronics %s:\n"
          "Version %c.%c.%c\n"
          "(HAL %ld.%ld.%ld_%ld)\n"
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
          HAL_GetHalVersion() >> 24,
          (HAL_GetHalVersion() >> 16) & 0xFF,
          (HAL_GetHalVersion() >> 8) & 0xFF,
          HAL_GetHalVersion()      & 0xFF,
          __DATE__, __TIME__);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigInfoCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for answering to Help command
  * @param  uint8_t *Answer Return String
  * @retval None
  */
__weak void ExtConfigHelpCommandCallback(uint8_t *Answer)
{
  sprintf((char *)Answer, "List of available command:\n"
          "1) Board Report\n"
          "- STM32 UID\n"
          "- Version Firmware\n"
          "- Info\n"
          "- Help\n\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigHelpCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for answering to PowerStatus command
  * @param  uint8_t *Answer Return String
  * @retval None
  */
__weak void ExtConfigPowerStatusCommandCallback(uint8_t *Answer)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Answer);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigPowerStatusCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the ChangePin command
  * @param  uint32_t NewPin
  * @retval None
  */
__weak void ExtConfigChangePinCommandCallback(uint32_t NewPin)
{
  BLE_MANAGER_PRINTF("New Board Pin= <%ld>\r\n", NewPin);
  BLE_StackValue.SecurePIN = NewPin;

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
__weak void ExtConfigClearDBCommandCallback(void)
{
  BLE_MANAGER_PRINTF("ExtConfigClearDBCommandCallback\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigClearDBCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the ReadCert command
  * @param  uint8_t *Certificate to register
  * @retval None
  */
__weak void ExtConfigReadCertCommandCallback(uint8_t *Certificate)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Certificate);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigReadCertCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the SetCert command
  * @param  uint8_t *Certificate registered certificate
  * @retval None
  */
__weak void ExtConfigSetCertCommandCallback(uint8_t *Certificate)
{
  BLE_MANAGER_PRINTF("Certificate From Dashboard= <%s>\r\n", Certificate);

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
__weak void ExtConfigRebootOnDFUModeCommandCallback(void)
{
  BLE_MANAGER_PRINTF("RebootOnDFUModeCommandCallback\r\n");

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
__weak void ExtConfigPowerOffCommandCallback(void)
{
  BLE_MANAGER_PRINTF("ExtConfigPowerOffCommandCallback\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigPowerOffCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for answering to ReadBanksFwId command
  * @param  uint8_t *CurBank Number Current Bank
  * @param  uint16_t *FwId1 Bank1 Firmware Id
  * @param  uint16_t *FwId2 Bank2 Firmware Id
  * @retval None
  */
__weak void ExtConfigReadBanksFwIdCommandCallback(uint8_t *CurBank, uint16_t *FwId1, uint16_t *FwId2)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(CurBank);
  UNUSED(FwId1);
  UNUSED(FwId2);

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
__weak void ExtConfigBanksSwapCommandCallback(void)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigReadBanksFwIdCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the SetName command
  * @param  uint8_t *NewName
  * @retval None
  */
__weak void ExtConfigSetNameCommandCallback(uint8_t *NewName)
{
  BLE_MANAGER_PRINTF("New Board Name = <%s>\r\n", NewName);
  /* Change the Board Name */
  sprintf(BLE_StackValue.BoardName, "%s", NewName);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigSetNameCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Custom commands definition
  * @param  JSON_Array *JSON_SensorArray
  * @retval None
  */
__weak void ExtConfigReadCustomCommandsCallback(JSON_Array *JSON_SensorArray)
{
  /* Clear the previous Costom Command List */
  ClearCustomCommandsList();

  if (CustomCommandPageLevel == 0)
  {

    /* Add all the custom Commands */
    if (AddCustomCommand("IntValue1", /* Name */
                         BLE_CUSTOM_COMMAND_INTEGER, /* Type */
                         100, /* Default Value */
                         -100, /* MIN */
                         200,  /* MAX */
                         NULL, /* Enum Int */
                         NULL, /* Enum String */
                         NULL, /* Description */
                         JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "IntValue1");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "IntValue1");
      return;
    }

    if (AddCustomCommand("IntValue2", /* Name */
                         BLE_CUSTOM_COMMAND_INTEGER, /* Type */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                         10, /* MIN */
                         3000,  /* MAX */
                         NULL, /* Enum Int */
                         NULL, /* Enum String */
                         NULL, /* Description */
                         JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "IntValue2");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "IntValue2");
      return;
    }

    if (AddCustomCommand("VoidCommand", /* Name */
                         BLE_CUSTOM_COMMAND_VOID, /* Type */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  /* MAX */
                         NULL, /* Enum Int */
                         NULL, /* Enum String */
                         "Example Void Command", /* Description */
                         JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "Command1");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "Command1");
      return;
    }

    if (AddCustomCommand("StringValue1", /* Name */
                         BLE_CUSTOM_COMMAND_STRING, /* Type */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                         20,  /* MAX */
                         NULL, /* Enum Int */
                         NULL, /* Enum String */
                         NULL, /* Description */
                         JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "StringValue1");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "StringValue1");
      return;
    }

    if (AddCustomCommand("BooleanValue", /* Name */
                         BLE_CUSTOM_COMMAND_BOOLEAN, /* Type */
                         1, /* Default Value */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  /* MAX */
                         NULL, /* Enum Int */
                         NULL, /* Enum String */
                         "Example for Boolean", /* Description */
                         JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "BooleanValue");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "BooleanValue");
      return;
    }

    if (AddCustomCommand("StringValue2", /* Name */
                         BLE_CUSTOM_COMMAND_STRING, /* Type */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                         4, /* MIN */
                         10,  /* MAX */
                         NULL, /* Enum Int */
                         NULL, /* Enum String */
                         "It's possible to add a  very very very very very very long description", /* Description */
                         JSON_SensorArray))
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
      if (AddCustomCommand("StringEnum", /* Name */
                           BLE_CUSTOM_COMMAND_ENUM_STRING, /* Type */
                           1, /* Default Value */
                           BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                           BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  /* MAX */
                           NULL, /* Enum Int */
                           (void *)ValidStringValues, /* Enum String */
                           "Example of Enum String", /* Description */
                           JSON_SensorArray))
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
      int32_t ValidIntValues[] = {-1, 12, 123, 321, BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN};
      if (AddCustomCommand("IntEnum", /* Name */
                           BLE_CUSTOM_COMMAND_ENUM_INTEGER, /* Type */
                           3, /* Default Value */
                           BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                           BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  /* MAX */
                           (void *) ValidIntValues, /* Enum Int */
                           NULL, /* Enum String */
                           "Example of Enum Integer", /* Description */
                           JSON_SensorArray))
      {
        BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "IntEnum");
      }
      else
      {
        BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "IntEnum");
        return;
      }
    }

    if (AddCustomCommand("ChangeCustomCommand", /* Name */
                         BLE_CUSTOM_COMMAND_VOID, /* Type */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  /* MAX */
                         NULL, /* Enum Int */
                         NULL, /* Enum String */
                         "Change the Custom Commands", /* Description */
                         JSON_SensorArray))
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
    if (AddCustomCommand("ReadCert", /* Name */
                         BLE_CUSTOM_COMMAND_STRING, /* Type */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                         4, /* MIN */
                         10,  /* MAX */
                         NULL, /* Enum Int */
                         NULL, /* Enum String */
                         "Invalid Command...", /* Description */
                         JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "ReadCert");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "ReadCert");
      return; /* not mandatory... it's the last one */
    }
  }
  else if (CustomCommandPageLevel == 1)
  {
    if (AddCustomCommand("ComeBackCustomCommand", /* Name */
                         BLE_CUSTOM_COMMAND_VOID, /* Type */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN,  /* MAX */
                         NULL, /* Enum Int */
                         NULL, /* Enum String */
                         "Come back to previous Custom Commands", /* Description */
                         JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "Command1");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "Command1");
      return;
    }

    if (AddCustomCommand("StringValueNewLevel", /* Name */
                         BLE_CUSTOM_COMMAND_STRING, /* Type */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* Default Value */
                         BLE_MANAGER_CUSTOM_COMMAND_VALUE_NAN, /* MIN */
                         20,  /* MAX */
                         NULL, /* Enum Int */
                         NULL, /* Enum String */
                         NULL, /* Description */
                         JSON_SensorArray))
    {
      BLE_MANAGER_PRINTF("Added Command <%s>\r\n", "StringValueNewLevel");
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Adding Command <%s>\r\n", "StringValue1");
      return;
    }
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigReadCustomCommandsCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the custom command
  * @param  BLE_CustomCommadResult_t *CustomCommand:
  * @param                            uint8_t *CommandName: Name of the command
  * @param                            CustomCommand->CommandType: Type of the command
  * @param                            int32_t IntValue:    Integer or boolean parameter
  * @param                            uint8_t *StringValue: String parameter
  * @retval None
  */
__weak void  ExtConfigCustomCommandCallback(BLE_CustomCommadResult_t *CustomCommand)
{
  BLE_MANAGER_PRINTF("Received Custom Command:\r\n");
  BLE_MANAGER_PRINTF("\tCommand Name: <%s>\r\n", CustomCommand->CommandName);
  BLE_MANAGER_PRINTF("\tCommand Type: <%d>\r\n", CustomCommand->CommandType);

  switch (CustomCommand->CommandType)
  {
    case BLE_CUSTOM_COMMAND_VOID:
      if (!strncmp((char *)CustomCommand->CommandName, "BleManagerReset", 15))
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
      BLE_MANAGER_PRINTF("\tInt    Value: <%ld>\r\n", CustomCommand->IntValue);
      break;
    case BLE_CUSTOM_COMMAND_ENUM_INTEGER:
      BLE_MANAGER_PRINTF("\tInt     Enum: <%ld>\r\n", CustomCommand->IntValue);
      break;
    case BLE_CUSTOM_COMMAND_BOOLEAN:
      BLE_MANAGER_PRINTF("\tInt    Value: <%ld>\r\n", CustomCommand->IntValue);
      break;
    case  BLE_CUSTOM_COMMAND_STRING:
      BLE_MANAGER_PRINTF("\tString Value: <%s>\r\n", CustomCommand->StringValue);
      break;
    case  BLE_CUSTOM_COMMAND_ENUM_STRING:
      BLE_MANAGER_PRINTF("\tString  Enum: <%s>\r\n", CustomCommand->StringValue);
      break;
  }

  if (!strncmp((char *)CustomCommand->CommandName, "ChangeCustomCommand", 20))
  {
    CustomCommandPageLevel = 1;
    SendNewCustomCommandList();
  }
  else if (!strncmp((char *)CustomCommand->CommandName, "ComeBackCustomCommand", 21))
  {
    CustomCommandPageLevel = 0;
    SendNewCustomCommandList();
  }
  else if (!strncmp((char *)CustomCommand->CommandName, "IntValue2", 9))
  {
    SendError("Example of Error");
  }
  else if (!strncmp((char *)CustomCommand->CommandName, "IntValue1", 9))
  {
    SendInfo("Example of Info");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigCustomCommandCallback could be implemented in the user file
           for managing the received command
   */
}

/**
  * @brief  Callback Function for managing the SetDate command
  * @param  uint8_t *NewDate
  * @retval None
  */
__weak void ExtConfigSetDateCommandCallback(uint8_t *NewDate)
{
  BLE_MANAGER_PRINTF("New Board Date= <%s>\r\n", NewDate);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigSetDateCommandCallback could be implemented in the user file
           for managing the received command (Insert the code for changing the RTC Date)
   */
}

/**
  * @brief  Callback Function for managing the SetTime command
  * @param  uint8_t *NewTime
  * @retval None
  */
__weak void ExtConfigSetTimeCommandCallback(uint8_t *NewTime)
{
  BLE_MANAGER_PRINTF("New Board Time= <%s>\r\n", NewTime);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigSetTimeCommandCallback could be implemented in the user file
           for managing the received command (Insert the code for changing the RTC Time)
   */
}
/**
  * @brief  Callback Function for managing the SetWiFi command
  * @param  BLE_WiFi_CredAcc_t NewWiFiCred
  * @retval None
  */
__weak void ExtConfigSetWiFiCommandCallback(BLE_WiFi_CredAcc_t NewWiFiCred)
{
  BLE_MANAGER_PRINTF("NewWiFiCred=\r\n");
  BLE_MANAGER_PRINTF("\tSSID    = <%s>\r\n", NewWiFiCred.SSID);
  BLE_MANAGER_PRINTF("\tPassWd = <%s>\r\n", NewWiFiCred.PassWd);
  BLE_MANAGER_PRINTF("\tSecurity= <%s>\r\n", NewWiFiCred.Security);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigSetTimeCommandCallback could be implemented in the user file
           for managing the received command (Insert the code for changing the Wi-Fi Credential)
   */
}

/**
  * @brief  Reads Sensor Config
  * @param  JSON_Array *JSON_SensorArray
  * @retval None
  */
__weak void ExtConfigReadSensorConfigCommandCallback(JSON_Array *JSON_SensorArray)
{
#define WRITE_BUFFER_SIZE_HTS221_H       (uint32_t)(256)
#define WRITE_BUFFER_SIZE_HTS221_T       (uint32_t)(256)
#define WRITE_BUFFER_SIZE_LPS22HH_P       (uint32_t)(1024)
#define WRITE_BUFFER_SIZE_LPS22HH_T       (uint32_t)(1024)
#define WRITE_BUFFER_SIZE_LSM6DSOX_A   (uint32_t)(16384)
#define WRITE_BUFFER_SIZE_LSM6DSOX_G   (uint32_t)(16384)
#define WRITE_BUFFER_SIZE_LSM6DSOX_MLC (uint32_t)(1024)

#define SAMPLES_PER_TIME_STAMP_1 1000
#define SAMPLES_PER_TIME_STAMP_2 200

#define FULL_SCALE_1             120.0f
#define FULL_SCALE_2             100.0f
#define FULL_SCALE_3             125.0f
#define FULL_SCALE_4             250.0f
#define FULL_SCALE_5             500.0f
#define FULL_SCALE_6             1000.0f
#define FULL_SCALE_7             2000.0f

#define ODR_1                    100.0f
#define ODR_2                    200.0f
#define ODR_3                    1260.0f
#define ODR_4                    104.0f
#define ODR_5                    208.0f
#define ODR_6                    417.0f
#define ODR_7                    833.0f
#define ODR_8                    1667.0f
#define ODR_9                    3333.0f
#define ODR_10                   6667.0f

#define USB_DATA_PACKET_SIZE_1   1600
#define USB_DATA_PACKET_SIZE_2   2048

  float temp;

  COM_Sensor_t SensorHTS221, SensorLPS22HH, SensorLSM6DSOX;
  JSON_Value *tempJSON1;
  BLE_MANAGER_PRINTF("Received the Read Sensors Config Command\r\n");

  /* HTS221 SENSOR DESCRIPTOR */
  strcpy(SensorHTS221.sensorDescriptor.name, "HTS221");
  SensorHTS221.sensorDescriptor.nSubSensors = 2;
  SensorHTS221.sensorDescriptor.id = 0;

  /* SUBSENSOR 0 DESCRIPTOR */
  SensorHTS221.sensorDescriptor.subSensorDescriptor[0].id = 0;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[0].sensorType = COM_TYPE_TEMP;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[0].dimensions = 1;
  strcpy(SensorHTS221.sensorDescriptor.subSensorDescriptor[0].dimensionsLabel[0], "tem");
  SensorHTS221.sensorDescriptor.subSensorDescriptor[0].dataType = DATA_TYPE_FLOAT;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[0].ODR[0] = 1.0f;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[0].ODR[1] = 7.0f;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[0].ODR[2] = 12.5f;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[0].ODR[3] = COM_END_OF_LIST_FLOAT;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[0].samplesPerTimestamp[0] = 0;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[0].samplesPerTimestamp[1] = SAMPLES_PER_TIME_STAMP_1;
  strcpy(SensorHTS221.sensorDescriptor.subSensorDescriptor[0].unit, "Celsius");
  SensorHTS221.sensorDescriptor.subSensorDescriptor[0].FS[0] = FULL_SCALE_1;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[0].FS[1] = COM_END_OF_LIST_FLOAT;

  /* SUBSENSOR 0 STATUS */
  SensorHTS221.sensorStatus.subSensorStatus[0].isActive = 1;
  SensorHTS221.sensorStatus.subSensorStatus[0].FS = FULL_SCALE_1;
  SensorHTS221.sensorStatus.subSensorStatus[0].sensitivity = 1.0f;
  SensorHTS221.sensorStatus.subSensorStatus[0].ODR = 12.5f;
  SensorHTS221.sensorStatus.subSensorStatus[0].measuredODR = 0.0f;
  SensorHTS221.sensorStatus.subSensorStatus[0].initialOffset = 0.0f;
  SensorHTS221.sensorStatus.subSensorStatus[0].samplesPerTimestamp = 50;
  SensorHTS221.sensorStatus.subSensorStatus[0].usbDataPacketSize = 16;
  SensorHTS221.sensorStatus.subSensorStatus[0].sdWriteBufferSize = WRITE_BUFFER_SIZE_HTS221_T;
  SensorHTS221.sensorStatus.subSensorStatus[0].comChannelNumber = -1;
  SensorHTS221.sensorStatus.subSensorStatus[0].ucfLoaded = 0;

  /* SUBSENSOR 1 DESCRIPTOR */
  SensorHTS221.sensorDescriptor.subSensorDescriptor[1].id = 1;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[1].sensorType = COM_TYPE_HUM;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[1].dimensions = 1;
  strcpy(SensorHTS221.sensorDescriptor.subSensorDescriptor[1].dimensionsLabel[0], "hum");
  SensorHTS221.sensorDescriptor.subSensorDescriptor[1].dataType = DATA_TYPE_FLOAT;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[1].ODR[0] = 1.0f;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[1].ODR[1] = 7.0f;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[1].ODR[2] = 12.5f;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[1].ODR[3] = COM_END_OF_LIST_FLOAT;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[1].samplesPerTimestamp[0] = 0;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[1].samplesPerTimestamp[1] = SAMPLES_PER_TIME_STAMP_1;
  strcpy(SensorHTS221.sensorDescriptor.subSensorDescriptor[1].unit, "%");
  SensorHTS221.sensorDescriptor.subSensorDescriptor[1].FS[0] = FULL_SCALE_2;
  SensorHTS221.sensorDescriptor.subSensorDescriptor[1].FS[1] = COM_END_OF_LIST_FLOAT;

  /* SUBSENSOR 1 STATUS */
  SensorHTS221.sensorStatus.subSensorStatus[1].isActive = 1;
  SensorHTS221.sensorStatus.subSensorStatus[1].FS = FULL_SCALE_2;
  SensorHTS221.sensorStatus.subSensorStatus[1].sensitivity = 1.0f;
  SensorHTS221.sensorStatus.subSensorStatus[1].ODR = 12.5f;
  SensorHTS221.sensorStatus.subSensorStatus[1].measuredODR = 0.0f;
  SensorHTS221.sensorStatus.subSensorStatus[1].initialOffset = 0.0f;
  SensorHTS221.sensorStatus.subSensorStatus[1].samplesPerTimestamp = 50;
  SensorHTS221.sensorStatus.subSensorStatus[1].usbDataPacketSize = 16;
  SensorHTS221.sensorStatus.subSensorStatus[1].sdWriteBufferSize = WRITE_BUFFER_SIZE_HTS221_H;
  SensorHTS221.sensorStatus.subSensorStatus[1].comChannelNumber = -1;
  SensorHTS221.sensorStatus.subSensorStatus[1].ucfLoaded = 0;

  /* LPS22HH SENSOR DESCRIPTOR */
  strcpy(SensorLPS22HH.sensorDescriptor.name, "LPS22HH");
  SensorLPS22HH.sensorDescriptor.nSubSensors = 2;
  SensorLPS22HH.sensorDescriptor.id = 1;

  /* SUBSENSOR 0 DESCRIPTOR */
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].id = 0;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].sensorType = COM_TYPE_PRESS;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].dimensions = 1;
  strcpy(SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].dimensionsLabel[0], "prs");
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].dataType = DATA_TYPE_FLOAT;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].ODR[0] = 1.0f;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].ODR[1] = 10.0f;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].ODR[2] = 25.0f;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].ODR[3] = 50.0f;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].ODR[4] = 75.0f;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].ODR[5] = ODR_1;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].ODR[6] = ODR_2;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].ODR[7] = COM_END_OF_LIST_FLOAT;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].samplesPerTimestamp[0] = 0;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].samplesPerTimestamp[1] = SAMPLES_PER_TIME_STAMP_1;
  strcpy(SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].unit, "hPa");
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].FS[0] = ODR_3;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[0].FS[1] = COM_END_OF_LIST_FLOAT;

  /* SUBSENSOR 0 STATUS */
  SensorLPS22HH.sensorStatus.subSensorStatus[0].isActive = 1;
  SensorLPS22HH.sensorStatus.subSensorStatus[0].FS = ODR_3;
  SensorLPS22HH.sensorStatus.subSensorStatus[0].sensitivity = 1.0f;
  SensorLPS22HH.sensorStatus.subSensorStatus[0].ODR = ODR_2;
  SensorLPS22HH.sensorStatus.subSensorStatus[0].measuredODR = 0.0f;
  SensorLPS22HH.sensorStatus.subSensorStatus[0].initialOffset = 0.0f;
  SensorLPS22HH.sensorStatus.subSensorStatus[0].samplesPerTimestamp = SAMPLES_PER_TIME_STAMP_2;
  SensorLPS22HH.sensorStatus.subSensorStatus[0].usbDataPacketSize = USB_DATA_PACKET_SIZE_1;
  SensorLPS22HH.sensorStatus.subSensorStatus[0].sdWriteBufferSize = WRITE_BUFFER_SIZE_LPS22HH_P;
  SensorLPS22HH.sensorStatus.subSensorStatus[0].comChannelNumber = -1;
  SensorLPS22HH.sensorStatus.subSensorStatus[0].ucfLoaded = 0;

  /* SUBSENSOR 1 DESCRIPTOR */
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].id = 1;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].sensorType = COM_TYPE_TEMP;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].dimensions = 1;
  strcpy(SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].dimensionsLabel[0], "tem");
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].dataType = DATA_TYPE_FLOAT;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].ODR[0] = 1.0f;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].ODR[1] = 10.0f;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].ODR[2] = 25.0f;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].ODR[3] = 50.0f;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].ODR[4] = 75.0f;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].ODR[5] = ODR_1;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].ODR[6] = ODR_2;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].ODR[7] = COM_END_OF_LIST_FLOAT;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].samplesPerTimestamp[0] = 0;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].samplesPerTimestamp[1] = SAMPLES_PER_TIME_STAMP_1;
  strcpy(SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].unit, "Celsius");
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].FS[0] = 85.0f;
  SensorLPS22HH.sensorDescriptor.subSensorDescriptor[1].FS[1] = COM_END_OF_LIST_FLOAT;

  /* SUBSENSOR 1 STATUS */
  SensorLPS22HH.sensorStatus.subSensorStatus[1].isActive = 1;
  SensorLPS22HH.sensorStatus.subSensorStatus[1].FS = 85.0f;
  SensorLPS22HH.sensorStatus.subSensorStatus[1].sensitivity = 1.0f;
  SensorLPS22HH.sensorStatus.subSensorStatus[1].ODR = ODR_2;
  SensorLPS22HH.sensorStatus.subSensorStatus[1].measuredODR = 0.0f;
  SensorLPS22HH.sensorStatus.subSensorStatus[1].initialOffset = 0.0f;
  SensorLPS22HH.sensorStatus.subSensorStatus[1].samplesPerTimestamp = SAMPLES_PER_TIME_STAMP_2;
  SensorLPS22HH.sensorStatus.subSensorStatus[1].usbDataPacketSize = USB_DATA_PACKET_SIZE_1;
  SensorLPS22HH.sensorStatus.subSensorStatus[1].sdWriteBufferSize = WRITE_BUFFER_SIZE_LPS22HH_T;
  SensorLPS22HH.sensorStatus.subSensorStatus[1].comChannelNumber = -1;
  SensorLPS22HH.sensorStatus.subSensorStatus[1].ucfLoaded = 0;

  /* SENSOR DESCRIPTOR */
  strcpy(SensorLSM6DSOX.sensorDescriptor.name, "LSM6DSOX");
  SensorLSM6DSOX.sensorDescriptor.nSubSensors = 3;
  SensorLSM6DSOX.sensorDescriptor.id = 2;

  /* SUBSENSOR 0 DESCRIPTOR */
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].id = 0;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].sensorType = COM_TYPE_ACC;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].dimensions = 3;
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].dimensionsLabel[0], "x");
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].dimensionsLabel[1], "y");
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].dimensionsLabel[2], "z");
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].dataType = DATA_TYPE_INT16;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].ODR[0] = 12.5f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].ODR[1] = 26.0f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].ODR[2] = 52.0f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].ODR[3] = ODR_4;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].ODR[4] = ODR_5;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].ODR[5] = ODR_6;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].ODR[6] = ODR_7;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].ODR[7] = ODR_8;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].ODR[8] = ODR_9;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].ODR[9] = ODR_10;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].ODR[10] = COM_END_OF_LIST_FLOAT;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].samplesPerTimestamp[0] = 0;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].samplesPerTimestamp[1] = SAMPLES_PER_TIME_STAMP_1;
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].unit, "g");
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].FS[0] = 2.0f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].FS[1] = 4.0f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].FS[2] = 8.0f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].FS[3] = 16.0f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[0].FS[4] = COM_END_OF_LIST_FLOAT;

  /* SUBSENSOR 0 STATUS */
  SensorLSM6DSOX.sensorStatus.subSensorStatus[0].isActive = 1;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[0].FS = 16.0f;
  temp = 0.0000305f * SensorLSM6DSOX.sensorStatus.subSensorStatus[0].FS;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[0].sensitivity = temp;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[0].ODR = ODR_10;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[0].measuredODR = 0.0f;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[0].initialOffset = 0.0f;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[0].samplesPerTimestamp = SAMPLES_PER_TIME_STAMP_1;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[0].usbDataPacketSize = USB_DATA_PACKET_SIZE_2;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[0].sdWriteBufferSize = WRITE_BUFFER_SIZE_LSM6DSOX_A;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[0].comChannelNumber = -1;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[0].ucfLoaded = 0;

  /* SUBSENSOR 1 DESCRIPTOR */
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].id = 1;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].sensorType = COM_TYPE_GYRO;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].dimensions = 3;
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].dimensionsLabel[0], "x");
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].dimensionsLabel[1], "y");
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].dimensionsLabel[2], "z");
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].dataType = DATA_TYPE_INT16;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].ODR[0] = 12.5f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].ODR[1] = 26.0f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].ODR[2] = 52.0f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].ODR[3] = ODR_4;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].ODR[4] = ODR_5;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].ODR[5] = ODR_6;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].ODR[6] = ODR_7;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].ODR[7] = ODR_8;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].ODR[8] = ODR_9;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].ODR[9] = ODR_10;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].ODR[10] = COM_END_OF_LIST_FLOAT;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].samplesPerTimestamp[0] = 0;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].samplesPerTimestamp[1] = SAMPLES_PER_TIME_STAMP_1;
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].unit, "mdps");
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].FS[0] = FULL_SCALE_3;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].FS[1] = FULL_SCALE_4;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].FS[2] = FULL_SCALE_5;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].FS[3] = FULL_SCALE_6;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].FS[4] = FULL_SCALE_7;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[1].FS[5] = COM_END_OF_LIST_FLOAT;

  /* SUBSENSOR 1 STATUS */
  SensorLSM6DSOX.sensorStatus.subSensorStatus[1].isActive = 1;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[1].FS = FULL_SCALE_7;
  temp = 0.035f * SensorLSM6DSOX.sensorStatus.subSensorStatus[1].FS;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[1].sensitivity = temp;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[1].ODR = ODR_10;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[1].measuredODR = 0.0f;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[1].initialOffset = 0.0f;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[1].samplesPerTimestamp = SAMPLES_PER_TIME_STAMP_1;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[1].usbDataPacketSize = USB_DATA_PACKET_SIZE_2;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[1].sdWriteBufferSize = WRITE_BUFFER_SIZE_LSM6DSOX_G;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[1].comChannelNumber = -1;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[1].ucfLoaded = 0;

  /* SUBSENSOR 2 DESCRIPTOR */
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].id = 2;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].sensorType = COM_TYPE_MLC;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].dimensions = 8;
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].dimensionsLabel[0], "1");
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].dimensionsLabel[1], "2");
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].dimensionsLabel[2], "3");
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].dimensionsLabel[3], "4");
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].dimensionsLabel[4], "5");
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].dimensionsLabel[5], "6");
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].dimensionsLabel[6], "7");
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].dimensionsLabel[7], "8");
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].dataType = DATA_TYPE_INT8;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].ODR[0] = 12.5f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].ODR[1] = 26.0f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].ODR[2] = 52.0f;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].ODR[3] = ODR_4;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].ODR[4] = COM_END_OF_LIST_FLOAT;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].samplesPerTimestamp[0] = 0;
  SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].samplesPerTimestamp[1] = SAMPLES_PER_TIME_STAMP_1;
  strcpy(SensorLSM6DSOX.sensorDescriptor.subSensorDescriptor[2].unit, "out");

  /* SUBSENSOR 2 STATUS */
  SensorLSM6DSOX.sensorStatus.subSensorStatus[2].isActive = 0;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[2].FS = 0.0f;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[2].sensitivity = 1.0f;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[2].ODR = 0.0f;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[2].measuredODR = 0.0f;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[2].initialOffset = 0.0f;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[2].samplesPerTimestamp = 1;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[2].usbDataPacketSize = 16;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[2].sdWriteBufferSize = WRITE_BUFFER_SIZE_LSM6DSOX_MLC;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[2].comChannelNumber = -1;
  SensorLSM6DSOX.sensorStatus.subSensorStatus[2].ucfLoaded = 0;

  /* Add the sensors to the Array */
  tempJSON1 = json_value_init_object();
  create_JSON_Sensor(&SensorHTS221, tempJSON1);
  json_array_append_value(JSON_SensorArray, tempJSON1);
  tempJSON1 = json_value_init_object();
  create_JSON_Sensor(&SensorLPS22HH, tempJSON1);
  json_array_append_value(JSON_SensorArray, tempJSON1);
  tempJSON1 = json_value_init_object();
  create_JSON_Sensor(&SensorLSM6DSOX, tempJSON1);
  json_array_append_value(JSON_SensorArray, tempJSON1);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigReadSensorConfigCommandCallback could be implemented in the user file
           for managing the received command with the used sensor
   */
}

/**
  * @brief  Set Sensor Config
  * @param  uint8_t *configuration
  * @retval None
  */
__weak void ExtConfigSetSensorConfigCommandCallback(uint8_t *configuration)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(configuration);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the ExtConfigSetSensorConfigCommandCallback could be implemented in the user file
           for managing the received command with the used sensor
   */
}
#endif /* BLE_MANAGER_USE_PARSON */
