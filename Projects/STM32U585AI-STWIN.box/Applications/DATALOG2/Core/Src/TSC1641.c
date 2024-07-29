/**
  ******************************************************************************
  * File Name          : TSC1641.h
  * Description        : This file provides code for the construction of prototypes and basic functions
  *                         of the TSC1641.
  * Version           : Alpha
  * Revision            : Under development
  ******************************************************************************
  */
#include "main.h"
#include "TSC1641.h"


/**
  * @brief  Read generic device register
  *
  * @param  ctx   read / write interface definitions(ptr)
  * @param  reg   register to read
  * @param  data  pointer to buffer that store the data read(ptr)
  * @param  len   number of consecutive register to read
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t TSC1641_read_reg(stmdev_ctx_t *ctx, uint8_t reg,
                         uint8_t *data,
                         uint16_t len)
{
  int32_t ret;

  ret = ctx->read_reg(ctx->handle, reg, data, len);

  return ret;
}

/**
  * @brief  Write generic device register
  *
  * @param  ctx   read / write interface definitions(ptr)
  * @param  reg   register to write
  * @param  data  pointer to data to write in register reg(ptr)
  * @param  len   number of consecutive register to write
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t TSC1641_write_reg(stmdev_ctx_t *ctx, uint8_t reg,
                          uint8_t *data,
                          uint16_t len)
{
  int32_t ret;

  ret = ctx->write_reg(ctx->handle, reg, data, len);

  return ret;
}

void TSC1641_SetConf(stmdev_ctx_t *ctx, Configuration *CONF1)
{
  uint8_t buffer[2];
  buffer[0] = ((CONF1->TSC1641_RESET << 7) + CONF1->TSC1641_AVG);
  buffer[1] = ((CONF1->TSC1641_CT) << 4) + ((CONF1->TSC1641_TEMP) << 3) + CONF1->TSC1641_MODE;
  HAL_StatusTypeDef ret = (HAL_StatusTypeDef)TSC1641_write_reg(ctx, TSC1641_RegAdd_Conf, buffer, 2);

  if (ret != HAL_OK)
  {
    while (1);
  }

}

void TSC1641_SetRShunt(stmdev_ctx_t *ctx, uint16_t r_shunt)
{
  uint8_t buffer[2];
  uint16_t TSC1641_RShunt_Val = r_shunt / 10;
  buffer[0] = (TSC1641_RShunt_Val >> 8);
  buffer[1] = ((uint8_t) TSC1641_RShunt_Val & 0xFF);
  TSC1641_write_reg(ctx, TSC1641_RegAdd_RShunt, buffer, 2);
}

void TSC1641_SetAlert(stmdev_ctx_t *ctx, Alert *ALERT1)
{
  uint8_t buffer[2];
  buffer[0] = ((ALERT1->TSC1641_SOL << 7) + (ALERT1->TSC1641_SUL << 6) + (ALERT1->TSC1641_LOL << 5) + (ALERT1->TSC1641_LUL << 4) + (ALERT1->TSC1641_POL << 3)
               + (ALERT1->TSC1641_TOL << 2) + (ALERT1->TSC1641_CNVR << 1));
  buffer[1] = ((ALERT1->TSC1641_APOL << 1)) + ALERT1->TSC1641_ALEN;

  TSC1641_write_reg(ctx, TSC1641_RegAdd_MaskAl, buffer, 2);
}

void TSC1641_SetLimit(stmdev_ctx_t *ctx, Limit *LIMIT)
{
  uint8_t buffer[2];

  // Write SHUNT OV limit
  buffer[0] = (LIMIT->VSHUNT_OV_LIM) >> 8;
  buffer[1] = LIMIT->VSHUNT_OV_LIM;
  TSC1641_write_reg(ctx, TSC1641_RegAdd_VshuntOV, buffer, 2);

  // Write SHUNT UV limit
  buffer[0] = (LIMIT->VSHUNT_UV_LIM) >> 8;
  buffer[1] = LIMIT->VSHUNT_UV_LIM;
  TSC1641_write_reg(ctx, TSC1641_RegAdd_VshuntUV, buffer, 2);

  // Write LOAD OV limit
  buffer[0] = (LIMIT->VLOAD_OV_LIM) >> 8;
  buffer[1] = LIMIT->VLOAD_OV_LIM;
  TSC1641_write_reg(ctx, TSC1641_RegAdd_VloadOV, buffer, 2);

  // Write LOAD UV limit
  buffer[0] = (LIMIT->VLOAD_OV_LIM) >> 8;
  buffer[1] = LIMIT->VLOAD_OV_LIM;
  TSC1641_write_reg(ctx, TSC1641_RegAdd_VloadUV, buffer, 2);

  // Write POWER OverLoad limit
  buffer[0] = (LIMIT->POWER_OV_LIM) >> 8;
  buffer[1] = LIMIT->POWER_OV_LIM;
  TSC1641_write_reg(ctx, TSC1641_RegAdd_PowerOL, buffer, 2);

  // Write TEMP OverLoad limit
  buffer[0] = (LIMIT->TEMP_OV_LIM) >> 8;
  buffer[1] = LIMIT->TEMP_OV_LIM;
  TSC1641_write_reg(ctx, TSC1641_RegAdd_TempOL, buffer, 2);

}

void TSC1641_GetAlert(stmdev_ctx_t *ctx, Flag *FLAG1)
{
  uint8_t buffer[2];

  TSC1641_read_reg(ctx, TSC1641_RegAdd_Alert, buffer, 2);

  FLAG1->TSC1641_NVMF = (buffer[0] & 0x80) >> 7;
  FLAG1->TSC1641_OVF = (buffer[0] & 0x40) >> 6;
  FLAG1->TSC1641_SATF = (buffer[0] & 0x20) >> 5;
  FLAG1->TSC1641_SOF = (buffer[1] & 0x40) >> 6;
  FLAG1->TSC1641_SUF = (buffer[1] & 0x20) >> 5;
  FLAG1->TSC1641_LOF = (buffer[1] & 0x10) >> 4;
  FLAG1->TSC1641_LUF = (buffer[1] & 0x08) >> 3;
  FLAG1->TSC1641_POF = (buffer[1] & 0x04) >> 2;
  FLAG1->TSC1641_TOF = (buffer[1] & 0x02) >> 1;
  FLAG1->TSC1641_CVRF = (buffer[1] & 0x01);
}

void TSC1641_GetShuntVal(stmdev_ctx_t *ctx, uint8_t Data[])
{
//
//  uint8_t Datasend[1] =
//  {
//      TSC1641_RegAdd_ShuntV };
//  TSC1641_write_reg(ctx, I2C_TSC1641_ADD_W, &Datasend[0], 1);
//  HAL_I2C_Master_Receive(hi2c, I2C_TSC1641_ADD_R, &Data[0], 2, 1000);
//
}

//
