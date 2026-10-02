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

#include "HardwareDetection.h"

#include "mx.h"

#include "UtilTask.h"

#include "I3CBusTask.h"
#include "SPIBusTask.h"
#include "IIS3DWB10ISTask.h"
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
#include "Iis3dwb10is_Ext_Acc_PnPL.h"
#include "Iis3dwb10is_Ext_Ispu_PnPL.h"
#include "Automode_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Deviceinformation_PnPL.h"

static IPnPLComponent_t *pIis2dulpx_Acc_PnPLObj = NULL;
/*static IPnPLComponent_t *pIIS2DULPX_Mlc_PnPLObj = NULL;*/
static IPnPLComponent_t *pIlps22qs_Press_PnPLObj = NULL;
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

static IPnPLComponent_t *pIIS3DWB10IS_Ext_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS3DWB10IS_Ext_ISPU_PnPLObj = NULL;

/**
  * Utility task object.
  */
static AManagedTaskEx *sUtilObj = NULL;

/**
  * Bus task object.
  */
static AManagedTaskEx *sI3CBusObj = NULL;

/**
  * Sensor task object.
  */
static AManagedTaskEx *sIIS2DULPXObj = NULL;
static AManagedTaskEx *sILPS22QSObj = NULL;
static AManagedTaskEx *sISM6HG256XObj = NULL;

static AManagedTaskEx *sIIS3DWB10ISExtObj = NULL;
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
  boolean_t ext_iis3dwb10is = FALSE;

  /* PnPL thread safe mutex creation */
  tx_mutex_create(&pnpl_mutex, "PnPL Mutex", TX_INHERIT);

  /* PnPL thread safe function registration */
  PnPL_SetLockUnlockCallbacks(PnPL_lock_fp, PnPL_unlock_fp);

  PnPLSetAllocationFunctions(SysAlloc, SysFree);

  ext_iis3dwb10is = HardwareDetection_Check_Ext_IIS3DWB10IS();

  /************ Allocate task objects ************/
  sUtilObj = UtilTaskAlloc(&MX_GPIO_LEDYellowInitParams, NULL);
  sDatalogAppObj = DatalogAppTaskAlloc();
  sI3CBusObj = I3CBusTaskAlloc(&MX_I3C1InitParams);
  sIIS2DULPXObj = IIS2DULPXTaskAlloc(NULL, NULL, NULL, true);
  sILPS22QSObj = ILPS22QSTaskAlloc(NULL, NULL, true);
  sISM6HG256XObj = ISM6HG256XTaskAlloc(NULL, NULL, NULL, true);

  if (ext_iis3dwb10is)
  {
    sIIS3DWB10ISExtObj = IIS3DWB10ISTaskAlloc(NULL/*&MX_GPIO_INT1_EXTERNAL_InitParams*/, NULL,
                                              &MX_GPIO_CS_EXTERNALInitParams);
  }

  /************ Add the task object to the context ************/
  res = ACAddTask(pAppContext, (AManagedTask *) sUtilObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sDatalogAppObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sI3CBusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2DULPXObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sILPS22QSObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sISM6HG256XObj);

  if (ext_iis3dwb10is)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sIIS3DWB10ISExtObj);
  }

  pIis2dulpx_Acc_PnPLObj = Iis2dulpx_Acc_PnPLAlloc();
  pIlps22qs_Press_PnPLObj = Ilps22qs_Press_PnPLAlloc();
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

  if (ext_iis3dwb10is)
  {
    pIIS3DWB10IS_Ext_ACC_PnPLObj = Iis3dwb10is_Ext_Acc_PnPLAlloc();
    pIIS3DWB10IS_Ext_ISPU_PnPLObj = Iis3dwb10is_Ext_Ispu_PnPLAlloc();
  }

  return res;
}

sys_error_code_t SysOnStartApplication(ApplicationContext *pAppContext)
{
  UNUSED(pAppContext);

  /************ Connect the sensor task to the bus ************/
  I3CBusTaskConnectDevice((I3CBusTask *) sI3CBusObj,
                          (I3CBusIF *)IIS2DULPXTaskGetSensorIF((IIS2DULPXTask *) sIIS2DULPXObj));
  I3CBusTaskConnectDevice((I3CBusTask *) sI3CBusObj,
                          (I3CBusIF *)ILPS22QSTaskGetSensorIF((ILPS22QSTask *) sILPS22QSObj));
  I3CBusTaskConnectDevice((I3CBusTask *) sI3CBusObj,
                          (I3CBusIF *)ISM6HG256XTaskGetSensorIF((ISM6HG256XTask *) sISM6HG256XObj));

  /************ Connect the Sensor events to the DatalogAppTask ************/
  IEventListener *DatalogAppListener = DatalogAppTask_GetEventListenerIF((DatalogAppTask *) sDatalogAppObj);
  IEventSrcAddEventListener(IIS2DULPXTaskGetEventSrcIF((IIS2DULPXTask *) sIIS2DULPXObj), DatalogAppListener);
  IEventSrcAddEventListener(ILPS22QSTaskGetPressEventSrcIF((ILPS22QSTask *) sILPS22QSObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM6HG256XTaskGetAccEventSrcIF((ISM6HG256XTask *) sISM6HG256XObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM6HG256XTaskGetHgAccEventSrcIF((ISM6HG256XTask *) sISM6HG256XObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM6HG256XTaskGetGyroEventSrcIF((ISM6HG256XTask *) sISM6HG256XObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM6HG256XTaskGetMlcEventSrcIF((ISM6HG256XTask *) sISM6HG256XObj), DatalogAppListener);

  if (sIIS3DWB10ISExtObj)
  {
    IEventSrcAddEventListener(IIS3DWB10ISTaskGetAccEventSrcIF((IIS3DWB10ISTask *) sIIS3DWB10ISExtObj),
                              DatalogAppListener);
    IEventSrcAddEventListener(IIS3DWB10ISTaskGetIspuEventSrcIF((IIS3DWB10ISTask *) sIIS3DWB10ISExtObj),
                              DatalogAppListener);
  }

  /************ Connect Sensor LL to be used for ucf management to the DatalogAppTask ************/
  if (sISM6HG256XObj)
  {
    DatalogAppTask_Set_ISM6HG256XMLC_IF((AManagedTask *) sISM6HG256XObj);
  }
  if (sIIS2DULPXObj)
  {
    DatalogAppTask_Set_IIS2DULPXMLC_IF((AManagedTask *) sIIS2DULPXObj);
  }
  if (sIIS3DWB10ISExtObj)
  {
    DatalogAppTask_Set_IIS3DWB10ISExtMLC_IF((AManagedTask *) sIIS3DWB10ISExtObj);
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
  Ilps22qs_Press_PnPLInit(pIlps22qs_Press_PnPLObj);
  Ism6hg256x_L_Acc_PnPLInit(pIsm6hg256x_L_Acc_PnPLObj);
  Ism6hg256x_H_Acc_PnPLInit(pIsm6hg256x_H_Acc_PnPLObj);
  Ism6hg256x_Gyro_PnPLInit(pIsm6hg256x_Gyro_PnPLObj);
  Ism6hg256x_Mlc_PnPLInit(pIsm6hg256x_Mlc_PnPLObj);

  if (sIIS3DWB10ISExtObj)
  {
    Iis3dwb10is_Ext_Acc_PnPLInit(pIIS3DWB10IS_Ext_ACC_PnPLObj);
    iis3dwb10is_ext_acc_set_enable(true, NULL);
    Iis3dwb10is_Ext_Ispu_PnPLInit(pIIS3DWB10IS_Ext_ISPU_PnPLObj);

    iis2dulpx_acc_set_enable(false, NULL);
    iis2mdc_mag_set_enable(false, NULL);
    ilps22qs_press_set_enable(false, NULL);
    ism330is_acc_set_enable(false, NULL);
    ism330is_gyro_set_enable(false, NULL);
    ism6hg256x_l_acc_set_enable(false, NULL);
    ism6hg256x_h_acc_set_enable(false, NULL);
    ism6hg256x_gyro_set_enable(false, NULL);
  }

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
