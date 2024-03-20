/**
  ******************************************************************************
  * @file    BLEDualProgram\Src\OTA.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.5.0
  * @date    27-Mar-2023
  * @brief   Over-the-Air Update API implementation
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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32l4xx_hal.h"
#include "OTA.h"
#include "bluenrg_conf.h"
#include "services/sysdebug.h"

/* Local types ---------------------------------------------------------------*/

/* Local defines -------------------------------------------------------------*/
/* Board FW OTA Position */
/* The 2 addresses are equal due to swap address in dual boot mode */
#define OTA_ADDRESS_START_BANK  0x08100000
#define OTA_FW_ID_BANK1 0x080FF000
#define OTA_FW_ID_BANK2 0x081FF000
#define OTA_FW_ID_MAGIC_NUM 0xDEADBEEF

/* Uncomment the following define for enabling the PRINTF capability if it's supported */
//#define OTA_ENABLE_PRINTF

#ifdef OTA_ENABLE_PRINTF
#define OTA_PRINTF(msg)  SYS_DEBUGF2(SYS_DBG_LEVEL_VERBOSE, msg)
#else /* OTA_ENABLE_PRINTF */
#define OTA_PRINTF(...)
#endif /* OTA_ENABLE_PRINTF */

/* Local Macros -------------------------------------------------------------*/
#define OTA_ERROR_FUNCTION() { while(1);}

/* Private variables ---------------------------------------------------------*/
static uint32_t SizeOfUpdateBlueFW = 0;
static uint32_t AspecteduwCRCValue = 0;
static uint32_t WritingAddress;

extern int32_t CurrentActiveBank;

/* CRC handler declaration */
CRC_HandleTypeDef   CrcHandle;

/* Private Function Prototypes -----------------------------------------------*/
static void DeleteOtherFlashBankFwId(void);

/* Exported functions  --------------------------------------------------*/

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
    OTA_PRINTF("OTA something wrong data_length=%ld RemSizeOfUpdate=%ld....\r\nPlease Try again\r\n", data_length,
               (*SizeOfUpdate));
    ReturnValue = -1;
    /* Reset for Restarting again */
    *SizeOfUpdate = 0;
  }
  else
  {
    uint64_t ValueToWrite;
    int32_t Counter;
    //OTA_PRINTF("OTA chunk data_length=%ld RemSizeOfUpdate=%ld\r\n",data_length,(*SizeOfUpdate));
    /* Save the received OTA packed ad save it to flash */
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    for (Counter = 0; Counter < data_length; Counter += 8)
    {
      memcpy((uint8_t *) &ValueToWrite, att_data + Counter, 8);

      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, WritingAddress, ValueToWrite) == HAL_OK)
      {
        WritingAddress += 8;
      }
      else
      {
        /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error
         FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
        OTA_ERROR_FUNCTION();
      }
    }
    /* Reduce the remaining bytes for OTA completion */
    *SizeOfUpdate -= data_length;

    if (*SizeOfUpdate == 0)
    {
      /* We had received the whole firmware and we have saved it in Flash */
      OTA_PRINTF("OTA Update saved\r\n");

      if (WriteMagicNum)
      {
        uint32_t uwCRCValue = 0;

        if (AspecteduwCRCValue)
        {
          /* Init CRC for OTA-integrity check */
          CrcHandle.Instance = CRC;
          /* The default polynomial is used */
          CrcHandle.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;

          /* The default init value is used */
          CrcHandle.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;

          /* The input data are not inverted */
          CrcHandle.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;

          /* The output data are not inverted */
          CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;

          /*  32-bit CRC length */
          CrcHandle.Init.CRCLength = CRC_POLYLENGTH_32B;

          /* The input data are 32-bit long words */
          CrcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;

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
            OTA_PRINTF("CRC  Initialized\n\r");
          }
          /* Compute the CRC */
          uwCRCValue = HAL_CRC_Calculate(&CrcHandle, (uint32_t *) OTA_ADDRESS_START_BANK, SizeOfUpdateBlueFW >> 2);

          if (uwCRCValue == AspecteduwCRCValue)
          {
            ReturnValue = 1;
            OTA_PRINTF("OTA CRC-checked\r\n");
          }
          else
          {
            OTA_PRINTF("OTA Error CRC-checking\r\n");
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
            OTA_PRINTF("Wrong CRC! Computed=%lx  expected=%lx ... Try again\r\n", uwCRCValue, AspecteduwCRCValue);
          }
        }
      }
    }

    /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
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
  OTA_PRINTF("Start FLASH Erase\r\n");

  SizeOfUpdateBlueFW = SizeOfUpdate;
  AspecteduwCRCValue = uwCRCValue;

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

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Clear OPTVERR bit set on virgin samples */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
  /* Clear PEMPTY bit set (as the code is executed from Flash which is not empty) */
  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PEMPTY) != 0)
  {
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PEMPTY);
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
    OTA_PRINTF("End FLASH Erase %ld Pages of 4KB\r\n", EraseInitStruct.NbPages);
  }

  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Delete Fw Id saved on the bank where we will make the FOTA */
  DeleteOtherFlashBankFwId();
}


/**
  * @brief Function for reading the Fw ID of the 2 Banks
  * @param uint16_t *FwId1 Firmware Id present on First Bank
  * @param uint16_t *FwId2 Firmware Id present on Second Bank
  * @retval None
  */
void ReadFlashBanksFwId(uint16_t *FwId1, uint16_t *FwId2)
{
  uint32_t *First;
  uint32_t *Second;

  //First/Second identification
  if (CurrentActiveBank == 1)
  {
    First  = (uint32_t *)OTA_FW_ID_BANK1;
    Second = (uint32_t *)OTA_FW_ID_BANK2;
  }
  else
  {
    First  = (uint32_t *)OTA_FW_ID_BANK2;
    Second = (uint32_t *)OTA_FW_ID_BANK1;
  }

  //Read First Bank
  if (First[0] == OTA_FW_ID_MAGIC_NUM)
  {
    *FwId1 = First[1] & 0xFFFF;
  }
  else
  {
    *FwId1 = OTA_OTA_FW_ID_NOT_VALID;
  }

  //Read Second Bank
  if (Second[0] == OTA_FW_ID_MAGIC_NUM)
  {
    *FwId2 = Second[1] & 0xFFFF;
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

  if (CurrentActiveBank == 1)
  {
    EraseInitStruct.Banks  = FLASH_BANK_2;
  }
  else
  {
    EraseInitStruct.Banks  = FLASH_BANK_1;
  }

  EraseInitStruct.Page        = 255;
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.NbPages     = 1;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Clear OPTVERR bit set on virgin samples */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
  /* Clear PEMPTY bit set (as the code is executed from Flash which is not empty) */
  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PEMPTY) != 0)
  {
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PEMPTY);
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
    OTA_PRINTF("End FLASH Erase %ld Pages of 4KB\r\n", EraseInitStruct.NbPages);
  }

  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

}

/**
  * @brief Function for updating the Fw Id of the current Bank if it's necessary
  * @param uint16_t FwId1 Firmware Id
  * @retval None
  */
void UpdateCurrFlashBankFwId(uint16_t FwId)
{
  uint16_t CurrFwId;

  CurrFwId = *((uint16_t *)(OTA_FW_ID_BANK1 + 4));

  /* Check if we need to update the Current Firmware Id */
  if (CurrFwId != FwId)
  {
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;
    OTA_PRINTF("Start FLASH Erase\r\n");
    uint64_t ValueToWrite;
    uint32_t *ValueToWrite32 = (uint32_t *) &ValueToWrite;
    static uint32_t LocalWritingAddress;

    LocalWritingAddress = OTA_FW_ID_BANK1;

    if (CurrentActiveBank == 1)
    {
      EraseInitStruct.Banks  = FLASH_BANK_1;
    }
    else
    {
      EraseInitStruct.Banks  = FLASH_BANK_2;
    }

    EraseInitStruct.Page        = 255;
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.NbPages     = 1;

    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
    /* Clear PEMPTY bit set (as the code is executed from Flash which is not empty) */
    if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PEMPTY) != 0)
    {
      __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PEMPTY);
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
      OTA_PRINTF("End FLASH Erase %ld Pages of 4KB\r\n", EraseInitStruct.NbPages);
    }

    HAL_FLASH_Lock();
    HAL_Delay(100);
    HAL_FLASH_Unlock();

    //* Update the Firmware id on Flash */
    ValueToWrite32[0] = OTA_FW_ID_MAGIC_NUM;
    ValueToWrite32[1] = FwId;
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, LocalWritingAddress, ValueToWrite) != HAL_OK)
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error
         FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
      OTA_ERROR_FUNCTION();
    }

    /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
  }
}

