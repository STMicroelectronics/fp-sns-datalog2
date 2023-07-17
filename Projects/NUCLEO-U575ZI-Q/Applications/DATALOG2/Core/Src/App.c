/**
 ******************************************************************************
 * @file    App.c
 * @author  SRA
 *
 * @brief   Define the Application main entry points
 *
 * ## Introduction
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
 * ## About this demo
 *
 * This demo demonstrates how to use the Sensor Manager eLooM component to operate the sensors
 * available in the X-NUCLEO-IKSO2A1 expansion board. The HelloWorld task select one sensor
 * and enable and configure it.
 *
 * ## How to use the demo
 *
 * connect the board through the ST-Link. Open a terminal
 * like [Tera Term](http://www.teraterm.org) to display the debug log using these parameters:
 * - Speed       : 115200
 * - Data        : 8 bit
 * - Parity      : none
 * - Stop bits   : 1
 * - Flow control: none
 *
 * Build the project and program the board.
 * At the beginning the application is in power mode E_PM_MODE_STATE1 and a
 * greetings message is displayed in the log.
 * To change state to E_POWER_MODE_SENSOR_ACTIVE press the user button. Now the
 * selected sensor start to produce data.
 *
 *********************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *********************************************************************************
 */

#include "services/sysdebug.h"
#include "services/ApplicationContext.h"
#include "AppPowerModeHelper.h"
#include "HelloWorldTask.h"
#include "DatalogAppTask.h"
#include "mx.h"

/* SensorManager include*/
#include "I2CBusTask.h"
#include "ISM330DHCXTask.h"
#include "IIS2DLPCTask.h"
#include "IIS2MDCTask.h"


/* PnPL include */
#include "PnPLCompManager.h"
#include "Automode_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Deviceinformation_PnPL.h"
#include "Iis2dlpc_Acc_PnPL.h"
#include "Iis2mdc_Mag_PnPL.h"
#include "Ism330dhcx_Acc_PnPL.h"
#include "Ism330dhcx_Gyro_PnPL.h"
#include "Ism330dhcx_Mlc_PnPL.h"

#include "ILog_Controller.h"
#include "ILog_Controller_vtbl.h"
#include "IIsm330dhcx_Mlc.h"
#include "IIsm330dhcx_Mlc_vtbl.h"

#include "parson.h"


/**
 * Application managed task.
 */
static AManagedTask *spHelloWorldObj = NULL;

/**
 * I2C bus task object.
 */
static AManagedTaskEx *spI2C2BusObj = NULL;

/**
 * Sensor task object: 3-axis accelerometer and 3-axis gyroscope
 */
static AManagedTaskEx *spISM330DHCXObj = NULL;

/**
 * Sensor task object: 3-axis accelerometer.
 */
static AManagedTaskEx *spIIS2DLPCObj = NULL;

/**
 * Sensor task object: 3-axis magnetometer
 */
static AManagedTaskEx *spIIS2MDCObj = NULL;

/**
 * DatalogApp object.
 */
static AManagedTaskEx *spDatalogAppObj = NULL;


/* PnPL objects */

static IPnPLComponent_t *pAutomode_PnPLObj = NULL;
static IPnPLComponent_t *pLog_Controller_PnPLObj = NULL;
static IPnPLComponent_t *pTags_Info_PnPLObj = NULL;
static IPnPLComponent_t *pAcquisition_Info_PnPLObj = NULL;
static IPnPLComponent_t *pFirmware_Info_PnPLObj = NULL;
static IPnPLComponent_t *pDeviceinformation_PnPLObj = NULL;
static IPnPLComponent_t *pIis2dlpc_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pIis2mdc_Mag_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330dhcx_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330dhcx_Gyro_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330dhcx_Mlc_PnPLObj = NULL;

//static ILog_Controller_t iLog_Controller;
static IIsm330dhcx_Mlc_t iIsm330dhcx_Mlc;


/* eLooM framework entry points definition */
/*******************************************/

sys_error_code_t SysLoadApplicationContext(ApplicationContext *pAppContext)
{
  assert_param(pAppContext != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* Workaround to set malloc/free function even if BLE Init fails */
  json_set_allocation_functions(SysAlloc, SysFree);

  /* Allocate the task objects */
  spHelloWorldObj = HelloWorldTaskAlloc(&MX_GPIO_LEDBlueInitParams, &MX_GPIO_UBInitParams);
  spDatalogAppObj = DatalogAppTaskAlloc();
  spI2C2BusObj     = I2CBusTaskAlloc(&MX_I2C1InitParams);
  spISM330DHCXObj  = ISM330DHCXTaskAlloc(&MX_GPIO_INT1_DHCXInitParams, NULL, NULL);
  spIIS2DLPCObj    = IIS2DLPCTaskAlloc(&MX_GPIO_INT2_DLPCInitParams, NULL);
  spIIS2MDCObj     = IIS2MDCTaskAlloc(&MX_GPIO_DRDY_MDCInitParams, NULL);


  /* Add the task object to the context. */
  res = ACAddTask(pAppContext, spHelloWorldObj);
  res = ACAddTask(pAppContext, (AManagedTask *) spDatalogAppObj);
  res = ACAddTask(pAppContext, (AManagedTask*) spI2C2BusObj);
  res = ACAddTask(pAppContext, (AManagedTask*) spISM330DHCXObj);
  res = ACAddTask(pAppContext, (AManagedTask*) spIIS2DLPCObj);
  res = ACAddTask(pAppContext, (AManagedTask*) spIIS2MDCObj);

  /* PnPL Components Allocation */
  pAutomode_PnPLObj = Automode_PnPLAlloc();
  pLog_Controller_PnPLObj = Log_Controller_PnPLAlloc();
  pTags_Info_PnPLObj = Tags_Info_PnPLAlloc();
  pAcquisition_Info_PnPLObj = Acquisition_Info_PnPLAlloc();
  pFirmware_Info_PnPLObj = Firmware_Info_PnPLAlloc();
  pDeviceinformation_PnPLObj = Deviceinformation_PnPLAlloc();
  pIis2dlpc_Acc_PnPLObj = Iis2dlpc_Acc_PnPLAlloc();
  pIis2mdc_Mag_PnPLObj = Iis2mdc_Mag_PnPLAlloc();
  pIsm330dhcx_Acc_PnPLObj = Ism330dhcx_Acc_PnPLAlloc();
  pIsm330dhcx_Gyro_PnPLObj = Ism330dhcx_Gyro_PnPLAlloc();
  pIsm330dhcx_Mlc_PnPLObj = Ism330dhcx_Mlc_PnPLAlloc();

  return res;
}

sys_error_code_t SysOnStartApplication(ApplicationContext *pAppContext)
{
  UNUSED(pAppContext);

  /* connect the sensor tasks to the bus. */
  (void) I2CBusTaskConnectDevice((I2CBusTask*) spI2C2BusObj, (I2CBusIF*)ISM330DHCXTaskGetSensorIF((ISM330DHCXTask*) spISM330DHCXObj));
  (void) I2CBusTaskConnectDevice((I2CBusTask*) spI2C2BusObj, (I2CBusIF*)IIS2DLPCTaskGetSensorIF((IIS2DLPCTask*) spIIS2DLPCObj));
  (void) I2CBusTaskConnectDevice((I2CBusTask*) spI2C2BusObj, (I2CBusIF*)IIS2MDCTaskGetSensorIF((IIS2MDCTask*) spIIS2MDCObj));

  /* Connect the Sensor events */
  IEventListener *DatalogAppListener = DatalogAppTask_GetEventListenerIF((DatalogAppTask *) spDatalogAppObj);
  IEventSrcAddEventListener(IIS2DLPCTaskGetEventSrcIF((IIS2DLPCTask *) spIIS2DLPCObj), DatalogAppListener);
  IEventSrcAddEventListener(IIS2MDCTaskGetMagEventSrcIF((IIS2MDCTask *) spIIS2MDCObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330DHCXTaskGetAccEventSrcIF((ISM330DHCXTask *) spISM330DHCXObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330DHCXTaskGetGyroEventSrcIF((ISM330DHCXTask *) spISM330DHCXObj), DatalogAppListener);
//  IEventSrcAddEventListener(IMP34DT05TaskGetEventSrcIF((IMP34DT05Task *) sIMP34DT05Obj), DatalogAppListener);

  /* Init&Add PnPL Components */
  Automode_PnPLInit(pAutomode_PnPLObj);
  Log_Controller_PnPLInit(pLog_Controller_PnPLObj, DatalogAppTask_GetILogControllerIF((DatalogAppTask *) spDatalogAppObj));
  Tags_Info_PnPLInit(pTags_Info_PnPLObj);
  Acquisition_Info_PnPLInit(pAcquisition_Info_PnPLObj);
  Firmware_Info_PnPLInit(pFirmware_Info_PnPLObj);
  Deviceinformation_PnPLInit(pDeviceinformation_PnPLObj);
  Iis2dlpc_Acc_PnPLInit(pIis2dlpc_Acc_PnPLObj);
  Iis2mdc_Mag_PnPLInit(pIis2mdc_Mag_PnPLObj);
  Ism330dhcx_Acc_PnPLInit(pIsm330dhcx_Acc_PnPLObj);
  Ism330dhcx_Gyro_PnPLInit(pIsm330dhcx_Gyro_PnPLObj);
  Ism330dhcx_Mlc_PnPLInit(pIsm330dhcx_Mlc_PnPLObj, &iIsm330dhcx_Mlc);

  return SYS_NO_ERROR_CODE;
}

/*
IApplicationErrorDelegate *SysGetErrorDelegate(void)
{
  // Install the application error manager delegate.
  static IApplicationErrorDelegate *s_pxErrDelegate = NULL;
  if (s_pxErrDelegate == NULL)
  {
    s_pxErrDelegate = AEMAlloc();
  }

  return s_pxErrDelegate;
}
*/

IAppPowerModeHelper *SysGetPowerModeHelper(void)
{
  /* Install the application power mode helper. */
  static IAppPowerModeHelper *s_pxPowerModeHelper = NULL;
  if (s_pxPowerModeHelper == NULL) {
    s_pxPowerModeHelper = AppPowerModeHelperAlloc();
  }

  return s_pxPowerModeHelper;
}



