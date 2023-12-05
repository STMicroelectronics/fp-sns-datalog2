/**
  ******************************************************************************
  * @file    SensorCommands.h
  * @author  SRA - MCD
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */
#ifndef SENSORCOMMANDS_H_
#define SENSORCOMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif

// Command ID. These are all commands supported by a sensor task.
#define SENSOR_CMD_ID_INIT            ((uint16_t)0x0001)              ///< START command ID.
#define SENSOR_CMD_ID_ENABLE          ((uint16_t)0x0002)              ///< ENABLE command ID.
#define SENSOR_CMD_ID_DISABLE         ((uint16_t)0x0003)              ///< DISABLE command ID.
/* SensorMems */
#define SENSOR_CMD_ID_SET_ODR         ((uint16_t)0x0004)              ///< SET ODR command ID.
#define SENSOR_CMD_ID_SET_FS          ((uint16_t)0x0005)              ///< SET FS command ID.
#define SENSOR_CMD_ID_SET_FIFO_WM     ((uint16_t)0x0006)              ///< SET Fifo WM command ID.
/* SensorAudio */
#define SENSOR_CMD_ID_SET_FREQUENCY   ((uint16_t)0x0004)              ///< SET ODR command ID.
#define SENSOR_CMD_ID_SET_VOLUME      ((uint16_t)0x0005)              ///< SET FS command ID.
#define SENSOR_CMD_ID_SET_RESOLUTION  ((uint16_t)0x0006)              ///< SET Fifo WM command ID.
/* SensorRanging */
//#define SENSOR_CMD_ID_SET_FREQUENCY        ((uint16_t)0x0004)              ///< SET ODR command ID.
#define SENSOR_CMD_ID_SET_RANGING_MODE     ((uint16_t)0x0005)              ///< SET ODR command ID.
//#define SENSOR_CMD_ID_SET_RESOLUTION       ((uint16_t)0x0006)              ///< SET ODR command ID.
#define SENSOR_CMD_ID_SET_INTEGRATION_TIME ((uint16_t)0x0007)              ///< SET ODR command ID.
#define SENSOR_CMD_ID_CONFIG_IT            ((uint16_t)0x0008)              ///< SET FS command ID.
#define SENSOR_CMD_ID_SET_ADDRESS          ((uint16_t)0x0009)              ///< SET Fifo WM command ID.
#define SENSOR_CMD_ID_SET_POWERMODE        ((uint16_t)0x000A)              ///< SET Fifo WM command ID.
/* SensorPresence */
#define SENSOR_CMD_ID_SET_DATA_FREQUENCY            ((uint16_t)0x0004)              ///< SET Data Frequency command ID.
#define SENSOR_CMD_ID_SET_AVERAGE_T_OBJECT          ((uint16_t)0x0005)              ///< SET Average T Object command ID.
#define SENSOR_CMD_ID_SET_AVERAGE_T_AMBIENT         ((uint16_t)0x0006)              ///< SET Average T Ambient command ID.
#define SENSOR_CMD_ID_SET_PRESENCE_THRESHOLD        ((uint16_t)0x0007)              ///< SET Presence Threshold command ID.
#define SENSOR_CMD_ID_SET_PRESENCE_HYSTERESIS       ((uint16_t)0x0008)              ///< SET Presence Hysteresis command ID.
#define SENSOR_CMD_ID_SET_MOTION_THRESHOLD          ((uint16_t)0x0009)              ///< SET Motion Threshold command ID.
#define SENSOR_CMD_ID_SET_MOTION_HYSTERESIS         ((uint16_t)0x000A)              ///< SET Motion Hysteresis command ID.
#define SENSOR_CMD_ID_SET_T_AMBIENT_SHOCK_THRESHOLD  ((uint16_t)0x000B)             ///< SET T Ambient Shock Threshold command ID.
#define SENSOR_CMD_ID_SET_T_AMBIENT_SHOCK_HYSTERESIS ((uint16_t)0x000C)             ///< SET T Ambient Shock Hysteresis command ID.
#define SENSOR_CMD_ID_SET_LPF_P_M_BANDWIDTH         ((uint16_t)0x000D)              ///< SET LPF_P_M Bandwidth command ID.
#define SENSOR_CMD_ID_SET_LPF_P_BANDWIDTH           ((uint16_t)0x000E)              ///< SET LPF_P Bandwidth command ID.
#define SENSOR_CMD_ID_SET_LPF_M_BANDWIDTH           ((uint16_t)0x000F)              ///< SET LPF_M Bandwidth command ID.
#define SENSOR_CMD_ID_SET_EMBEDDED_COMPENSATION     ((uint16_t)0x0010)              ///< SET Embedded Compensation command ID.
#define SENSOR_CMD_ID_SET_SOFTWARE_COMPENSATION     ((uint16_t)0x0011)              ///< SET Software Compensation command ID.
#define SENSOR_CMD_ID_SET_TRANSMITTANCE             ((uint16_t)0x0012)              ///< SET Transmittance command ID.
#define SENSOR_CMD_ID_SET_SW_COMPENSATION_PARAMETERS ((uint16_t)0x0013)             ///< SET SW Compensation Algorithm Parameters command ID.
/* SensorLight */
#define SENSOR_CMD_ID_SET_INTERMEASUREMENT_TIME     ((uint16_t)0x0004)              ///< SET Intermeasurement Time command ID.
#define SENSOR_CMD_ID_SET_EXPOSURE_TIME             ((uint16_t)0x0005)              ///< SET Exposure Time ID.
#define SENSOR_CMD_ID_SET_LIGHT_CH1_GAIN            ((uint16_t)0x0006)              ///< SET Channel 1 Gain command ID.
#define SENSOR_CMD_ID_SET_LIGHT_CH2_GAIN            ((uint16_t)0x0007)              ///< SET Channel 2 Gain command ID.
#define SENSOR_CMD_ID_SET_LIGHT_CH3_GAIN            ((uint16_t)0x0008)              ///< SET Channel 3 Gain command ID.
#define SENSOR_CMD_ID_SET_LIGHT_CH4_GAIN            ((uint16_t)0x0009)              ///< SET Channel 4 Gain command ID.
#define SENSOR_CMD_ID_SET_LIGHT_CH5_GAIN            ((uint16_t)0x000A)              ///< SET Channel 5 Gain command ID.
#define SENSOR_CMD_ID_SET_LIGHT_CH6_GAIN            ((uint16_t)0x000B)              ///< SET Channel 6 Gain command ID.



#ifdef __cplusplus
}
#endif

#endif /* SENSORCOMMANDS_H_ */
