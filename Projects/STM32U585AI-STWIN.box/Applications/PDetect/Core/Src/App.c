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
#include "stts22h_reg.h"

#include "mx.h"

#include "UtilTask.h"
#include "SPIBusTask.h"
#include "I2CBusTask.h"
#include "ISM330DHCXTask.h"
#include "IIS2MDCTask.h"
#include "ILPS22QSTask.h"
#include "STTS22HTask.h"
#include "IMP34DT05Task.h"
#include "IIS2ICLXTask.h"

#include "VL53L8CXTask.h"
#include "STHS34PF80Task.h"
#include "VD6283TXTask.h"

#include "SHT40Task.h"
#include "SGP40Task.h"
#include "LPS22DFTask.h"

#include "DatalogAppTask.h"
#include "App_model.h"

#include "PnPLCompManager.h"
#include "Ism330dhcx_Acc_PnPL.h"
#include "Ism330dhcx_Gyro_PnPL.h"
#include "Iis2mdc_Mag_PnPL.h"
#include "Stts22h_Temp_PnPL.h"
#include "Ilps22qs_Press_PnPL.h"
#include "Imp34dt05_Mic_PnPL.h"
#include "Iis2iclx_Acc_PnPL.h"
#include "Automode_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Deviceinformation_PnPL.h"

#include "Vl53l8cx_Tof_PnPL.h"
#include "Vd6283tx_Als_PnPL.h"
#include "Sths34pf80_Tmos_PnPL.h"
#include "Vl53l8cx_2_Tof_PnPL.h"
#include "Vd6283tx_2_Als_PnPL.h"
#include "Sths34pf80_2_Tmos_PnPL.h"
#include "Vl53l8cx_3_Tof_PnPL.h"
#include "Vd6283tx_3_Als_PnPL.h"
#include "Sths34pf80_3_Tmos_PnPL.h"

#include "Sgp40_Voc_PnPL.h"
#include "Sht40_Hum_PnPL.h"
#include "Sht40_Temp_PnPL.h"
#include "Lps22df_Press_PnPL.h"

static uint8_t BoardId = BOARD_ID_BOXA;
static uint8_t FwId = USB_FW_ID_DATALOG2_BOXA;

static IPnPLComponent_t *pLogControllerPnPLObj = NULL;
static IPnPLComponent_t *pDeviceInfoPnPLObj = NULL;
static IPnPLComponent_t *pFirmwareInfoPnPLObj = NULL;
static IPnPLComponent_t *pAcquisitionInfoPnPLObj = NULL;
static IPnPLComponent_t *pTagsInfoPnPLObj = NULL;
static IPnPLComponent_t *pISM330DHCX_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330DHCX_GYRO_PnPLObj = NULL;
static IPnPLComponent_t *pIIS2MDC_MAG_PnPLObj = NULL;
static IPnPLComponent_t *pSTTS22H_TEMP_PnPLObj = NULL;
static IPnPLComponent_t *pILPS22QS_PRESS_PnPLObj = NULL;
static IPnPLComponent_t *pIMP34DT05_MIC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS2ICLX_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pAutomodePnPLObj = NULL;
/* PDETECT 1 */
static IPnPLComponent_t *pVl53l8cx_Tof_PnPLObj = NULL;
static IPnPLComponent_t *pVd6283tx_Als_PnPLObj = NULL;
static IPnPLComponent_t *pSths34pf80_Tmos_PnPLObj = NULL;
/* PDETECT 2 */
static IPnPLComponent_t *pVl53l8cx_2_Tof_PnPLObj = NULL;
static IPnPLComponent_t *pVd6283tx_2_Als_PnPLObj = NULL;
static IPnPLComponent_t *pSths34pf80_2_Tmos_PnPLObj = NULL;
/* PDETECT 3 */
static IPnPLComponent_t *pVl53l8cx_3_Tof_PnPLObj = NULL;
static IPnPLComponent_t *pVd6283tx_3_Als_PnPLObj = NULL;
static IPnPLComponent_t *pSths34pf80_3_Tmos_PnPLObj = NULL;

static IPnPLComponent_t *pSgp40_Voc_PnPLObj = NULL;
static IPnPLComponent_t *pSht40_Hum_PnPLObj = NULL;
static IPnPLComponent_t *pSht40_Temp_PnPLObj = NULL;
static IPnPLComponent_t *pLps22df_Press_PnPLObj = NULL;


/**
  * Utility task object.
  */
static AManagedTaskEx *sUtilObj = NULL;

/**
  * Bus task object.
  */
static AManagedTaskEx *sSPI2BusObj = NULL;
static AManagedTaskEx *sI2C2BusObj = NULL;
static AManagedTaskEx *sI2C3BusObj = NULL;

/**
  * Sensor task object.
  */
static AManagedTaskEx *sISM330DHCXObj = NULL;
static AManagedTaskEx *sIIS2MDCObj = NULL;
static AManagedTaskEx *sILPS22QSObj = NULL;
static AManagedTaskEx *sSTTS22HObj = NULL;
static AManagedTaskEx *sIMP34DT05Obj = NULL;
static AManagedTaskEx *sIIS2ICLXObj = NULL;
/* PDETECT 1 */
static AManagedTaskEx *spVL53L8CXObj = NULL;
static AManagedTaskEx *spSTHS34PF80Obj = NULL;
static AManagedTaskEx *spVD6283TXObj = NULL;
/* PDETECT 2 */
static AManagedTaskEx *spVL53L8CX_2Obj = NULL;
static AManagedTaskEx *spSTHS34PF80_2Obj = NULL;
static AManagedTaskEx *spVD6283TX_2Obj = NULL;
/* PDETECT 3 */
static AManagedTaskEx *spVL53L8CX_3Obj = NULL;
static AManagedTaskEx *spSTHS34PF80_3Obj = NULL;
static AManagedTaskEx *spVD6283TX_3Obj = NULL;

static AManagedTaskEx *spSHT40Obj = NULL;
static AManagedTaskEx *spSGP40Obj = NULL;
static AManagedTaskEx *spLPS22DFObj = NULL;
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
  uint8_t sths34pf80_address;
  boolean_t ext_pdetect = FALSE;
  boolean_t ext_pdetect2 = FALSE;
  boolean_t ext_pdetect3 = FALSE;
  boolean_t ext_sensirion = FALSE;

  /* PnPL thread safe mutex creation */
  tx_mutex_create(&pnpl_mutex, "PnPL Mutex", TX_INHERIT);

  /* PnPL thread safe function registration */
  PnPL_SetLockUnlockCallbacks(PnPL_lock_fp, PnPL_unlock_fp);

  PnPLSetAllocationFunctions(SysAlloc, SysFree);

  /* Check if PDETECT is connected */
  ext_pdetect = HardwareDetection_Check_Ext_PDETECT(&sths34pf80_address);
  ext_pdetect2 = HardwareDetection_Check_Ext_PDETECT2(&sths34pf80_address);
  ext_pdetect3 = HardwareDetection_Check_Ext_PDETECT3(&sths34pf80_address);
  ext_sensirion = HardwareDetection_Check_Ext_SENSIRION();

  /************ Allocate task objects ************/
  sUtilObj = UtilTaskAlloc(&MX_TIM4InitParams, &MX_GPIO_PA8InitParams, &MX_GPIO_PA0InitParams, &MX_GPIO_PD0InitParams, &MX_TIM5InitParams, &MX_ADC4InitParams,
                           &MX_GPIO_UBInitParams, &MX_GPIO_LED1InitParams, &MX_GPIO_LED2InitParams);
  sDatalogAppObj = DatalogAppTaskAlloc();
  sI2C2BusObj = I2CBusTaskAlloc(&MX_I2C2InitParams);
  sSPI2BusObj = SPIBusTaskAlloc(&MX_SPI2InitParams);
  sIIS2ICLXObj = IIS2ICLXTaskAlloc(NULL, NULL, &MX_GPIO_CS_ICLXInitParams);
  sIIS2MDCObj = IIS2MDCTaskAlloc(&MX_GPIO_INT_MAGInitParams, NULL);
  sISM330DHCXObj = ISM330DHCXTaskAlloc(&MX_GPIO_INT1_DHCXInitParams, NULL, &MX_GPIO_CS_DHCXInitParams);
  sSTTS22HObj = STTS22HTaskAlloc(NULL, NULL, STTS22H_I2C_ADD_L);
  sILPS22QSObj = ILPS22QSTaskAlloc(NULL, NULL);
  sIMP34DT05Obj = IMP34DT05TaskAlloc(&MX_ADF1InitParams);
  if (ext_pdetect == true)
  {
    sI2C3BusObj = I2CBusTaskAlloc(&MX_I2C3InitParams);
    if (ext_pdetect2 == false && ext_pdetect3 == false && ext_sensirion == false)
    {
      HAL_GPIO_WritePin(GPIO3_EX_GPIO_Port, GPIO3_EX_Pin, GPIO_PIN_SET);
      spVL53L8CXObj = VL53L8CXTaskAlloc(&MX_GPIO_INT_TOFInitParams, NULL, NULL);
      spSTHS34PF80Obj = STHS34PF80TaskAlloc(&MX_GPIO_INT_TMOSInitParams, NULL, NULL);
      spVD6283TXObj = VD6283TXTaskAlloc(NULL, NULL, NULL);
    }
    else
    {
      spVL53L8CXObj = VL53L8CXTaskAlloc(NULL, NULL, &MX_GPIO3_EXInitParams);
      spSTHS34PF80Obj = STHS34PF80TaskAlloc(NULL, NULL, &MX_GPIO3_EXInitParams);
      spVD6283TXObj = VD6283TXTaskAlloc(NULL, NULL, &MX_GPIO3_EXInitParams);
      if (ext_pdetect2 == true)
      {
        spVL53L8CX_2Obj = VL53L8CXTaskAllocSetName(NULL, NULL, &MX_GPIO2_EXInitParams, "vl53l8cx_2");
        spSTHS34PF80_2Obj = STHS34PF80TaskAllocSetName(NULL, NULL, &MX_GPIO2_EXInitParams, "sths34pf80_2");
        spVD6283TX_2Obj = VD6283TXTaskAllocSetName(NULL, NULL, &MX_GPIO2_EXInitParams, "vd6283tx_2");
      }
      if (ext_pdetect3 == true)
      {
        spVL53L8CX_3Obj = VL53L8CXTaskAllocSetName(NULL, NULL, &MX_GPIO1_EXInitParams, "vl53l8cx_3");
        spSTHS34PF80_3Obj = STHS34PF80TaskAllocSetName(NULL, NULL, &MX_GPIO1_EXInitParams, "sths34pf80_3");
        spVD6283TX_3Obj = VD6283TXTaskAllocSetName(NULL, NULL, &MX_GPIO1_EXInitParams, "vd6283tx_3");
      }
      if (ext_sensirion == true)
      {
        spSHT40Obj = SHT40TaskAlloc(NULL, NULL);
        spSGP40Obj = SGP40TaskAlloc(NULL, NULL);
        spLPS22DFObj = LPS22DFTaskAlloc(NULL, NULL);
      }
    }
  }


  /************ Add the task object to the context ************/
  res = ACAddTask(pAppContext, (AManagedTask *) sUtilObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sDatalogAppObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sI2C2BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSPI2BusObj);
  if (spSTHS34PF80Obj)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sI2C3BusObj);
    res = ACAddTask(pAppContext, (AManagedTask *)spVL53L8CXObj);
    res = ACAddTask(pAppContext, (AManagedTask *)spSTHS34PF80Obj);
    res = ACAddTask(pAppContext, (AManagedTask *)spVD6283TXObj);
  }
  if (spSTHS34PF80_2Obj)
  {
    res = ACAddTask(pAppContext, (AManagedTask *)spVL53L8CX_2Obj);
    res = ACAddTask(pAppContext, (AManagedTask *)spSTHS34PF80_2Obj);
    res = ACAddTask(pAppContext, (AManagedTask *)spVD6283TX_2Obj);
  }
  if (spSTHS34PF80_3Obj)
  {
    res = ACAddTask(pAppContext, (AManagedTask *)spVL53L8CX_3Obj);
    res = ACAddTask(pAppContext, (AManagedTask *)spSTHS34PF80_3Obj);
    res = ACAddTask(pAppContext, (AManagedTask *)spVD6283TX_3Obj);
  }
  if (spLPS22DFObj)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) spSHT40Obj);
    res = ACAddTask(pAppContext, (AManagedTask *) spSGP40Obj);
    res = ACAddTask(pAppContext, (AManagedTask *) spLPS22DFObj);
  }
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2ICLXObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2MDCObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sILPS22QSObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIMP34DT05Obj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSTTS22HObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sISM330DHCXObj);

  pIIS2ICLX_ACC_PnPLObj = Iis2iclx_Acc_PnPLAlloc();
  pIIS2MDC_MAG_PnPLObj = Iis2mdc_Mag_PnPLAlloc();
  pILPS22QS_PRESS_PnPLObj = Ilps22qs_Press_PnPLAlloc();
  pIMP34DT05_MIC_PnPLObj = Imp34dt05_Mic_PnPLAlloc();
  pSTTS22H_TEMP_PnPLObj = Stts22h_Temp_PnPLAlloc();
  pISM330DHCX_ACC_PnPLObj = Ism330dhcx_Acc_PnPLAlloc();
  pISM330DHCX_GYRO_PnPLObj = Ism330dhcx_Gyro_PnPLAlloc();
  if (spSTHS34PF80Obj)
  {
    pVl53l8cx_Tof_PnPLObj = Vl53l8cx_Tof_PnPLAlloc();
    pSths34pf80_Tmos_PnPLObj = Sths34pf80_Tmos_PnPLAlloc();
    pVd6283tx_Als_PnPLObj = Vd6283tx_Als_PnPLAlloc();
  }
  if (spSTHS34PF80_2Obj)
  {
    pVl53l8cx_2_Tof_PnPLObj = Vl53l8cx_2_Tof_PnPLAlloc();
    pSths34pf80_2_Tmos_PnPLObj = Sths34pf80_2_Tmos_PnPLAlloc();
    pVd6283tx_2_Als_PnPLObj = Vd6283tx_2_Als_PnPLAlloc();
  }
  if (spSTHS34PF80_3Obj)
  {
    pVl53l8cx_3_Tof_PnPLObj = Vl53l8cx_3_Tof_PnPLAlloc();
    pSths34pf80_3_Tmos_PnPLObj = Sths34pf80_3_Tmos_PnPLAlloc();
    pVd6283tx_3_Als_PnPLObj = Vd6283tx_3_Als_PnPLAlloc();
  }
  if (spLPS22DFObj)
  {
    pSgp40_Voc_PnPLObj = Sgp40_Voc_PnPLAlloc();
    pSht40_Hum_PnPLObj = Sht40_Hum_PnPLAlloc();
    pSht40_Temp_PnPLObj = Sht40_Temp_PnPLAlloc();
    pLps22df_Press_PnPLObj = Lps22df_Press_PnPLAlloc();
  }

  pDeviceInfoPnPLObj = Deviceinformation_PnPLAlloc();
  pAcquisitionInfoPnPLObj = Acquisition_Info_PnPLAlloc();
  pTagsInfoPnPLObj = Tags_Info_PnPLAlloc();
  pFirmwareInfoPnPLObj = Firmware_Info_PnPLAlloc();
  pLogControllerPnPLObj = Log_Controller_PnPLAlloc();
  pAutomodePnPLObj = Automode_PnPLAlloc();


  PnPLSetBOARDID(BoardId);
  PnPLSetFWID(FwId);

  return res;
}

sys_error_code_t SysOnStartApplication(ApplicationContext *pAppContext)
{
  UNUSED(pAppContext);

  /************ Connect the sensor task to the bus ************/
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C2BusObj, (I2CBusIF *)IIS2MDCTaskGetSensorIF((IIS2MDCTask *) sIIS2MDCObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C2BusObj, (I2CBusIF *)ILPS22QSTaskGetSensorIF((ILPS22QSTask *) sILPS22QSObj));

  /* Use I2C2 for Internal STTS22H */
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C2BusObj, (I2CBusIF *)STTS22HTaskGetSensorIF((STTS22HTask *) sSTTS22HObj));
  SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)IIS2ICLXTaskGetSensorIF((IIS2ICLXTask *) sIIS2ICLXObj));
  SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj,
                          (SPIBusIF *)ISM330DHCXTaskGetSensorIF((ISM330DHCXTask *) sISM330DHCXObj));
  if (spSTHS34PF80Obj)
  {
    I2CBusTaskConnectDevice((I2CBusTask *)sI2C3BusObj, (I2CBusIF *)VL53L8CXTaskGetSensorIF((VL53L8CXTask *)spVL53L8CXObj));
    I2CBusTaskConnectDevice((I2CBusTask *)sI2C3BusObj,
                            (I2CBusIF *)STHS34PF80TaskGetSensorIF((STHS34PF80Task *)spSTHS34PF80Obj));
    I2CBusTaskConnectDevice((I2CBusTask *)sI2C3BusObj, (I2CBusIF *)VD6283TXTaskGetSensorIF((VD6283TXTask *)spVD6283TXObj));
  }
  if (spSTHS34PF80_2Obj)
  {
    I2CBusTaskConnectDevice((I2CBusTask *)sI2C3BusObj,
                            (I2CBusIF *)VL53L8CXTaskGetSensorIF((VL53L8CXTask *)spVL53L8CX_2Obj));
    I2CBusTaskConnectDevice((I2CBusTask *)sI2C3BusObj,
                            (I2CBusIF *)STHS34PF80TaskGetSensorIF((STHS34PF80Task *)spSTHS34PF80_2Obj));
    I2CBusTaskConnectDevice((I2CBusTask *)sI2C3BusObj,
                            (I2CBusIF *)VD6283TXTaskGetSensorIF((VD6283TXTask *)spVD6283TX_2Obj));
  }
  if (spSTHS34PF80_3Obj)
  {
    I2CBusTaskConnectDevice((I2CBusTask *)sI2C3BusObj,
                            (I2CBusIF *)VL53L8CXTaskGetSensorIF((VL53L8CXTask *)spVL53L8CX_3Obj));
    I2CBusTaskConnectDevice((I2CBusTask *)sI2C3BusObj,
                            (I2CBusIF *)STHS34PF80TaskGetSensorIF((STHS34PF80Task *)spSTHS34PF80_3Obj));
    I2CBusTaskConnectDevice((I2CBusTask *)sI2C3BusObj,
                            (I2CBusIF *)VD6283TXTaskGetSensorIF((VD6283TXTask *)spVD6283TX_3Obj));
  }
  if (spLPS22DFObj)
  {
    I2CBusTaskConnectDevice((I2CBusTask *)sI2C3BusObj, (I2CBusIF *)SHT40TaskGetSensorIF((SHT40Task *)spSHT40Obj));
    I2CBusTaskConnectDevice((I2CBusTask *)sI2C3BusObj, (I2CBusIF *)SGP40TaskGetSensorIF((SGP40Task *)spSGP40Obj));
    I2CBusTaskConnectDevice((I2CBusTask *)sI2C3BusObj, (I2CBusIF *)LPS22DFTaskGetSensorIF((LPS22DFTask *)spLPS22DFObj));
  }

  /************ Connect the Sensor events to the DatalogAppTask ************/
  IEventListener *DatalogAppListener = DatalogAppTask_GetEventListenerIF((DatalogAppTask *) sDatalogAppObj);
  IEventSrcAddEventListener(IIS2ICLXTaskGetEventSrcIF((IIS2ICLXTask *) sIIS2ICLXObj), DatalogAppListener);
  IEventSrcAddEventListener(IIS2MDCTaskGetMagEventSrcIF((IIS2MDCTask *) sIIS2MDCObj), DatalogAppListener);
  IEventSrcAddEventListener(ILPS22QSTaskGetPressEventSrcIF((ILPS22QSTask *) sILPS22QSObj), DatalogAppListener);
  IEventSrcAddEventListener(IMP34DT05TaskGetEventSrcIF((IMP34DT05Task *) sIMP34DT05Obj), DatalogAppListener);
  IEventSrcAddEventListener(STTS22HTaskGetTempEventSrcIF((STTS22HTask *) sSTTS22HObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330DHCXTaskGetAccEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
  IEventSrcAddEventListener(ISM330DHCXTaskGetGyroEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
  if (spSTHS34PF80Obj)
  {
    IEventSrcAddEventListener(VL53L8CXTaskGetEventSrcIF((VL53L8CXTask *)spVL53L8CXObj), DatalogAppListener);
    IEventSrcAddEventListener(STHS34PF80TaskGetEventSrcIF((STHS34PF80Task *)spSTHS34PF80Obj), DatalogAppListener);
    IEventSrcAddEventListener(VD6283TXTaskGetEventSrcIF((VD6283TXTask *)spVD6283TXObj), DatalogAppListener);
  }
  if (spSTHS34PF80_2Obj)
  {
    IEventSrcAddEventListener(VL53L8CXTaskGetEventSrcIF((VL53L8CXTask *)spVL53L8CX_2Obj), DatalogAppListener);
    IEventSrcAddEventListener(STHS34PF80TaskGetEventSrcIF((STHS34PF80Task *)spSTHS34PF80_2Obj), DatalogAppListener);
    IEventSrcAddEventListener(VD6283TXTaskGetEventSrcIF((VD6283TXTask *)spVD6283TX_2Obj), DatalogAppListener);
  }
  if (spSTHS34PF80_3Obj)
  {
    IEventSrcAddEventListener(VL53L8CXTaskGetEventSrcIF((VL53L8CXTask *)spVL53L8CX_3Obj), DatalogAppListener);
    IEventSrcAddEventListener(STHS34PF80TaskGetEventSrcIF((STHS34PF80Task *)spSTHS34PF80_3Obj), DatalogAppListener);
    IEventSrcAddEventListener(VD6283TXTaskGetEventSrcIF((VD6283TXTask *)spVD6283TX_3Obj), DatalogAppListener);
  }
  if (spLPS22DFObj)
  {
    IEventSrcAddEventListener(SGP40TaskGetTempEventSrcIF((SGP40Task *)spSGP40Obj), DatalogAppListener);
    IEventSrcAddEventListener(SHT40TaskGetHumEventSrcIF((SHT40Task *)spSHT40Obj), DatalogAppListener);
    IEventSrcAddEventListener(SHT40TaskGetTempEventSrcIF((SHT40Task *)spSHT40Obj), DatalogAppListener);
    IEventSrcAddEventListener(LPS22DFTaskGetPressEventSrcIF((LPS22DFTask *)spLPS22DFObj), DatalogAppListener);
  }

  /************ Other PnPL Components ************/
  Deviceinformation_PnPLInit(pDeviceInfoPnPLObj);
  Firmware_Info_PnPLInit(pFirmwareInfoPnPLObj);
  Acquisition_Info_PnPLInit(pAcquisitionInfoPnPLObj);
  Tags_Info_PnPLInit(pTagsInfoPnPLObj);
  Log_Controller_PnPLInit(pLogControllerPnPLObj);
  Automode_PnPLInit(pAutomodePnPLObj);

  /************ Sensor PnPL Components ************/
  Iis2iclx_Acc_PnPLInit(pIIS2ICLX_ACC_PnPLObj);
  Iis2mdc_Mag_PnPLInit(pIIS2MDC_MAG_PnPLObj);
  Ilps22qs_Press_PnPLInit(pILPS22QS_PRESS_PnPLObj);
  Imp34dt05_Mic_PnPLInit(pIMP34DT05_MIC_PnPLObj);
  Stts22h_Temp_PnPLInit(pSTTS22H_TEMP_PnPLObj);
  Ism330dhcx_Acc_PnPLInit(pISM330DHCX_ACC_PnPLObj);
  Ism330dhcx_Gyro_PnPLInit(pISM330DHCX_GYRO_PnPLObj);
  if (spSTHS34PF80Obj)
  {
    Vl53l8cx_Tof_PnPLInit(pVl53l8cx_Tof_PnPLObj);
    Sths34pf80_Tmos_PnPLInit(pSths34pf80_Tmos_PnPLObj);
    Vd6283tx_Als_PnPLInit(pVd6283tx_Als_PnPLObj);
  }
  if (spSTHS34PF80_2Obj)
  {
    Vl53l8cx_2_Tof_PnPLInit(pVl53l8cx_2_Tof_PnPLObj);
    Sths34pf80_2_Tmos_PnPLInit(pSths34pf80_2_Tmos_PnPLObj);
    Vd6283tx_2_Als_PnPLInit(pVd6283tx_2_Als_PnPLObj);
  }
  if (spSTHS34PF80_3Obj)
  {
    Vl53l8cx_3_Tof_PnPLInit(pVl53l8cx_3_Tof_PnPLObj);
    Sths34pf80_3_Tmos_PnPLInit(pSths34pf80_3_Tmos_PnPLObj);
    Vd6283tx_3_Als_PnPLInit(pVd6283tx_3_Als_PnPLObj);
  }
  if (spLPS22DFObj)
  {
    Sgp40_Voc_PnPLInit(pSgp40_Voc_PnPLObj);
    Sht40_Hum_PnPLInit(pSht40_Hum_PnPLObj);
    Sht40_Temp_PnPLInit(pSht40_Temp_PnPLObj);
    Lps22df_Press_PnPLInit(pLps22df_Press_PnPLObj);
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
