/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    BLE_Implementation_Template.h
  * @author  System Research & Applications Team - Catania Lab.
  * @brief   BLE Implementation header template file.
  *          This file should be copied to the application folder and renamed
  *          to BLE_Implementation.h.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BLE_IMPLEMENTATION_H_
#define _BLE_IMPLEMENTATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/**
  * User can added here the header file for the selected BLE features.
  * For example:
  * #include "BLE_Environmental.h"
  * #include "BLE_Inertial.h"
  */

#include "BLE_AudioSceneClassification.h"

#include "BLE_HighSpeedDataLog.h"

#include "BLE_AccEvent.h"

#include "BLE_AudioLevel.h"

#include "BLE_Battery.h"

#include "BLE_Environmental.h"

#include "BLE_FiniteStateMachine.h"

#include "BLE_GasConcentration.h"

#include "BLE_Gnss.h"

#include "BLE_Inertial.h"

#include "BLE_Led.h"

#include "BLE_MachineLearningCore.h"

#include "BLE_ObjectsDetection.h"

#include "BLE_Piano.h"

#include "BLE_ActivityRecognition.h"

#include "BLE_AudioSourceLocalization.h"

#include "BLE_CarryPosition.h"

#include "BLE_ECompass.h"

#include "BLE_FitnessActivities.h"

#include "BLE_GestureRecognition.h"

#include "BLE_MotionAlgorithms.h"

#include "BLE_MotionIntensity.h"

#include "BLE_PedometerAlgorithm.h"

#include "BLE_SensorFusion.h"

#include "BLE_TiltSensing.h"

#include "BLE_FFT_AlarmSubrangeStatus.h"

#include "BLE_FFT_Amplitude.h"

#include "BLE_TimeDomain.h"

#include "BLE_TimeDomainAlarmAccPeakStatus.h"

#include "BLE_TimeDomainAlarmSpeed_RMS_Status.h"

#include "BLE_GeneralPurpose.h"

#include "BLE_Ota.h"

#include "BLE_SDLogging.h"

#include "BLE_PnPLike.h"

#include "BLE_EventCounter.h"

#include "BLE_Json.h"

#include "BLE_NeaiAnomalyDetection.h"

#include "BLE_NeaiNClassClassification.h"

#include "BLE_Normalization.h"

#include "BLE_GestureNavigation.h"

#include "BLE_BinaryContent.h"

#include "BLE_QVAR.h"

/* Exported Defines --------------------------------------------------------*/
#define STM32L4xx

/* For Set Certificate Command */
#define SET_CERT      1
/* For Custom Command */
#define READ_CUSTOM_COMMANDS      1
/* Enable/Disable BlueNRG config extend services */
#define ENABLE_EXT_CONFIG      1
/* Enable/Disable BlueNRG config services */
#define ENABLE_CONFIG      1
/* For Set Date Command */
#define SET_DATE      1
/* Enable/Disable Secure Connection */
#define ENABLE_SECURE_CONNECTION      0
/* For Version Fw Command */
#define VERSION_FW      1
/* For Info Command */
#define INFO      1
/* For Change Secure PIN Command */
#define CHANGE_PIN      1
/* For Clear Secure Data Base Command */
#define CLEAR_DB      1
/* For UID Command */
#define UID_COMMAND      1
/* For Read Certificate Command */
#define READ_CERT      1
/* For Swapping the Flash Banks */
#define BANKS_SWAP      1
/* Number of audio channels (Max audio channels 4) */
#define AUDIO_CHANNELS_NUMBER      1
/* Number of the general purpose features to use */
#define NUM_GENERAL_PURPOSE      1
/* For Help Command */
#define HELP      1
/* For Reading the Flash Banks Fw Ids */
#define READ_BANKS_FW_ID      1
/* Enable/Disable magnetometer data (Disable= 0- Enable=1) */
#define ENABLE_MAG_DATA      1
/* For Reboot on DFU Command */
#define REBOOT_ON_DFU_MODE      1
/* For Set Time Command */
#define SET_TIME      1
/* For Power Status Command */
#define POWER_STATUS      1
/* Secure PIN */
#define SECURE_PIN      123456
/* Audio Scene Classificatio algorithm code */
#define ALGORITHM_CODE      0
/* Enable/Disable pressure data (Disable= 0- Enable=1) */
#define ENABLE_ENV_PRESSURE_DATA      1
/* Enable/Disable Random Secure PIN */
#define ENABLE_RANDOM_SECURE_PIN      0
/* For Set board Name Command */
#define SET_NAME      1
/* Enable/Disable BlueNRG console services */
#define ENABLE_CONSOLE      1
/* Enable/Disable giroscope data (Disable= 0- Enable=1) */
#define ENABLE_GYRO_DATA      1
/* Enable/Disable humidity data (Disable= 0- Enable=1) */
#define ENABLE_ENV_HUMIDITY_DATA      1
/* Size of the general purpose feature */
#define GENERAL_PURPOSE_SIZE_1      3
/* Number of quaternion to send (max value 3) */
#define NUMBER_OF_QUATERNION      1
/* Enable/Disable number of temperature (0, 1, 2) */
#define ENABLE_ENV_TEMPERATURE_DATA      1
/* For Set Wi-Fi Command */
#define SET_WIFI      1
/* For Set sensor config */
#define SENSOR_CONFIG      1
/* Supported hardware platform */
#define USED_PLATFORM      0x7EU
/* For Power off Command */
#define POWER_OFF      1
/* Enable/Disable accelerometer data (Disable= 0- Enable=1) */
#define ENABLE_ACC_DATA      1
/* Select the used hardware platform
 *
 * STEVAL-WESU1                         --> BLE_MANAGER_STEVAL_WESU1_PLATFORM
 * STEVAL-STLKT01V1 (SensorTile)        --> BLE_MANAGER_SENSOR_TILE_PLATFORM
 * STEVAL-BCNKT01V1 (BlueCoin)          --> BLE_MANAGER_BLUE_COIN_PLATFORM
 * STEVAL-IDB008Vx                      --> BLE_MANAGER_STEVAL_IDB008VX_PLATFORM
 * STEVAL-BCN002V1B (BlueTile)          --> BLE_MANAGER_STEVAL_BCN002V1_PLATFORM
 * STEVAL-MKSBOX1V1 (SensorTile.box)    --> BLE_MANAGER_SENSOR_TILE_BOX_PLATFORM
 * DISCOVERY-IOT01A                     --> BLE_MANAGER_DISCOVERY_IOT01A_PLATFORM
 * STEVAL-STWINKT1                      --> BLE_MANAGER_STEVAL_STWINKT1_PLATFORM
 * STEVAL-STWINKT1B                     --> BLE_MANAGER_STEVAL_STWINKT1B_PLATFORM
 * STEVAL_STWINBX1                      --> BLE_MANAGER_STEVAL_STWINBX1_PLATFORM
 * SENSOR_TILE_BOX_PRO                  --> BLE_MANAGER_SENSOR_TILE_BOX_PRO_PLATFORM
 * STEVAL_ASTRA1                        --> BLE_MANAGER_STEVAL_ASTRA1_PLATFORM
 * STM32NUCLEO Board                    --> BLE_MANAGER_NUCLEO_PLATFORM
 * STM32F446RE_NUCLEO Board             --> BLE_MANAGER_STM32F446RE_NUCLEO_PLATFORM
 * STM32L053R8_NUCLEO Board             --> BLE_MANAGER_STM32L053R8_NUCLEO_PLATFORM
 * STM32L476RG_NUCLEO Board             --> BLE_MANAGER_STM32L476RG_NUCLEO_PLATFORM
 * STM32F401RE_NUCLEO Board             --> BLE_MANAGER_STM32F401RE_NUCLEO_PLATFORM
 * Not defined platform                 --> BLE_MANAGER_UNDEF_PLATFORM
 *
 * For example:
 * #define BLE_MANAGER_USED_PLATFORM  BLE_MANAGER_NUCLEO_PLATFORM
 *
*/

/* Used platform */
#define BLE_MANAGER_USED_PLATFORM       USED_PLATFORM

/* STM32 Unique ID */
#define BLE_STM32_UUID          UID_BASE

/* STM32 MCU_ID */
#ifdef DBGMCU_BASE
#define BLE_STM32_MCU_ID        ((uint32_t *)DBGMCU_BASE)
#else /* DBGMCU_BASE */
#define BLE_STM32_MCU_ID        ((uint32_t *)0x00000000UL)
#endif /* DBGMCU_BASE */

/* STM32  Microcontrolles type */
#define BLE_STM32_MICRO "STM32L4xx"

/* USER CODE BEGIN 1 */

/* Package Version firmware */
#define BLE_VERSION_FW_MAJOR  '3'
#define BLE_VERSION_FW_MINOR  '0'
#define BLE_VERSION_FW_PATCH  '0'

/* Firmware Package Name */
#define BLE_FW_PACKAGENAME    "X-CUBE-BLEMGR"

/* USER CODE END 1 */

/* Feature mask for Accelerometer events */
#define FEATURE_MASK_ACC_EVENTS 0x00000400

/* Feature mask for Temperature1 */
#define FEATURE_MASK_TEMP1 0x00040000
/* Feature mask for Temperature2 */
#define FEATURE_MASK_TEMP2 0x00010000
/* Feature mask for Pressure */
#define FEATURE_MASK_PRESS 0x00100000
/* Feature mask for Humidity */
#define FEATURE_MASK_HUM   0x00080000

/* Feature mask for Accelerometer */
#define FEATURE_MASK_ACC   0x00800000
/* Feature mask for Gyroscope */
#define FEATURE_MASK_GRYO  0x00400000
/* Feature mask for Magnetometer */
#define FEATURE_MASK_MAG   0x00200000

/* Feature mask for LED */
#define FEATURE_MASK_LED 0x20000000

/* Feature mask for e-compass */
#define FEATURE_MASK_ECOMPASS 0x00000040

/* Feature mask for Sensor fusion short precision */
#define FEATURE_MASK_SENSORFUSION_SHORT 0x00000100

/* W2ST command for asking the calibration status */
#define W2ST_COMMAND_CAL_STATUS 0xFF
/* W2ST command for resetting the calibration */
#define W2ST_COMMAND_CAL_RESET  0x00
/* W2ST command for stopping the calibration process */
#define W2ST_COMMAND_CAL_STOP   0x01

/* Exported Variables ------------------------------------------------------- */

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/* Exported functions ------------------------------------------------------- */
extern void BLE_InitCustomService(void);
extern void BLE_SetCustomAdvertiseData(uint8_t *manuf_data);
extern void BluetoothInit(void);
extern void DisconnectionCompletedFunction(void);
#ifdef BLUENRG_1_2
extern void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t Address_Type, uint8_t addr[6]);
#endif /* BLUENRG_1_2 */
#ifdef BLUENRG_MS
extern void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t addr[6]);
#endif /* BLUENRG_MS */
#ifdef BLUENRG_LP
extern void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t Address_Type, uint8_t addr[6]);
#endif /* BLUENRG_LP */
extern void SetBoardName(void);
extern void AttrModConfigFunction(uint8_t *att_data, uint8_t data_length);
extern void PairingCompletedFunction(uint8_t PairingStatus);
extern void SetConnectableFunction(uint8_t *ManufData);
extern void AciGattTxPoolAvailableEventFunction(void);
extern void HardwareErrorEventHandlerFunction(uint8_t Hardware_Code);
extern uint32_t DebugConsoleParsing(uint8_t *att_data, uint8_t data_length);
extern void WriteRequestConfigFunction(uint8_t *att_data, uint8_t data_length);

extern void ReadRequestEnvFunction(int32_t *Press, uint16_t *Hum, int16_t *Temp1, int16_t *Temp2);

#ifdef BLE_MANAGER_USE_PARSON
/***********************************************************************************************
  * Callback functions prototypes to manage the extended configuration characteristic commands *
  **********************************************************************************************/
extern void ExtExtConfigUidCommandCallback(uint8_t **UID);
extern void ExtConfigVersionFwCommandCallback(uint8_t *Answer);
extern void ExtConfigInfoCommandCallback(uint8_t *Answer);
extern void ExtConfigHelpCommandCallback(uint8_t *Answer);
extern void ExtConfigPowerStatusCommandCallback(uint8_t *Answer);

extern void ExtConfigChangePinCommandCallback(uint32_t NewPin);
extern void ExtConfigClearDBCommandCallback(void);
extern void ExtConfigReadCertCommandCallback(uint8_t *Certificate);
extern void ExtConfigSetCertCommandCallback(uint8_t *Certificate);

extern void ExtConfigRebootOnDFUModeCommandCallback(void);
extern void ExtConfigPowerOffCommandCallback(void);
extern void ExtConfigReadBanksFwIdCommandCallback(uint8_t *CurBank, uint16_t *FwId1, uint16_t *FwId2);
extern void ExtConfigBanksSwapCommandCallback(void);

extern void ExtConfigSetNameCommandCallback(uint8_t *NewName);
extern void ExtConfigReadCustomCommandsCallback(JSON_Array *JSON_SensorArray);
extern void  ExtConfigCustomCommandCallback(BLE_CustomCommadResult_t *CustomCommand);
extern void ExtConfigSetDateCommandCallback(uint8_t *NewDate);
extern void ExtConfigSetTimeCommandCallback(uint8_t *NewTime);
extern void ExtConfigSetWiFiCommandCallback(BLE_WiFi_CredAcc_t NewWiFiCred);
extern void ExtConfigReadSensorConfigCommandCallback(JSON_Array *JSON_SensorArray);
extern void ExtConfigSetSensorConfigCommandCallback(uint8_t *configuration);
#endif /* BLE_MANAGER_USE_PARSON */

/**************************************************************
  * Callback functions prototypes to manage the notify events *
  *************************************************************/

extern void NotifyEventAudioSceneClassification(BLE_NotifyEvent_t Event);

extern void NotifyEventHighSpeedDataLog(BLE_NotifyEvent_t Event);

extern void NotifyEventAccEvent(BLE_NotifyEvent_t Event);

extern void NotifyEventAudioLevel(BLE_NotifyEvent_t Event);

extern void NotifyEventBattery(BLE_NotifyEvent_t Event);

extern void NotifyEventEnv(BLE_NotifyEvent_t Event);

extern void NotifyEventFiniteStateMachine(BLE_NotifyEvent_t Event);

extern void NotifyEventGasConcentration(BLE_NotifyEvent_t Event);

extern void NotifyEventGnss(BLE_NotifyEvent_t Event);

extern void NotifyEventInertial(BLE_NotifyEvent_t Event);

extern void NotifyEventLed(BLE_NotifyEvent_t Event);

extern void NotifyEventMachineLearningCore(BLE_NotifyEvent_t Event);

extern void NotifyEventObjectsDetection(BLE_NotifyEvent_t Event);

extern void NotifyEventPiano(BLE_NotifyEvent_t Event);

extern void NotifyEventActRec(BLE_NotifyEvent_t Event);

extern void NotifyEventAudioSourceLocalization(BLE_NotifyEvent_t Event);

extern void NotifyEventCarryPosition(BLE_NotifyEvent_t Event);

extern void NotifyEventECompass(BLE_NotifyEvent_t Event);

extern void NotifyEventFitnessActivities(BLE_NotifyEvent_t Event);
extern void WriteRequestFitnessActivities(uint8_t FitnessActivitie);

extern void NotifyEventGestureRecognition(BLE_NotifyEvent_t Event);

extern void NotifyEventMotionAlgorithms(BLE_NotifyEvent_t Event, BLE_MotionAlgorithmsType_t Algorithm);
extern void WriteRequestMotionAlgorithms(BLE_MotionAlgorithmsType_t Algorithm);

extern void NotifyEventMotionIntensity(BLE_NotifyEvent_t Event);

extern void NotifyEventPedometerAlgorithm(BLE_NotifyEvent_t Event);

extern void NotifyEventSensorFusion(BLE_NotifyEvent_t Event);

extern void NotifyEventTiltSensing(BLE_NotifyEvent_t Event);

extern void NotifyEventFFT_AlarmSubrangeStatus(BLE_NotifyEvent_t Event);

extern void NotifyEventFFT_Amplitude(BLE_NotifyEvent_t Event);

extern void NotifyEventTimeDomain(BLE_NotifyEvent_t Event);

extern void NotifyEventTimeDomainAlarmAccPeakStatus(BLE_NotifyEvent_t Event);

extern void NotifyEventTimeDomainAlarmSpeed_RMS_Status(BLE_NotifyEvent_t Event);

extern void NotifyEventGeneralPurpose(uint8_t GP_CharNum, BLE_NotifyEvent_t Event);

extern void NotifyEventSDLog(BLE_NotifyEvent_t Event);
extern void WriteRequestSDLog(uint8_t *att_data, uint8_t data_length);

#ifdef BLE_MANAGER_USE_PARSON
extern void NotifyEventPnpLike(BLE_NotifyEvent_t Event);
extern void WriteRequestPnPLikeFunctionPointer(uint8_t *received_msg, uint8_t msg_length);
#endif /* BLE_MANAGER_USE_PARSON */

extern void NotifyEventEventCounter(BLE_NotifyEvent_t Event);

#ifdef BLE_MANAGER_USE_PARSON
extern void NotifyEventJson(BLE_NotifyEvent_t Event);
extern void WriteRequestJson(uint8_t *received_msg, uint8_t msg_length);
#endif /* BLE_MANAGER_USE_PARSON */

extern void NotifyEventNeaiAnomalyDetection(BLE_NotifyEvent_t Event);
extern void WriteRequestNeaiAnomalyDetection(uint8_t *att_data, uint8_t data_length);

extern void NotifyEventNeaiNClassClassification(BLE_NotifyEvent_t Event);
extern void WriteRequestNeaiNClassClassification(uint8_t *att_data, uint8_t data_length);

extern void NotifyEventNormalization(BLE_NotifyEvent_t Event);

extern void NotifyEventGestureNavigation(BLE_NotifyEvent_t Event);

extern void NotifyEventBinaryContent(BLE_NotifyEvent_t Event);
extern void WriteRequestBinaryContent(uint8_t *received_msg, uint32_t msg_length);

extern void NotifyEventQVAR(BLE_NotifyEvent_t Event);

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */

#ifdef __cplusplus
}
#endif

#endif /* _BLE_IMPLEMENTATION_H_ */

