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
#include "ISM330DHCXTask.h"
#include "IIS2DLPCTask.h"
#include "IIS2MDCTask.h"
#include "DatalogAppTask.h"
#include "App_model.h"

#include "PnPLCompManager.h"

#include "Iis2dlpc_Acc_PnPL.h"
#include "Iis2mdc_Mag_PnPL.h"
#include "Ism330dhcx_Acc_PnPL.h"
#include "Ism330dhcx_Gyro_PnPL.h"
#include "Ism330dhcx_Mlc_PnPL.h"
#include "Automode_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Deviceinformation_PnPL.h"

static IPnPLComponent_t *pIis2dlpc_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pIis2mdc_Mag_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330dhcx_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330dhcx_Gyro_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330dhcx_Mlc_PnPLObj = NULL;
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
static AManagedTaskEx *sISM330DHCXObj = NULL;
static AManagedTaskEx *sIIS2DLPCObj = NULL;
static AManagedTaskEx *sIIS2MDCObj = NULL;

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
  sUtilObj = UtilTaskAlloc(NULL, &MX_GPIO_UBInitParams);
  sDatalogAppObj   = DatalogAppTaskAlloc();
  sI2CBusObj     = I2CBusTaskAlloc(&MX_I2C1InitParams);
  sISM330DHCXObj  = ISM330DHCXTaskAlloc(&MX_GPIO_INT1_DHCXInitParams, NULL, NULL);
  sIIS2DLPCObj    = IIS2DLPCTaskAlloc(&MX_GPIO_INT2_DLPCInitParams, NULL);
  sIIS2MDCObj     = IIS2MDCTaskAlloc(&MX_GPIO_DRDY_MDCInitParams, NULL);


  /************ Add the task object to the context ************/
  res = ACAddTask(pAppContext, (AManagedTask *) sUtilObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sDatalogAppObj);
  res = ACAddTask(pAppContext, (AManagedTask *)sI2CBusObj);
  res = ACAddTask(pAppContext, (AManagedTask *)sISM330DHCXObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2DLPCObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2MDCObj);

  pIis2dlpc_Acc_PnPLObj = Iis2dlpc_Acc_PnPLAlloc();
  pIis2mdc_Mag_PnPLObj = Iis2mdc_Mag_PnPLAlloc();
  pIsm330dhcx_Acc_PnPLObj = Ism330dhcx_Acc_PnPLAlloc();
  pIsm330dhcx_Gyro_PnPLObj = Ism330dhcx_Gyro_PnPLAlloc();
  pIsm330dhcx_Mlc_PnPLObj = Ism330dhcx_Mlc_PnPLAlloc();
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
                          (I2CBusIF *)ISM330DHCXTaskGetSensorIF((ISM330DHCXTask *) sISM330DHCXObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj, (I2CBusIF *)IIS2MDCTaskGetSensorIF((IIS2MDCTask *) sIIS2MDCObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2CBusObj,
                          (I2CBusIF *)IIS2DLPCTaskGetSensorIF((IIS2DLPCTask *) sIIS2DLPCObj));

  /************ Connect the Sensor events to the DatalogAppTask ************/
  IEventListener *DatalogAppListener = DatalogAppTask_GetEventListenerIF((DatalogAppTask *) sDatalogAppObj);
  IEventSrcAddEventListener(IIS2DLPCTaskGetEventSrcIF((IIS2DLPCTask *) sIIS2DLPCObj), DatalogAppListener);
  IEventSrcAddEventListener(IIS2MDCTaskGetMagEventSrcIF((IIS2MDCTask *) sIIS2MDCObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330DHCXTaskGetAccEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330DHCXTaskGetGyroEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330DHCXTaskGetMlcEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);

  /************ Connect Sensor LL to be used for ucf management to the DatalogAppTask ************/
  if (sISM330DHCXObj)
  {
    DatalogAppTask_SetMLCIF((AManagedTask *) sISM330DHCXObj);
  }

  /************ Other PnPL Components ************/
  Log_Controller_PnPLInit(pLog_Controller_PnPLObj);
  Tags_Info_PnPLInit(pTags_Info_PnPLObj);
  Acquisition_Info_PnPLInit(pAcquisition_Info_PnPLObj);
  Firmware_Info_PnPLInit(pFirmware_Info_PnPLObj);
  Deviceinformation_PnPLInit(pDeviceinformation_PnPLObj);
  Automode_PnPLInit(pAutomode_PnPLObj);

  /************ Sensor PnPL Components ************/
  Iis2dlpc_Acc_PnPLInit(pIis2dlpc_Acc_PnPLObj);
  Iis2mdc_Mag_PnPLInit(pIis2mdc_Mag_PnPLObj);
  Ism330dhcx_Gyro_PnPLInit(pIsm330dhcx_Gyro_PnPLObj);
  Ism330dhcx_Acc_PnPLInit(pIsm330dhcx_Acc_PnPLObj);
  Ism330dhcx_Mlc_PnPLInit(pIsm330dhcx_Mlc_PnPLObj);

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
