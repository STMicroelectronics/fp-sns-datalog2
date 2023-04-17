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
#include "LSM6DSV16XTask.h"
#include "LIS2MDLTask.h"
#include "LIS2DU12Task.h"
#include "LPS22DFTask.h"
#include "STTS22HTask.h"
#include "MP23DB01HPTask.h"

#include "DatalogAppTask.h"

#include "PnPLCompManager.h"
#include "Deviceinformation_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Lis2mdl_Mag_PnPL.h"
#include "Lis2du12_Acc_PnPL.h"
#include "Stts22h_Temp_PnPL.h"
#include "Lps22df_Press_PnPL.h"
#include "Lsm6dsv16x_Acc_PnPL.h"
#include "Lsm6dsv16x_Gyro_PnPL.h"
#include "Lsm6dsv16x_Mlc_PnPL.h"
#include "Mp23db01hp_Mic_PnPL.h"
#include "Automode_PnPL.h"
#include "parson.h"

static uint8_t FW_ID = FW_ID_DATALOG2;

static IPnPLComponent_t *pLSM6DSV16X_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pLSM6DSV16X_GYRO_PnPLObj = NULL;
static IPnPLComponent_t *pLSM6DSV16X_MLC_PnPLObj = NULL;
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
static AManagedTaskEx *sI2C1BusObj = NULL;

/**
  * Sensor task object.
  */
static AManagedTaskEx *sLSM6DSV16XObj = NULL;
static AManagedTaskEx *sLIS2MDLObj = NULL;
static AManagedTaskEx *sLIS2DU12Obj = NULL;
static AManagedTaskEx *sLPS22DFObj = NULL;
static AManagedTaskEx *sSTTS22HObj = NULL;
static AManagedTaskEx *sMP23DB01HPObj = NULL;

/**
  * DatalogApp
  */
static AManagedTaskEx *sDatalogAppObj = NULL;

/**
  * specifies the map (PM_APP, PM_SM). It re-map the state of the application into the state of the Sensor Manager.
  */
static EPowerMode spAppPMState2SMPMStateMap[] =
{
  E_POWER_MODE_STATE1,
  E_POWER_MODE_SLEEP_1,
  E_POWER_MODE_SENSORS_ACTIVE,
  E_POWER_MODE_RESERVED
};


/* Private functions declaration */
/*********************************/

/**
  * Re-map the PM State Machine of the Sensor Manager managed tasks used in the application according to the following map:
  *
  * | App State                      | Sensor Manager State         |
  * | :----------------------------- | ---------------------------: |
  * | E_POWER_MODE_STATE1            | E_POWER_MODE_STATE1          |
  * | E_POWER_MODE_SLEEP_1           | E_POWER_MODE_SLEEP_1         |
  * | E_POWER_MODE_SENSORS_ACTIVE    | E_POWER_MODE_SENSORS_ACTIVE  |
  * | E_POWER_MODE_STARTING          | E_POWER_MODE_STATE1          |
  *
  * @param pPMState2PMStateMap [IN] specifies the map (PM_APP, PM_SM).
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
  */
static sys_error_code_t SensorManagerStateMachineRemap(EPowerMode *pPMState2PMStateMap);

/* eLooM framework entry points definition */
/*******************************************/

sys_error_code_t SysLoadApplicationContext(ApplicationContext *pAppContext)
{
  assert_param(pAppContext);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* Allocate the task objects */
  sUtilObj = UtilTaskAlloc(&MX_GPIO_SW1InitParams, &MX_GPIO_LED1InitParams, &MX_GPIO_LED2InitParams, &MX_GPIO_LED3InitParams);
  sDatalogAppObj = DatalogAppTaskAlloc();
  sI2C1BusObj = I2CBusTaskAlloc(&MX_I2C1InitParams);
  sSPI2BusObj = SPIBusTaskAlloc(&MX_SPI2InitParams);
  sLIS2DU12Obj = LIS2DU12TaskAlloc(&MX_GPIO_ACC_INT1InitParams, &MX_GPIO_SPI_SEN_CS_AInitParams);
  sLIS2MDLObj = LIS2MDLTaskAlloc(&MX_GPIO_MAG_DRDYInitParams, NULL);
  sLPS22DFObj = LPS22DFTaskAlloc(NULL, NULL);
  sMP23DB01HPObj = MP23DB01HPTaskAlloc(&MX_ADF1InitParams);
  sLSM6DSV16XObj = LSM6DSV16XTaskAlloc(&MX_GPIO_IMU_INT1InitParams, NULL, &MX_GPIO_SPI_SEN_CS_GInitParams);
  sSTTS22HObj = STTS22HTaskAlloc(NULL, NULL, 0x71);

  /* Add the task object to the context. */
  res = ACAddTask(pAppContext, (AManagedTask *) sUtilObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sDatalogAppObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sI2C1BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSPI2BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLIS2DU12Obj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLIS2MDLObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLPS22DFObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sMP23DB01HPObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLSM6DSV16XObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSTTS22HObj);

  pLIS2DU12_ACC_PnPLObj = Lis2du12_Acc_PnPLAlloc();
  pLIS2MDL_MAG_PnPLObj = Lis2mdl_Mag_PnPLAlloc();
  pLPS22DF_PRESS_PnPLObj = Lps22df_Press_PnPLAlloc();
  pMP23DB01HP_MIC_PnPLObj = Mp23db01hp_Mic_PnPLAlloc();
  pLSM6DSV16X_ACC_PnPLObj = Lsm6dsv16x_Acc_PnPLAlloc();
  pLSM6DSV16X_GYRO_PnPLObj = Lsm6dsv16x_Gyro_PnPLAlloc();
  pLSM6DSV16X_MLC_PnPLObj = Lsm6dsv16x_Mlc_PnPLAlloc();
  pSTTS22H_TEMP_PnPLObj = Stts22h_Temp_PnPLAlloc();

  pDeviceInfoPnPLObj = Deviceinformation_PnPLAlloc();
  pAcquisitionInfoPnPLObj = Acquisition_Info_PnPLAlloc();
  pTagsInfoPnPLObj = Tags_Info_PnPLAlloc();
  pFirmwareInfoPnPLObj = Firmware_Info_PnPLAlloc();
  pLogControllerPnPLObj = Log_Controller_PnPLAlloc();
  pAutomodePnPLObj = Automode_PnPLAlloc();

  PnPLSetFWID(FW_ID);

  return res;
}

sys_error_code_t SysOnStartApplication(ApplicationContext *pAppContext)
{
  UNUSED(pAppContext);

  //  /* Re-map the state machine of the Integrated tasks */
  SensorManagerStateMachineRemap(spAppPMState2SMPMStateMap);

  /* connect the sensor task to the bus. */
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C1BusObj, (I2CBusIF *)LIS2MDLTaskGetSensorIF((LIS2MDLTask *) sLIS2MDLObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C1BusObj, (I2CBusIF *)LPS22DFTaskGetSensorIF((LPS22DFTask *) sLPS22DFObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C1BusObj, (I2CBusIF *)STTS22HTaskGetSensorIF((STTS22HTask *) sSTTS22HObj));
  SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)LIS2DU12TaskGetSensorIF((LIS2DU12Task *) sLIS2DU12Obj));
  SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)LSM6DSV16XTaskGetSensorIF((LSM6DSV16XTask *) sLSM6DSV16XObj));

  IEventListener *DatalogAppListener = DatalogAppTask_GetEventListenerIF((DatalogAppTask *) sDatalogAppObj);
  IEventSrcAddEventListener(LIS2DU12TaskGetEventSrcIF((LIS2DU12Task *) sLIS2DU12Obj), DatalogAppListener);
  IEventSrcAddEventListener(LIS2MDLTaskGetMagEventSrcIF((LIS2MDLTask *) sLIS2MDLObj), DatalogAppListener);
  IEventSrcAddEventListener(LPS22DFTaskGetPressEventSrcIF((LPS22DFTask *) sLPS22DFObj), DatalogAppListener);
  IEventSrcAddEventListener(MP23DB01HPTaskGetEventSrcIF((MP23DB01HPTask *) sMP23DB01HPObj), DatalogAppListener);
  IEventSrcAddEventListener(LSM6DSV16XTaskGetAccEventSrcIF((LSM6DSV16XTask *) sLSM6DSV16XObj), DatalogAppListener);
  IEventSrcAddEventListener(LSM6DSV16XTaskGetGyroEventSrcIF((LSM6DSV16XTask *) sLSM6DSV16XObj), DatalogAppListener);
  IEventSrcAddEventListener(LSM6DSV16XTaskGetMlcEventSrcIF((LSM6DSV16XTask *) sLSM6DSV16XObj), DatalogAppListener);
  IEventSrcAddEventListener(STTS22HTaskGetTempEventSrcIF((STTS22HTask *) sSTTS22HObj), DatalogAppListener);

//Sensor PnPL Components
  Lis2du12_Acc_PnPLInit(pLIS2DU12_ACC_PnPLObj);
  Lis2mdl_Mag_PnPLInit(pLIS2MDL_MAG_PnPLObj);
  Lps22df_Press_PnPLInit(pLPS22DF_PRESS_PnPLObj);
  Mp23db01hp_Mic_PnPLInit(pMP23DB01HP_MIC_PnPLObj);
  Lsm6dsv16x_Acc_PnPLInit(pLSM6DSV16X_ACC_PnPLObj);
  Lsm6dsv16x_Gyro_PnPLInit(pLSM6DSV16X_GYRO_PnPLObj);
  Lsm6dsv16x_Mlc_PnPLInit(pLSM6DSV16X_MLC_PnPLObj, DatalogAppTask_GetIMLCControllerIF((DatalogAppTask *) sDatalogAppObj,
                          (AManagedTask *) sLSM6DSV16XObj));
  Stts22h_Temp_PnPLInit(pSTTS22H_TEMP_PnPLObj);

//Other PnPL Components
  Deviceinformation_PnPLInit(pDeviceInfoPnPLObj);
  Firmware_Info_PnPLInit(pFirmwareInfoPnPLObj);
  Acquisition_Info_PnPLInit(pAcquisitionInfoPnPLObj);
  Tags_Info_PnPLInit(pTagsInfoPnPLObj);
  Log_Controller_PnPLInit(pLogControllerPnPLObj, DatalogAppTask_GetILogControllerIF((DatalogAppTask *) sDatalogAppObj));
  Automode_PnPLInit(pAutomodePnPLObj);

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

/* Private function definition */
/*******************************/

static sys_error_code_t SensorManagerStateMachineRemap(EPowerMode *pPMState2PMStateMap)
{
  assert_param(pPMState2PMStateMap != NULL);

  AMTSetPMStateRemapFunc((AManagedTask *) sI2C1BusObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sSPI2BusObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sLIS2DU12Obj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sLIS2MDLObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sLPS22DFObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sMP23DB01HPObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sLSM6DSV16XObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sSTTS22HObj, pPMState2PMStateMap);

  return SYS_NO_ERROR_CODE;
}
