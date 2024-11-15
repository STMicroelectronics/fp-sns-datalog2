/**
  ******************************************************************************
  * @file    App.c
  * @author  SRA
  * @brief   Define the Application main entry points
  *
  * The framework `weak` function are redefined in this file and they link
  * the application specific code with the framework.
  *
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

#include "services/sysdebug.h"
#include "services/ApplicationContext.h"
#include "AppPowerModeHelper.h"

#include "mx.h"

#include "UtilTask.h"
#include "SPIBusTask.h"
#include "I2CBusTask.h"
#include "IIS3DWBTask.h"
#include "ISM330ISTask.h"
#include "LSM6DSV16XTask.h"
#include "LSM6DSV16BXTask.h"
#include "LIS2MDLTask.h"
#include "LIS2DU12Task.h"
#include "LPS22DFTask.h"
#include "STTS22HTask.h"
#include "MP23DB01HPTask.h"

#include "DatalogAppTask.h"
#include "HardwareDetection.h"

#include "PnPLCompManager.h"
#include "Deviceinformation_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Lis2mdl_Mag_PnPL.h"
#include "Lis2du12_Acc_PnPL.h"
#include "Ism330is_Acc_PnPL.h"
#include "Ism330is_Gyro_PnPL.h"
#include "Ism330is_Ispu_PnPL.h"
#include "Stts22h_Temp_PnPL.h"
#include "Lps22df_Press_PnPL.h"
#include "Lsm6dsv16bx_Acc_PnPL.h"
#include "Lsm6dsv16bx_Gyro_PnPL.h"
#include "Lsm6dsv16bx_Mlc_PnPL.h"
#include "Lsm6dsv16x_Acc_PnPL.h"
#include "Lsm6dsv16x_Gyro_PnPL.h"
#include "Lsm6dsv16x_Mlc_PnPL.h"
#include "Mp23db01hp_Mic_PnPL.h"
#include "Automode_PnPL.h"
#include "parson.h"

static uint8_t BOARD_ID = BOARD_ID_PROA;
static uint8_t FW_ID = USB_FW_ID_DATALOG2_PROA;

static IPnPLComponent_t *pLSM6DSV16BX_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pLSM6DSV16BX_GYRO_PnPLObj = NULL;
static IPnPLComponent_t *pLSM6DSV16BX_MLC_PnPLObj = NULL;
static IPnPLComponent_t *pLSM6DSV16X_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pLSM6DSV16X_GYRO_PnPLObj = NULL;
static IPnPLComponent_t *pLSM6DSV16X_MLC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330IS_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330IS_GYRO_PnPLObj = NULL;
static IPnPLComponent_t *pISM330IS_ISPU_PnPLObj = NULL;
static IPnPLComponent_t *pLIS2MDL_MAG_PnPLObj = NULL;
static IPnPLComponent_t *pLIS2DU12_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pSTTS22H_TEMP_PnPLObj = NULL;
static IPnPLComponent_t *pLPS22DF_PRESS_PnPLObj = NULL;
static IPnPLComponent_t *pMP23DB01HP_MIC_PnPLObj = NULL;
static IPnPLComponent_t *pLogControllerPnPLObj = NULL;
static IPnPLComponent_t *pDeviceInfoPnPLObj = NULL;
static IPnPLComponent_t *pFirmwareInfoPnPLObj = NULL;
static IPnPLComponent_t *pAcquisitionInfoPnPLObj = NULL;
static IPnPLComponent_t *pTagsInfoPnPLObj = NULL;
static IPnPLComponent_t *pAutomodePnPLObj = NULL;

/**
  * Utility task object.
  */
static AManagedTaskEx *sUtilObj = NULL;

/**
  * SPI bus task object.
  */
static AManagedTaskEx *sSPI2BusObj = NULL;
static AManagedTaskEx *sSPI3BusObj = NULL;
static AManagedTaskEx *sI2C1BusObj = NULL;

/**
  * Sensor task object.
  */
static AManagedTaskEx *sLSM6DSV16BXObj = NULL;
static AManagedTaskEx *sLSM6DSV16XObj = NULL;
static AManagedTaskEx *sLIS2MDLObj = NULL;
static AManagedTaskEx *sLIS2DU12Obj = NULL;
static AManagedTaskEx *sLPS22DFObj = NULL;
static AManagedTaskEx *sSTTS22HObj = NULL;
static AManagedTaskEx *sMP23DB01HPObj = NULL;
static AManagedTaskEx *sISM330ISObj = NULL;


/**
  * DatalogApp
  */
static AManagedTaskEx *sDatalogAppObj = NULL;

/**
  * Pnpl mutex definition for thread safe purpose
  */
static TX_MUTEX pnpl_mutex;

/**
  * Private function declaration
  */
static void PnPL_lock_fp(void);
static void PnPL_unlock_fp(void);

/* eLooM framework entry points definition */
/*******************************************/

sys_error_code_t SysLoadApplicationContext(ApplicationContext *pAppContext)
{
  assert_param(pAppContext);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  boolean_t ext_lsm6dsv16bx = FALSE;
  boolean_t ext_iis330is = FALSE;
  hwd_st25dv_version st25dv_version;

  /* PnPL thread safe mutex creation */
  tx_mutex_create(&pnpl_mutex, "PnPL Mutex", TX_INHERIT);

  /* PnPL thread safe function registration */
  PnPL_SetLockUnlockCallbacks(PnPL_lock_fp, PnPL_unlock_fp);

  PnPLSetAllocationFunctions(SysAlloc, SysFree);

  /* Check availability of external sensors */
  ext_iis330is = HardwareDetection_Check_Ext_ISM330IS();
  ext_lsm6dsv16bx = HardwareDetection_Check_Ext_LSM6DSV16BX();

  /* Check NFC chip version */
  st25dv_version = HardwareDetection_Check_ST25DV();
  if (st25dv_version == ST25DV04KC)
  {
    BOARD_ID = BOARD_ID_PROB;
    FW_ID = USB_FW_ID_DATALOG2_PROB;
  }

  /* Allocate the task objects */
  sUtilObj = UtilTaskAlloc(&MX_GPIO_SW1InitParams, &MX_GPIO_LED1InitParams, &MX_GPIO_LED2InitParams, &MX_GPIO_LED3InitParams);
  sDatalogAppObj = DatalogAppTaskAlloc();
  sI2C1BusObj = I2CBusTaskAlloc(&MX_I2C1InitParams);
  sSPI2BusObj = SPIBusTaskAlloc(&MX_SPI2InitParams);
  sSPI3BusObj = SPIBusTaskAlloc(&MX_SPI3InitParams);
  sLIS2DU12Obj = LIS2DU12TaskAlloc(&MX_GPIO_ACC_INT2InitParams, &MX_GPIO_SPI_SEN_CS_AInitParams);
  sLIS2MDLObj = LIS2MDLTaskAlloc(NULL, NULL);
  sLPS22DFObj = LPS22DFTaskAlloc(NULL, NULL);
  sMP23DB01HPObj = MP23DB01HPTaskAlloc(&MX_ADF1InitParams);
  sSTTS22HObj = STTS22HTaskAlloc(NULL, NULL, 0x71);

  /* Use the external ISM330IS with ISPU or the onboard LSM6DSV16X with MLC */
  if (ext_iis330is)
  {
    sISM330ISObj = ISM330ISTaskAlloc(&MX_GPIO_INT2_EXTERNALInitParams, &MX_GPIO_INT1_EXTERNALInitParams, &MX_GPIO_CS_EXTERNALInitParams);
  }
  else if (ext_lsm6dsv16bx)
  {
    sLSM6DSV16BXObj = LSM6DSV16BXTaskAlloc(&MX_GPIO_INT1_EXTERNALInitParams, NULL, &MX_GPIO_CS_EXTERNALInitParams);
  }
  else
  {
    sLSM6DSV16XObj = LSM6DSV16XTaskAlloc(&MX_GPIO_IMU_INT1InitParams, NULL, &MX_GPIO_SPI_SEN_CS_GInitParams);
  }

  /* Add the task object to the context. */
  res = ACAddTask(pAppContext, (AManagedTask *) sUtilObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sDatalogAppObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sI2C1BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSPI2BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSPI3BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLIS2DU12Obj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLIS2MDLObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLPS22DFObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sMP23DB01HPObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSTTS22HObj);

  /* Use the external ISM330IS with ISPU or the onboard LSM6DSV16X with MLC */
  if (ext_iis330is)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sISM330ISObj);
  }
  else if (ext_lsm6dsv16bx)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sLSM6DSV16BXObj);
  }
  else
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sLSM6DSV16XObj);
  }

  pLIS2DU12_ACC_PnPLObj = Lis2du12_Acc_PnPLAlloc();
  pLIS2MDL_MAG_PnPLObj = Lis2mdl_Mag_PnPLAlloc();
  pLPS22DF_PRESS_PnPLObj = Lps22df_Press_PnPLAlloc();
  pMP23DB01HP_MIC_PnPLObj = Mp23db01hp_Mic_PnPLAlloc();
  pSTTS22H_TEMP_PnPLObj = Stts22h_Temp_PnPLAlloc();

  /* Use the external ISM330IS with ISPU or the onboard LSM6DSV16X with MLC */
  if (sISM330ISObj)
  {
    pISM330IS_ACC_PnPLObj = Ism330is_Acc_PnPLAlloc();
    pISM330IS_GYRO_PnPLObj = Ism330is_Gyro_PnPLAlloc();
    pISM330IS_ISPU_PnPLObj = Ism330is_Ispu_PnPLAlloc();
  }
  if (sLSM6DSV16BXObj)
  {
    pLSM6DSV16BX_ACC_PnPLObj = Lsm6dsv16bx_Acc_PnPLAlloc();
    pLSM6DSV16BX_GYRO_PnPLObj = Lsm6dsv16bx_Gyro_PnPLAlloc();
    pLSM6DSV16BX_MLC_PnPLObj = Lsm6dsv16bx_Mlc_PnPLAlloc();
  }
  if (sLSM6DSV16XObj)
  {
    pLSM6DSV16X_ACC_PnPLObj = Lsm6dsv16x_Acc_PnPLAlloc();
    pLSM6DSV16X_GYRO_PnPLObj = Lsm6dsv16x_Gyro_PnPLAlloc();
    pLSM6DSV16X_MLC_PnPLObj = Lsm6dsv16x_Mlc_PnPLAlloc();
  }

  pDeviceInfoPnPLObj = Deviceinformation_PnPLAlloc();
  pAcquisitionInfoPnPLObj = Acquisition_Info_PnPLAlloc();
  pTagsInfoPnPLObj = Tags_Info_PnPLAlloc();
  pFirmwareInfoPnPLObj = Firmware_Info_PnPLAlloc();
  pLogControllerPnPLObj = Log_Controller_PnPLAlloc();
  pAutomodePnPLObj = Automode_PnPLAlloc();

  PnPLSetBOARDID(BOARD_ID);
  PnPLSetFWID(FW_ID);

  return res;
}

sys_error_code_t SysOnStartApplication(ApplicationContext *pAppContext)
{
  UNUSED(pAppContext);

  /* connect the sensor task to the bus. */
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C1BusObj, (I2CBusIF *)LIS2MDLTaskGetSensorIF((LIS2MDLTask *) sLIS2MDLObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C1BusObj, (I2CBusIF *)LPS22DFTaskGetSensorIF((LPS22DFTask *) sLPS22DFObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C1BusObj, (I2CBusIF *)STTS22HTaskGetSensorIF((STTS22HTask *) sSTTS22HObj));
  SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)LIS2DU12TaskGetSensorIF((LIS2DU12Task *) sLIS2DU12Obj));

  /* Use the external ISM330IS with ISPU or the onboard LSM6DSV16X with MLC */
  if (sISM330ISObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI3BusObj, (SPIBusIF *)ISM330ISTaskGetSensorIF((ISM330ISTask *) sISM330ISObj));
  }
  if (sLSM6DSV16BXObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI3BusObj,
                            (SPIBusIF *)LSM6DSV16BXTaskGetSensorIF((LSM6DSV16BXTask *) sLSM6DSV16BXObj));
  }
  if (sLSM6DSV16XObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj,
                            (SPIBusIF *)LSM6DSV16XTaskGetSensorIF((LSM6DSV16XTask *) sLSM6DSV16XObj));
  }

  /************ Connect the Sensor events to the DatalogAppTask ************/
  IEventListener *DatalogAppListener = DatalogAppTask_GetEventListenerIF((DatalogAppTask *) sDatalogAppObj);
  IEventSrcAddEventListener(LIS2DU12TaskGetEventSrcIF((LIS2DU12Task *) sLIS2DU12Obj), DatalogAppListener);
  IEventSrcAddEventListener(LIS2MDLTaskGetMagEventSrcIF((LIS2MDLTask *) sLIS2MDLObj), DatalogAppListener);
  IEventSrcAddEventListener(LPS22DFTaskGetPressEventSrcIF((LPS22DFTask *) sLPS22DFObj), DatalogAppListener);
  IEventSrcAddEventListener(MP23DB01HPTaskGetEventSrcIF((MP23DB01HPTask *) sMP23DB01HPObj), DatalogAppListener);
  IEventSrcAddEventListener(STTS22HTaskGetTempEventSrcIF((STTS22HTask *) sSTTS22HObj), DatalogAppListener);

  /* Use the external ISM330IS with ISPU or the onboard LSM6DSV16X with MLC */
  if (sISM330ISObj)
  {
    IEventSrcAddEventListener(ISM330ISTaskGetAccEventSrcIF((ISM330ISTask *) sISM330ISObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM330ISTaskGetGyroEventSrcIF((ISM330ISTask *) sISM330ISObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM330ISTaskGetMlcEventSrcIF((ISM330ISTask *) sISM330ISObj), DatalogAppListener);
  }
  if (sLSM6DSV16BXObj)
  {
    IEventSrcAddEventListener(LSM6DSV16BXTaskGetAccEventSrcIF((LSM6DSV16BXTask *) sLSM6DSV16BXObj), DatalogAppListener);
    IEventSrcAddEventListener(LSM6DSV16BXTaskGetGyroEventSrcIF((LSM6DSV16BXTask *) sLSM6DSV16BXObj), DatalogAppListener);
    IEventSrcAddEventListener(LSM6DSV16BXTaskGetMlcEventSrcIF((LSM6DSV16BXTask *) sLSM6DSV16BXObj), DatalogAppListener);
  }
  if (sLSM6DSV16XObj)
  {
    IEventSrcAddEventListener(LSM6DSV16XTaskGetAccEventSrcIF((LSM6DSV16XTask *) sLSM6DSV16XObj), DatalogAppListener);
    IEventSrcAddEventListener(LSM6DSV16XTaskGetGyroEventSrcIF((LSM6DSV16XTask *) sLSM6DSV16XObj), DatalogAppListener);
    IEventSrcAddEventListener(LSM6DSV16XTaskGetMlcEventSrcIF((LSM6DSV16XTask *) sLSM6DSV16XObj), DatalogAppListener);
  }
  /************ Connect Sensor LL to be used for ucf management to the DatalogAppTask ************/
  if (sLSM6DSV16BXObj)
  {
    DatalogAppTask_SetMLCIF((AManagedTask *) sLSM6DSV16BXObj);
  }
  else
  {
    DatalogAppTask_SetMLCIF((AManagedTask *) sLSM6DSV16XObj);
  }

  DatalogAppTask_SetIspuIF((AManagedTask *) sISM330ISObj);

  //Other PnPL Components
  Deviceinformation_PnPLInit(pDeviceInfoPnPLObj);
  Firmware_Info_PnPLInit(pFirmwareInfoPnPLObj);
  Acquisition_Info_PnPLInit(pAcquisitionInfoPnPLObj);
  Tags_Info_PnPLInit(pTagsInfoPnPLObj);
  Log_Controller_PnPLInit(pLogControllerPnPLObj);
  Automode_PnPLInit(pAutomodePnPLObj);

  //Sensor PnPL Components
  Lis2du12_Acc_PnPLInit(pLIS2DU12_ACC_PnPLObj);
  Lis2mdl_Mag_PnPLInit(pLIS2MDL_MAG_PnPLObj);
  Lps22df_Press_PnPLInit(pLPS22DF_PRESS_PnPLObj);
  Mp23db01hp_Mic_PnPLInit(pMP23DB01HP_MIC_PnPLObj);
  Stts22h_Temp_PnPLInit(pSTTS22H_TEMP_PnPLObj);

  /* Use the external ISM330IS with ISPU or the onboard LSM6DSV16X with MLC */
  if (sISM330ISObj)
  {
    Ism330is_Acc_PnPLInit(pISM330IS_ACC_PnPLObj);
    Ism330is_Gyro_PnPLInit(pISM330IS_GYRO_PnPLObj);
    Ism330is_Ispu_PnPLInit(pISM330IS_ISPU_PnPLObj);
  }
  if (sLSM6DSV16BXObj)
  {
    Lsm6dsv16bx_Acc_PnPLInit(pLSM6DSV16BX_ACC_PnPLObj);
    Lsm6dsv16bx_Gyro_PnPLInit(pLSM6DSV16BX_GYRO_PnPLObj);
    Lsm6dsv16bx_Mlc_PnPLInit(pLSM6DSV16BX_MLC_PnPLObj);
  }
  if (sLSM6DSV16XObj)
  {
    Lsm6dsv16x_Acc_PnPLInit(pLSM6DSV16X_ACC_PnPLObj);
    Lsm6dsv16x_Gyro_PnPLInit(pLSM6DSV16X_GYRO_PnPLObj);
    Lsm6dsv16x_Mlc_PnPLInit(pLSM6DSV16X_MLC_PnPLObj);
  }

  return SYS_NO_ERROR_CODE;
}

IAppPowerModeHelper *SysGetPowerModeHelper(void)
{
  // Install the application power mode helper.
  static IAppPowerModeHelper *s_pxPowerModeHelper = NULL;
  if (s_pxPowerModeHelper == NULL)
  {
    s_pxPowerModeHelper = AppPowerModeHelperAlloc();
  }

  return s_pxPowerModeHelper;
}

void EXT_INT1_EXTI_Callback(uint16_t nPin)
{
  if (sISM330ISObj)
  {
    ISM330ISTask_EXTI_Callback(nPin);
  }
  else
  {
    LSM6DSV16BXTask_EXTI_Callback(nPin);
  }
}

void EXT_INT2_EXTI_Callback(uint16_t nPin)
{
  if (sISM330ISObj)
  {
    INT2_ISM330IS_EXTI_Callback(nPin);
  }
  else
  {
    INT2_DSV16BX_EXTI_Callback(nPin);
  }
}

static void PnPL_lock_fp(void)
{
  tx_mutex_get(&pnpl_mutex, TX_NO_WAIT);
}

static void PnPL_unlock_fp(void)
{
  tx_mutex_put(&pnpl_mutex);
}

