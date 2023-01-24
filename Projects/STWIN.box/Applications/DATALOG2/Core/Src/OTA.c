/**
  ******************************************************************************
  * @file    OTA.c
  * @author  SRA
  * @brief   Over-the-Air Update API implementation
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *                             
  *
  ******************************************************************************
  */
  
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "STWIN.box.h"
#include "OTA.h"
#include "bluenrg_conf.h"
#include "services/sysdebug.h"

/* Local types ---------------------------------------------------------------*/
/* Typedef FwId and BoardName Structure */
typedef struct
{
  uint32_t FwIdMagicNum;
  uint32_t BoardNameMagicNum;
  uint8_t  BoardName[8]; // 7 Char + termination
  uint16_t FwId;
  uint8_t  Padding[14]; //we could write Multiply of 16 bytes at a time...
} FwId_BoardName_t;

/* Local defines -------------------------------------------------------------*/
/* Board FW OTA Position */
/* The 2 addresses are equal due to swap address in dual boot mode */
#define OTA_ADDRESS_START_BANK 0x08100000
#define FW_ID_BOARD_NAME_BANK1 0x080FE000
#define FW_ID_BOARD_NAME_BANK2 0x081FE000

/* Magic number for a valid Fw Id saved on flash */
#define FW_ID_BOARD_NAME_MAGIC_NUM    0xDEADBEEF

/* Uncomment the following define for enabling the PRINTF capability */
//#define OTA_ENABLE_PRINTF

#ifdef OTA_ENABLE_PRINTF
#define OTA_PRINTF(msg)  SYS_DEBUGF2(SYS_DBG_LEVEL_VERBOSE, msg)
#else /* OTA_ENABLE_PRINTF */
#define OTA_PRINTF(...)
#endif /* OTA_ENABLE_PRINTF */

/* Local Macros --------------------------------------------------------------*/
#define OTA_ERROR_FUNCTION() { while(1);}

/* Private variables ---------------------------------------------------------*/
static uint32_t SizeOfUpdateBlueFW = 0;
static uint32_t AspecteduwCRCValue = 0;
static uint32_t WritingAddress;
static uint32_t BufferValueToWrite[4];
static uint32_t ValuesSavedOnBuffer = 0;
static uint8_t *PointerToBuffer = (uint8_t *) BufferValueToWrite;

/* Variables for Saving FwId and Board's Name for the 2 banks */
static FwId_BoardName_t CurrentFwInfo, OtherBankFwInfo;

/* Private Function Prototypes -----------------------------------------------*/
static void DeleteOtherFlashBankFwId(void);

/* Exported functions --------------------------------------------------------*/
extern int32_t CurrentActiveBank;
/**
  * @brief Function for Updating the Firmware
  * @param uint32_t *SizeOfUpdate Remaining size of the firmware image [bytes]
  * @param uint8_t *att_data attribute data
  * @param int32_t data_length length of the data
  * @param uint8_t WriteMagicNum 1/0 for writing or not the magic number
  * @retval int8_t Return value for checking purpouse (1/-1 == Ok/Error)
  */
int8_t UpdateFWBlueMS(uint32_t *SizeOfUpdate, uint8_t *att_data, int32_t data_length, uint8_t WriteMagicNum)
{
  int8_t ReturnValue = 0;
  /* Save the Packed received */

  if (data_length > (*SizeOfUpdate))
  {
    /* Too many bytes...Something wrong... necessity to send it again... */
    OTA_PRINTF(("OTA something wrong"));
    ReturnValue = -1;
    /* Reset for Restarting again */
    *SizeOfUpdate = 0;
  }
  else
  {
    int32_t Counter;
    int32_t FirstChunk = 0;
    int32_t LastMult16 = 0;
    int32_t Written = 0;
    /* Save the received OTA packed ad save it to flash */
    /* Disable instruction cache prior to internal cacheable memory update */
    if (HAL_ICACHE_Disable() != HAL_OK)
    {
      OTA_ERROR_FUNCTION();
    }
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    /* Fill Remaming bytes for reaching 16bytes */
    if (ValuesSavedOnBuffer != 0)
    {
      FirstChunk = (16 - ValuesSavedOnBuffer);
      if (FirstChunk > data_length)
      {
        FirstChunk = data_length;
      }
      memcpy(PointerToBuffer, att_data, FirstChunk);
      Written += FirstChunk;
      ValuesSavedOnBuffer += FirstChunk;

      /* If we have enough data */
      if (ValuesSavedOnBuffer == 16)
      {
        ValuesSavedOnBuffer = 0;
        PointerToBuffer = (uint8_t *) BufferValueToWrite;
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, WritingAddress, ((uint32_t)BufferValueToWrite)) == HAL_OK)
        {
          WritingAddress += 16;
        }
        else
        {
          /* Error occurred while writing data in Flash memory.
             User can add here some code to deal with this error
             FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
          OTA_ERROR_FUNCTION();
        }
      }
    }

    /* We move at steps of 16 */
    LastMult16 = ((uint32_t)(data_length - FirstChunk)) & (~(((uint32_t)0xF)));
    for (Counter = FirstChunk; Counter < LastMult16; Counter += 16)
    {
      memcpy(PointerToBuffer, att_data + Counter, 16);
      Written += 16;
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, WritingAddress, ((uint32_t)BufferValueToWrite)) == HAL_OK)
      {
        WritingAddress += 16;
      }
      else
      {
        /* Error occurred while writing data in Flash memory.
           User can add here some code to deal with this error
           FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
        OTA_ERROR_FUNCTION();
      }
    }

    /* Last Section of this chunk */
    if (Written < data_length)
    {
      memcpy(PointerToBuffer, att_data + Written, (data_length - Written));
      ValuesSavedOnBuffer += (data_length - Written);
      PointerToBuffer += (data_length - Written);
    }

    /* Reduce the remaining bytes for OTA completion */
    *SizeOfUpdate -= data_length;

    if (*SizeOfUpdate == 0)
    {
      //Check if we need to dump the last bytes
      if (ValuesSavedOnBuffer != 0)
      {
        ValuesSavedOnBuffer = 0;
        PointerToBuffer = (uint8_t *) BufferValueToWrite;
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, WritingAddress, ((uint32_t)BufferValueToWrite)) != HAL_OK)
        {
          /* Error occurred while writing data in Flash memory.
             User can add here some code to deal with this error
             FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
          OTA_ERROR_FUNCTION();
        }
      }

      /* We had received the whole firmware and we have saved it in Flash */
      OTA_PRINTF(("OTA Update saved\r\n"));

      if (WriteMagicNum)
      {
        uint32_t uwCRCValue = 0;

        if (AspecteduwCRCValue)
        {
          /* Make the CRC integrity check */
          /* CRC handler declaration */
          CRC_HandleTypeDef   CrcHandle;

          /* Init CRC for OTA-integrity check */
          CrcHandle.Instance = CRC;
          /* The default polynomial is used */
          CrcHandle.Init.DefaultPolynomialUse    = DEFAULT_POLYNOMIAL_ENABLE;

          /* The default init value is used */
          CrcHandle.Init.DefaultInitValueUse     = DEFAULT_INIT_VALUE_ENABLE;

          /* The input data are not inverted */
          CrcHandle.Init.InputDataInversionMode  = CRC_INPUTDATA_INVERSION_NONE;

          /* The output data are not inverted */
          CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;

          /* The input data are 32-bit long words */
          CrcHandle.InputDataFormat              = CRC_INPUTDATA_FORMAT_WORDS;

          __HAL_RCC_CRC_CLK_ENABLE();

          if (HAL_CRC_GetState(&CrcHandle) != HAL_CRC_STATE_RESET)
          {
            HAL_CRC_DeInit(&CrcHandle);
          }

          if (HAL_CRC_Init(&CrcHandle) != HAL_OK)
          {
            /* Initialization Error */
            OTA_ERROR_FUNCTION();
          }
          else
          {
            OTA_PRINTF(("CRC  Initialized\n\r"));
          }
          /* Compute the CRC */
          uwCRCValue = HAL_CRC_Calculate(&CrcHandle, (uint32_t *)OTA_ADDRESS_START_BANK, SizeOfUpdateBlueFW >> 2);

          if (uwCRCValue == AspecteduwCRCValue)
          {
            ReturnValue = 1;
            OTA_PRINTF(("OTA CRC-checked\r\n"));
          }
          else
          {
            OTA_PRINTF(("OTA Error CRC-checking\r\n"));
          }
        }
        else
        {
          ReturnValue = 1;

        }
        if (ReturnValue != 1)
        {
          ReturnValue = -1;
          if (AspecteduwCRCValue)
          {
            OTA_PRINTF(("Wrong CRC!"));
          }
        }
      }
    }

    /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();

    /* Re-enable instruction cache */
    if (HAL_ICACHE_Enable() != HAL_OK)
    {
      OTA_ERROR_FUNCTION();
    }
  }
  return ReturnValue;
}

/**
  * @brief Start Function for Updating the Firmware
  * @param uint32_t SizeOfUpdate  size of the firmware image [bytes]
  * @param uint32_t uwCRCValue expected CRV value
  * @retval None
  */
void StartUpdateFWBlueMS(uint32_t SizeOfUpdate, uint32_t uwCRCValue)
{
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;
  OTA_PRINTF(("Start FLASH Erase\r\n"));

  SizeOfUpdateBlueFW = SizeOfUpdate;
  AspecteduwCRCValue = uwCRCValue;
  ValuesSavedOnBuffer = 0;
  PointerToBuffer = (uint8_t *) BufferValueToWrite;

  WritingAddress = OTA_ADDRESS_START_BANK;

  if (CurrentActiveBank == 1)
  {
    EraseInitStruct.Banks       = FLASH_BANK_2;
  }
  else
  {
    EraseInitStruct.Banks       = FLASH_BANK_1;
  }

  EraseInitStruct.Page        = 0;
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.NbPages     = (SizeOfUpdate + 16 + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;

  /* Disable instruction cache prior to internal cacheable memory update */
  if (HAL_ICACHE_Disable() != HAL_OK)
  {
    OTA_ERROR_FUNCTION();
  }

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  {
    /* Error occurred while sector erase.
      User can add here some code to deal with this error.
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
      FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
    OTA_ERROR_FUNCTION();
  }
  else
  {
    OTA_PRINTF(("End FLASH Erase %ld Pages of %dBytes\r\n"));
  }

  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Re-enable instruction cache */
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    OTA_ERROR_FUNCTION();
  }

  /* Delete Fw Id saved on the bank where we will make the FOTA */
  DeleteOtherFlashBankFwId();
}

/**
  * @brief Function for reading the Fw ID of the 2 Banks
  * @param uint16_t *FwId1 Firmware Id of the Current Bank
  * @param uint16_t *FwId2 Firmware Id of the Other Bank
  * @retval None
  */
void ReadFlashBanksFwId(uint16_t *FwId1, uint16_t *FwId2)
{
  /* Current Bank */
  memcpy((void *)&CurrentFwInfo, (void *)FW_ID_BOARD_NAME_BANK1, sizeof(FwId_BoardName_t));
  if (CurrentFwInfo.FwIdMagicNum == FW_ID_BOARD_NAME_MAGIC_NUM)
  {
    *FwId1 = CurrentFwInfo.FwId;
  }
  else
  {
    *FwId1 = OTA_OTA_FW_ID_NOT_VALID;
  }

  /* Other Bank */
  memcpy((void *)&OtherBankFwInfo, (void *)FW_ID_BOARD_NAME_BANK2, sizeof(FwId_BoardName_t));
  if (OtherBankFwInfo.FwIdMagicNum == FW_ID_BOARD_NAME_MAGIC_NUM)
  {
    *FwId2 = OtherBankFwInfo.FwId;
  }
  else
  {
    *FwId2 = OTA_OTA_FW_ID_NOT_VALID;
  }
}

/**
  * @brief Function for deleting the Fw Id of the other Bank Before Fota
  * @param None
  * @retval None
  */
static void DeleteOtherFlashBankFwId(void)
{
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;
  uint32_t LocalWritingAddress;

  /* Read the Other Bank's info */
  if (CurrentActiveBank == 1)
  {
    EraseInitStruct.Banks  = FLASH_BANK_2;
  }
  else
  {
    EraseInitStruct.Banks  = FLASH_BANK_1;
  }

  memcpy((void *)&OtherBankFwInfo, (void *)FW_ID_BOARD_NAME_BANK2, sizeof(FwId_BoardName_t));
  LocalWritingAddress = FW_ID_BOARD_NAME_BANK2;

  EraseInitStruct.Page        = FLASH_PAGE_NB - 1;
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.NbPages     = 1;

  /* Disable instruction cache prior to internal cacheable memory update */
  if (HAL_ICACHE_Disable() != HAL_OK)
  {
    OTA_ERROR_FUNCTION();
  }

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  {
    /* Error occurred while sector erase.
      User can add here some code to deal with this error.
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
      FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
    OTA_ERROR_FUNCTION();
  }
  else
  {
    OTA_PRINTF(("End FLASH Erase"));
  }

  /* Save the updated Bank's info */
  {
    uint32_t BankInfoAddress = (uint32_t)&OtherBankFwInfo;

    /* Change the Fw Id to Not Valid */
    OtherBankFwInfo.FwIdMagicNum = 0x00;
    OtherBankFwInfo.FwId = OTA_OTA_FW_ID_NOT_VALID;
    /* We Don't Delete any Eventual Board Name Saved */

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, LocalWritingAddress, BankInfoAddress) == HAL_OK)
    {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, LocalWritingAddress + 16, BankInfoAddress + 16) == HAL_OK)
      {
      }
      else
      {
        OTA_ERROR_FUNCTION();
      }
    }
    else
    {
      OTA_ERROR_FUNCTION();
    }
  }
  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Re-enable instruction cache */
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    OTA_ERROR_FUNCTION();
  }
}

/**
  * @brief Function for updating the Fw Id and or Board Name of the current Bank if it's necessary
  * @param uint16_t CurrFwId Current FwId
  * @param uint8_t *NewName Current Board's Name (if it's == NULL don't change it)
  * @retval None
  */
void UpdateCurrFlashBankFwIdBoardName(uint16_t FwId, uint8_t *NewName)
{
  /* Read the Actual Banks' Info */
  memcpy((void *)&CurrentFwInfo, (void *)FW_ID_BOARD_NAME_BANK1, sizeof(FwId_BoardName_t));

  if (NewName != NULL)
  {
    /* We need to change both the Banks' info */
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;
    uint32_t LocalWritingAddress;
    uint32_t BankInfoAddress;

    /* Read the Other Banks' Info */
    memcpy((void *)&OtherBankFwInfo, (void *)FW_ID_BOARD_NAME_BANK2, sizeof(FwId_BoardName_t));

    /* Updated the Board Name and the Fw Id for the current Bank */
    CurrentFwInfo.BoardNameMagicNum = FW_ID_BOARD_NAME_MAGIC_NUM;
    memcpy(CurrentFwInfo.BoardName, NewName, 7);
    CurrentFwInfo.BoardName[7] = '\0';
    CurrentFwInfo.FwId = FwId;
    CurrentFwInfo.FwIdMagicNum = FW_ID_BOARD_NAME_MAGIC_NUM;

    /* Update the Board Name also for Other Bank */
    OtherBankFwInfo.BoardNameMagicNum = FW_ID_BOARD_NAME_MAGIC_NUM;
    memcpy(OtherBankFwInfo.BoardName, NewName, 7);
    OtherBankFwInfo.BoardName[7] = '\0';
    /* We don't change any Eventual FwId Present on the Other Banks */

    /* Disable instruction cache prior to internal cacheable memory update */
    if (HAL_ICACHE_Disable() != HAL_OK)
    {
      OTA_ERROR_FUNCTION();
    }

    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    EraseInitStruct.Page        = FLASH_PAGE_NB - 1;
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.NbPages     = 1;

    OTA_PRINTF(("Start FLASH Erase Current Bank\r\n"));

    if (CurrentActiveBank == 1)
    {
      EraseInitStruct.Banks  = FLASH_BANK_1;
    }
    else
    {
      EraseInitStruct.Banks  = FLASH_BANK_2;
    }

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
    {
      /* Error occurred while sector erase.
        User can add here some code to deal with this error.
        SectorError will contain the faulty sector and then to know the code error on this sector,
        user can call function 'HAL_FLASH_GetError()'
        FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
      OTA_ERROR_FUNCTION();
    }
    else
    {
      OTA_PRINTF(("End FLASH Erase"));
    }

    OTA_PRINTF(("Update Current Bank Info\r\n"));

    LocalWritingAddress = FW_ID_BOARD_NAME_BANK1;
    BankInfoAddress = (uint32_t)&CurrentFwInfo;
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, LocalWritingAddress, BankInfoAddress) == HAL_OK)
    {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, LocalWritingAddress + 16, BankInfoAddress + 16) == HAL_OK)
      {
      }
      else
      {
        OTA_ERROR_FUNCTION();
      }
    }
    else
    {
      OTA_ERROR_FUNCTION();
    }

    OTA_PRINTF(("Start FLASH Erase Other Bank\r\n"));

    if (CurrentActiveBank == 1)
    {
      EraseInitStruct.Banks  = FLASH_BANK_2;
    }
    else
    {
      EraseInitStruct.Banks  = FLASH_BANK_1;
    }

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
    {
      /* Error occurred while sector erase.
        User can add here some code to deal with this error.
        SectorError will contain the faulty sector and then to know the code error on this sector,
        user can call function 'HAL_FLASH_GetError()'
        FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
      OTA_ERROR_FUNCTION();
    }
    else
    {
      OTA_PRINTF(("End FLASH Erase"));
    }

    OTA_PRINTF(("Update Other Bank Info\r\n"));

    LocalWritingAddress = FW_ID_BOARD_NAME_BANK2;
    BankInfoAddress = (uint32_t)&OtherBankFwInfo;
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, LocalWritingAddress, BankInfoAddress) == HAL_OK)
    {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, LocalWritingAddress + 16, BankInfoAddress + 16) == HAL_OK)
      {
      }
      else
      {
        OTA_ERROR_FUNCTION();
      }
    }
    else
    {
      OTA_ERROR_FUNCTION();
    }

    /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();

    /* Re-enable instruction cache */
    if (HAL_ICACHE_Enable() != HAL_OK)
    {
      OTA_ERROR_FUNCTION();
    }

  }
  else if (CurrentFwInfo.FwId != FwId)
  {
    /* We need to update the Current Firmware Id */
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;
    uint32_t LocalWritingAddress;
    uint32_t BankInfoAddress;

    CurrentFwInfo.FwId = FwId;
    CurrentFwInfo.FwIdMagicNum = FW_ID_BOARD_NAME_MAGIC_NUM;
    /* We keep Any present Board's Name saved on Flash */

    /* Disable instruction cache prior to internal cacheable memory update */
    if (HAL_ICACHE_Disable() != HAL_OK)
    {
      OTA_ERROR_FUNCTION();
    }

    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    EraseInitStruct.Page        = FLASH_PAGE_NB - 1;
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.NbPages     = 1;

    OTA_PRINTF(("Start FLASH Erase Current Bank\r\n"));

    if (CurrentActiveBank == 1)
    {
      EraseInitStruct.Banks  = FLASH_BANK_1;
    }
    else
    {
      EraseInitStruct.Banks  = FLASH_BANK_2;
    }

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
    {
      /* Error occurred while sector erase.
        User can add here some code to deal with this error.
        SectorError will contain the faulty sector and then to know the code error on this sector,
        user can call function 'HAL_FLASH_GetError()'
        FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
      OTA_ERROR_FUNCTION();
    }
    else
    {
      OTA_PRINTF(("End FLASH Erase"));
    }

    OTA_PRINTF(("Update Current Bank Info\r\n"));

    LocalWritingAddress = FW_ID_BOARD_NAME_BANK1;
    BankInfoAddress = (uint32_t)&CurrentFwInfo;
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, LocalWritingAddress, BankInfoAddress) == HAL_OK)
    {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, LocalWritingAddress + 16, BankInfoAddress + 16) == HAL_OK)
      {
      }
      else
      {
        OTA_ERROR_FUNCTION();
      }
    }
    else
    {
      OTA_ERROR_FUNCTION();
    }

    /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();

    /* Re-enable instruction cache */
    if (HAL_ICACHE_Enable() != HAL_OK)
    {
      OTA_ERROR_FUNCTION();
    }
  }
}

/**
  * @brief API for reading the Board Name saved on Flash
  * @param None
  * @retval Current Board's Name
  */
uint8_t *ReadFlashBoardName(void)
{
  uint8_t *RetValue = NULL;

  memcpy((void *)&CurrentFwInfo, (void *)FW_ID_BOARD_NAME_BANK1, sizeof(FwId_BoardName_t));
  if (CurrentFwInfo.BoardNameMagicNum == FW_ID_BOARD_NAME_MAGIC_NUM)
  {
    RetValue = CurrentFwInfo.BoardName;
  }
  return RetValue;
}


