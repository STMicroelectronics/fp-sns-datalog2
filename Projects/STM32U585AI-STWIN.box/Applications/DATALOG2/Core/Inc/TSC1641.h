/**
  ******************************************************************************
  * File Name          : TSC1641.h
  * Description        : This file provides code for the configuration
  *                     register and headers for the TSC1641.
  * Version            : Alpha
  * Revision           : Under development
  ******************************************************************************
  */
#include "stdint.h"


/** @defgroup STMicroelectronics sensors common types
  * @{
  *
  */

#ifndef MEMS_SHARED_TYPES
#define MEMS_SHARED_TYPES

typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t bit0       : 1;
  uint8_t bit1       : 1;
  uint8_t bit2       : 1;
  uint8_t bit3       : 1;
  uint8_t bit4       : 1;
  uint8_t bit5       : 1;
  uint8_t bit6       : 1;
  uint8_t bit7       : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t bit7       : 1;
  uint8_t bit6       : 1;
  uint8_t bit5       : 1;
  uint8_t bit4       : 1;
  uint8_t bit3       : 1;
  uint8_t bit2       : 1;
  uint8_t bit1       : 1;
  uint8_t bit0       : 1;
#endif /* DRV_BYTE_ORDER */
} bitwise_t;

#define PROPERTY_DISABLE                (0U)
#define PROPERTY_ENABLE                 (1U)

/** @addtogroup  Interfaces_Functions
  * @brief       This section provide a set of functions used to read and
  *              write a generic register of the device.
  *              MANDATORY: return 0 -> no Error.
  * @{
  *
  */

typedef int32_t (*stmdev_write_ptr)(void *, uint8_t, uint8_t *, uint16_t);
typedef int32_t (*stmdev_read_ptr)(void *, uint8_t, uint8_t *, uint16_t);
typedef void (*stmdev_mdelay_ptr)(uint32_t millisec);

typedef struct
{
  /** Component mandatory fields **/
  stmdev_write_ptr  write_reg;
  stmdev_read_ptr   read_reg;
  /** Component optional fields **/
  stmdev_mdelay_ptr   mdelay;
  /** Customizable optional pointer **/
  void *handle;
} stmdev_ctx_t;

/**
  * @}
  *
  */

#endif /* MEMS_SHARED_TYPES */


#define I2C_TSC1641_ADD_R           0x81 //A0 GND - A1 GND
#define I2C_TSC1641_ADD_W           0x80 //A0 GND - A1 GND


#define TSC1641_REG_SIZE            0x08


#define TSC1641_RegAdd_Conf         0x00                // 0x0067
#define TSC1641_RegAdd_ShuntV       0x01
#define TSC1641_RegAdd_LoadV        0x02
#define TSC1641_RegAdd_Power        0x03
#define TSC1641_RegAdd_Current      0x04
#define TSC1641_RegAdd_Temp         0x05                // 0x8000
#define TSC1641_RegAdd_MaskAl       0x06
#define TSC1641_RegAdd_Alert        0x07                // Signal alerts
#define TSC1641_RegAdd_RShunt       0x08
#define TSC1641_RegAdd_VshuntOV     0x09
#define TSC1641_RegAdd_VshuntUV     0x0A
#define TSC1641_RegAdd_VloadOV      0x0B
#define TSC1641_RegAdd_VloadUV      0x0C
#define TSC1641_RegAdd_PowerOL      0x0D
#define TSC1641_RegAdd_TempOL       0x0E
#define TSC1641_RegAdd_ManufID      0xFE
#define TSC1641_RegAdd_DieID        0xFF


#define TSC1641_Conf_On             0x01
#define TSC1641_Conf_Off            0x00

#define TSC1641_Conf_Avg_1          0x00
#define TSC1641_Conf_Avg_4          0x01
#define TSC1641_Conf_Avg_16         0x02
#define TSC1641_Conf_Avg_64         0x03
#define TSC1641_Conf_Avg_128        0x04
#define TSC1641_Conf_Avg_256        0x05
#define TSC1641_Conf_Avg_512        0x06
#define TSC1641_Conf_Avg_1024       0x07

#define TSC1641_Conf_CT_128         0x00
#define TSC1641_Conf_CT_256         0x01
#define TSC1641_Conf_CT_512         0x02
#define TSC1641_Conf_CT_1024        0x03
#define TSC1641_Conf_CT_2048        0x04
#define TSC1641_Conf_CT_4096        0x05
#define TSC1641_Conf_CT_8192        0x06
#define TSC1641_Conf_CT_16384       0x07
#define TSC1641_Conf_CT_32768       0x08


#define TSC1641_Mode_ShutDown       0x00
#define TSC1641_Mode_Vsh_Trig       0x01
#define TSC1641_Mode_Vload_Trig     0x02
#define TSC1641_Mode_Vshload_Trig   0x03
#define TSC1641_Mode_Idle           0x04
#define TSC1641_Mode_VshCont        0x05
#define TSC1641_Mode_VloadCont      0x06
#define TSC1641_Mode_VshloadCont    0x07

// Definition of the parameters in configuration register :
typedef struct Configuration Configuration;
struct Configuration
{
  uint8_t TSC1641_RESET ;
  uint8_t TSC1641_NVM ;
  uint8_t TSC1641_AVG ;
  uint8_t TSC1641_CT ;
  uint8_t TSC1641_TEMP ;
  uint8_t TSC1641_MODE ;
} ;


// Definition of the parameters in alert register :
typedef struct Alert Alert;
struct Alert
{
  uint8_t TSC1641_SOL ;   // Shunt Voltage Over voltage
  uint8_t TSC1641_SUL ;   // Shunt Voltage Under voltage
  uint8_t TSC1641_LOL ;   // Load Voltage Over voltage
  uint8_t TSC1641_LUL ;   // Load Voltage Under voltage
  uint8_t TSC1641_POL ;   // Power Over Limit
  uint8_t TSC1641_TOL ;   // Temperature Over Limit
  uint8_t TSC1641_CNVR ;  // Conversion ready alert enable
  uint8_t TSC1641_APOL ;  // Alert polarity
  uint8_t TSC1641_ALEN ;  // Alert Latch Enable
} ;

// Definition of the parameters in alert register :
typedef struct Flag Flag;
struct Flag
{
  uint8_t TSC1641_NVMF ;  // NVM state flag
  uint8_t TSC1641_OVF ;   // Math Overflow Flag
  uint8_t TSC1641_SATF ;  // Measurement saturation Flag
  uint8_t TSC1641_SOF ;   // Shunt Voltage Over voltage
  uint8_t TSC1641_SUF ;   // Shunt Voltage Under voltage
  uint8_t TSC1641_LOF ;   // Load Voltage Over voltage
  uint8_t TSC1641_LUF ;   // Load Voltage Under voltage
  uint8_t TSC1641_POF ;   // Power Over Limit
  uint8_t TSC1641_TOF ;   // Temperature Over Limit
  uint8_t TSC1641_CVRF ;  // Conversion ready alert enable
} ;


// Definition of the parameters in alert register :
typedef struct Limit Limit;
struct Limit
{
  uint16_t VSHUNT_OV_LIM ;  // Vsunt Over voltage limit value
  uint16_t VSHUNT_UV_LIM ;    // Vshunt Under voltage limit value
  uint16_t VLOAD_OV_LIM ; // Vload Over voltage limit value
  uint16_t VLOAD_UV_LIM ;   // Vload Under voltage limit value
  uint16_t POWER_OV_LIM ;   // Power over limit value
  uint16_t TEMP_OV_LIM ;    // Temperature over limit value
} ;

// Prototype declaration
int32_t TSC1641_read_reg(stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len);
int32_t TSC1641_write_reg(stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len);
void TSC1641_SetConf(stmdev_ctx_t *ctx, Configuration *CONF1);
void TSC1641_SetAlert(stmdev_ctx_t *ctx, Alert *ALERT1);
void TSC1641_GetAlert(stmdev_ctx_t *ctx, Flag *FLAG);
void TSC1641_SetLimit(stmdev_ctx_t *ctx, Limit *LIMIT);
void TSC1641_SetRShunt(stmdev_ctx_t *ctx, uint16_t r_shunt);

