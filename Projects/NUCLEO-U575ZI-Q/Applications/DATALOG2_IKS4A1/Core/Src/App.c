/**
  ******************************************************************************
  * @file    App.c
  * @author  SRA
  * @brief   Define the Application main entry points
  *
  *
  * This file is the main entry point for the user code.
  *
  * The framework `weak` functions are redefined in this file and they link
  * the application specific code with the framework:
  * - SysLoadApplicationContext(): it is the first application defined function
  *   called by the framework. Here we define all managed tasks. A managed task
  *   implements one or more application specific feature.
  * - SysOnStartApplication(): this function is called by the framework
  *   when the system is initialized (all managed task objects have been
  *   initialized), and before the INIT task release the control. Here we
  *   link the application objects according to the application design.
  *
  * The execution time  between the two above functions is called
  * *system initialization*. During this period only the INIT task is running.
  *
  * Each managed task will be activated in turn to initialize its hardware
  * resources, if any - MyTask_vtblHardwareInit() - and its software
  * resources - MyTask_vtblOnCreateTask().
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

#include "stts22h_reg.h"

#include "UtilTask.h"

#include "I2CBusTask.h"
#include "LIS2DUXS12Task.h"
#include "LIS2MDLTask.h"
#include "LPS22DFTask.h"
#include "LSM6DSO16ISTask.h"
#include "LSM6DSV16XTask.h"
#include "SHT40Task.h"
#include "STTS22HTask.h"
#include "DatalogAppTask.h"
#include "App_model.h"

#include "PnPLCompManager.h"
#include "Lis2duxs12_Acc_PnPL.h"
#include "Lis2duxs12_Mlc_PnPL.h"
#include "Lis2mdl_Mag_PnPL.h"
#include "Lps22df_Press_PnPL.h"
#include "Lsm6dso16is_Acc_PnPL.h"
#include "Lsm6dso16is_Gyro_PnPL.h"
#include "Lsm6dso16is_Ispu_PnPL.h"
#include "Lsm6dsv16x_Acc_PnPL.h"
#include "Lsm6dsv16x_Gyro_PnPL.h"
#include "Lsm6dsv16x_Mlc_PnPL.h"
#include "Sht40_Hum_PnPL.h"
#include "Sht40_Temp_PnPL.h"
#include "Stts22h_Temp_PnPL.h"
#include "Automode_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Deviceinformation_PnPL.h"

static IPnPLComponent_t *pLis2duxs12_Acc_PnPLObj = NULL;
//static IPnPLComponent_t *pLis2duxs12_Mlc_PnPLObj = NULL;
static IPnPLComponent_t *pLis2mdl_Mag_PnPLObj = NULL;
static IPnPLComponent_t *pLps22df_Press_PnPLObj = NULL;
static IPnPLComponent_t *pLsm6dso16is_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pLsm6dso16is_Gyro_PnPLObj = NULL;
static IPnPLComponent_t *pLsm6dso16is_Ispu_PnPLObj = NULL;
static IPnPLComponent_t *pLsm6dsv16x_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pLsm6dsv16x_Gyro_PnPLObj = NULL;
static IPnPLComponent_t *pLsm6dsv16x_Mlc_PnPLObj = NULL;
static IPnPLComponent_t *pSht40_Hum_PnPLObj = NULL;
static IPnPLComponent_t *pSht40_Temp_PnPLObj = NULL;
static IPnPLComponent_t *pStts22h_Temp_PnPLObj = NULL;
static IPnPLComponent_t *pAutomode_PnPLObj = NULL;
static IPnPLComponent_t *pLog_Controller_PnPLObj = NULL;
static IPnPLComponent_t *pTags_Info_PnPLObj = NULL;
static IPnPLComponent_t *pAcquisition_Info_PnPLObj = NULL;
static IPnPLComponent_t *pFirmware_Info_PnPLObj = NULL;
static IPnPLComponent_t *pDeviceinformation_PnPLObj = NULL;

/**
  * Utility task object.
  */
static AManagedTaskEx *sUtilObj = NULL;

/**
  * Bus task object.
  */
static AManagedTaskEx *sI2CBusObj = NULL;

/**
  * Sensor task object.
  */
static AManagedTaskEx *sLIS2DUXS12Obj = NULL;
static AManagedTaskEx *sLIS2MDLObj = NULL;
static AManagedTaskEx *sLPS22DFObj = NULL;
static AManagedTaskEx *sLSM6DSO16ISObj = NULL;
static AManagedTaskEx *sLSM6DSV16XObj = NULL;
static AManagedTaskEx *sSHT40Obj = NULL;
static AManagedTaskEx *sSTTS22HObj = NULL;
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

  /* PnPL thread safe mutex creation */
  tx_mutex_create(&pnpl_mutex, "PnPL Mutex", TX_INHERIT);

  /* PnPL thread safe function registration */
  PnPL_SetLockUnlockCallbacks(PnPL_lock_fp, PnPL_unlock_fp);

  PnPLSetAllocationFunctions(SysAlloc, SysFree);

  /************ Allocate task objects ************/
  sUtilObj = UtilTaskAlloc(&MX_GPIO_LEDBlueInitParams, NULL);
  sDatalogAppObj = DatalogAppTaskAlloc();
  sI2CBusObj = I2CBusTaskAlloc(&MX_I2C1InitParams);
  sLIS2DUXS12Obj = LIS2DUXS12TaskAlloc(&MX_GPIO_LIS2DUXS12_INTInitParams, NULL, NULL);
  sLIS2MDLObj = LIS2MDLTaskAlloc(&MX_GPIO_LIS2MDL_DRDYInitParams, NULL);
  sLPS22DFObj = LPS22DFTaskAlloc(NULL, NULL);
  sLSM6DSO16ISObj = LSM6DSO16ISTaskAlloc(&MX_GPIO_LSM6DSO16IS_INT2InitParams, NULL, NULL);
  sLSM6DSV16XObj = LSM6DSV16XTaskAlloc(&MX_GPIO_LSM6DSV16X_INT1InitParams, NULL, NULL);
  sSHT40Obj = SHT40TaskAlloc(NULL, NULL);
  sSTTS22HObj = STTS22HTaskAlloc(NULL, NULL, STTS22H_I2C_ADD_H);


  /************ Add the task object to the context ************/
  res = ACAddTask(pAppContext, (AManagedTask *) sUtilObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sDatalogAppObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sI2CBusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLIS2DUXS12Obj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLIS2MDLObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLPS22DFObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLSM6DSO16ISObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLSM6DSV16XObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSHT40Obj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSTTS22HObj);

  pLis2duxs12_Acc_PnPLObj = Lis2duxs12_Acc_PnPLAlloc();
//  pLis2duxs12_Mlc_PnPLObj = Lis2duxs12_Mlc_PnPLAlloc();
  pLis2mdl_Mag_PnPLObj = Lis2mdl_Mag_PnPLAlloc();
  pLps22df_Press_PnPLObj = Lps22df_Press_PnPLAlloc();
  pLsm6dso16is_Acc_PnPLObj = Lsm6dso16is_Acc_PnPLAlloc();
  pLsm6dso16is_Gyro_PnPLObj = Lsm6dso16is_Gyro_PnPLAlloc();
  pLsm6dso16is_Ispu_PnPLObj = Lsm6dso16is_Ispu_PnPLAlloc();
  pLsm6dsv16x_Acc_PnPLObj = Lsm6dsv16x_Acc_PnPLAlloc();
  pLsm6dsv16x_Gyro_PnPLObj = Lsm6dsv16x_Gyro_PnPLAlloc();
  pLsm6dsv16x_Mlc_PnPLObj = Lsm6dsv16x_Mlc_PnPLAlloc();
  pSht40_Hum_PnPLObj = Sht40_Hum_PnPLAlloc();
  pSht40_Temp_PnPLObj = Sht40_Temp_PnPLAlloc();
  pStts22h_Temp_PnPLObj = Stts22h_Temp_PnPLAlloc();
  pAutomode_PnPLObj = Automode_PnPLAlloc();
  pLog_Controller_PnPLObj = Log_Controller_PnPLAlloc();
  pTags_Info_PnPLObj = Tags_Info_PnPLAlloc();
  pAcquisition_Info_PnPLObj = Acquisition_Info_PnPLAlloc();
  pFirmware_Info_PnPLObj = Firmware_Info_PnPLAlloc();
  pDeviceinformation_PnPLObj = Deviceinformation_PnPLAlloc();

  return res;
}

sys_error_code_t SysOnStartApplication(ApplicationContext *pAppContext)
{
  UNUSED(pAppContext);

  /************ Connect the sensor task to the bus ************/
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj,
                          (I2CBusIF *)LIS2DUXS12TaskGetSensorIF((LIS2DUXS12Task *) sLIS2DUXS12Obj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj, (I2CBusIF *)LIS2MDLTaskGetSensorIF((LIS2MDLTask *) sLIS2MDLObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj, (I2CBusIF *)LPS22DFTaskGetSensorIF((LPS22DFTask *) sLPS22DFObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj,
                          (I2CBusIF *)LSM6DSO16ISTaskGetSensorIF((LSM6DSO16ISTask *) sLSM6DSO16ISObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj,
                          (I2CBusIF *)LSM6DSV16XTaskGetSensorIF((LSM6DSV16XTask *) sLSM6DSV16XObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj, (I2CBusIF *)SHT40TaskGetSensorIF((SHT40Task *) sSHT40Obj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj, (I2CBusIF *)STTS22HTaskGetSensorIF((STTS22HTask *) sSTTS22HObj));

  /************ Connect the Sensor events to the DatalogAppTask ************/
  IEventListener *DatalogAppListener = DatalogAppTask_GetEventListenerIF((DatalogAppTask *) sDatalogAppObj);
  IEventSrcAddEventListener(LIS2DUXS12TaskGetEventSrcIF((LIS2DUXS12Task *) sLIS2DUXS12Obj), DatalogAppListener);
//  IEventSrcAddEventListener(LIS2DUXS12TaskGetMlcEventSrcIF((LIS2DUXS12Task *) sLIS2DUXS12Obj), DatalogAppListener);
  IEventSrcAddEventListener(LIS2MDLTaskGetMagEventSrcIF((LIS2MDLTask *) sLIS2MDLObj), DatalogAppListener);
  IEventSrcAddEventListener(LPS22DFTaskGetPressEventSrcIF((LPS22DFTask *) sLPS22DFObj), DatalogAppListener);
  IEventSrcAddEventListener(LSM6DSO16ISTaskGetAccEventSrcIF((LSM6DSO16ISTask *) sLSM6DSO16ISObj), DatalogAppListener);
  IEventSrcAddEventListener(LSM6DSO16ISTaskGetGyroEventSrcIF((LSM6DSO16ISTask *) sLSM6DSO16ISObj), DatalogAppListener);
  IEventSrcAddEventListener(LSM6DSO16ISTaskGetIspuEventSrcIF((LSM6DSO16ISTask *) sLSM6DSO16ISObj), DatalogAppListener);
  IEventSrcAddEventListener(LSM6DSV16XTaskGetAccEventSrcIF((LSM6DSV16XTask *) sLSM6DSV16XObj), DatalogAppListener);
  IEventSrcAddEventListener(LSM6DSV16XTaskGetGyroEventSrcIF((LSM6DSV16XTask *) sLSM6DSV16XObj), DatalogAppListener);
  IEventSrcAddEventListener(LSM6DSV16XTaskGetMlcEventSrcIF((LSM6DSV16XTask *) sLSM6DSV16XObj), DatalogAppListener);
  IEventSrcAddEventListener(SHT40TaskGetHumEventSrcIF((SHT40Task *) sSHT40Obj), DatalogAppListener);
  IEventSrcAddEventListener(SHT40TaskGetTempEventSrcIF((SHT40Task *) sSHT40Obj), DatalogAppListener);
  IEventSrcAddEventListener(STTS22HTaskGetTempEventSrcIF((STTS22HTask *) sSTTS22HObj), DatalogAppListener);

  /************ Connect Sensor LL to be used for ucf management to the DatalogAppTask ************/
  if (sLSM6DSV16XObj)
  {
    DatalogAppTask_Set_LSMDSV16XMLC_IF((AManagedTask *) sLSM6DSV16XObj);
  }
//  if (sLIS2DUXS12Obj)
//  {
//    DatalogAppTask_Set_LIS2DUXS12MLC_IF((AManagedTask *) sLIS2DUXS12Obj);
//  }
  if (sLSM6DSO16ISObj)
  {
    DatalogAppTask_Set_LSM6DSO16ISMLC_IF((AManagedTask *) sLSM6DSO16ISObj);
  }

  /************ Other PnPL Components ************/
  Automode_PnPLInit(pAutomode_PnPLObj);
  Log_Controller_PnPLInit(pLog_Controller_PnPLObj);
  Tags_Info_PnPLInit(pTags_Info_PnPLObj);
  Acquisition_Info_PnPLInit(pAcquisition_Info_PnPLObj);
  Firmware_Info_PnPLInit(pFirmware_Info_PnPLObj);
  Deviceinformation_PnPLInit(pDeviceinformation_PnPLObj);

  /************ Sensor PnPL Components ************/
  Lis2duxs12_Acc_PnPLInit(pLis2duxs12_Acc_PnPLObj);
//  Lis2duxs12_Mlc_PnPLInit(pLis2duxs12_Mlc_PnPLObj);
  Lis2mdl_Mag_PnPLInit(pLis2mdl_Mag_PnPLObj);
  Lps22df_Press_PnPLInit(pLps22df_Press_PnPLObj);
  Lsm6dso16is_Acc_PnPLInit(pLsm6dso16is_Acc_PnPLObj);
  Lsm6dso16is_Gyro_PnPLInit(pLsm6dso16is_Gyro_PnPLObj);
  Lsm6dso16is_Ispu_PnPLInit(pLsm6dso16is_Ispu_PnPLObj);
  Lsm6dsv16x_Acc_PnPLInit(pLsm6dsv16x_Acc_PnPLObj);
  Lsm6dsv16x_Gyro_PnPLInit(pLsm6dsv16x_Gyro_PnPLObj);
  Lsm6dsv16x_Mlc_PnPLInit(pLsm6dsv16x_Mlc_PnPLObj);
  Sht40_Hum_PnPLInit(pSht40_Hum_PnPLObj);
  Sht40_Temp_PnPLInit(pSht40_Temp_PnPLObj);
  Stts22h_Temp_PnPLInit(pStts22h_Temp_PnPLObj);

  return SYS_NO_ERROR_CODE;
}

IAppPowerModeHelper *SysGetPowerModeHelper(void)
{
  /* Install the application power mode helper. */
  static IAppPowerModeHelper *s_pxPowerModeHelper = NULL;
  if (s_pxPowerModeHelper == NULL)
  {
    s_pxPowerModeHelper = AppPowerModeHelperAlloc();
  }

  return s_pxPowerModeHelper;
}


static void PnPL_lock_fp(void)
{
  tx_mutex_get(&pnpl_mutex, TX_NO_WAIT);
}

static void PnPL_unlock_fp(void)
{
  tx_mutex_put(&pnpl_mutex);
}
