/**
  ******************************************************************************
  * @file    BLE_NeaiExtrapolation.c
  * @author  System Research & Applications Team - System LAB DU.
  * @version 1.0.0
  * @date    2-May-2024
  * @brief   NEAI Extrapolation info services APIs.
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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "BLE_Manager.h"
#include "BLE_ManagerCommon.h"
#include "services/sysdebug.h"

/* Private define ------------------------------------------------------------*/
#define COPY_NEAI_EXTRAPOLATION_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x24,0x00,0x02,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#ifndef NEAI_STUB_LIB_ID
#define NEAI_STUB_LIB_ID  "st_stub21212121212121212"
#endif

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventNeaiExtrapolation_t  CustomNotifyEventE=NULL;
CustomWriteRequestExtrapolation_t  CustomWriteRequestE=NULL;
/* Private Types ----------------------------------------------------------- */

/* Private variables ---------------------------------------------------------*/

/* Data structure pointer for NEAI Extrapolation info service */
static BleCharTypeDef ExtBleChar;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_Neai_Extrapolation(void *BleCharPointer,uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data);
static void Write_Request_Neai_Extrapolation(void *BleCharPointer,uint16_t handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data);

BleCharTypeDef* BLE_InitNeaiExtrapolationService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer= NULL;

  /* Init data structure pointer for N-Class Extrapolation info service */
  BleCharPointer = &ExtBleChar;
  memset(BleCharPointer,0,sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB= AttrMod_Request_Neai_Extrapolation;
  BleCharPointer->Write_Request_CB = Write_Request_Neai_Extrapolation;
  COPY_NEAI_EXTRAPOLATION_CHAR_UUID((BleCharPointer->uuid));

  BleCharPointer->Char_UUID_Type= UUID_TYPE_128;
  BleCharPointer->Char_Value_Length= 248U;
  BleCharPointer->Char_Properties= ((uint8_t)(CHAR_PROP_NOTIFY))|((uint8_t)(CHAR_PROP_WRITE));
  BleCharPointer->Security_Permissions= ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask= GATT_NOTIFY_ATTRIBUTE_WRITE;
  BleCharPointer->Enc_Key_Size= 16;
  BleCharPointer->Is_Variable= 1;

  if(CustomWriteRequestE == NULL) {
    BLE_MANAGER_PRINTF("CustomWriteRequestNeaiExtrapolation function Not Defined\r\n");
  }

  BLE_MANAGER_PRINTF("BLE NEAI Extrapolation char is ok\r\n");

  return BleCharPointer;
}

/**
* @brief  Update NEAI Extrapolation Characteristic Value
* @param  BLE_E_output_t output contains info about:
* - phase = (IDLE=0x00) | (EXTRAPOLATION=0x01) | (BUSY=0x02)
* - state = (NEAI_OK=0x00) | (NEAI_NOINIT=0x01) | ...
* - extrapolated_value = ...
* - measurement_unit = ...
* - timestamp = ...
*
* ONLY THE PHASE FIELD IS MANDATORY
*
* @retval tBleStatus:          Status
*/
tBleStatus BLE_NeaiExtrapolationUpdate(BLE_E_output_t output)
{
  tBleStatus ret;
  JSON_Value *rootJSON;
  JSON_Object *JSON_Status;
  char *serializedJSON;
  uint32_t size;

  rootJSON = json_value_init_object();
  JSON_Status = json_value_get_object(rootJSON);

  if(output.phase == NEAI_EXT_PHASE_IDLE){
    json_object_dotset_number(JSON_Status, "phase", NEAI_EXT_PHASE_IDLE);
  }
  else if(output.phase == NEAI_EXT_PHASE_BUSY){
    json_object_dotset_number(JSON_Status, "phase", NEAI_EXT_PHASE_BUSY);
  }
  else if(output.phase == NEAI_EXT_PHASE_EXTRAPOLATION){
    ret = json_object_dotset_number(JSON_Status, "phase", output.phase);
    ret = json_object_dotset_number(JSON_Status, "state", output.state);
    if(output.state == NEAI_EXT_STATE_OK){
      ret = json_object_dotset_number(JSON_Status, "target", output.extrapolated_value);
      if(strlen(output.measurement_unit)<6)
        ret = json_object_dotset_string(JSON_Status, "unit", output.measurement_unit);
      else
        ret = json_object_dotset_string(JSON_Status, "unit", "unit");
    }
  }
  if (strcmp(NEAI_STUB_LIB_ID, NEAI_ID_E) != 0)
    ret = json_object_dotset_boolean(JSON_Status, "stub", false);

  ret = json_object_dotset_number(JSON_Status, "timestamp", HAL_GetTick());

  serializedJSON = json_serialize_to_string(rootJSON);
  size = json_serialization_size(rootJSON);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ExtBleChar, 0, size, (uint8_t*)serializedJSON);

  json_free_serialized_string(serializedJSON);
  json_value_free(rootJSON);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS){
    if(BLE_StdErr_Service==BLE_SERV_ENABLE){
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating NEAI Extrapolation Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      BLE_MANAGER_PRINTF("Error: Updating NEAI Extrapolation Char\r\n");
    }
  }
  return ret;
}

/**
 * @brief  This function is called when there is a change on the gatt attribute
 *         With this function it's possible to understand if Piano is subscribed or not to the one service
 * @param  void *VoidCharPointer
 * @param  uint16_t attr_handle Handle of the attribute
 * @param  uint16_t Offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
 *                          - Bits 0-14: offset of the reported value inside the attribute.
 *                          - Bit 15: if the entire value of the attribute does not fit inside a single ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event,
 *                            this bit is set to 1 to notify that other ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
 * @param  uint8_t data_length length of the data
 * @param  uint8_t *att_data attribute data
 * @retval None
 */

static void AttrMod_Request_Neai_Extrapolation(void *VoidCharPointer,uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data)
{
  if(CustomNotifyEventE!=NULL) {
    if (att_data[0] == 01U) {
      CustomNotifyEventE(BLE_NOTIFY_SUB);
    } else if (att_data[0] == 0U){
      CustomNotifyEventE(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else {
    BLE_MANAGER_PRINTF("CustomNotifyEventEXT function Not Defined\r\n");
  }

  if(BLE_StdTerm_Service==BLE_SERV_ENABLE) {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite,"--->NEAI_E=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite,BytesToWrite);
  } else {
    BLE_MANAGER_PRINTF("--->NEAI_E=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif
}

/**
 * @brief  This event is given when a write request is received by the server from the client.
 * @param  void *VoidCharPointer
 * @param  uint16_t handle Handle of the attribute
 * @retval None
 */
static void Write_Request_Neai_Extrapolation(void *BleCharPointer,uint16_t handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data)
{
  if(CustomWriteRequestE != NULL) {
    CustomWriteRequestE(att_data, data_length);
  } else {
    BLE_MANAGER_PRINTF("\r\n\nWrite request EXT not defined\r\n\n");
  }
}
