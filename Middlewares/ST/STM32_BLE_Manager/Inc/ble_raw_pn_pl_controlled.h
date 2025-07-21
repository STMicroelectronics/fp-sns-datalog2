/**
  ******************************************************************************
  * @file    ble_raw_pn_pl_controlled.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Raw Characteristcs controlled by PnP-Like nessage info services APIs.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Exported functions ------------------------------------------------------- */
/**
  * @brief Init PnPLike Raw Service
  * @param uint8_t max_char_size Max Dimension of the BLE Char
  * @param uint8_t is_variable variable/Fixed length flag
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Raw PnPL Controlled service
  */
extern ble_char_object_t *ble_init_raw_pn_pl_controlled_service(uint8_t max_char_size, uint8_t is_variable);


/**
  * @brief  Update Raw PnPL Controlled characteristic value
  * @param  uint8_t *data Data to Update
  * @param  uint8_t data_size Size of data to Update
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_raw_pn_pl_controlled_status_update(uint8_t *data, uint8_t data_size);

/**
  * @brief  Return the max Char length for Raw PnPL Controlled characteristic
  * @param  None
  * @retval uint16_t   Max Char length
  */
extern uint16_t ble_raw_pn_pl_controlled_max_char_size(void);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_raw_pn_p_like_controlled(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_RAW_PNPL_CONTROLLED_H_ */

