/**
  ******************************************************************************
  * @file    SensorTileBoxPro_nfctag.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.1.0
  * @date    20-July-2023
  * @brief   This file contains definitions for the SensorTileBoxPro_nfctag.c
  *          specific functions.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SENSORTILEBOXPRO_NFCTAG_H__
#define __SENSORTILEBOXPRO_NFCTAG_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "SensorTileBoxPro_conf.h"
#include "st25dv.h"
#include "st25dvxxkc.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define NFCTAG_4K_SIZE            ((uint32_t) 0x200)

#define BSP_NFCTAG_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#define BSP_NFCTAG_GPIO_PIN GPIO_PIN_12
#define BSP_NFCTAG_GPIO_PORT GPIOE
#define BSP_NFCTAG_GPIO_PRIO 0

#define H_EXTI_12 nfc_exti

/* External variables --------------------------------------------------------*/
extern EXTI_HandleTypeDef nfc_exti;
/* Exported macro ------------------------------------------------------------*/
/* Exported function	--------------------------------------------------------*/

int32_t BSP_NFCTAG_Init( uint32_t Instance );
int32_t BSP_NFCTAG_GPIO_Init(void);
void BSP_NFCTAG_GPIO_Callback(void);

void BSP_NFCTAG_DeInit( uint32_t Instance );
uint8_t BSP_NFCTAG_isInitialized( uint32_t Instance );
int32_t BSP_NFCTAG_ReadID( uint32_t Instance, uint8_t * const wai_id );
int32_t BSP_NFCTAG_ConfigIT( uint32_t Instance, const uint16_t ITConfig );
int32_t BSP_NFCTAG_GetITStatus( uint32_t Instance, uint16_t * const ITConfig );
int32_t BSP_NFCTAG_ReadData( uint32_t Instance, uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size );
int32_t BSP_NFCTAG_WriteData( uint32_t Instance, const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size );
int32_t BSP_NFCTAG_ReadRegister( uint32_t Instance, uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size );
int32_t BSP_NFCTAG_WriteRegister( uint32_t Instance, const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size );
int32_t BSP_NFCTAG_IsDeviceReady( uint32_t Instance,const uint32_t Trials );

uint32_t BSP_NFCTAG_GetByteSize( uint32_t Instance );
int32_t BSP_NFCTAG_ReadICRev( uint32_t Instance, uint8_t * const pICRev );
int32_t BSP_NFCTAG_ReadITPulse( uint32_t Instance, void * const pITtime );
int32_t BSP_NFCTAG_WriteITPulse( uint32_t Instance, const uint8_t ITtime );
int32_t BSP_NFCTAG_ReadUID( uint32_t Instance, void * const pUid );
int32_t BSP_NFCTAG_ReadDSFID( uint32_t Instance, uint8_t * const pDsfid );
int32_t BSP_NFCTAG_ReadDsfidRFProtection( uint32_t Instance, void * const pLockDsfid );
int32_t BSP_NFCTAG_ReadAFI( uint32_t Instance, uint8_t * const pAfi );
int32_t BSP_NFCTAG_ReadAfiRFProtection( uint32_t Instance, void * const pLockAfi );
int32_t BSP_NFCTAG_ReadI2CProtectZone( uint32_t Instance, void * const pProtZone );
int32_t BSP_NFCTAG_WriteI2CProtectZonex(uint32_t Instance, const uint8_t Zone,  const uint8_t ReadWriteProtection );
int32_t BSP_NFCTAG_ReadLockCCFile(uint32_t Instance, void * const pLockCCFile );
int32_t BSP_NFCTAG_WriteLockCCFile(const uint32_t Instance, const uint8_t NbBlockCCFile, const uint8_t LockCCFile);
int32_t BSP_NFCTAG_ReadLockCFG(uint32_t Instance, void * const pLockCfg );
int32_t BSP_NFCTAG_WriteLockCFG(uint32_t Instance, const uint8_t LockCfg );
int32_t BSP_NFCTAG_PresentI2CPassword(const uint32_t Instance, const void *const PassWord);
int32_t BSP_NFCTAG_WriteI2CPassword(const uint32_t Instance, const void *const PassWord);
int32_t BSP_NFCTAG_ReadRFZxSS(const uint32_t Instance, const uint8_t Zone, void *const pRfprotZone);
int32_t BSP_NFCTAG_WriteRFZxSS(const uint32_t Instance, const uint8_t Zone, const void *const RfProtZone);
int32_t BSP_NFCTAG_ReadEndZonex(const uint32_t Instance, const uint8_t EndZone, uint8_t *pEndZ);
int32_t BSP_NFCTAG_WriteEndZonex(const uint32_t Instance, const uint8_t EndZone, const uint8_t EndZ);
int32_t BSP_NFCTAG_InitEndZone(const uint32_t Instance);
int32_t BSP_NFCTAG_CreateUserZone(const uint32_t Instance, uint16_t Zone1Length, uint16_t Zone2Length, uint16_t Zone3Length, uint16_t Zone4Length);
int32_t BSP_NFCTAG_ReadMemSize(const uint32_t Instance, void *const pSizeInfo);
int32_t BSP_NFCTAG_ReadEHMode(const uint32_t Instance, void *const pEH_mode);
int32_t BSP_NFCTAG_WriteEHMode(const uint32_t Instance, const uint8_t EH_mode);
int32_t BSP_NFCTAG_ReadRFMngt(const uint32_t Instance, void *const pRF_Mngt);
int32_t BSP_NFCTAG_WriteRFMngt(const uint32_t Instance, const uint8_t Rfmngt);
int32_t BSP_NFCTAG_GetRFDisable(const uint32_t Instance, void *const pRFDisable);
int32_t BSP_NFCTAG_SetRFDisable(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetRFDisable(const uint32_t Instance);
int32_t BSP_NFCTAG_GetRFSleep(const uint32_t Instance, void *const pRFSleep);
int32_t BSP_NFCTAG_SetRFSleep(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetRFSleep(const uint32_t Instance);
int32_t BSP_NFCTAG_ReadMBMode(const uint32_t Instance, void *const pMB_mode);
int32_t BSP_NFCTAG_WriteMBMode(const uint32_t Instance, const uint8_t MB_mode);
int32_t BSP_NFCTAG_ReadMBWDG(const uint32_t Instance, uint8_t *const pWdgDelay);
int32_t BSP_NFCTAG_WriteMBWDG(const uint32_t Instance, const uint8_t WdgDelay);
int32_t BSP_NFCTAG_ReadMailboxData(const uint32_t Instance, uint8_t *const pData, const uint16_t TarAddr,const uint16_t NbByte);
int32_t BSP_NFCTAG_WriteMailboxData(const uint32_t Instance, const uint8_t *const pData, const uint16_t NbByte);
int32_t BSP_NFCTAG_ReadMailboxRegister(const uint32_t Instance, uint8_t *const pData, const uint16_t TarAddr, const uint16_t NbByte);
int32_t BSP_NFCTAG_WriteMailboxRegister(const uint32_t Instance, const uint8_t *const pData, const uint16_t TarAddr, \
                                                          const uint16_t NbByte);
int32_t BSP_NFCTAG_ReadI2CSecuritySession_Dyn(const uint32_t Instance, void *const pSession);
int32_t BSP_NFCTAG_ReadITSTStatus_Dyn(const uint32_t Instance, uint8_t *const pITStatus);
int32_t BSP_NFCTAG_ReadGPO_Dyn(const uint32_t Instance, uint8_t *GPOConfig);
int32_t BSP_NFCTAG_GetGPO_en_Dyn(const uint32_t Instance, void *const pGPO_en);
int32_t BSP_NFCTAG_SetGPO_en_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetGPO_en_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ReadEHCtrl_Dyn(const uint32_t Instance, void *const pEH_CTRL);
int32_t BSP_NFCTAG_GetEHENMode_Dyn(const uint32_t Instance, void *const pEH_Val);
int32_t BSP_NFCTAG_SetEHENMode_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetEHENMode_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_GetEHON_Dyn(const uint32_t Instance, void *const pEHON);
int32_t BSP_NFCTAG_GetRFField_Dyn(const uint32_t Instance, void *const pRF_Field);
int32_t BSP_NFCTAG_GetVCC_Dyn(const uint32_t Instance, void *const pVCC);
int32_t BSP_NFCTAG_ReadRFMngt_Dyn(const uint32_t Instance, void *const pRF_Mngt);
int32_t BSP_NFCTAG_WriteRFMngt_Dyn(const uint32_t Instance, const uint8_t RF_Mngt);
int32_t BSP_NFCTAG_GetRFDisable_Dyn(const uint32_t Instance, void *const pRFDisable);
int32_t BSP_NFCTAG_SetRFDisable_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetRFDisable_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_GetRFSleep_Dyn(const uint32_t Instance, void *const pRFSleep);
int32_t BSP_NFCTAG_SetRFSleep_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetRFSleep_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ReadMBCtrl_Dyn(const uint32_t Instance, void *const pCtrlStatus);
int32_t BSP_NFCTAG_GetMBEN_Dyn(const uint32_t Instance, void *const pMBEN);
int32_t BSP_NFCTAG_SetMBEN_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetMBEN_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ReadMBLength_Dyn(const uint32_t Instance, uint8_t *const pMBLength);

#ifdef __cplusplus
 }
#endif
#endif /* __SENSORTILEBOXPRO_NFCTAG_H__ */

