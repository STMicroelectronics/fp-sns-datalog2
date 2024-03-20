/**
  ******************************************************************************
  * @file    BLE_Gnss.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Gnss info services APIs.
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
#ifndef _BLE_GNSS_H_
#define _BLE_GNSS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventGnss_t)(BLE_NotifyEvent_t Gnss);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventGnss_t CustomNotifyEventGnss;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init gnss info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for gnss info service
  */
extern BleCharTypeDef *BLE_InitGnssService(void);

/**
  * @brief  Update Gnss characteristic
  * @param  int32_t GnssLat latitude
  * @param  int32_t GnssLon longitude
  * @param  uint32_t GnssAlt altitude
  * @param  uint8_t N_SAT Number of satellites in view
  * @param  uint8_t SIG_QUAL GNSS signal quality indicator
  * @retval tBleStatus   Status
  */
tBleStatus BLE_GnssUpdate(int32_t Latitude, int32_t Longitude, uint32_t Altitude, uint8_t NumberSat,
                          uint8_t SigQuality);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_GNSS_H_ */

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
