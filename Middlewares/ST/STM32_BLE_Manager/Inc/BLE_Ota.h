/**
  ******************************************************************************
  * @file    BLE_Ota.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Ota characteristic APIs.
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
#ifndef _BLE_OTA_H_
#define _BLE_OTA_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BLUE_WB

/* Exported typedef --------------------------------------------------------- */
typedef void(*CustomWriteRequestOta_t)(uint8_t *att_data, uint8_t data_length);

/* Exported Variables ------------------------------------------------------- */
extern CustomWriteRequestOta_t CustomWriteRequestOta;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init OTA characteristic
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for ota characteristic
  */
extern BleCharTypeDef *BLE_InitOtaService(void);

#else /* BLUE_WB */
#error "This Feature is only for WB BLE Chip"
#endif /* BLUE_WB */

#ifdef __cplusplus
}
#endif

#endif /* _BLE_GNSS_H_ */

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
