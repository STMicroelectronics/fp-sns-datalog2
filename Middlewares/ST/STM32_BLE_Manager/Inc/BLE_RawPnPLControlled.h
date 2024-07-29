/**
  ******************************************************************************
  * @file    BLE_RawPnPLControlled.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.11.0
  * @date    15-February-2024
  * @brief   Raw Characteristcs controlled by PnP-Like nessage info services APIs.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BLE_RAW_PNPL_CONTROLLED_H_
#define _BLE_RAW_PNPL_CONTROLLED_H_

#ifdef __cplusplus
extern "C" {
#endif


/** Format type for Raw PnP-L Controlled type:
*  enable: Boolean (mandatory)
*  format: RawPnPLEntryFormat, (mandatory)
*  elements: Int=1, (optional, default value =1)
*  unit: String?=null, (optional)
*  max: Double?=null, (optional)
*  min: Double?=null (optional)
*
* RawPnPLEntryFormat possible formats:
*  "uint8_t",
*  "int8_t",
*  "uint16_t",
*  "int16_t",
*  "uint32_t",
*  "int32_t",
*  "float"
**/

/** Format type for Raw PnP-L Controlled Custom format
* json type like
* {
*   "output":
*   [
*     {
*       "name": "Acc x [g]",
*       "type": "float"
*     },
*     {
*       "name": "Acc y [g]",
*       "type": "float"
*     },
*     {
*       "name": "Acc z [g]",
*       "type": "float"
*     },
*     {
*       "name": "Prs [hPa]",
*       "type": "float"
*     },
*     {
*       "name": "Fall",
*       "type": "uint8_t"
*     }
*   ]
* 
*  Fields:
*  name: String, (mandatory)
*  type:RawPnPLCustomEntryFormat, (mandatory)
*  elements: Int=1 (optional, default value =1)
* 
* RawPnPLCustomEntryFormat possible formats:
*  "uint8_t",
*  "char",
*  "int8_t",
*  "uint16_t",
*  "int16_t",
*  "uint32_t",
*  "int32_t",
*  "float"
}
**/


/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventRawPnPLControlled_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventRawPnPLControlled_t CustomNotifyEventRawPnPLControlled;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief Init PnPLike Raw Service
  * @param uint8_t MaxCharSize Max Dimension of the BLE Char
  * @param uint8_t isVariable variable/Fixed length flag
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Raw PnPL Controlled service
  */
extern BleCharTypeDef *BLE_InitRawPnPLControlledService(uint8_t MaxCharSize, uint8_t isVariable);


/**
  * @brief  Update Raw PnPL Controlled characteristic value
  * @param  uint8_t *Data data to Update
  * @param  uint8_t DataSize Size of data to Update
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_RawPnPLControlledStatusUpdate(uint8_t *Data, uint8_t DataSize);

/**
  * @brief  Return the max Char length for Raw PnPL Controlled characteristic
  * @param  None
  * @retval uint16_t   Max Char length
  */
extern uint16_t BLE_RawPnPLControlledMaxCharSize(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_RAW_PNPL_CONTROLLED_H_ */

