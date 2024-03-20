/**
  ******************************************************************************
  * @file    SensorTileBoxPro_nfctag.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.1.0
  * @date    20-July-2023
  * @brief   This file provides a set of functions needed to manage a nfc dual 
  *          interface eeprom memory.
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

/* Includes ------------------------------------------------------------------*/
#include "SensorTileBoxPro_nfctag.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

#ifndef NULL
#define NULL      (void *) 0
#endif


/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static NFCTAG_DrvTypeDef *Nfctag_Drv = NULL;
/* static uint8_t NfctagInitialized = 0; */
static ST25DV_Object_t ST25DVObj;
static ST25DVxxKC_Object_t ST25DVxxKCObj;

static void *NfcTagObj;

/* Global variables ----------------------------------------------------------*/
EXTI_HandleTypeDef nfc_exti = {.Line = EXTI_LINE_12};



/* Private function prototypes -----------------------------------------------*/
static uint32_t NFC_GetTick(void);
static void NFCTAG_GPIO_EXTI_Callback(void);
/* Functions Definition ------------------------------------------------------*/

int32_t BSP_NFCTAG_ST25DV_Probe (uint32_t Instance)
{
  int32_t status;
  ST25DV_IO_t IO;
  UNUSED(Instance);

  /* Configure the component */
  IO.Init         = BSP_I2C2_Init;
  IO.DeInit       = BSP_I2C2_DeInit;
  IO.IsReady      = BSP_I2C2_IsReady;
  IO.Read         = BSP_I2C2_ReadReg16;
  
  IO.Write        = (ST25DV_Write_Func) BSP_I2C2_WriteReg16;
  IO.GetTick      = NFC_GetTick;

  status = ST25DV_RegisterBusIO (&ST25DVObj, &IO);
  if(status != NFCTAG_OK)
    return NFCTAG_ERROR;

  Nfctag_Drv = (NFCTAG_DrvTypeDef *)(void *)&St25Dv_Drv;

  if(Nfctag_Drv->Init != NULL)
  {
    status = Nfctag_Drv->Init(&ST25DVObj);
    if(status != NFCTAG_OK)
    {
      Nfctag_Drv = NULL;
      return NFCTAG_ERROR;
    }
    NfcTagObj = (void *)&ST25DVObj;
  } else {
    Nfctag_Drv = NULL;
    return NFCTAG_ERROR;
  }
  return NFCTAG_OK;
}

int32_t BSP_NFCTAG_ST25DVxxKC_Probe (uint32_t Instance)
{
  int32_t status;
  ST25DVxxKC_IO_t IO;
  UNUSED(Instance);

  /* Configure the component */
  IO.Init         = BSP_I2C2_Init;
  IO.DeInit       = BSP_I2C2_DeInit;
  IO.IsReady      = BSP_I2C2_IsReady;
  IO.Read         = BSP_I2C2_ReadReg16;
  
  IO.Write        = (ST25DVxxKC_Write_Func) BSP_I2C2_WriteReg16;
  IO.GetTick      = BSP_GetTick;

  status = ST25DVxxKC_RegisterBusIO (&ST25DVxxKCObj, &IO);
  if(status != NFCTAG_OK)
    return NFCTAG_ERROR;

  Nfctag_Drv = (NFCTAG_DrvTypeDef *)(void *)&St25Dvxxkc_Drv;
  if(Nfctag_Drv->Init != NULL)
  {
    status = Nfctag_Drv->Init(&ST25DVxxKCObj);
    if(status != NFCTAG_OK)
    {
      Nfctag_Drv = NULL;
      return NFCTAG_ERROR;
    }
    NfcTagObj = (void *)&ST25DVxxKCObj;
  } else {
    Nfctag_Drv = NULL;
    return NFCTAG_ERROR;
  }
  return NFCTAG_OK;
}

int32_t BSP_NFCTAG_Init (uint32_t Instance)
{
  if(BSP_NFCTAG_ST25DVxxKC_Probe(Instance) == BSP_ERROR_NONE) 
  { 
    return NFCTAG_OK;
  }
  
   if(BSP_NFCTAG_ST25DV_Probe(Instance) == BSP_ERROR_NONE) 
  { 
    return NFCTAG_OK;
  }
  
  return NFCTAG_ERROR;
}

static uint32_t NFC_GetTick(void) {
  return (uint32_t) BSP_GetTick();
}


/**
  * @brief  Deinitializes peripherals used by the I2C NFCTAG driver
  * @param  None
  * @retval None
  */
void BSP_NFCTAG_DeInit( uint32_t Instance )
{ 
  UNUSED(Instance);

  if(Nfctag_Drv != NULL)
  {
    Nfctag_Drv = NULL;
    ST25DVObj.IsInitialized = 0U;
    ST25DVxxKCObj.IsInitialized = 0U;
  }
}

/**
  * @brief  Check if the nfctag is initialized
  * @param  None
  * @retval 0 if the nfctag is not initialized, 1 if the nfctag is already initialized
  */
uint8_t BSP_NFCTAG_isInitialized( uint32_t Instance )
{
  UNUSED(Instance);
  return ST25DVObj.IsInitialized | ST25DVxxKCObj.IsInitialized;
}

/**
 * @brief  BSP NFCTAG GPIO callback
 * @param  Node
 * @retval None.
 */
__weak void BSP_NFCTAG_GPIO_Callback(void)
{
  /* This function should be implemented by the user application.
     It is called into this driver when an event on Button is triggered. */
}

/**
  * @brief Enable the GPIO pin
  * @retval BSP status
  */
int32_t BSP_NFCTAG_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  int32_t ret = BSP_ERROR_NONE;

  /* GPIO Ports Clock Enable */
  BSP_NFCTAG_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin =  BSP_NFCTAG_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BSP_NFCTAG_GPIO_PORT, &GPIO_InitStruct);
  
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI12_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI12_IRQn);
  
  if (HAL_EXTI_RegisterCallback(&nfc_exti,  HAL_EXTI_COMMON_CB_ID, NFCTAG_GPIO_EXTI_Callback) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }else {
    HAL_NVIC_SetPriority(EXTI12_IRQn,  BSP_NFCTAG_GPIO_PRIO, 0x00);
    HAL_NVIC_EnableIRQ(EXTI12_IRQn);
  }
  
  return ret;
}

/**
  * @brief  User EXTI line detection callbacks.
  * @retval None
  */
static void NFCTAG_GPIO_EXTI_Callback(void)
{
  BSP_NFCTAG_GPIO_Callback();
}

/**
  * @brief  Read the ID of the nfctag
  * @param  wai_id : the pointer where the who_am_i of the device is stored
  * @retval NFCTAG enum status
  */
int32_t BSP_NFCTAG_ReadID( uint32_t Instance, uint8_t * const wai_id )
{
  UNUSED(Instance);
  if ( Nfctag_Drv->ReadID == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->ReadID(NfcTagObj, wai_id );
}

/**
  * @brief  Check if the nfctag is available
  * @param  Trials : Number of trials
  * @retval NFCTAG enum status
  */
int32_t BSP_NFCTAG_IsDeviceReady( uint32_t Instance, const uint32_t Trials )
{
  UNUSED(Instance);
  if ( Nfctag_Drv->IsReady == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->IsReady(NfcTagObj, Trials );
}

/**
  * @brief  Configure nfctag interrupt
  * @param  ITConfig : store interrupt to configure
  *                  - 0x01 => RF BUSY
  *                  - 0x02 => WIP
  * @retval NFCTAG enum status
  */
int32_t BSP_NFCTAG_ConfigIT( uint32_t Instance, const uint16_t ITConfig )
{
  UNUSED(Instance);
  if ( Nfctag_Drv->ConfigIT == NULL )
  {
    return NFCTAG_ERROR;
  }
  return Nfctag_Drv->ConfigIT(NfcTagObj, ITConfig );
}

/**
  * @brief  Get nfctag interrupt configutration
  * @param  ITConfig : store interrupt configuration
  *                  - 0x01 => RF BUSY
  *                  - 0x02 => WIP
  * @retval NFCTAG enum status
  */
int32_t BSP_NFCTAG_GetITStatus(uint32_t Instance,  uint16_t * const ITConfig )
{
  UNUSED(Instance);
  if ( Nfctag_Drv->GetITStatus == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->GetITStatus(NfcTagObj, ITConfig );
}

/**
  * @brief  Reads data in the nfctag at specific address
  * @param  pData : pointer to store read data
  * @param  TarAddr : I2C data memory address to read
  * @param  Size : Size in bytes of the value to be read
  * @retval NFCTAG enum status
  */
int32_t BSP_NFCTAG_ReadData( uint32_t Instance, uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size )
{
  UNUSED(Instance);
  if ( Nfctag_Drv->ReadData == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->ReadData(NfcTagObj, pData, TarAddr, Size );
}

/**
  * @brief  Writes data in the nfctag at specific address
  * @param  pData : pointer to the data to write
  * @param  TarAddr : I2C data memory address to write
  * @param  Size : Size in bytes of the value to be written
  * @retval NFCTAG enum status
  */
int32_t BSP_NFCTAG_WriteData( uint32_t Instance, const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size )
{
  UNUSED(Instance);
  if ( Nfctag_Drv->WriteData == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->WriteData(NfcTagObj, pData, TarAddr, Size );
}

/**
  * @brief  Reads nfctag Register
  * @param  pData : pointer to store read data
  * @param  TarAddr : I2C register address to read
  * @param  Size : Size in bytes of the value to be read
  * @retval NFCTAG enum status
  */
int32_t BSP_NFCTAG_ReadRegister( uint32_t Instance, uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size )
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadRegister(NfcTagObj, pData, TarAddr, Size);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadRegister(NfcTagObj, pData, TarAddr, Size);
  }
  
  return ret_value;
}

/**
  * @brief  Writes nfctag Register
  * @param  pData : pointer to the data to write
  * @param  TarAddr : I2C register address to write
  * @param  Size : Size in bytes of the value to be written
  * @retval NFCTAG enum status
  */
int32_t BSP_NFCTAG_WriteRegister( uint32_t Instance, const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size )
{
  UNUSED(Instance);
   int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;

  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteRegister(NfcTagObj, pData, TarAddr, Size);
    if(ret_value == NFCTAG_OK)
    {
      while(Nfctag_Drv->IsReady(NfcTagObj, 1) != NFCTAG_OK) {};
      return NFCTAG_OK;
    }
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteRegister(NfcTagObj, pData, TarAddr, Size);
    if(ret_value == NFCTAG_OK)
    {
      while(Nfctag_Drv->IsReady(NfcTagObj, 1) != NFCTAG_OK) {};
      return NFCTAG_OK;
    }
  }
  return ret_value;
}

/**
  * @brief  Return the size of the nfctag
  * @retval Size of the NFCtag in Bytes
  */
uint32_t BSP_NFCTAG_GetByteSize( uint32_t Instance )
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  ST25DV_MEM_SIZE mem_size;
  ST25DVxxKC_MEM_SIZE_t mem_size_xxkc;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ST25DV_ReadMemSize(NfcTagObj, &mem_size);
    ret_value = (mem_size.BlockSize + 1UL) * (mem_size.Mem_Size + 1UL);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ST25DVxxKC_ReadMemSize(NfcTagObj, &mem_size_xxkc);
    ret_value = (mem_size_xxkc.BlockSize + 1UL) * (mem_size_xxkc.Mem_Size + 1UL);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the ST25DV IC Revision.
  * @param  pICRev Pointer on the uint8_t used to return the ST25DV IC Revision number.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadICRev( uint32_t Instance, uint8_t * const pICRev )
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadICRev(NfcTagObj, pICRev);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadICRev(NfcTagObj, pICRev);
  }
    
  return ret_value;
}


/**
  * @brief  Reads the ST25DV ITtime duration for the GPO pulses.
  * @param  pITtime Pointer used to return the coefficient for the GPO Pulse duration (Pulse duration = 302,06 us - ITtime * 512 / fc).
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadITPulse(uint32_t Instance, void * const pITtime )
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadITPulse(NfcTagObj, (ST25DV_PULSE_DURATION *)pITtime);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadITPulse(NfcTagObj, (ST25DVxxKC_PULSE_DURATION_E *)pITtime);
  }
  
  return ret_value;
}

/**
  * @brief    Configures the ST25DV ITtime duration for the GPO pulse.
  * @details  Needs the I2C Password presentation to be effective.
  * @param    ITtime Coefficient for the Pulse duration to be written (Pulse duration = 302,06 us - ITtime * 512 / fc)
  * @retval   int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteITPulse( uint32_t Instance, const uint8_t ITtime )
{
  UNUSED(Instance);
    int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteITPulse(NfcTagObj, (ST25DV_PULSE_DURATION)ITtime);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteITPulse(NfcTagObj, (ST25DVxxKC_PULSE_DURATION_E)ITtime);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the  UID.
  * @param  pUid Pointer used to return the  UID value.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadUID( uint32_t Instance, void * const pUid )
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadUID(NfcTagObj, (ST25DV_UID *)pUid);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadUID(NfcTagObj, (ST25DVxxKC_UID_t *)pUid);
  }
  
  return ret_value;
  
}

/**
  * @brief  Reads the  DSFID.
  * @param  pDsfid Pointer used to return the  DSFID value.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadDSFID( uint32_t Instance, uint8_t * const pDsfid )
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadDSFID(NfcTagObj, pDsfid);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadDSFID(NfcTagObj, pDsfid);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the DSFID RF Lock state.
  * @param  pLockDsfid Pointer used to return the DSFID lock state.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadDsfidRFProtection( uint32_t Instance, void * const pLockDsfid )
{
  UNUSED(Instance);
    int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadDsfidRFProtection(NfcTagObj, (ST25DV_LOCK_STATUS *)pLockDsfid);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadDsfidRFProtection(NfcTagObj, (ST25DVxxKC_LOCK_STATUS_E *)pLockDsfid);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the AFI.
  * @param  pAfi Pointer used to return the AFI value.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadAFI( uint32_t Instance, uint8_t * const pAfi )
{
  UNUSED(Instance);
   int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadAFI(NfcTagObj, pAfi);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadAFI(NfcTagObj, pAfi);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the AFI RF Lock state.
  * @param  pLockAfi Pointer used to return the ASFID lock state.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadAfiRFProtection( uint32_t Instance, void * const pLockAfi )
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadAfiRFProtection(NfcTagObj, (ST25DV_LOCK_STATUS *)pLockAfi);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadAfiRFProtection(NfcTagObj, (ST25DVxxKC_LOCK_STATUS_E *)pLockAfi);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the I2C Protected Area state.
  * @param  pProtZone Pointer used to return the Protected Area state.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadI2CProtectZone( uint32_t Instance, void * const pProtZone )
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadI2CProtectZone(NfcTagObj, (ST25DV_I2C_PROT_ZONE *)pProtZone);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadI2CProtectZone(NfcTagObj, (ST25DVxxKC_I2C_PROT_ZONE_t *)pProtZone);
  }
  
  return ret_value;
}

/**
  * @brief    Sets the I2C write-protected state to an EEPROM Area.
  * @details  Needs the I2C Password presentation to be effective.
  * @param    Zone                value coresponding to the area to protect.
  * @param    ReadWriteProtection value corresponding to the protection to be set.
  * @return   int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteI2CProtectZonex(uint32_t Instance, const uint8_t Zone,  const uint8_t ReadWriteProtection )
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteI2CProtectZonex(NfcTagObj, (ST25DV_PROTECTION_ZONE)Zone,
                                                          (ST25DV_PROTECTION_CONF)ReadWriteProtection);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteI2CProtectZonex(NfcTagObj, (ST25DVxxKC_PROTECTION_ZONE_E)Zone,
                                                          (ST25DVxxKC_PROTECTION_CONF_E)ReadWriteProtection);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the CCile protection state.
  * @param  pLockCCFile Pointer corresponding to the lock state of the CCFile.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadLockCCFile(const uint32_t Instance, void *const pLockCCFile)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadLockCCFile(NfcTagObj, (ST25DV_LOCK_CCFILE *)pLockCCFile);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadLockCCFile(NfcTagObj, (ST25DVxxKC_LOCK_CCFILE_t *)pLockCCFile);
  }
  
  return ret_value;
}

/**
  * @brief  Locks the CCile to prevent any RF write access.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  NbBlockCCFile value corresponding to the number of blocks to be locked.
  * @param  LockCCFile    value corresponding to the lock state to apply on the CCFile.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteLockCCFile(const uint32_t Instance, const uint8_t NbBlockCCFile, const uint8_t LockCCFile)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteLockCCFile(NfcTagObj, (ST25DV_CCFILE_BLOCK)NbBlockCCFile, (ST25DV_LOCK_STATUS)LockCCFile);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteLockCCFile(NfcTagObj, (ST25DVxxKC_CCFILE_BLOCK_E)NbBlockCCFile, \
                                                          (ST25DVxxKC_LOCK_STATUS_E)LockCCFile);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the Cfg registers protection.
  * @param  pLockCfg Pointer corresponding to the Cfg registers lock state.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadLockCFG(const uint32_t Instance, void *const pLockCfg)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadLockCFG(NfcTagObj, (ST25DV_LOCK_STATUS *)pLockCfg);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadLockCFG(NfcTagObj, (ST25DVxxKC_LOCK_STATUS_E *)pLockCfg);
  }
  
  return ret_value;
}

/**
  * @brief  Lock/Unlock the Cfg registers, to prevent any RF write access.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  LockCfg value corresponding to the lock state to be written.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteLockCFG(const uint32_t Instance, const uint8_t LockCfg)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteLockCFG(NfcTagObj, (ST25DV_LOCK_STATUS)LockCfg);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteLockCFG(NfcTagObj, (ST25DVxxKC_LOCK_STATUS_E)LockCfg);
  }
  
  return ret_value;
}

/**
  * @brief  Presents I2C password, to authorize the I2C writes to protected areas.
  * @param  PassWord Password value on 32bits
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_PresentI2CPassword(const uint32_t Instance, const void *const PassWord)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  ST25DV_PASSWD *password_tmp;
  ST25DVxxKC_PASSWD_t *passwordxxkc_tmp;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    password_tmp = (ST25DV_PASSWD *)PassWord;
    
    ret_value = ST25DV_PresentI2CPassword(NfcTagObj, *password_tmp);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    passwordxxkc_tmp = (ST25DVxxKC_PASSWD_t *)PassWord;
    
    ret_value = ST25DVxxKC_PresentI2CPassword(NfcTagObj, *passwordxxkc_tmp);
  }
  
  return ret_value;
}

/**
  * @brief  Writes a new I2C password.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  PassWord New I2C PassWord value on 32bits.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteI2CPassword(const uint32_t Instance, const void *const PassWord)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  ST25DV_PASSWD *password_tmp;
  ST25DVxxKC_PASSWD_t *passwordxxkc_tmp;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    password_tmp = (ST25DV_PASSWD *)PassWord;
    
    ret_value = ST25DV_WriteI2CPassword(NfcTagObj, *password_tmp);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    passwordxxkc_tmp = (ST25DVxxKC_PASSWD_t *)PassWord;
    
    ret_value = ST25DVxxKC_WriteI2CPassword(NfcTagObj, *passwordxxkc_tmp);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the RF Zone Security Status (defining the allowed RF accesses).
  * @param  Zone        value coresponding to the protected area.
  * @param  pRfprotZone Pointer corresponding to the area protection state.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadRFZxSS(const uint32_t Instance, const uint8_t Zone,
                                                          void *const pRfprotZone)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadRFZxSS(NfcTagObj, (ST25DV_PROTECTION_ZONE)Zone, (ST25DV_RF_PROT_ZONE *)pRfprotZone);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadRFZxSS(NfcTagObj, (ST25DVxxKC_PROTECTION_ZONE_E)Zone,
                                                          (ST25DVxxKC_RF_PROT_ZONE_t *)pRfprotZone);
  }
  
  return ret_value;
}

/**
  * @brief  Writes the RF Zone Security Status (defining the allowed RF accesses)
  * @details  Needs the I2C Password presentation to be effective.
  * @param  Zone        value corresponding to the area on which to set the RF protection.
  * @param  RfProtZone  Pointer defining the protection to be set on the area.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteRFZxSS(const uint32_t Instance, const uint8_t Zone, const void *const RfProtZone)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  ST25DV_RF_PROT_ZONE *rfprotzone_tmp;
  ST25DVxxKC_RF_PROT_ZONE_t *rfprotzonexxkc_tmp;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    rfprotzone_tmp = (ST25DV_RF_PROT_ZONE *)RfProtZone;
    
    ret_value = ST25DV_WriteRFZxSS(NfcTagObj, (ST25DV_PROTECTION_ZONE)Zone, *rfprotzone_tmp);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    rfprotzonexxkc_tmp = (ST25DVxxKC_RF_PROT_ZONE_t *)RfProtZone;
    
    ret_value = ST25DVxxKC_WriteRFZxSS(NfcTagObj, (ST25DVxxKC_PROTECTION_ZONE_E)Zone,
                                                          *rfprotzonexxkc_tmp);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the value of the end area address.
  * @param  EndZone value corresponding to an area end address.
  * @param  pEndZ   Pointer used to return the end address of the area.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadEndZonex(const uint32_t Instance, const uint8_t EndZone, uint8_t *pEndZ)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadEndZonex(NfcTagObj, (ST25DV_END_ZONE)EndZone, pEndZ);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadEndZonex(NfcTagObj, (ST25DVxxKC_END_ZONE_E)EndZone, pEndZ);
  }
  
  return ret_value;
}

/**
  * @brief    Sets the end address of an area.
  * @details  Needs the I2C Password presentation to be effective.
  * @note     The ST25DV-I2C answers a NACK when setting the EndZone2 & EndZone3 to same value 
  *           than repectively EndZone1 & EndZone2.\n These NACKs are ok.
  * @param  EndZone value corresponding to an area.
  * @param  EndZ   End zone value to be written.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteEndZonex(const uint32_t Instance, const uint8_t EndZone, const uint8_t EndZ)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteEndZonex(NfcTagObj, (ST25DV_END_ZONE)EndZone, EndZ);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteEndZonex(NfcTagObj, (ST25DVxxKC_END_ZONE_E)EndZone, EndZ);
  }
  
  return ret_value;
}

/**
  * @brief  Initializes the end address of the ST25DV-I2C areas with their default values (end of memory).
  * @details  Needs the I2C Password presentation to be effective.
  *           The ST25DV-I2C answers a NACK when setting the EndZone2 & EndZone3 to same value
  *           than repectively EndZone1 & EndZone2. These NACKs are ok.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_InitEndZone(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_InitEndZone(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_InitEndZone(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Creates user areas with defined lengths.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  Zone1Length Length of area1 in bytes (32 to 8192, 0x20 to 0x2000)
  * @param  Zone2Length Length of area2 in bytes (0 to 8128, 0x00 to 0x1FC0)
  * @param  Zone3Length Length of area3 in bytes (0 to 8064, 0x00 to 0x1F80)
  * @param  Zone4Length Length of area4 in bytes (0 to 8000, 0x00 to 0x1F40)
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_CreateUserZone(const uint32_t Instance, uint16_t Zone1Length, uint16_t Zone2Length, \
                                                          uint16_t Zone3Length, uint16_t Zone4Length)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_CreateUserZone(NfcTagObj, Zone1Length, Zone2Length, Zone3Length, Zone4Length);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_CreateUserZone(NfcTagObj, Zone1Length, Zone2Length, Zone3Length, Zone4Length);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the ST25DV-I2C Memory Size.
  * @param  pSizeInfo Pointer used to return the Memory size information.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadMemSize(const uint32_t Instance, void *const pSizeInfo)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadMemSize(NfcTagObj, (ST25DV_MEM_SIZE *)pSizeInfo);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadMemSize(NfcTagObj, (ST25DVxxKC_MEM_SIZE_t *)pSizeInfo);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the Energy harvesting mode.
  * @param  pEH_mode Pointer corresponding to the Energy Harvesting state.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadEHMode(const uint32_t Instance, void *const pEH_mode)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadEHMode(NfcTagObj, (ST25DV_EH_MODE_STATUS *)pEH_mode);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadEHMode(NfcTagObj, (ST25DVxxKC_EH_MODE_STATUS_E *)pEH_mode);
  }
  
  return ret_value;
}

/**
  * @brief  Sets the Energy harvesting mode.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  EH_mode value for the Energy harvesting mode to be set.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteEHMode(const uint32_t Instance, const uint8_t EH_mode)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteEHMode(NfcTagObj, (ST25DV_EH_MODE_STATUS)EH_mode);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteEHMode(NfcTagObj, (ST25DVxxKC_EH_MODE_STATUS_E)EH_mode);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the RF Management configuration.
  * @param  pRF_Mngt Pointer used to return the RF Management configuration.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadRFMngt(const uint32_t Instance, void *const pRF_Mngt)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadRFMngt(NfcTagObj, (ST25DV_RF_MNGT *)pRF_Mngt);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadRFMngt(NfcTagObj, (ST25DVxxKC_RF_MNGT_t *)pRF_Mngt);
  }
  
  return ret_value;
}

/**
  * @brief  Sets the RF Management configuration.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  Rfmngt Value of the RF Management configuration to be written.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteRFMngt(const uint32_t Instance, const uint8_t Rfmngt)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteRFMngt(NfcTagObj, Rfmngt);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteRFMngt(NfcTagObj, Rfmngt);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the RFDisable register information.
  * @param  pRFDisable Pointer corresponding to the RF Disable status.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_GetRFDisable(const uint32_t Instance, void *const pRFDisable)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_GetRFDisable(NfcTagObj, (ST25DV_EN_STATUS *)pRFDisable);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_GetRFDisable(NfcTagObj, (ST25DVxxKC_EN_STATUS_E *)pRFDisable);
  }
  
  return ret_value;
}

/**
  * @brief  Sets the RF Disable configuration.
  * @details  Needs the I2C Password presentation to be effective.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_SetRFDisable(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_SetRFDisable(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_SetRFDisable(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Resets the RF Disable configuration
  * @details  Needs the I2C Password presentation to be effective.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ResetRFDisable(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ResetRFDisable(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ResetRFDisable(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the RFSleep register information.
  * @param  pRFSleep Pointer corresponding to the RF Sleep status.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_GetRFSleep(const uint32_t Instance, void *const pRFSleep)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_GetRFSleep(NfcTagObj, (ST25DV_EN_STATUS *)pRFSleep);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_GetRFSleep(NfcTagObj, (ST25DVxxKC_EN_STATUS_E *)pRFSleep);
  }
  
  return ret_value;
}

/**
  * @brief  Sets the RF Sleep configuration.
  * @details  Needs the I2C Password presentation to be effective.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_SetRFSleep(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_SetRFSleep(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_SetRFSleep(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Resets the RF Sleep configuration.
  * @details  Needs the I2C Password presentation to be effective.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ResetRFSleep(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ResetRFSleep(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ResetRFSleep(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the Mailbox mode.
  * @param  pMB_mode Pointer used to return the Mailbox mode.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadMBMode(const uint32_t Instance, void *const pMB_mode)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadMBMode(NfcTagObj, (ST25DV_EN_STATUS *)pMB_mode);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadMBMode(NfcTagObj, (ST25DVxxKC_EN_STATUS_E *)pMB_mode);
  }
  
  return ret_value;
}

/**
  * @brief  Sets the Mailbox mode.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  MB_mode value corresponding to the Mailbox mode to be set.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteMBMode(const uint32_t Instance, const uint8_t MB_mode)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteMBMode(NfcTagObj, (ST25DV_EN_STATUS)MB_mode);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteMBMode(NfcTagObj, (ST25DVxxKC_EN_STATUS_E)MB_mode);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the Mailbox watchdog duration coefficient.
  * @param  pWdgDelay Pointer on a uint8_t used to return the watchdog duration coefficient.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadMBWDG(const uint32_t Instance, uint8_t *const pWdgDelay)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadMBWDG(NfcTagObj, pWdgDelay);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadMBWDG(NfcTagObj, pWdgDelay);
  }
  
  return ret_value;
}

/**
  * @brief  Writes the Mailbox watchdog coefficient delay
  * @details  Needs the I2C Password presentation to be effective.
  * @param  WdgDelay Watchdog duration coefficient to be written (Watch dog duration = MB_WDG*30 ms +/- 6%).
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteMBWDG(const uint32_t Instance, const uint8_t WdgDelay)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteMBWDG(NfcTagObj, WdgDelay);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteMBWDG(NfcTagObj, WdgDelay);
  }
  
  return ret_value;
}

/**
  * @brief  Reads N bytes of data from the Mailbox, starting at the specified byte offset.
  * @param  pData   Pointer on the buffer used to return the read data.
  * @param  Offset  Offset in the Mailbox memory, byte number to start the read.
  * @param  NbByte  Number of bytes to be read.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadMailboxData(const uint32_t Instance, uint8_t *const pData, const uint16_t TarAddr, \
                                                          const uint16_t NbByte)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadMailboxData(NfcTagObj, pData, TarAddr, NbByte);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadMailboxData(NfcTagObj, pData, TarAddr, NbByte);
  }
  
  return ret_value;
}

/**
  * @brief  Writes N bytes of data in the Mailbox, starting from first Mailbox Address.
  * @param  pData   Pointer to the buffer containing the data to be written.
  * @param  NbByte  Number of bytes to be written.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteMailboxData(const uint32_t Instance, const uint8_t *const pData, const uint16_t NbByte)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteMailboxData(NfcTagObj, pData, NbByte);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteMailboxData(NfcTagObj, pData, NbByte);
  }
  
  return ret_value;
}

/**
  * @brief  Reads N bytes from the mailbox registers, starting at the specified I2C address.
  * @param  pData   Pointer on the buffer used to return the data.
  * @param  TarAddr I2C memory address to be read.
  * @param  NbByte  Number of bytes to be read.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadMailboxRegister(const uint32_t Instance, uint8_t *const pData, const uint16_t TarAddr,
                                                          const uint16_t NbByte)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadMailboxRegister(NfcTagObj, pData, TarAddr, NbByte);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadMailboxRegister(NfcTagObj, pData, TarAddr, NbByte);
  }
  
  return ret_value;
}

/**
  * @brief  Writes N bytes to the specified mailbox register.
  * @param  pData   Pointer on the data to be written.
  * @param  TarAddr I2C register address to be written.
  * @param  NbByte  Number of bytes to be written.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteMailboxRegister(const uint32_t Instance, const uint8_t *const pData, const uint16_t TarAddr, \
                                                          const uint16_t NbByte)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteMailboxRegister(NfcTagObj, pData, TarAddr, NbByte);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteMailboxRegister(NfcTagObj, pData, TarAddr, NbByte);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the status of the security session open register.
  * @param  pSession Pointer used to return the session status.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadI2CSecuritySession_Dyn(const uint32_t Instance, void *const pSession)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadI2CSecuritySession_Dyn(NfcTagObj, (ST25DV_I2CSSO_STATUS *)pSession);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadI2CSecuritySession_Dyn(NfcTagObj, (ST25DVxxKC_I2CSSO_STATUS_E *)pSession);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the IT status register from the ST25DV-I2C.
  * @param  pITStatus Pointer on uint8_t, used to return the IT status, such as:
  *                       - RFUSERSTATE = 0x01
  *                       - RFBUSY = 0x02
  *                       - RFINTERRUPT = 0x04
  *                       - FIELDFALLING = 0x08
  *                       - FIELDRISING = 0x10
  *                       - RFPUTMSG = 0x20
  *                       - RFGETMSG = 0x40
  *                       - RFWRITE = 0x80
  *
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadITSTStatus_Dyn(const uint32_t Instance, uint8_t *const pITStatus)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadITSTStatus_Dyn(NfcTagObj, pITStatus);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadITSTStatus_Dyn(NfcTagObj, pITStatus);
  }
  
  return ret_value;
}

/**
  * @brief  Read value of dynamic GPO register configuration.
  * @param  pGPO pointer of the dynamic GPO configuration to store.
  * @retval NFCTAG enum status.
  */
int32_t BSP_NFCTAG_ReadGPO_Dyn(const uint32_t Instance, uint8_t *GPOConfig)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadGPO_Dyn(NfcTagObj, GPOConfig);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadGPO_Dyn(NfcTagObj, GPOConfig);
  }
  
  return ret_value;
}

/**
  * @brief  Get dynamique GPO enable status
  * @param  pGPO_en pointer of the GPO enable status to store
  * @retval NFCTAG enum status
  */
int32_t BSP_NFCTAG_GetGPO_en_Dyn(const uint32_t Instance, void *const pGPO_en)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_GetGPO_en_Dyn(NfcTagObj, (ST25DV_EN_STATUS *)pGPO_en);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_GetGPO_en_Dyn(NfcTagObj, (ST25DVxxKC_EN_STATUS_E *)pGPO_en);
  }
  
  return ret_value;
}

/**
  * @brief  Set dynamique GPO enable configuration.
  * @param  None No parameters.
  * @retval NFCTAG enum status.
  */
int32_t BSP_NFCTAG_SetGPO_en_Dyn(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_SetGPO_en_Dyn(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_SetGPO_en_Dyn(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Reset dynamique GPO enable configuration.
  * @param  None No parameters.
  * @retval NFCTAG enum status.
  */
int32_t BSP_NFCTAG_ResetGPO_en_Dyn(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ResetGPO_en_Dyn(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ResetGPO_en_Dyn(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Read value of dynamic EH Ctrl register configuration
  * @param  pEH_CTRL : pointer of the dynamic EH Ctrl configuration to store
  * @retval NFCTAG enum status
  */
int32_t BSP_NFCTAG_ReadEHCtrl_Dyn(const uint32_t Instance, void *const pEH_CTRL)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadEHCtrl_Dyn(NfcTagObj, (ST25DV_EH_CTRL *)pEH_CTRL);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadEHCtrl_Dyn(NfcTagObj, (ST25DVxxKC_EH_CTRL_t *)pEH_CTRL);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the Energy Harvesting dynamic status.
  * @param  pEH_Val Pointer used to return the Energy Harvesting dynamic status.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_GetEHENMode_Dyn(const uint32_t Instance, void *const pEH_Val)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_GetEHENMode_Dyn(NfcTagObj, (ST25DV_EN_STATUS *)pEH_Val);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_GetEHENMode_Dyn(NfcTagObj, (ST25DVxxKC_EN_STATUS_E *)pEH_Val);
  }
  
  return ret_value;
}

/**
  * @brief  Dynamically sets the Energy Harvesting mode.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_SetEHENMode_Dyn(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_SetEHENMode_Dyn(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_SetEHENMode_Dyn(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Dynamically unsets the Energy Harvesting mode.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ResetEHENMode_Dyn(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ResetEHENMode_Dyn(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ResetEHENMode_Dyn(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the EH_ON status from the EH_CTRL_DYN register.
  * @param  pEHON Pointer used to return the EHON status.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_GetEHON_Dyn(const uint32_t Instance, void *const pEHON)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_GetEHON_Dyn(NfcTagObj, (ST25DV_EN_STATUS *)pEHON);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_GetEHON_Dyn(NfcTagObj, (ST25DVxxKC_EN_STATUS_E *)pEHON);
  }
  
  return ret_value;
}

/**
  * @brief  Checks if RF Field is present in front of the ST25DV-I2C.
  * @param  pRF_Field Pointer used to return the field presence.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_GetRFField_Dyn(const uint32_t Instance, void *const pRF_Field)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_GetRFField_Dyn(NfcTagObj, (ST25DV_FIELD_STATUS *)pRF_Field);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_GetRFField_Dyn(NfcTagObj, (ST25DVxxKC_FIELD_STATUS_E *)pRF_Field);
  }
  
  return ret_value;
}

/**
  * @brief  Check if VCC is supplying the ST25DV-I2C.
  * @param  pVCC pointer of the VCC status to store
  * @retval NFCTAG enum status.
  */
int32_t BSP_NFCTAG_GetVCC_Dyn(const uint32_t Instance, void *const pVCC)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_GetVCC_Dyn(NfcTagObj, (ST25DV_VCC_STATUS *)pVCC);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_GetVCC_Dyn(NfcTagObj, (ST25DVxxKC_VCC_STATUS_E *)pVCC);
  }
  
  return ret_value;
}

/**
  * @brief  Read value of dynamic RF Management configuration
  * @param  pRF_Mngt pointer of the dynamic RF Management configuration to store
  * @retval NFCTAG enum status
  */
int32_t BSP_NFCTAG_ReadRFMngt_Dyn(const uint32_t Instance, void *const pRF_Mngt)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadRFMngt_Dyn(NfcTagObj, (ST25DV_RF_MNGT *)pRF_Mngt);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadRFMngt_Dyn(NfcTagObj, (ST25DVxxKC_RF_MNGT_t *)pRF_Mngt);
  }
  
  return ret_value;
}


/**
  * @brief  Writes a value to the RF Management dynamic register.
  * @param  RF_Mngt Value to be written to the RF Management dynamic register.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_WriteRFMngt_Dyn(const uint32_t Instance, const uint8_t RF_Mngt)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_WriteRFMngt_Dyn(NfcTagObj, RF_Mngt);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_WriteRFMngt_Dyn(NfcTagObj, RF_Mngt);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the RFDisable dynamic register information.
  * @param  pRFDisable Pointer used to return the RF Disable state.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_GetRFDisable_Dyn(const uint32_t Instance, void *const pRFDisable)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_GetRFDisable_Dyn(NfcTagObj, (ST25DV_EN_STATUS *)pRFDisable);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_GetRFDisable_Dyn(NfcTagObj, (ST25DVxxKC_EN_STATUS_E *)pRFDisable);
  }
  
  return ret_value;
}

/**
  * @brief  Sets the RF Disable dynamic configuration.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_SetRFDisable_Dyn(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_SetRFDisable_Dyn(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_SetRFDisable_Dyn(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Unsets the RF Disable dynamic configuration.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ResetRFDisable_Dyn(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ResetRFDisable_Dyn(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ResetRFDisable_Dyn(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the RFSleep dynamic register information.
  * @param  pRFSleep Pointer used to return the RF Sleep state.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_GetRFSleep_Dyn(const uint32_t Instance, void *const pRFSleep)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_GetRFSleep_Dyn(NfcTagObj, (ST25DV_EN_STATUS *)pRFSleep);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_GetRFSleep_Dyn(NfcTagObj, (ST25DVxxKC_EN_STATUS_E *)pRFSleep);
  }
  
  return ret_value;
}

/**
  * @brief  Sets the RF Sleep dynamic configuration.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_SetRFSleep_Dyn(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_SetRFSleep_Dyn(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_SetRFSleep_Dyn(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Unsets the RF Sleep dynamic configuration.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ResetRFSleep_Dyn(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ResetRFSleep_Dyn(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ResetRFSleep_Dyn(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the Mailbox ctrl dynamic register.
  * @param  pCtrlStatus Pointer structure used to return
  *         the dynamic Mailbox ctrl information.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadMBCtrl_Dyn(const uint32_t Instance, void *const pCtrlStatus)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadMBCtrl_Dyn(NfcTagObj, (ST25DV_MB_CTRL_DYN_STATUS *)pCtrlStatus);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadMBCtrl_Dyn(NfcTagObj, (ST25DVxxKC_MB_CTRL_DYN_STATUS_t *)pCtrlStatus);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the Mailbox Enable dynamic configuration.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_GetMBEN_Dyn(const uint32_t Instance, void *const pMBEN)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_GetMBEN_Dyn(NfcTagObj, (ST25DV_EN_STATUS *)pMBEN);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_GetMBEN_Dyn(NfcTagObj, (ST25DVxxKC_EN_STATUS_E *)pMBEN);
  }
  
  return ret_value;
}

/**
  * @brief  Sets the Mailbox Enable dynamic configuration.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_SetMBEN_Dyn(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_SetMBEN_Dyn(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_SetMBEN_Dyn(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Unsets the Mailbox Enable dynamic configuration.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ResetMBEN_Dyn(const uint32_t Instance)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ResetMBEN_Dyn(NfcTagObj);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ResetMBEN_Dyn(NfcTagObj);
  }
  
  return ret_value;
}

/**
  * @brief  Reads the Mailbox message length dynamic register.
  * @param  pMBLength Pointer on a uint8_t used to return the Mailbox message length.
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ReadMBLength_Dyn(const uint32_t Instance, uint8_t *const pMBLength)
{
  UNUSED(Instance);
  int32_t ret_value = BSP_ERROR_PERIPH_FAILURE;
  
  if(ST25DVObj.IsInitialized == 1)
  {
    ret_value = ST25DV_ReadMBLength_Dyn(NfcTagObj, pMBLength);
  }
  
  if(ST25DVxxKCObj.IsInitialized == 1)
  {
    ret_value = ST25DVxxKC_ReadMBLength_Dyn(NfcTagObj, pMBLength);
  }
  
  return ret_value;
}

