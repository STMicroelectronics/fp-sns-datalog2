/**
  ******************************************************************************
  * @file    SensorDef.h
  * @author  SRA - MCD
  *
  *
  *
  * @brief
  *
  *
  *
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
#ifndef SENSORDEF_H_
#define SENSORDEF_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "services/eloom_sysbase.h"


#define COM_TYPE_ACC     1
#define COM_TYPE_MAG     2
#define COM_TYPE_GYRO    3
#define COM_TYPE_TEMP    4
#define COM_TYPE_PRESS   5
#define COM_TYPE_HUM     6
#define COM_TYPE_MIC     7
#define COM_TYPE_MLC     8
#define COM_TYPE_ISPU    9
#define COM_TYPE_TOF    10
#define COM_TYPE_CAM    11
#define COM_TYPE_TMOS   12
#define COM_TYPE_ALS    13
#define COM_TYPE_POW 14

#define DATA_TYPE_UINT8  (uint8_t)(0x00)
#define DATA_TYPE_INT8   (uint8_t)(0x01)
#define DATA_TYPE_UINT16 (uint8_t)(0x02)
#define DATA_TYPE_INT16  (uint8_t)(0x03)
#define DATA_TYPE_UINT32 (uint8_t)(0x04)
#define DATA_TYPE_INT32  (uint8_t)(0x05)
#define DATA_TYPE_FLOAT  (uint8_t)(0x06)

#define COM_END_OF_LIST_INT      -1
#define COM_END_OF_LIST_FLOAT    -1.0f

#define COM_LIST_SEPARATOR_INT   -2
#define COM_LIST_SEPARATOR_FLOAT -2.0f

#ifndef SM_MAX_SENSOR_COMBO
#define SM_MAX_SENSOR_COMBO  4
#endif
#ifndef SM_MAX_SUPPORTED_ODR
#define SM_MAX_SUPPORTED_ODR 16
#endif
#ifndef SM_MAX_SUPPORTED_FS
#define SM_MAX_SUPPORTED_FS  16
#endif
#ifndef SM_MAX_DIM_LABELS
#define SM_MAX_DIM_LABELS    16U
#endif
#ifndef SM_DIM_LABELS_LENGTH
#define SM_DIM_LABELS_LENGTH 4U
#endif

#define ISENSOR_CLASS_MEMS         0U
#define ISENSOR_CLASS_AUDIO        1U
#define ISENSOR_CLASS_RANGING      2U
#define ISENSOR_CLASS_LIGHT        3U
#define ISENSOR_CLASS_CAMERA       4U
#define ISENSOR_CLASS_PRESENCE     5U
#define ISENSOR_CLASS_POWERMONITOR 6U
#define ISENSOR_CLASS_UNKNOWN      255U

#define ALS_NUM_CHANNELS 6U

/**
  * Create  type name for _SensorDescriptor_t.
  */
typedef struct _SensorDescriptor_t SensorDescriptor_t;

/**
  *  SensorDescriptor_t internal structure.
  */
struct _SensorDescriptor_t
{
  /**
    * Specifies the sensor name.
    */
  char p_name[SM_MAX_DIM_LABELS];

  /**
    * Specifies the sensor type (ACC, GYRO, TEMP, ...).
    */
  uint8_t sensor_type;
};


/**
  * ranging Profile Config parameters
  */
typedef struct
{
  uint32_t ranging_profile;
  uint32_t timing_budget; /*!< Expressed in milliseconds */
  uint32_t frequency; /*!< Expressed in Hz */
  boolean_t enable_ambient; /*<! Enable: 1, Disable: 0 */
  boolean_t enable_signal; /*<! Enable: 1, Disable: 0 */
  uint32_t mode;
} ProfileConfig_t;

/**
  * ranging IT Config parameters
  */
typedef struct
{
  uint32_t criteria; /*!< interrupt generation criteria */
  uint32_t low_threshold; /*!< expressed in millimeters */
  uint32_t high_threshold; /*!< expressed in millimeters */
} ITConfig_t;

/**
  * SW Compensation Algorithm Config parameters
  */
typedef struct
{
  uint8_t comp_type; /*<! None: 0, Linear: 1, Non-linear: 2 */
  uint8_t comp_filter_flag; /*<! Enable: 1, Disable: 0 */
  uint16_t mot_ths; /*!< Expressed in LSB */
  uint16_t pres_ths; /*!< Expressed in LSB */
  uint8_t abs_static_flag; /*<! Enable: 1, Disable: 0 */
} CompensationAlgorithmConfig_t;

typedef struct SensorStatusMems_t
{
  /**
    * Specifies the full scale.
    */
  float fs;

  /**
    * Specifies the sensitivity.
    */
  float sensitivity;

  /**
    * Specifies the nominal data rate.
    */
  float odr;

  /**
    * Specifies the effective data rate.
    */
  float measured_odr;
} SensorStatusMems;

typedef struct SensorStatusAudio_t
{
  /**
     * Specifies the sampling frequency.
     */
  uint32_t frequency;

  /**
    * Specifies the volume.
    */
  uint8_t volume;

  /**
    * Specifies the bit depth.
    */
  uint8_t resolution;
} SensorStatusAudio;

typedef struct SensorStatusRanging_t
{
  /**
    * ranging Profile Config parameters
    */
  ProfileConfig_t profile_config;

  /**
    * ranging IT Config parameters
    */
  ITConfig_t it_config;

  /**
    * ranging sensor address
    */
  uint32_t address;

  /*
    * ranging sensor Power mode
    * */
  uint32_t power_mode;
} SensorStatusRanging;

typedef struct SensorStatusCamera_t
{
  /**
     * camera Config parameters, specifies the the mode to retrieve image frame
     */
  uint8_t mode;
  boolean_t suspend;
  uint32_t resolution;
  uint32_t pixelFormat;
  uint32_t lightMode;
  uint32_t colorEffect;
  int32_t brightness;
  int32_t saturation;
  int32_t contrast;
  int32_t hueDegree;
  uint32_t mirrorFlip;
  uint32_t zoom;
  boolean_t nightMode;
  uint16_t with;
  uint16_t height;
  uint32_t clockValue;
  uint32_t address;
} SensorStatusCamera;

typedef struct SensorStatusPresence_t
{
  /**
    * Specifies the nominal data rate.
    */
  float data_frequency;

  /**
    * Specifies the effective data rate.
    */
  float measured_data_frequency;

  /**
    * Specifies the transmittance of the lens.
    */
  float Transmittance;

  /**
    * Specifies the average T object.
    */
  uint16_t average_tobject;

  /**
    * Specifies the average T ambient.
    */
  uint16_t average_tambient;

  /**
    * Specifies the presence threshold.
    */
  uint16_t presence_threshold;

  /**
    * Specifies the motion threshold.
    */
  uint16_t motion_threshold;

  /**
    * Specifies the T ambient threshold.
    */
  uint16_t tambient_shock_threshold;

  /**
    * Specifies the LPF_P_M Bandwidth.
    */
  uint16_t lpf_p_m_bandwidth;

  /**
    * Specifies the LPF_P Bandwidth.
    */
  uint16_t lpf_p_bandwidth;

  /**
    * Specifies the LPF_M Bandwidth.
    */
  uint16_t lpf_m_bandwidth;

  /**
    * Specifies the presence hysteresis.
    */
  uint8_t presence_hysteresis;

  /**
    * Specifies the motion hysteresis.
    */
  uint8_t motion_hysteresis;

  /**
    * Specifies the T ambient hysteresis.
    */
  uint8_t tambient_shock_hysteresis;

  /**
    * Specifies whether embedded compensation is active or not.
    */
  uint8_t embedded_compensation;

  /**
    * Specifies whether software compensation is active or not..
    */
  uint8_t software_compensation;

  /**
    * Specifies software compensation algorithm parameters.
    */
  CompensationAlgorithmConfig_t AlgorithmConfig;

} SensorStatusPresence;


typedef struct SensorStatusLight_t
{
  /**
    * Specifies the nominal data rate [ms].
    */
  uint32_t intermeasurement_time;

  /**
    * Specifies the exposure time [us].
    */
  uint32_t exposure_time;

  /**
    * Specifies the gain.
    */
  float gain[ ALS_NUM_CHANNELS ];

  /**
    * Specifies the effective data rate [s].
    */
  float measured_intermeasurement_time;
} SensorStatusLight;

typedef struct SensorStatusPowerMeter_t
{
  /**
    * Specifies the ADC conversion time [us].
    */
  uint32_t adc_conversion_time;

  /**
    * Specifies the Rshunt resistor value [uohm].
    */
  uint32_t r_shunt;
} SensorStatusPowerMeter;

typedef struct _SensorStatus_t
{
  /**
    * Specifies the ISensor Class (ISENSOR_CLASS_MEMS, ISENSOR_CLASS_AUDIO, ...)
    */
  uint8_t isensor_class;

  /**
    * Specifies the sensor name.
    */
  char p_name[SM_MAX_DIM_LABELS];

  /**
    * Specifies if is active or not.
    */
  boolean_t is_active;

  union Type_t
  {
    SensorStatusMems mems;
    SensorStatusAudio audio;
    SensorStatusRanging ranging;
    SensorStatusCamera camera;
    SensorStatusPresence presence;
    SensorStatusLight light;
    SensorStatusPowerMeter power_meter;
  } type;

} SensorStatus_t;

/* Public API declaration */
/**************************/

/* Inline functions definition */
/*******************************/

#ifdef __cplusplus
}
#endif

#endif /* SENSORDEF_H_ */
