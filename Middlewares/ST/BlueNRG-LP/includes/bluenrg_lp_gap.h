/**
  ******************************************************************************
  * @file    bluenrg_lp_gap.h
  * @author  AMS - RF Application team
  * @version V1.0.0
  * @date    23-May-2019
  * @brief   Header file for BLE GAP layer constants
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2019 STMicroelectronics</center></h2>
  ******************************************************************************
  */
#ifndef __GAP_H__
#define __GAP_H__

#include <link_layer.h>

/**
 *@addtogroup GAP GAP
 *@brief API for GAP layer.
 *@{
 */

/**
 * @name GAP UUIDs
 * @{
 */
#define GAP_SERVICE_UUID                              (0x1800)
#define DEVICE_NAME_CHAR_UUID                         (0x2A00)
#define APPEARANCE_CHAR_UUID                          (0x2A01)
#define PERIPHERAL_PREFERRED_CONN_PARAMS_CHAR_UUID    (0x2A04)
#define CENTRAL_ADDRESS_RESOLUTION_CHAR_UUID          (0x2AA6)
/**
 * @}
 */

/**
 * @name Characteristic value lengths
 * @{
 */
#define APPEARANCE_CHAR_LEN			                (2)
#define PERIPHERAL_PREFERRED_CONN_PARAMS_CHAR_LEN	(8)
#define CENTRAL_ADDRESS_RESOLUTION_CHAR_LEN	        (1)
/**
 * @}
 */

/**
 * @defgroup AD_Types AD Types
 * @brief AD types for adv data and scan response data
 * @{
 */

#define AD_TYPE_FLAGS				     (0x01)

/**
 * @anchor Flags_AD_Type_bits
 * @name Flags AD Type bits
 * @brief Bits in Flags AD Type
 * @{
 */
#define FLAG_BIT_LE_LIMITED_DISCOVERABLE_MODE   (0x01)
#define FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE   (0x02)
#define FLAG_BIT_BR_EDR_NOT_SUPPORTED           (0x04)
#define FLAG_BIT_LE_BR_EDR_CONTROLLER           (0x08)
#define FLAG_BIT_LE_BR_EDR_HOST                 (0x10)
/**
 * @}
 */

/**
 * @name Service UUID AD types
 * @{
 */
#define AD_TYPE_16_BIT_SERV_UUID			    (0x02)
#define AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST     (0x03)
#define AD_TYPE_32_BIT_SERV_UUID                (0x04)
#define AD_TYPE_32_BIT_SERV_UUID_CMPLT_LIST     (0x05)
#define AD_TYPE_128_BIT_SERV_UUID               (0x06)
#define AD_TYPE_128_BIT_SERV_UUID_CMPLT_LIST    (0x07)
/**
 * @}
 */

/**
 * @name Local name AD types
 * @{
 */
#define AD_TYPE_SHORTENED_LOCAL_NAME            (0x08)
#define AD_TYPE_COMPLETE_LOCAL_NAME             (0x09)
/**
 * @}
 */

/** TX power level AD type*/
#define AD_TYPE_TX_POWER_LEVEL                  (0x0A)

/** Class of device */
#define AD_TYPE_CLASS_OF_DEVICE	                (0x0D)

/** Security manager TK value AD type */
#define AD_TYPE_SEC_MGR_TK_VALUE                (0x10)

/** Security manager OOB flags */
#define AD_TYPE_SEC_MGR_OOB_FLAGS               (0x11)

/** Slave connection interval AD type */
#define AD_TYPE_SLAVE_CONN_INTERVAL	            (0x12)

/**
 * @name Service solicitation UUID list AD types
 * @{
 */
#define AD_TYPE_SERV_SOLICIT_16_BIT_UUID_LIST   (0x14)
#define AD_TYPE_SERV_SOLICIT_32_BIT_UUID_LIST   (0x1F)
#define AD_TYPE_SERV_SOLICIT_128_BIT_UUID_LIST  (0x15)
/**
 * @}
 */

/** Service data AD type */
#define AD_TYPE_16_BIT_UUID_SERVICE_DATA        (0x16)
#define AD_TYPE_32_BIT_UUID_SERVICE_DATA        (0x20)
#define AD_TYPE_128_BIT_UUID_SERVICE_DATA       (0x21)

/** Manufaturer specific data AD type */
#define AD_TYPE_MANUFACTURER_SPECIFIC_DATA      (0xFF)

/**
 * @}
 */


/**
 * @defgroup GAP_Modes GAP modes
 * @{
 */
#define GAP_MODE_NON_DISCOVERABLE             0
#define GAP_MODE_LIMITED_DISCOVERABLE         1
#define GAP_MODE_GENERAL_DISCOVERABLE         2
#define GAP_MODE_BROADCAST                    3
/**
 * @}
 */

/**
 * @name Privacy type
 * @{
 */
#define PRIVACY_DISABLED                        (0x00)
#define PRIVACY_HOST_ENABLED                    (0x01)
#define PRIVACY_CONTROLLER_ENABLED              (0x02)
/**
 * @}
 */
   
/**
 * @name Operation
 * @{
 */   
#define ADV_COMPLETE_DATA						(0x03)
#define ADV_UNCHANGED_DATA						(0x04)   
/**
 * @}
 */

/**
 * @name Recommended advertising intervals
 * Intervals in terms of 625 micro seconds
 * @{
 */
#define GAP_ADV_INT_MIN_USER_INITIATED_CONN          (48)   /* 30 ms  */
#define GAP_ADV_INT_MAX_USER_INITIATED_CONN          (96)   /* 60 ms  */
#define GAP_ADV_INT_MIN_USER_INITIATED_NONCONN       (160)  /* 100 ms */
#define GAP_ADV_INT_MAX_USER_INITIATED_NONCONN       (240)  /* 150 ms */
#define GAP_ADV_INT_MIN_BACKGROUND                   (1600) /* 1 s    */
#define GAP_ADV_INT_MAX_BACKGROUND                   (1920) /* 1.2 s  */   
/**
 * @}
 */

/**
 * @anchor gap_roles
 * @name GAP Roles
 * @{
*/
#define GAP_PERIPHERAL_ROLE                     (0x01)
#define GAP_BROADCASTER_ROLE                    (0x02)
#define GAP_CENTRAL_ROLE                        (0x04)
#define GAP_OBSERVER_ROLE                       (0x08)
/**
 * @}
 */

/**
 * @anchor gap_procedure_codes
 * @name GAP procedure codes
 * Procedure codes for aci_gap_start_procedure()
 * and aci_gap_terminate_proc() commands.
 * @{
 */    
#define GAP_LIMITED_DISCOVERY_PROC                  (0x00)
#define GAP_GENERAL_DISCOVERY_PROC                  (0x01)
#define GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC      (0x02)
#define GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC   (0x03)
#define GAP_SELECTIVE_CONNECTION_ESTABLISHMENT_PROC (0x04)
#define GAP_OBSERVATION_PROC                        (0x05)
#define GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC    (0x06)
#define GAP_NAME_DISCOVERY_PROC                     (0x07)

/**
 * @}
 */


/**
 * @}
 */

#endif /* __GAP_H__ */
