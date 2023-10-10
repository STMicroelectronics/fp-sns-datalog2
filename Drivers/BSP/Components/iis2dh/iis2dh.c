/**
******************************************************************************
* @file    iis2dh.c
* @author  SRA
* @brief   IIS2DH driver file
******************************************************************************
* @attention
*
* <h2><center>&copy; Copyright (c) 2021 STMicroelectronics. 
* All rights reserved.</center></h2>
*
* This software component is licensed by ST under BSD 3-Clause license,
* the "License"; You may not use this file except in compliance with the 
* License. You may obtain a copy of the License at:
*                        opensource.org/licenses/BSD-3-Clause
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "iis2dh.h"

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup Component Component
 * @{
 */

/** @defgroup IIS2DH IIS2DH
 * @{
 */

/** @defgroup IIS2DH_Exported_Variables IIS2DH Exported Variables
 * @{
 */

IIS2DH_CommonDrv_t IIS2DH_COMMON_Driver =
{
  IIS2DH_Init,
  IIS2DH_DeInit,
  IIS2DH_ReadID,
  IIS2DH_GetCapabilities,
};

IIS2DH_ACC_Drv_t IIS2DH_ACC_Driver =
{
  IIS2DH_ACC_Enable,
  IIS2DH_ACC_Disable,
  IIS2DH_ACC_GetSensitivity,
  IIS2DH_ACC_GetOutputDataRate,
  IIS2DH_ACC_SetOutputDataRate,
  IIS2DH_ACC_GetFullScale,
  IIS2DH_ACC_SetFullScale,
  IIS2DH_ACC_GetAxes,
  IIS2DH_ACC_GetAxesRaw,
};

/**
 * @}
 */

/** @defgroup IIS2DH_Private_Function_Prototypes IIS2DH Private Function Prototypes
 * @{
 */

static int32_t ReadRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint16_t Length);
static int32_t WriteRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint16_t Length);
static int32_t IIS2DH_ACC_SetOutputDataRate_When_Enabled(IIS2DH_Object_t *pObj, float Odr);
static int32_t IIS2DH_ACC_SetOutputDataRate_When_Disabled(IIS2DH_Object_t *pObj, float Odr);

/**
 * @}
 */

/** @defgroup IIS2DH_Exported_Functions IIS2DH Exported Functions
 * @{
 */

/**
 * @brief  Register Component Bus IO operations
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_RegisterBusIO(IIS2DH_Object_t *pObj, IIS2DH_IO_t *pIO)
{
  int32_t ret;

  if (pObj == NULL)
  {
    ret = IIS2DH_ERROR;
  }
  else
  {
    pObj->IO.Init      = pIO->Init;
    pObj->IO.DeInit    = pIO->DeInit;
    pObj->IO.BusType   = pIO->BusType;
    pObj->IO.Address   = pIO->Address;
    pObj->IO.WriteReg  = pIO->WriteReg;
    pObj->IO.ReadReg   = pIO->ReadReg;
    pObj->IO.GetTick   = pIO->GetTick;

    pObj->Ctx.read_reg  = ReadRegWrap;
    pObj->Ctx.write_reg = WriteRegWrap;
    pObj->Ctx.handle   = pObj;

    if (pObj->IO.Init != NULL)
    {
      ret = pObj->IO.Init();
    }
    else
    {
      ret = IIS2DH_ERROR;
    }
  }

  return ret;
}

/**
 * @brief  Initialize the IIS2DH sensor
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_Init(IIS2DH_Object_t *pObj)
{
  /* Set SPI mode*/
  if (iis2dh_spi_mode_set(&(pObj->Ctx), IIS2DH_SPI_4_WIRE) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }
  
  /* Reboot memory content */
  if (iis2dh_boot_set(&(pObj->Ctx), PROPERTY_ENABLE) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }  
  
  /* Switch off all axes */
  iis2dh_ctrl_reg1_t ctrl_reg1;
  int32_t ret;

  ret = iis2dh_read_reg(&(pObj->Ctx), IIS2DH_CTRL_REG1, (uint8_t*)&ctrl_reg1, 1);
  if (ret == 0) {
    ctrl_reg1.xen = 0;
    ctrl_reg1.yen = 0;
    ctrl_reg1.zen = 0;
    (void)iis2dh_write_reg(&(pObj->Ctx), IIS2DH_CTRL_REG1, (uint8_t*)&ctrl_reg1, 1);
  }
  
  /* Output data rate selection - power down. */
  if (iis2dh_data_rate_set(&(pObj->Ctx), IIS2DH_POWER_DOWN) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }
  
  /* Switch off temperature */  
  if (iis2dh_temperature_meas_set(&(pObj->Ctx), IIS2DH_TEMP_DISABLE) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }
  
  /* High-pass filter disabled */
  if (iis2dh_high_pass_on_outputs_set(&(pObj->Ctx), 0) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }  
  
  /* Enable BDU */
  if (iis2dh_block_data_update_set(&(pObj->Ctx), PROPERTY_ENABLE) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }
  
  /* Big/Little Endian data selection configuration */
  if (iis2dh_data_format_set(&(pObj->Ctx), IIS2DH_LSB_AT_LOW_ADD) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }
  
  /* Full scale selection. */
  if (iis2dh_full_scale_set(&(pObj->Ctx), IIS2DH_2g) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  /* Power mode selection */
  if (iis2dh_operating_mode_set(&(pObj->Ctx), IIS2DH_LP_8bit) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }
    
  /* Set interrupt structure for DRDY */
/***************************************/
  iis2dh_ctrl_reg3_t ctrl_reg3;
  iis2dh_ctrl_reg5_t ctrl_reg5;

  ret = iis2dh_read_reg(&(pObj->Ctx), IIS2DH_CTRL_REG3, (uint8_t*)&ctrl_reg3, 1);
  if (ret == 0) {
    ctrl_reg3.i1_drdy1 = 1;
    (void)iis2dh_write_reg(&(pObj->Ctx), IIS2DH_CTRL_REG3, (uint8_t*)&ctrl_reg3, 1);
  }  
  
  ret = iis2dh_read_reg(&(pObj->Ctx), IIS2DH_CTRL_REG5, (uint8_t*)&ctrl_reg5, 1);
  if (ret == 0) {
    ctrl_reg5.lir_int1 = 1;
    (void)iis2dh_write_reg(&(pObj->Ctx), IIS2DH_CTRL_REG5, (uint8_t*)&ctrl_reg5, 1);
  }
/****************************************/
  
  pObj->is_initialized = 1;
  
  return IIS2DH_OK;
}

/**
 * @brief  Deinitialize the IIS2DH sensor
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_DeInit(IIS2DH_Object_t *pObj)
{
  /* Disable the component */
  if (IIS2DH_ACC_Disable(pObj) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  /* Reset output data rate. */
  pObj->acc_odr = IIS2DH_ODR_1Hz;

  pObj->is_initialized = 0;

  return IIS2DH_OK;
}

/**
 * @brief  Read component ID
 * @param  pObj the device pObj
 * @param  Id the WHO_AM_I value
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_ReadID(IIS2DH_Object_t *pObj, uint8_t *Id)
{
  if (iis2dh_device_id_get(&(pObj->Ctx), Id) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  return IIS2DH_OK;
}

/**
 * @brief  Get IIS2DH sensor capabilities
 * @param  pObj Component object pointer
 * @param  Capabilities pointer to IIS2DH sensor capabilities
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_GetCapabilities(IIS2DH_Object_t *pObj, IIS2DH_Capabilities_t *Capabilities)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);

  Capabilities->Acc          = 1;
  Capabilities->Gyro         = 0;
  Capabilities->Magneto      = 0;
  Capabilities->LowPower     = 1;
  Capabilities->GyroMaxFS    = 0;
  Capabilities->AccMaxFS     = 16;
  Capabilities->MagMaxFS     = 0;
  Capabilities->GyroMaxOdr   = 0.0f;
  Capabilities->AccMaxOdr    = 5376.0f;
  Capabilities->MagMaxOdr    = 0.0f;
  return IIS2DH_OK;
}

/**
 * @brief  Enable the IIS2DH accelerometer sensor
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_ACC_Enable(IIS2DH_Object_t *pObj)
{
  /* Check if the component is already enabled */
  if (pObj->acc_is_enabled == 1U)
  {
    return IIS2DH_OK;
  }

  /* Output data rate selection. */
  (void)IIS2DH_ACC_SetOutputDataRate_When_Disabled(pObj, 5300.0f);
  
    /* Switch on all axes */
  iis2dh_ctrl_reg1_t ctrl_reg1;
  int32_t ret;

  ret = iis2dh_read_reg(&(pObj->Ctx), IIS2DH_CTRL_REG1, (uint8_t*)&ctrl_reg1, 1);
  if (ret == 0) {
    ctrl_reg1.xen = 1;
    ctrl_reg1.yen = 1;
    ctrl_reg1.zen = 1;
    (void)iis2dh_write_reg(&(pObj->Ctx), IIS2DH_CTRL_REG1, (uint8_t*)&ctrl_reg1, 1);
  }

  pObj->acc_is_enabled = 1;

  return IIS2DH_OK;
}

/**
 * @brief  Disable the IIS2DH accelerometer sensor
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_ACC_Disable(IIS2DH_Object_t *pObj)
{
  /* Check if the component is already disabled */
  if (pObj->acc_is_enabled == 0U)
  {
    return IIS2DH_OK;
  }

  /* Output data rate selection - power down. */
  if (iis2dh_data_rate_set(&(pObj->Ctx), IIS2DH_POWER_DOWN) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  pObj->acc_is_enabled = 0;

  return IIS2DH_OK;
}

/**
* @brief  Get the IIS2DH accelerometer sensor sensitivity
* @param  pObj the device pObj
* @param  Sensitivity pointer
* @retval 0 in case of success, an error code otherwise
*/
int32_t IIS2DH_ACC_GetSensitivity(IIS2DH_Object_t *pObj, float *Sensitivity)
{
  int32_t ret = IIS2DH_OK;
  iis2dh_fs_t full_scale;
  iis2dh_op_md_t mode;
  
  /* Read actual full scale selection from sensor. */
  if (iis2dh_full_scale_get(&(pObj->Ctx), &full_scale) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }
  
  /* Read actual power mode selection from sensor. */
  if (iis2dh_operating_mode_get(&(pObj->Ctx), &mode) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }
  
  switch(mode)
  {
  case IIS2DH_NM_10bit:
    switch (full_scale)
    {
    case IIS2DH_2g:
      *Sensitivity = IIS2DH_ACC_SENSITIVITY_2G_NM;
      break;
      
    case IIS2DH_4g:
      *Sensitivity = IIS2DH_ACC_SENSITIVITY_4G_NM;
      break;
      
    case IIS2DH_8g:
      *Sensitivity = IIS2DH_ACC_SENSITIVITY_8G_NM;
      break;
      
    case IIS2DH_16g:
      *Sensitivity = IIS2DH_ACC_SENSITIVITY_16G_NM;
      break;
      
    default:
      *Sensitivity = -1.0f;
      ret = IIS2DH_ERROR;
      break;
    }
    break;
    
  case IIS2DH_HR_12bit:
    switch (full_scale)
    {
    case IIS2DH_2g:
      *Sensitivity = IIS2DH_ACC_SENSITIVITY_2G_HRM;
      break;
      
    case IIS2DH_4g:
      *Sensitivity = IIS2DH_ACC_SENSITIVITY_4G_HRM;
      break;
      
    case IIS2DH_8g:
      *Sensitivity = IIS2DH_ACC_SENSITIVITY_8G_HRM;
      break;
      
    case IIS2DH_16g:
      *Sensitivity = IIS2DH_ACC_SENSITIVITY_16G_HRM;
      break;
      
    default:
      *Sensitivity = -1.0f;
      ret = IIS2DH_ERROR;
      break;
    }
    break;   
    
  case IIS2DH_LP_8bit:
    switch (full_scale)
    {
    case IIS2DH_2g:
      *Sensitivity = IIS2DH_ACC_SENSITIVITY_2G_LPM;
      break;
      
    case IIS2DH_4g:
      *Sensitivity = IIS2DH_ACC_SENSITIVITY_4G_LPM;
      break;
      
    case IIS2DH_8g:
      *Sensitivity = IIS2DH_ACC_SENSITIVITY_8G_LPM;
      break;
      
    case IIS2DH_16g:
      *Sensitivity = IIS2DH_ACC_SENSITIVITY_16G_LPM;
      break;
      
    default:
      *Sensitivity = -1.0f;
      ret = IIS2DH_ERROR;
      break;
    }
    break;  
    
  default:
    *Sensitivity = -1.0f;
    ret = IIS2DH_ERROR;
    break;
  }
  
  return ret;
}

/**
 * @brief  Get the IIS2DH accelerometer sensor output data rate
 * @param  pObj the device pObj
 * @param  Odr pointer where the output data rate is written
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_ACC_GetOutputDataRate(IIS2DH_Object_t *pObj, float *odr)
{
  int32_t ret = IIS2DH_OK;
  iis2dh_odr_t odr_low_level;

  /* Get current output data rate. */
  if (iis2dh_data_rate_get(&(pObj->Ctx), &odr_low_level) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }
  
  switch (odr_low_level)
  {
  case IIS2DH_POWER_DOWN:
    *odr =  0.0f;
    break;
  case IIS2DH_ODR_1Hz:
    *odr =  1.0f;
    break;
  case IIS2DH_ODR_10Hz:
    *odr =  10.0f;
    break;
  case IIS2DH_ODR_25Hz:
    *odr = 25.0f;
    break;
  case IIS2DH_ODR_50Hz:
    *odr = 50.0f;
    break;
  case IIS2DH_ODR_100Hz:
    *odr = 100.0f;
    break;
  case IIS2DH_ODR_200Hz:
    *odr = 200.0f;
    break;
  case IIS2DH_ODR_400Hz:
    *odr = 400.0f;
    break;
  case IIS2DH_ODR_1kHz620_LP:
    *odr = 1620.0f;
    break;
  case IIS2DH_ODR_5kHz376_LP_1kHz344_NM_HP:
    *odr = 1344.0f;
    break;
  default:
    *odr = -1.0f;
    ret = IIS2DH_ERROR;
    break;
  }

  return ret;
}

/**
 * @brief  Set the IIS2DH accelerometer sensor output data rate
 * @param  pObj the device pObj
 * @param  Odr the output data rate value to be set
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_ACC_SetOutputDataRate(IIS2DH_Object_t *pObj, float Odr)
{
  /* Check if the component is enabled */
  if (pObj->acc_is_enabled == 1U)
  {
    return IIS2DH_ACC_SetOutputDataRate_When_Enabled(pObj, Odr);
  }
  else
  {
    return IIS2DH_ACC_SetOutputDataRate_When_Disabled(pObj, Odr);
  }
}

/**
 * @brief  Get the IIS2DH accelerometer sensor full scale
 * @param  pObj the device pObj
 * @param  FullScale pointer where the full scale is written
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_ACC_GetFullScale(IIS2DH_Object_t *pObj, int32_t *FullScale)
{
  int32_t ret = IIS2DH_OK;
  iis2dh_fs_t fs_low_level;

  /* Read actual full scale selection from sensor. */
  if (iis2dh_full_scale_get(&(pObj->Ctx), &fs_low_level) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  switch (fs_low_level)
  {
    case IIS2DH_2g:
      *FullScale =  2;
      break;

    case IIS2DH_4g:
      *FullScale =  4;
      break;

    case IIS2DH_8g:
      *FullScale =  8;
      break;

    case IIS2DH_16g:
      *FullScale = 16;
      break;

    default:
      *FullScale = -1;
      ret = IIS2DH_ERROR;
      break;
  }

  return ret;
}

/**
 * @brief  Set the IIS2DH accelerometer sensor full scale
 * @param  pObj the device pObj
 * @param  FullScale the functional full scale to be set
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_ACC_SetFullScale(IIS2DH_Object_t *pObj, int32_t FullScale)
{
  iis2dh_fs_t new_fs;

  /* Seems like MISRA C-2012 rule 14.3a violation but only from single file statical analysis point of view because
     the parameter passed to the function is not known at the moment of analysis */
  new_fs = (FullScale <= 2) ? IIS2DH_2g
         : (FullScale <= 4) ? IIS2DH_4g
         : (FullScale <= 8) ? IIS2DH_8g
         :                    IIS2DH_16g;

  if (iis2dh_full_scale_set(&(pObj->Ctx), new_fs) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  return IIS2DH_OK;
}

/**
 * @brief  Get the IIS2DH accelerometer sensor raw axes
 * @param  pObj the device pObj
 * @param  Value pointer where the raw values of the axes are written
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_ACC_GetAxesRaw(IIS2DH_Object_t *pObj, IIS2DH_AxesRaw_t *Value)
{
  int16_t divisor = 1;
  iis2dh_axis3bit16_t data_raw;
  int32_t ret = IIS2DH_OK;
  iis2dh_op_md_t op_mode;

  /* Read operative mode from sensor. */
  if (iis2dh_operating_mode_get(&(pObj->Ctx), &op_mode) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  /* Store the sensitivity based on actual full scale. */
  switch (op_mode)
  {
    case IIS2DH_HR_12bit:
      divisor = 16;
      break;

    case IIS2DH_NM_10bit:
      divisor = 64;
      break;

    case IIS2DH_LP_8bit:
      divisor = 256;
      break;

    default:
      ret = IIS2DH_ERROR;
      break;
  }

  if (ret == IIS2DH_ERROR)
  {
    return ret;
  }

  /* Read raw data values. */
  if (iis2dh_acceleration_raw_get(&(pObj->Ctx), data_raw.i16bit) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  /* Format the data. */
  Value->x = (data_raw.i16bit[0] / divisor);
  Value->y = (data_raw.i16bit[1] / divisor);
  Value->z = (data_raw.i16bit[2] / divisor);

  return ret;
}

/**
 * @brief  Get the IIS2DH accelerometer sensor axes
 * @param  pObj the device pObj
 * @param  Acceleration pointer where the values of the axes are written
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_ACC_GetAxes(IIS2DH_Object_t *pObj, IIS2DH_Axes_t *Acceleration)
{
  IIS2DH_AxesRaw_t data_raw;
  float sensitivity = 0.0f;

  /* Read raw data values. */
  if (IIS2DH_ACC_GetAxesRaw(pObj, &data_raw) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  /* Get IIS2DH actual sensitivity. */
  if (IIS2DH_ACC_GetSensitivity(pObj, &sensitivity) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  /* Calculate the data. */
  Acceleration->x = (int32_t)((float)((float)data_raw.x * sensitivity));
  Acceleration->y = (int32_t)((float)((float)data_raw.y * sensitivity));
  Acceleration->z = (int32_t)((float)((float)data_raw.z * sensitivity));

  return IIS2DH_OK;
}

/**
 * @brief  Get the IIS2DH register value
 * @param  pObj the device pObj
 * @param  Reg address to be read
 * @param  Data pointer where the value is written
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_Read_Reg(IIS2DH_Object_t *pObj, uint8_t Reg, uint8_t *Data)
{
  if (iis2dh_read_reg(&(pObj->Ctx), Reg, Data, 1) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  return IIS2DH_OK;
}

/**
 * @brief  Set the IIS2DH register value
 * @param  pObj the device pObj
 * @param  Reg address to be written
 * @param  Data value to be written
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_Write_Reg(IIS2DH_Object_t *pObj, uint8_t Reg, uint8_t Data)
{
  if (iis2dh_write_reg(&(pObj->Ctx), Reg, &Data, 1) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  return IIS2DH_OK;
}


/**
 * @brief  Get the IIS2DH ACC data ready bit value
 * @param  pObj the device pObj
 * @param  Status the status of data ready bit
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_ACC_Get_DRDY_Status(IIS2DH_Object_t *pObj, uint8_t *Status)
{
  if (iis2dh_xl_data_ready_get(&(pObj->Ctx), Status) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  return IIS2DH_OK;
}

/**
 * @brief  Get the IIS2DH ACC initialization status
 * @param  pObj the device pObj
 * @param  Status 1 if initialized, 0 otherwise
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_ACC_Get_Init_Status(IIS2DH_Object_t *pObj, uint8_t *Status)
{
  if (pObj == NULL)
  {
    return IIS2DH_ERROR;
  }

  *Status = pObj->is_initialized;

  return IIS2DH_OK;
}

/**
 * @brief  Get the number of samples contained into the FIFO
 * @param  pObj the device pObj
 * @param  NumSamples the number of samples contained into the FIFO
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_FIFO_Get_Num_Samples(IIS2DH_Object_t *pObj, uint16_t *NumSamples)
{
  if (iis2dh_fifo_data_level_get(&(pObj->Ctx), (uint8_t *)NumSamples) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  return IIS2DH_OK;
}

/**
 * @brief  Set the FIFO mode
 * @param  pObj the device pObj
 * @param  Mode FIFO mode
 * @retval 0 in case of success, an error code otherwise
 */
int32_t IIS2DH_FIFO_Set_Mode(IIS2DH_Object_t *pObj, uint8_t Mode)
{
  int32_t ret = IIS2DH_OK;

  /* Verify that the passed parameter contains one of the valid values. */
  switch ((iis2dh_fm_t)Mode)
  {
    case IIS2DH_BYPASS_MODE:
    case IIS2DH_FIFO_MODE:
    case IIS2DH_STREAM_TO_FIFO_MODE:
    case IIS2DH_DYNAMIC_STREAM_MODE:
      break;

    default:
      ret = IIS2DH_ERROR;
      break;
  }

  if (ret == IIS2DH_ERROR)
  {
    return ret;
  }

  if (iis2dh_fifo_mode_set(&(pObj->Ctx), (iis2dh_fm_t)Mode) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  return ret;
}


/**
 * @}
 */

/** @defgroup IIS2DH_Private_Functions IIS2DH Private Functions
 * @{
 */

/**
 * @brief  Set the IIS2DH accelerometer sensor output data rate when enabled
 * @param  pObj the device pObj
 * @param  Odr the functional output data rate to be set
 * @retval 0 in case of success, an error code otherwise
 */
static int32_t IIS2DH_ACC_SetOutputDataRate_When_Enabled(IIS2DH_Object_t *pObj, float Odr)
{
  iis2dh_odr_t new_odr;

  new_odr = (Odr <=    1.0f) ? IIS2DH_ODR_1Hz                                  
          : (Odr <=   10.0f) ? IIS2DH_ODR_10Hz                    
          : (Odr <=   25.0f) ? IIS2DH_ODR_25Hz                    
          : (Odr <=   50.0f) ? IIS2DH_ODR_50Hz                    
          : (Odr <=  100.0f) ? IIS2DH_ODR_100Hz                   
          : (Odr <=  200.0f) ? IIS2DH_ODR_200Hz                   
          : (Odr <=  400.0f) ? IIS2DH_ODR_400Hz                   
          : (Odr <=  1620.0f) ? IIS2DH_ODR_1kHz620_LP              
          :                     IIS2DH_ODR_5kHz376_LP_1kHz344_NM_HP;
                               
  /* Output data rate selection. */
  if (iis2dh_data_rate_set(&(pObj->Ctx), new_odr) != IIS2DH_OK)
  {
    return IIS2DH_ERROR;
  }

  if (Odr <= 1.6f)
  {
    /* Set low-power mode for 1.6 Hz ODR */
    if (iis2dh_operating_mode_set(&(pObj->Ctx), IIS2DH_LP_8bit) != IIS2DH_OK)
    {
      return IIS2DH_ERROR;
    }
  }

  if (Odr > 200.0f)
  {
    /* Set high-performance mode for ODR higher then 200 Hz */
    if (iis2dh_operating_mode_set(&(pObj->Ctx), IIS2DH_HR_12bit) != IIS2DH_OK)
    {
      return IIS2DH_ERROR;
    }
  }

  return IIS2DH_OK;
}

/**
 * @brief  Set the IIS2DH accelerometer sensor output data rate when disabled
 * @param  pObj the device pObj
 * @param  Odr the functional output data rate to be set
 * @retval 0 in case of success, an error code otherwise
 */
static int32_t IIS2DH_ACC_SetOutputDataRate_When_Disabled(IIS2DH_Object_t *pObj, float Odr)
{
  pObj->acc_odr = (Odr <=    1.0f) ? IIS2DH_ODR_1Hz                                  
                : (Odr <=   10.0f) ? IIS2DH_ODR_10Hz                    
                : (Odr <=   25.0f) ? IIS2DH_ODR_25Hz                    
                : (Odr <=   50.0f) ? IIS2DH_ODR_50Hz                    
                : (Odr <=  100.0f) ? IIS2DH_ODR_100Hz                   
                : (Odr <=  200.0f) ? IIS2DH_ODR_200Hz                   
                : (Odr <=  400.0f) ? IIS2DH_ODR_400Hz                   
                : (Odr <=  1620.0f) ? IIS2DH_ODR_1kHz620_LP              
                :                     IIS2DH_ODR_5kHz376_LP_1kHz344_NM_HP;

  return IIS2DH_OK;
}

/**
 * @brief  Wrap Read register component function to Bus IO function
 * @param  Handle the device handler
 * @param  Reg the register address
 * @param  pData the stored data pointer
 * @param  Length the length
 * @retval 0 in case of success, an error code otherwise
 */
static int32_t ReadRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint16_t Length)
{
  IIS2DH_Object_t *pObj = (IIS2DH_Object_t *)Handle;

  return pObj->IO.ReadReg(pObj->IO.Address, Reg, pData, Length);
}

/**
 * @brief  Wrap Write register component function to Bus IO function
 * @param  Handle the device handler
 * @param  Reg the register address
 * @param  pData the stored data pointer
 * @param  Length the length
 * @retval 0 in case of success, an error code otherwise
 */
static int32_t WriteRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint16_t Length)
{
  IIS2DH_Object_t *pObj = (IIS2DH_Object_t *)Handle;

  return pObj->IO.WriteReg(pObj->IO.Address, Reg, pData, Length);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
