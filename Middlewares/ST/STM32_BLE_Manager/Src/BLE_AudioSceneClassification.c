/**
  ******************************************************************************
  * @file    BLE_AudioSceneClasssification.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.8.0
  * @date    02-December-2022
  * @brief   Add Audio Scene Classification service using vendor specific profiles.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private define ------------------------------------------------------------*/
#define COPY_AUDIO_SCENE_CLASS_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x03,0x00,0x02,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Exported variables --------------------------------------------------------*/

CustomNotifyEventAudioSceneClass_t CustomNotifyEventAudioSceneClass=NULL;
CustomReadRequestAudioSceneClass_t CustomReadRequestAudioSceneClass=NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Audio Scene Classification service */
static BleCharTypeDef BleCharAudioSceneClass;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_AudioSceneClass(void *BleCharPointer,uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data);
#if (BLUE_CORE != BLUENRG_LP)
static void Read_Request_AudioSceneClass(void *BleCharPointer,uint16_t handle);
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_AudioSceneClass(void *BleCharPointer,
                             uint16_t handle,
                             uint16_t Connection_Handle,
                             uint8_t Operation_Type,
                             uint16_t Attr_Val_Offset,
                             uint8_t Data_Length,
                             uint8_t Data[]);
#endif /* (BLUE_CORE != BLUENRG_LP) */

/**
 * @brief  Init Audio Scene Classification service
 * @param  None
 * @retval BleCharTypeDef* BleCharPointer: Data structure pointer forActivity Classification service
 */
BleCharTypeDef* BLE_InitAudioSceneClassService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for Activity Classification info service */
  BleCharPointer = &BleCharAudioSceneClass;
  memset(BleCharPointer,0,sizeof(BleCharTypeDef));  
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_AudioSceneClass;
  BleCharPointer->Read_Request_CB= Read_Request_AudioSceneClass;
  COPY_AUDIO_SCENE_CLASS_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type =UUID_TYPE_128;
  BleCharPointer->Char_Value_Length=2+1; /* 2 byte timestamp, 1 byte aucoustic scene classification */
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_READ);
  BleCharPointer->Security_Permissions=ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask=GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size=16;
  BleCharPointer->Is_Variable=0;
  
  if(CustomReadRequestAudioSceneClass == NULL) {
    BLE_MANAGER_PRINTF("Error: Read request Audio Scene Classification function not defined\r\n");
  }
  
  BLE_MANAGER_PRINTF("BLE Audio Scene Classification features ok\r\n");
  
  return BleCharPointer;
}


/**
 * @brief  Update Audio Scene Classification characteristic
 * @param  BLE_ASC_output_t ASC_Code Audio Scene Classification Code
 * @retval tBleStatus   Status
 */
tBleStatus BLE_AudioSceneClassUpdate(BLE_ASC_output_t ASC_Code)
{  
  tBleStatus ret;
  uint8_t buff[2+1];

  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  buff[2] = (uint8_t) ASC_Code;

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharAudioSceneClass, 0, 2+1,buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS){
    if(BLE_StdErr_Service==BLE_SERV_ENABLE){
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating ASC Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      BLE_MANAGER_PRINTF("Error Updating ASC Char\r\n");
    }
  }
  return ret;
}

/**
 * @brief  This function is called when there is a change on the gatt attribute
 *         With this function it's possible to understand if battery is subscribed or not to the one service
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
static void AttrMod_Request_AudioSceneClass(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data)
{
  if(CustomNotifyEventAudioSceneClass!=NULL) {
    if (att_data[0] == 01U) {
      CustomNotifyEventAudioSceneClass(BLE_NOTIFY_SUB);
    } else if (att_data[0] == 0U){
      CustomNotifyEventAudioSceneClass(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else {
     BLE_MANAGER_PRINTF("CustomNotifyEventAudioSceneClass function Not Defined\r\n");
  }
  
 if(BLE_StdTerm_Service==BLE_SERV_ENABLE) {
   BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite,"--->ASC=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
   Term_Update(BufferToWrite,BytesToWrite);
 } else {
   BLE_MANAGER_PRINTF("--->ASC=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
 }
#endif
}

/**
 * @brief  This event is given when a read request is received by the server from the client.
 * @param  void *VoidCharPointer
 * @param  uint16_t handle Handle of the attribute
 * @retval None
 */
#if (BLUE_CORE != BLUENRG_LP)
static void Read_Request_AudioSceneClass(void *VoidCharPointer,uint16_t handle)
{
  if(CustomReadRequestAudioSceneClass != NULL) {
    BLE_ASC_output_t ASC_Code;
    CustomReadRequestAudioSceneClass(&ASC_Code);
    BLE_AudioSceneClassUpdate(ASC_Code);
  } else {
    BLE_MANAGER_PRINTF("\r\n\nRead request ASC function not defined\r\n\n");
  }
}
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_AudioSceneClass(void *BleCharPointer,
                                         uint16_t handle,
                                         uint16_t Connection_Handle,
                                         uint8_t Operation_Type,
                                         uint16_t Attr_Val_Offset,
                                         uint8_t Data_Length,
                                         uint8_t Data[])
{
  tBleStatus ret;
  if(CustomReadRequestAudioSceneClass != NULL) {
    BLE_ASC_output_t ASC_Code;
    uint8_t buff[2+1];

    CustomReadRequestAudioSceneClass(&ASC_Code);
 
    STORE_LE_16(buff  ,(HAL_GetTick()>>3));
    buff[2] = (uint8_t) ASC_Code;
    
    ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, 2+1,buff);
    if (ret != (tBleStatus)BLE_STATUS_SUCCESS){
      if(BLE_StdErr_Service==BLE_SERV_ENABLE){
        BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating ASC Char\n");
        Stderr_Update(BufferToWrite,BytesToWrite);
      } else {
        BLE_MANAGER_PRINTF("Error: Updating ASC Char\r\n");
      }
    }
  } else {
    BLE_MANAGER_PRINTF("\r\n\nRead request ASC function not defined\r\n\n");
  }
  
  ret = aci_gatt_srv_authorize_resp_nwk(Connection_Handle, handle,
                                      Operation_Type, 0, Attr_Val_Offset,
                                      Data_Length, Data);
  if( ret != BLE_STATUS_SUCCESS) {
    BLE_MANAGER_PRINTF("aci_gatt_srv_authorize_resp_nwk() failed: 0x%02x\r\n", ret);
  }
}
#endif /* (BLUE_CORE != BLUENRG_LP) */


