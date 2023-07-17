/**
 ******************************************************************************
 * @file    BLE_Implementation.c
 * @author  SRA
 * @brief   BLE Implementation
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
#include "OTA.h"
#include "App_model.h"
#include "ble_stream_class.h"
#include "UtilTask.h"
#include "services/SQuery.h"

/* Private Types ------------------------------------------------------------*/
typedef struct
{

  uint8_t mlc_id;
  BLE_NotifyEvent_t mlc_enabled;
  uint8_t mlc_out[9];
  uint8_t mlc_data_ready;

  uint8_t pnpl_id;

  /* for sensor data define circular buffer to accumulate until it's able to send it */

}BLE_stream;

BLE_stream customStream;


/* Exported Variables --------------------------------------------------------*/
volatile uint8_t paired = FALSE;
uint32_t SizeOfUpdateBlueFW = 0;
volatile uint32_t NeedToClearSecureDB=0;

/* Private variables ---------------------------------------------------------*/
static uint32_t NeedToRebootBoard = 0;
static uint32_t NeedToSwapBanks = 0;



/* Imported Variables --------------------------------------------------------*/
uint8_t CurrentActiveBank = 0;

/* Private functions ---------------------------------------------------------*/
static uint32_t DebugConsoleParsing(uint8_t *att_data, uint8_t data_length);
static void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t Address_Type, uint8_t Addr[6]);
static void DisconnectionCompletedFunction(void);
static void PairingCompletedFunction(uint8_t PairingStatus);
static uint32_t DebugConsoleCommandParsing(uint8_t *att_data, uint8_t data_length);

static void MLCisNotificationSubscribed(BLE_NotifyEvent_t Event);

/* Private defines -----------------------------------------------------------*/

/* STM32 Unique ID */
#define STM32_UUID ((uint32_t *)UID_BASE)

/* STM32 MCU_ID */
#define STM32_MCU_ID ((uint32_t *)DBGMCU_BASE)

/**********************************************************************************************
 * Callback functions prototypes to manage the extended configuration characteristic commands *
 **********************************************************************************************/
static void ExtExtConfigUidCommandCallback(uint8_t **UID);
static void ExtConfigInfoCommandCallback(uint8_t *Answer);
static void ExtConfigHelpCommandCallback(uint8_t *Answer);
static void ExtConfigVersionFwCommandCallback(uint8_t *Answer);
static void ExtConfigClearDBCommandCallback(void);
static void ExtConfigSetNameCommandCallback(uint8_t *NewName);

static void ExtConfigReadBanksFwIdCommandCallback(uint8_t *CurBank, uint16_t *FwId1, uint16_t *FwId2);
static void ExtConfigBanksSwapCommandCallback(void);

static void BLE_SetCustomStreamID(void);
static sys_error_code_t BLE_PostCustomData(uint8_t id_stream, uint8_t *buf, uint32_t size);
static void BLE_SendCustomData(uint8_t id_stream);
static void BLE_SendCommand(char *buf, uint32_t size);

/**
 * @brief Initialize the BlueNRG stack and services
 * @param  None
 * @retval None
 */
void BLE_BluetoothInit(void)
{
  /* BlueNRG stack setting */
  BLE_StackValue.ConfigValueOffsets = CONFIG_DATA_PUBADDR_OFFSET;
  BLE_StackValue.ConfigValuelength = CONFIG_DATA_PUBADDR_LEN;
  BLE_StackValue.GAP_Roles = GAP_PERIPHERAL_ROLE;
  BLE_StackValue.IO_capabilities = IO_CAP_DISPLAY_ONLY;
  BLE_StackValue.AuthenticationRequirements = BONDING;
  BLE_StackValue.MITM_ProtectionRequirements = MITM_PROTECTION_REQUIRED;
  BLE_StackValue.SecureConnectionSupportOptionCode = SC_IS_SUPPORTED;
  BLE_StackValue.SecureConnectionKeypressNotification = KEYPRESS_IS_NOT_SUPPORTED;

  /* Set the Board Name */
  {
    uint8_t *BoardName = ReadFlashBoardName();

    if(BoardName!=NULL) {
      /* If there is Saved Board Name */
      memcpy(BLE_StackValue.BoardName,BoardName,8);
    } else {
      /* Use the Default Board Name */
      sprintf(BLE_StackValue.BoardName,"%s",BLE_FW_PACKAGENAME);
    }
  }

  /* To set the TX power level of the bluetooth device ( -2 dBm )*/
  BLE_StackValue.EnableHighPowerMode = 0; /*  Low Power */

  /* Values: 0x00 ... 0x31 - The value depends on the device */
  BLE_StackValue.PowerAmplifierOutputLevel = 25;

  BLE_StackValue.AdvIntervalMin= ADV_INTERV_MIN;
  BLE_StackValue.AdvIntervalMax= ADV_INTERV_MAX;

  BLE_StackValue.OwnAddressType = RANDOM_ADDR;

  /* BlueNRG services setting */
  BLE_StackValue.EnableConfig = 0;
  BLE_StackValue.EnableConsole = 1;
  BLE_StackValue.EnableExtConfig = 1;

  /* For Enabling the Secure Connection */
  BLE_StackValue.EnableSecureConnection = 0;
  /* Default Secure PIN */
  BLE_StackValue.SecurePIN = 123456;
  /* For creating a Random Secure PIN */
  BLE_StackValue.EnableRandomSecurePIN = 0;

  if(BLE_StackValue.EnableSecureConnection)
  {
    /* Using the Secure Connection, the Rescan should be done by BLE chip */
    BLE_StackValue.ForceRescan = 0;
  }
  else
  {
    BLE_StackValue.ForceRescan = 1;
  }

  InitBleManager();

  /* Check if we are running from Bank1 or Bank2 */
  FLASH_OBProgramInitTypeDef    OBInit;
  /* Allow Access to Flash control registers and user Flash */
  HAL_FLASH_Unlock();
  /* Allow Access to option bytes sector */
  HAL_FLASH_OB_Unlock();
  /* Get the Dual boot configuration status */
  HAL_FLASHEx_OBGetConfig(&OBInit);
  if (((OBInit.USERConfig) & (OB_SWAP_BANK_ENABLE)) == OB_SWAP_BANK_ENABLE)
  {
    CurrentActiveBank= 2;
  }
  else
  {
    CurrentActiveBank= 1;
  }
  HAL_FLASH_OB_Lock();
  HAL_FLASH_Lock();

  //Update the Current Fw ID saved in flash if it's neceessary
  UpdateCurrFlashBankFwIdBoardName(PnPLGetFWID(), NULL);

  char mac_string[18];
  sprintf(mac_string, "%x:%x:%x:%x:%x:%x", BLE_StackValue.BleMacAddress[5], BLE_StackValue.BleMacAddress[4], BLE_StackValue.BleMacAddress[3],
          BLE_StackValue.BleMacAddress[2], BLE_StackValue.BleMacAddress[1], BLE_StackValue.BleMacAddress[0]);
  set_mac_address(mac_string);
}

/**
 * @brief  Enable BlueNRG-LP Interrupt.
 * @param  None
 * @retval None
 */
void InitBLEIntForBlueNRGLP(void)
{
  HAL_EXTI_GetHandle(&hexti11, EXTI_LINE_11);
  HAL_NVIC_SetPriority(HCI_TL_SPI_EXTI_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(HCI_TL_SPI_EXTI_IRQn);
}

/**
 * @brief  Custom Service Initialization.
 * @param  None
 * @retval None
 */
void BLE_InitCustomService(void)
{
  /* Custom Function for Debug Console Command parsing */
  CustomDebugConsoleParsingCallback = &DebugConsoleParsing;

  /* Custom Function for Disconnection Completed */
  CustomDisconnectionCompleted = &DisconnectionCompletedFunction;

  /* Custom Function for Connection Completed */
  CustomConnectionCompleted = &ConnectionCompletedFunction;

  CustomPairingCompleted = &PairingCompletedFunction;

  /* Enable notification callback */

  CustomNotifyEventMachineLearningCore = &MLCisNotificationSubscribed;

  /* Define Custom Function for Write Request PnPLike */
  CustomWriteRequestPnPLike = &WriteRequestCommandLikeFunction;

  /***************************************/

  /* Service initialization PnPLike feature */
  if(BleManagerAddChar(BLE_InitPnPLikeService()) == 0)
  {
    PRINT_DBG("Error adding PnPLike characteristic\r\n");
  }

  /* Service initialization for Machine Learning Core feature */
  if(BleManagerAddChar(BLE_InitMachineLearningCoreService(BLE_MLC_4_REG)) == 0)
  {
    PRINT_DBG("Error adding Machine Learning Core characteristic\r\n");
  }

  /* Service initialization for Machine Learning Core feature */
  if(BleManagerAddChar(BLE_InitHighSpeedDataLogService()) == 0)
  {
    PRINT_DBG("Error adding HSD characteristic\r\n");
  }

  /***************************************/

  /***********************************************************************************
   * Callback functions to manage the extended configuration characteristic commands *
   ***********************************************************************************/
  CustomExtConfigUidCommandCallback = &ExtExtConfigUidCommandCallback;
  CustomExtConfigInfoCommandCallback = &ExtConfigInfoCommandCallback;
  CustomExtConfigHelpCommandCallback = &ExtConfigHelpCommandCallback;
  CustomExtConfigVersionFwCommandCallback = &ExtConfigVersionFwCommandCallback;
  CustomExtConfigClearDBCommandCallback = &ExtConfigClearDBCommandCallback;
  CustomExtConfigSetNameCommandCallback   = ExtConfigSetNameCommandCallback;

  CustomExtConfigReadBanksFwIdCommandCallback       = &ExtConfigReadBanksFwIdCommandCallback;
  {
    uint16_t FwId1,FwId2;

    ReadFlashBanksFwId(&FwId1,&FwId2);
    if(FwId2!=OTA_OTA_FW_ID_NOT_VALID) {
      /* Enable the BanksSwap only if there is a valid fw on second bank */
      CustomExtConfigBanksSwapCommandCallback           = &ExtConfigBanksSwapCommandCallback;
    }
  }

  /* Init custom ble stream callbakc */
  ble_stream_SetCustomStreamIDCallback = &BLE_SetCustomStreamID;
  ble_stream_PostCustomDataCallback = &BLE_PostCustomData;
  ble_stream_SendCustomDataCallback = &BLE_SendCustomData;
  ble_stream_SendCommandCallback = &BLE_SendCommand;

  /* Custom stream initialization */
  customStream.mlc_enabled = BLE_NOTIFY_UNSUB;
  customStream.mlc_data_ready = 0;

}

uint8_t BLE_GetFWID(void)
{
  return PnPLGetFWID();
}


/**
 * @brief Assign ID to each stream
 * @param  None
 * @retval None
 */
static void BLE_SetCustomStreamID(void)
{
  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  customStream.mlc_id = SQNextByNameAndType(&querySM, "lsm6dsv16x", COM_TYPE_MLC);
}

/**
 * @brief Save data in the relevant buffer
 * @param  uint8_t id_stream indicates the correct stream to be filled
 * @param  uint8_t *buf data to be saved
 * @param  uint32_t size number of byte to be saved
 * @retval None
 */
static sys_error_code_t BLE_PostCustomData(uint8_t id_stream, uint8_t *buf, uint32_t size)
{
  sys_error_code_t res = SYS_NOT_IMPLEMENTED_ERROR_CODE;

  if(id_stream == customStream.mlc_id)
  {
    memcpy(customStream.mlc_out, buf, size);
    customStream.mlc_data_ready = 1;
    res = SYS_NO_ERROR_CODE;
  }

  return res;

}

/**
 * @brief Send data on the relevant characteristics if possible
 * @param  uint8_t id_stream indicates the correct stream to be sent
 * @retval None
 */
static void BLE_SendCustomData(uint8_t id_stream)
{
  if(id_stream == customStream.mlc_id)
  {
    if(customStream.mlc_data_ready && (customStream.mlc_enabled == BLE_NOTIFY_SUB))
    {
      BLE_MachineLearningCoreUpdate((uint8_t*) &customStream.mlc_out, (uint8_t*) &customStream.mlc_out[8]);
    }
  }

}

static void BLE_SendCommand(char *buf, uint32_t size)
{
  tBleStatus ret;
  uint32_t j = 0, chunk, tot_len;
  uint8_t *buffer_out;
  uint32_t length_wTP;

  if ((size % 19U) == 0U)
  {
    length_wTP = (size / 19U) + size;
  }
  else
  {
    length_wTP = (size / 19U) + 1U + size;
  }

  buffer_out = BLE_MallocFunction(sizeof(uint8_t) * length_wTP);

  if (buffer_out == NULL)
  {
    BLE_MANAGER_PRINTF("Error: Mem calloc error [%ld]: %d@%s\r\n", length_wTP, __LINE__, __FILE__);
    // TODO: manage error
    return;
  }
  else
  {
    tot_len = BLE_Command_TP_Encapsulate(buffer_out, (uint8_t *) buf, size, 20);

    j = 0;

    /* Data are sent as notifications*/
    while (j < tot_len)
    {
      /* TODO: different MTU must be managed with MaxBLECharLen */

      chunk = MIN(20, tot_len - j);

      ret = BLE_PnPLikeUpdate(&buffer_out[j], chunk);

      if (ret == BLE_STATUS_INSUFFICIENT_RESOURCES)
      {
        ble_sendMSG_wait();
      }
      else if (ret == BLE_STATUS_SUCCESS)
      {
        j += chunk;
      }
    }
  }

  BLE_FreeFunction(buffer_out);
}


/**
* @brief  This function makes the parsing of the Debug Console
* @param  uint8_t *att_data attribute data
* @param  uint8_t data_length length of the data
* @retval uint32_t SendBackData true/false
*/
static uint32_t DebugConsoleParsing(uint8_t * att_data, uint8_t data_length)
{
  /* By default Answer with the same message received */
  uint32_t SendBackData =1;

  if(SizeOfUpdateBlueFW!=0) {
    /* Firmware update */
    int8_t RetValue = UpdateFWBlueMS(&SizeOfUpdateBlueFW,att_data, data_length,1);
    if(RetValue!=0) {
      Term_Update(((uint8_t *)&RetValue),1);
      if(RetValue==1) {
        /* if OTA checked */
        PRINT_DBG("%s will restart after the disconnection\r\n",BLE_FW_PACKAGENAME);
        HAL_Delay(1000);
        NeedToSwapBanks=1;

        /* Swap the Flash Banks */
        SwitchBank();
      }
    }
    SendBackData=0;
  } else {
    /* Received one write from Client on Terminal characteristc */
    SendBackData = DebugConsoleCommandParsing(att_data,data_length);
  }

  return SendBackData;
}


/**
 * @brief  This function makes the parsing of the Debug Console Commands
 * @param  uint8_t *att_data attribute data
 * @param  uint8_t data_length length of the data
 * @retval uint32_t SendBackData true/false
 */
static uint32_t DebugConsoleCommandParsing(uint8_t * att_data, uint8_t data_length)
{
  uint32_t SendBackData = 1;

  /* Help Command */
  if(!strncmp("help",(char *)(att_data),4)) {
    /* Print Legend */
    SendBackData=0;

    BytesToWrite =sprintf((char *)BufferToWrite,
                          "info\n"
                          "clearDB\n");
    Term_Update(BufferToWrite,BytesToWrite);

  } else if(!strncmp("versionFw",(char *)(att_data),9)) {
    BytesToWrite =sprintf((char *)BufferToWrite,"%s_%s_%c.%c.%c\r\n",
                          "U585",
                          BLE_FW_PACKAGENAME,
                          FW_VERSION_MAJOR,
                          FW_VERSION_MINOR,
                          FW_VERSION_PATCH);
    Term_Update(BufferToWrite,BytesToWrite);
    SendBackData=0;
  } else if(!strncmp("info",(char *)(att_data),4)) {
    SendBackData=0;

    BytesToWrite =sprintf((char *)BufferToWrite,"\r\nSTMicroelectronics %s:\n"
       "\tVersion %c.%c.%c\n"
      "\tSensorTile.box PRO board"
        "\n",
        BLE_FW_PACKAGENAME,
        FW_VERSION_MAJOR,FW_VERSION_MINOR,FW_VERSION_PATCH);
    Term_Update(BufferToWrite,BytesToWrite);

    BytesToWrite =sprintf((char *)BufferToWrite,"\t(HAL %ld.%ld.%ld_%ld)\n"
      "\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
      " (IAR)\n",
#elif defined (__CC_ARM)
      " (KEIL)\n",
#elif defined (__GNUC__)
      " (STM32CubeIDE)\n",
#endif
    (long)(HAL_GetHalVersion() >>24),
    (long)((HAL_GetHalVersion() >>16)&0xFF),
    (long)((HAL_GetHalVersion() >> 8)&0xFF),
    (long)(HAL_GetHalVersion()      &0xFF),
    __DATE__,__TIME__);
    Term_Update(BufferToWrite,BytesToWrite);
    BytesToWrite =sprintf((char *)BufferToWrite,"Current Bank =%d\n",CurrentActiveBank);
    Term_Update(BufferToWrite,BytesToWrite);
  } else if(!strncmp("upgradeFw",(char *)(att_data),9)) {
    uint32_t uwCRCValue;
    uint8_t *PointerByte = (uint8_t*) &SizeOfUpdateBlueFW;

    PointerByte[0]=att_data[ 9];
    PointerByte[1]=att_data[10];
    PointerByte[2]=att_data[11];
    PointerByte[3]=att_data[12];

    /* Check the Maximum Possible OTA size */
    if(SizeOfUpdateBlueFW>OTA_MAX_PROG_SIZE) {
      PRINT_DBG("OTA %s SIZE=%ld > %d Max Allowed\r\n",BLE_FW_PACKAGENAME,SizeOfUpdateBlueFW, OTA_MAX_PROG_SIZE);
      /* Answer with a wrong CRC value for signaling the problem to BlueMS application */
      BufferToWrite[0]= att_data[13];
      BufferToWrite[1]=(att_data[14]!=0) ? 0 : 1;/* In order to be sure to have a wrong CRC */
      BufferToWrite[2]= att_data[15];
      BufferToWrite[3]= att_data[16];
      BytesToWrite = 4;
      Term_Update(BufferToWrite,BytesToWrite);
    } else {
      PointerByte = (uint8_t*) &uwCRCValue;
      PointerByte[0]=att_data[13];
      PointerByte[1]=att_data[14];
      PointerByte[2]=att_data[15];
      PointerByte[3]=att_data[16];

      PRINT_DBG("OTA %s SIZE=%ld uwCRCValue=%lx\r\n",BLE_FW_PACKAGENAME,SizeOfUpdateBlueFW,uwCRCValue);

      /* Reset the Flash */
      StartUpdateFWBlueMS(SizeOfUpdateBlueFW,uwCRCValue);

      /* Signal that we are ready sending back the CRV value*/
      BufferToWrite[0] = PointerByte[0];
      BufferToWrite[1] = PointerByte[1];
      BufferToWrite[2] = PointerByte[2];
      BufferToWrite[3] = PointerByte[3];
      BytesToWrite = 4;
      Term_Update(BufferToWrite,BytesToWrite);
    }

    SendBackData=0;
  }  else if(!strncmp("uid",(char *)(att_data),3)) {
    /* Write back the STM32 UID */
    uint8_t *uid = (uint8_t *)STM32_UUID;
    uint32_t MCU_ID = STM32_MCU_ID[0]&0xFFF;
    BytesToWrite =sprintf((char *)BufferToWrite,"%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X_%.3lX\n",
                          uid[ 3],uid[ 2],uid[ 1],uid[ 0],
                          uid[ 7],uid[ 6],uid[ 5],uid[ 4],
                          uid[11],uid[ 10],uid[9],uid[8],
                          (unsigned long)MCU_ID);
    Term_Update(BufferToWrite,BytesToWrite);
    SendBackData=0;
  } else  if(!strncmp("clearDB",(char *)(att_data),7)) {
    BytesToWrite =sprintf((char *)BufferToWrite,"\nThe Secure database will be cleared\n");
    Term_Update(BufferToWrite,BytesToWrite);
    NeedToClearSecureDB=1;
    SendBackData=0;
  }

  return SendBackData;
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  None
 * @retval None
 */
static void ConnectionCompletedFunction(uint16_t ConnectionHandle, uint8_t Address_Type, uint8_t Addr[6])
{

  aci_l2cap_connection_parameter_update_req(ConnectionHandle,
                                              6 /* interval_min*/,
                                              6 /* interval_max */,
                                              0   /* slave_latency */,
                                              400 /*timeout_multiplier*/);


  /* Check if the device is already bonded */
  if( aci_gap_is_device_bonded(Address_Type,Addr) ==BLE_STATUS_SUCCESS) {
    PRINT_DBG("Device already bounded\r\n");
    paired = TRUE;
  }

  streamMsg_t msg;
  msg.messageId = BLE_ISTREAM_MSG_CONNECTED;
  msg.streamID = 0;
  ble_stream_msg(&msg);

}

/**
 * @brief  This function is called when the peer device get disconnected.
 * @param  None 
 * @retval None
 */
static void DisconnectionCompletedFunction(void)
{
  paired = FALSE;
  set_connectable = TRUE;

  /* Reset for any problem during FOTA update */
  SizeOfUpdateBlueFW = 0;

  if(NeedToRebootBoard)
  {
    NeedToRebootBoard = 0;
    HAL_NVIC_SystemReset();
  }

  if(NeedToSwapBanks)
  {
    NeedToSwapBanks = 0;
    SwitchBank();
  }

  HAL_Delay(100);

  streamMsg_t msg;
  msg.messageId = BLE_ISTREAM_MSG_DISCONNECTED;
  ble_stream_msg(&msg);

  setConnectable();
}



/**
 * @brief  This function is called when there is a Pairing Complete event.
 * @param  uint8_t PairingStatus 
 * @retval None
 */
static void PairingCompletedFunction(uint8_t PairingStatus)
{
  /* Pairing Status:
   * 0x00 -> "Success",
   * 0x01 -> "Timeout",
   * 0x02 -> "Pairing Failed",
   * 0x03 -> "Encryption failed, LTK missing on local device",
   * 0x04 -> "Encryption failed, LTK missing on peer device",
   * 0x05 -> "Encryption not supported by remote device"
   */
  if(PairingStatus == 0x00 /* Success */)
  {
    paired = TRUE;
  }
  else
  {
    paired = FALSE;
  }
}

/**
 * @brief  This function is called when the characteristic is subscribed or unsubscribed
 * @param  event Enum type for Service Notification Change
 * @retval None
 */
static void MLCisNotificationSubscribed(BLE_NotifyEvent_t Event)
{
  customStream.mlc_enabled = Event;
}

/***********************************************************************************
 * Callback functions to manage the extended configuration characteristic commands *
 ***********************************************************************************/

/**
 * @brief  Callback Function for answering to the UID command
 * @param  uint8_t **UID STM32 UID Return value
 * @retval None
 */
static void ExtExtConfigUidCommandCallback(uint8_t **UID)
{
  *UID = (uint8_t*) STM32_UUID;
}

/**
 * @brief  Callback Function for answering to ClearDB Command
 * @param  None
 * @retval None
 */
static void ExtConfigClearDBCommandCallback(void)
{
  NeedToClearSecureDB = 1;
}



/**
 * @brief  Callback Function for answering to Info command
 * @param  uint8_t *Answer Return String
 * @retval None
 */
static void ExtConfigInfoCommandCallback(uint8_t *Answer)
{
  sprintf((char *)Answer,"\r\nSTMicroelectronics %s:\n"
       "\tVersion %c.%c.%c\n"
      "\tSensorTile.box PRO board"
      "\n\t(HAL %ld.%ld.%ld_%ld)\n"
      "\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
      " (IAR)\n"
#elif defined (__CC_ARM)
      " (KEIL)\n"
#elif defined (__GNUC__)
      " (STM32CubeIDE)\n"
#endif
       "\tCurrent Bank =%ld\n",
        BLE_FW_PACKAGENAME,
        FW_VERSION_MAJOR,FW_VERSION_MINOR,FW_VERSION_PATCH,
        (long)(HAL_GetHalVersion() >>24),
        (long)((HAL_GetHalVersion() >>16)&0xFF),
        (long)((HAL_GetHalVersion() >> 8)&0xFF),
        (long)(HAL_GetHalVersion()      &0xFF),
         __DATE__,__TIME__,
        (long)CurrentActiveBank);
}

/**
 * @brief  Callback Function for answering to SetName command
 * @param  uint8_t *NewName New Name
 * @retval None
 */
static void ExtConfigSetNameCommandCallback(uint8_t *NewName)
{
  PRINT_DBG("Received a new Board's Name=%s\r\n",NewName);
  /* Update the Board's name in flash */
  UpdateCurrFlashBankFwIdBoardName(PnPLGetFWID(),NewName);

  /* Update the Name for BLE Advertise */
  sprintf(BLE_StackValue.BoardName,"%s",NewName);
}

/**
 * @brief  Callback Function for answering to Help command
 * @param  uint8_t *Answer Return String
 * @retval None
 */
static void ExtConfigHelpCommandCallback(uint8_t *Answer)
{
  sprintf((char*) Answer, "Help Message.....");
}

/**
 * @brief  Callback Function for answering to VersionFw command
 * @param  uint8_t *Answer Return String
 * @retval None
 */
static void ExtConfigVersionFwCommandCallback(uint8_t *Answer)
{
  sprintf((char *)Answer,"%s_%s_%c.%c.%c\r\n",
                  "U585",
                  BLE_FW_PACKAGENAME,
                  FW_VERSION_MAJOR,
                  FW_VERSION_MINOR,
                  FW_VERSION_PATCH);
}


/**
 * @brief  Callback Function for answering to ReadBanksFwId command
 * @param  uint8_t *CurBank Number Current Bank
 * @param  uint16_t *FwId1 Bank1 Firmware Id
 * @param  uint16_t *FwId2 Bank2 Firmware Id
 * @retval None
 */
static void ExtConfigReadBanksFwIdCommandCallback (uint8_t *CurBank,uint16_t *FwId1,uint16_t *FwId2)
{
  ReadFlashBanksFwId(FwId1,FwId2);
  *CurBank=CurrentActiveBank;
}


/**
 * @brief  Callback Function for answering to BanksSwap command
 * @param  None
 * @retval None
 */
static void ExtConfigBanksSwapCommandCallback(void)
{
  uint16_t FwId1, FwId2;

  ReadFlashBanksFwId(&FwId1, &FwId2);
  if(FwId2 != OTA_OTA_FW_ID_NOT_VALID)
  {
    PRINT_DBG("Swapping to Bank%d\n", (CurrentActiveBank == 1) ? 0 : 1);
    PRINT_DBG("%s will restart after the disconnection\r\n", BLE_FW_PACKAGENAME);
    HAL_Delay(1000);
    NeedToSwapBanks = 1;
  }
  else
  {
    PRINT_DBG("Not Valid fw on Bank%d\n\tCommand Rejected\n", (CurrentActiveBank == 1) ? 0 : 1);
    PRINT_DBG("\tLoad a Firmware on Bank%d\n", (CurrentActiveBank == 1) ? 0 : 1);
  }
}
