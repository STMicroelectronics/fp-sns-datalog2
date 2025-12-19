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

#include "UtilTask.h"

#include "I2CBusTask.h"
#include "IIS2DULPXTask.h"
#include "IIS2MDCTask.h"
#include "ILPS22QSTask.h"
#include "ISM330ISTask.h"
#include "ISM6HG256XTask.h"
#include "DatalogAppTask.h"
#include "App_model.h"

#include "PnPLCompManager.h"
#include "Iis2dulpx_Acc_PnPL.h"
#include "Iis2dulpx_Mlc_PnPL.h"
#include "Iis2mdc_Mag_PnPL.h"
#include "Ilps22qs_Press_PnPL.h"
#include "Ism330is_Acc_PnPL.h"
#include "Ism330is_Gyro_PnPL.h"
#include "Ism330is_Ispu_PnPL.h"
#include "Ism6hg256x_L_Acc_PnPL.h"
#include "Ism6hg256x_H_Acc_PnPL.h"
#include "Ism6hg256x_Gyro_PnPL.h"
#include "Ism6hg256x_Mlc_PnPL.h"
#include "Automode_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Deviceinformation_PnPL.h"

static IPnPLComponent_t *pIis2dulpx_Acc_PnPLObj = NULL;
//static IPnPLComponent_t *pIIS2DULPX_Mlc_PnPLObj = NULL;
static IPnPLComponent_t *pIis2mdc_Mag_PnPLObj = NULL;
static IPnPLComponent_t *pIlps22qs_Press_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330is_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330is_Gyro_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330is_Ispu_PnPLObj = NULL;
static IPnPLComponent_t *pIsm6hg256x_L_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pIsm6hg256x_H_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pIsm6hg256x_Gyro_PnPLObj = NULL;
static IPnPLComponent_t *pIsm6hg256x_Mlc_PnPLObj = NULL;
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
static AManagedTaskEx *sSPIBusObj = NULL;

/**
  * Sensor task object.
  */
static AManagedTaskEx *sIIS2DULPXObj = NULL;
static AManagedTaskEx *sIIS2MDCObj = NULL;
static AManagedTaskEx *sILPS22QSObj = NULL;
static AManagedTaskEx *sISM330ISObj = NULL;
static AManagedTaskEx *sISM6HG256XObj = NULL;
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
  sUtilObj = UtilTaskAlloc(&MX_GPIO_LD1InitParams, NULL);
  sDatalogAppObj = DatalogAppTaskAlloc();
  sI2CBusObj = I2CBusTaskAlloc(&MX_I2C1InitParams);
  sIIS2DULPXObj = IIS2DULPXTaskAlloc(&MX_GPIO_IIS2DULPX_INT1InitParams, NULL, NULL);
  sIIS2MDCObj = IIS2MDCTaskAlloc(&MX_GPIO_IIS2MDC_DRDYInitParams, NULL);
  sILPS22QSObj = ILPS22QSTaskAlloc(NULL, NULL);
  sISM330ISObj = ISM330ISTaskAlloc(&MX_GPIO_ISM330IS_INT2InitParams, NULL, NULL);
  sISM6HG256XObj = ISM6HG256XTaskAlloc(&MX_GPIO_ISM6HG256X_INT1InitParams, NULL, NULL);

  /************ Add the task object to the context ************/
  res = ACAddTask(pAppContext, (AManagedTask *) sUtilObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sDatalogAppObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sI2CBusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSPIBusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2DULPXObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2MDCObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sILPS22QSObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sISM330ISObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sISM6HG256XObj);

  pIis2dulpx_Acc_PnPLObj = Iis2dulpx_Acc_PnPLAlloc();
//  pIIS2DULPX_Mlc_PnPLObj = IIS2DULPX_Mlc_PnPLAlloc();
  pIis2mdc_Mag_PnPLObj = Iis2mdc_Mag_PnPLAlloc();
  pIlps22qs_Press_PnPLObj = Ilps22qs_Press_PnPLAlloc();
  pIsm330is_Acc_PnPLObj = Ism330is_Acc_PnPLAlloc();
  pIsm330is_Gyro_PnPLObj = Ism330is_Gyro_PnPLAlloc();
  pIsm330is_Ispu_PnPLObj = Ism330is_Ispu_PnPLAlloc();
  pIsm6hg256x_L_Acc_PnPLObj = Ism6hg256x_L_Acc_PnPLAlloc();
  pIsm6hg256x_H_Acc_PnPLObj = Ism6hg256x_H_Acc_PnPLAlloc();
  pIsm6hg256x_Gyro_PnPLObj = Ism6hg256x_Gyro_PnPLAlloc();
  pIsm6hg256x_Mlc_PnPLObj = Ism6hg256x_Mlc_PnPLAlloc();
  pAutomode_PnPLObj = Automode_PnPLAlloc();
  pLog_Controller_PnPLObj = Log_Controller_PnPLAlloc();
  pTags_Info_PnPLObj = Tags_Info_PnPLAlloc();
  pAcquisition_Info_PnPLObj = Acquisition_Info_PnPLAlloc();
  pFirmware_Info_PnPLObj = Firmware_Info_PnPLAlloc();
  pDeviceinformation_PnPLObj = Deviceinformation_PnPLAlloc();
  pAutomode_PnPLObj = Automode_PnPLAlloc();

  return res;
}

sys_error_code_t SysOnStartApplication(ApplicationContext *pAppContext)
{
  UNUSED(pAppContext);

  /************ Connect the sensor task to the bus ************/
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj,
                          (I2CBusIF *)IIS2DULPXTaskGetSensorIF((IIS2DULPXTask *) sIIS2DULPXObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj, (I2CBusIF *)IIS2MDCTaskGetSensorIF((IIS2MDCTask *) sIIS2MDCObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj, (I2CBusIF *)ILPS22QSTaskGetSensorIF((ILPS22QSTask *) sILPS22QSObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj,
                          (I2CBusIF *)ISM330ISTaskGetSensorIF((ISM330ISTask *) sISM330ISObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj,
                          (I2CBusIF *)ISM6HG256XTaskGetSensorIF((ISM6HG256XTask *) sISM6HG256XObj));

  /************ Connect the Sensor events to the DatalogAppTask ************/
  IEventListener *DatalogAppListener = DatalogAppTask_GetEventListenerIF((DatalogAppTask *) sDatalogAppObj);
  IEventSrcAddEventListener(IIS2DULPXTaskGetEventSrcIF((IIS2DULPXTask *) sIIS2DULPXObj), DatalogAppListener);
//  IEventSrcAddEventListener(IIS2DULPXTaskGetMlcEventSrcIF((IIS2DULPXTask *) sIIS2DULPXObj), DatalogAppListener);
  IEventSrcAddEventListener(IIS2MDCTaskGetMagEventSrcIF((IIS2MDCTask *) sIIS2MDCObj), DatalogAppListener);
  IEventSrcAddEventListener(ILPS22QSTaskGetPressEventSrcIF((ILPS22QSTask *) sILPS22QSObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330ISTaskGetAccEventSrcIF((ISM330ISTask *) sISM330ISObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330ISTaskGetGyroEventSrcIF((ISM330ISTask *) sISM330ISObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330ISTaskGetMlcEventSrcIF((ISM330ISTask *) sISM330ISObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM6HG256XTaskGetAccEventSrcIF((ISM6HG256XTask *) sISM6HG256XObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM6HG256XTaskGetHgAccEventSrcIF((ISM6HG256XTask *) sISM6HG256XObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM6HG256XTaskGetGyroEventSrcIF((ISM6HG256XTask *) sISM6HG256XObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM6HG256XTaskGetMlcEventSrcIF((ISM6HG256XTask *) sISM6HG256XObj), DatalogAppListener);

  /************ Connect Sensor LL to be used for ucf management to the DatalogAppTask ************/
  if (sISM6HG256XObj)
  {
    DatalogAppTask_Set_ISM6HG256XMLC_IF((AManagedTask *) sISM6HG256XObj);
  }
//  if (sIIS2DULPXObj)
//  {
//    DatalogAppTask_Set_IIS2DULPXMLC_IF((AManagedTask *) sIIS2DULPXObj);
//  }
  if (sISM330ISObj)
  {
    DatalogAppTask_Set_ISM330ISMLC_IF((AManagedTask *) sISM330ISObj);
  }

  /************ Other PnPL Components ************/
  Automode_PnPLInit(pAutomode_PnPLObj);
  Log_Controller_PnPLInit(pLog_Controller_PnPLObj);
  Tags_Info_PnPLInit(pTags_Info_PnPLObj);
  Acquisition_Info_PnPLInit(pAcquisition_Info_PnPLObj);
  Firmware_Info_PnPLInit(pFirmware_Info_PnPLObj);
  Deviceinformation_PnPLInit(pDeviceinformation_PnPLObj);

  /************ Sensor PnPL Components ************/
  Iis2dulpx_Acc_PnPLInit(pIis2dulpx_Acc_PnPLObj);
//  IIS2DULPX_Mlc_PnPLInit(pIIS2DULPX_Mlc_PnPLObj);
  Iis2mdc_Mag_PnPLInit(pIis2mdc_Mag_PnPLObj);
  Ilps22qs_Press_PnPLInit(pIlps22qs_Press_PnPLObj);
  Ism330is_Acc_PnPLInit(pIsm330is_Acc_PnPLObj);
  Ism330is_Gyro_PnPLInit(pIsm330is_Gyro_PnPLObj);
  Ism330is_Ispu_PnPLInit(pIsm330is_Ispu_PnPLObj);
  Ism6hg256x_L_Acc_PnPLInit(pIsm6hg256x_L_Acc_PnPLObj);
  Ism6hg256x_H_Acc_PnPLInit(pIsm6hg256x_H_Acc_PnPLObj);
  Ism6hg256x_Gyro_PnPLInit(pIsm6hg256x_Gyro_PnPLObj);
  Ism6hg256x_Mlc_PnPLInit(pIsm6hg256x_Mlc_PnPLObj);

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
