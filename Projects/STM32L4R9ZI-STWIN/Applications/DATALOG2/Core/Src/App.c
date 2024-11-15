/**
  ******************************************************************************
  * @file    App.c
  * @author  SRA - GPM
  *
  *
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
  * This is the most simple eLooM application. It shows how to setup an eLooM project.
  * It create one managed task (::HelloWorldTask_t). The task display
  * a greeting message in the debug log every second.
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
  *********************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *********************************************************************************
  */

#include "services/sysdebug.h"
#include "services/ApplicationContext.h"
#include "AppPowerModeHelper.h"
#include "mx.h"

#include "UtilTask.h"
#include "SPIBusTask.h"
#include "I2CBusTask.h"
#include "IIS3DWBTask.h"
#include "ISM330DHCXTask.h"
#include "IIS2MDCTask.h"
#include "LPS22HHTask.h"
#include "IIS2DHTask.h"
#include "STTS751Task.h"
#include "IMP23ABSU_DFSDMTask.h"
#include "IMP34DT05_DFSDMTask.h"

#include "DatalogAppTask.h"
#include "App_model.h"

#include "PnPLCompManager.h"
#include "Deviceinformation_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Iis3dwb_Acc_PnPL.h"
#include "Ism330dhcx_Acc_PnPL.h"
#include "Ism330dhcx_Gyro_PnPL.h"
#include "Ism330dhcx_Mlc_PnPL.h"
#include "Iis2mdc_Mag_PnPL.h"
#include "Lps22hh_Press_PnPL.h"
#include "Lps22hh_Temp_PnPL.h"
#include "Iis2dh_Acc_PnPL.h"
#include "Stts751_Temp_PnPL.h"
#include "Imp23absu_Mic_PnPL.h"
#include "Imp34dt05_Mic_PnPL.h"
#include "Automode_PnPL.h"

static IPnPLComponent_t *pIis3dwb_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pIis2mdc_Mag_PnPLObj = NULL;
static IPnPLComponent_t *pImp23absu_Mic_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330dhcx_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330dhcx_Gyro_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330dhcx_Mlc_PnPLObj = NULL;
static IPnPLComponent_t *pImp34dt05_Mic_PnPLObj = NULL;
static IPnPLComponent_t *pIis2dh_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pStts751_Temp_PnPLObj = NULL;
static IPnPLComponent_t *pLps22hh_Press_PnPLObj = NULL;
static IPnPLComponent_t *pLps22hh_Temp_PnPLObj = NULL;
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
static AManagedTaskEx *sSPI3BusObj = NULL;
static AManagedTaskEx *sI2C2BusObj = NULL;

/**
  * Sensor task object.
  */
static AManagedTaskEx *sIIS3DWBObj = NULL;
static AManagedTaskEx *sISM330DHCXObj = NULL;
static AManagedTaskEx *sIIS2MDCObj = NULL;
static AManagedTaskEx *sLPS22HHObj = NULL;
static AManagedTaskEx *sIIS2DHObj = NULL;
static AManagedTaskEx *sSTTS751Obj = NULL;
static AManagedTaskEx *sIMP23ABSUObj = NULL;
static AManagedTaskEx *sIMP34DT05Obj = NULL;

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
  assert_param(pAppContext != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* PnPL thread safe mutex creation */
  tx_mutex_create(&pnpl_mutex, "PnPL Mutex", TX_INHERIT);

  /* PnPL thread safe function registration */
  PnPL_SetLockUnlockCallbacks(PnPL_lock_fp, PnPL_unlock_fp);

  PnPLSetAllocationFunctions(SysAlloc, SysFree);

  /* Allocate the task objects */
  sUtilObj = UtilTaskAlloc(&MX_TIM4InitParams, &MX_GPIO_PF6InitParams, &MX_GPIO_PB0InitParams, &MX_GPIO_PF8InitParams, &MX_TIM3InitParams, &MX_ADC1InitParams,
                           &MX_GPIO_UBInitParams, &MX_GPIO_LED1InitParams, &MX_GPIO_LED2InitParams);

  sSPI3BusObj = SPIBusTaskAlloc(&MX_SPI3InitParams);
  sI2C2BusObj = I2CBusTaskAlloc(&MX_I2C2InitParams);
  sDatalogAppObj = DatalogAppTaskAlloc();
  sIIS3DWBObj = IIS3DWBTaskAlloc(&MX_GPIO_INT1_DWBInitParams, &MX_GPIO_CS_DWBInitParams);
  sIIS2MDCObj = IIS2MDCTaskAlloc(&MX_GPIO_INT_MAGInitParams, NULL);
  sISM330DHCXObj = ISM330DHCXTaskAlloc(&MX_GPIO_INT1_DHCXInitParams, &MX_GPIO_INT2_DHCXInitParams, &MX_GPIO_CS_DHCXInitParams);
  sLPS22HHObj = LPS22HHTaskAlloc(NULL, NULL);
  sIIS2DHObj = IIS2DHTaskAlloc(NULL, &MX_GPIO_CS_DHInitParams);
  sSTTS751Obj = STTS751TaskAlloc(NULL, NULL);
  sIMP23ABSUObj = IMP23ABSUTaskAlloc(&MX_DFSDMCH0F1InitParams, &MX_ADC1InitParams);
  sIMP34DT05Obj = IMP34DT05TaskAlloc(&MX_DFSDMCH5F0InitParams);

  /* Add the task object to the context. */

  /************ Add the task object to the context ************/
  res = ACAddTask(pAppContext, (AManagedTask *) sDatalogAppObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSPI3BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sI2C2BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS3DWBObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2MDCObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sLPS22HHObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sISM330DHCXObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2DHObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSTTS751Obj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIMP23ABSUObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIMP34DT05Obj);
  res = ACAddTask(pAppContext, (AManagedTask *) sUtilObj);

  /* PnPL Components Allocation */
  pIis3dwb_Acc_PnPLObj = Iis3dwb_Acc_PnPLAlloc();
  pIis2mdc_Mag_PnPLObj = Iis2mdc_Mag_PnPLAlloc();
  pImp23absu_Mic_PnPLObj = Imp23absu_Mic_PnPLAlloc();
  pIsm330dhcx_Acc_PnPLObj = Ism330dhcx_Acc_PnPLAlloc();
  pIsm330dhcx_Gyro_PnPLObj = Ism330dhcx_Gyro_PnPLAlloc();
  pIsm330dhcx_Mlc_PnPLObj = Ism330dhcx_Mlc_PnPLAlloc();
  pImp34dt05_Mic_PnPLObj = Imp34dt05_Mic_PnPLAlloc();
  pIis2dh_Acc_PnPLObj = Iis2dh_Acc_PnPLAlloc();
  pStts751_Temp_PnPLObj = Stts751_Temp_PnPLAlloc();
  pLps22hh_Press_PnPLObj = Lps22hh_Press_PnPLAlloc();
  pLps22hh_Temp_PnPLObj = Lps22hh_Temp_PnPLAlloc();
  pAutomode_PnPLObj = Automode_PnPLAlloc();
  pLog_Controller_PnPLObj = Log_Controller_PnPLAlloc();
  pTags_Info_PnPLObj = Tags_Info_PnPLAlloc();
  pAcquisition_Info_PnPLObj = Acquisition_Info_PnPLAlloc();
  pFirmware_Info_PnPLObj = Firmware_Info_PnPLAlloc();
  pDeviceinformation_PnPLObj = Deviceinformation_PnPLAlloc();

  PnPLSetFWID(USB_FW_ID_DATALOG2);

  return res;
}

sys_error_code_t SysOnStartApplication(ApplicationContext *pAppContext)
{
  UNUSED(pAppContext);

  /************ Connect the sensor task to the bus ************/
  SPIBusTaskConnectDevice((SPIBusTask *) sSPI3BusObj, (SPIBusIF *)IIS2DHTaskGetSensorIF((IIS2DHTask *) sIIS2DHObj));
  SPIBusTaskConnectDevice((SPIBusTask *) sSPI3BusObj, (SPIBusIF *)IIS3DWBTaskGetSensorIF((IIS3DWBTask *) sIIS3DWBObj));
  SPIBusTaskConnectDevice((SPIBusTask *) sSPI3BusObj,
                          (SPIBusIF *)ISM330DHCXTaskGetSensorIF((ISM330DHCXTask *) sISM330DHCXObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C2BusObj, (I2CBusIF *)IIS2MDCTaskGetSensorIF((IIS2MDCTask *) sIIS2MDCObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C2BusObj, (I2CBusIF *)LPS22HHTaskGetSensorIF((LPS22HHTask *) sLPS22HHObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C2BusObj, (I2CBusIF *)STTS751TaskGetSensorIF((STTS751Task *) sSTTS751Obj));

  /************ Connect the Sensor events to the DatalogAppTask ************/
  IEventListener *DatalogAppListener = DatalogAppTask_GetEventListenerIF((DatalogAppTask *) sDatalogAppObj);
  IEventSrcAddEventListener(IIS3DWBTaskGetEventSrcIF((IIS3DWBTask *) sIIS3DWBObj), DatalogAppListener);
  IEventSrcAddEventListener(IIS2MDCTaskGetMagEventSrcIF((IIS2MDCTask *) sIIS2MDCObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330DHCXTaskGetAccEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330DHCXTaskGetGyroEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330DHCXTaskGetMlcEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
  IEventSrcAddEventListener(LPS22HHTaskGetPressEventSrcIF((LPS22HHTask *) sLPS22HHObj), DatalogAppListener);
  IEventSrcAddEventListener(LPS22HHTaskGetTempEventSrcIF((LPS22HHTask *) sLPS22HHObj), DatalogAppListener);
  IEventSrcAddEventListener(IIS2DHTaskGetEventSrcIF((IIS2DHTask *) sIIS2DHObj), DatalogAppListener);
  IEventSrcAddEventListener(STTS751TaskGetTempEventSrcIF((STTS751Task *) sSTTS751Obj), DatalogAppListener);
  IEventSrcAddEventListener(IMP23ABSUTaskGetEventSrcIF((IMP23ABSUTask *) sIMP23ABSUObj), DatalogAppListener);
  IEventSrcAddEventListener(IMP34DT05TaskGetEventSrcIF((IMP34DT05Task *) sIMP34DT05Obj), DatalogAppListener);

  /************ Connect Sensor LL to be used for ucf management to the DatalogAppTask ************/
  DatalogAppTask_SetMLCIF((AManagedTask *) sISM330DHCXObj);

  /* Init&Add PnPL Components */
  Automode_PnPLInit(pAutomode_PnPLObj);
  Log_Controller_PnPLInit(pLog_Controller_PnPLObj);
  Tags_Info_PnPLInit(pTags_Info_PnPLObj);
  Acquisition_Info_PnPLInit(pAcquisition_Info_PnPLObj);
  Firmware_Info_PnPLInit(pFirmware_Info_PnPLObj);
  Deviceinformation_PnPLInit(pDeviceinformation_PnPLObj);
  Iis3dwb_Acc_PnPLInit(pIis3dwb_Acc_PnPLObj);
  Iis2mdc_Mag_PnPLInit(pIis2mdc_Mag_PnPLObj);
  Imp23absu_Mic_PnPLInit(pImp23absu_Mic_PnPLObj);
  Ism330dhcx_Acc_PnPLInit(pIsm330dhcx_Acc_PnPLObj);
  Ism330dhcx_Gyro_PnPLInit(pIsm330dhcx_Gyro_PnPLObj);
  Ism330dhcx_Mlc_PnPLInit(pIsm330dhcx_Mlc_PnPLObj);
  Imp34dt05_Mic_PnPLInit(pImp34dt05_Mic_PnPLObj);
  Iis2dh_Acc_PnPLInit(pIis2dh_Acc_PnPLObj);
  Stts751_Temp_PnPLInit(pStts751_Temp_PnPLObj);
  Lps22hh_Press_PnPLInit(pLps22hh_Press_PnPLObj);
  Lps22hh_Temp_PnPLInit(pLps22hh_Temp_PnPLObj);

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

