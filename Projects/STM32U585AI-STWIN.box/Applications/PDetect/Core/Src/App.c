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

#include "DatalogAppTask.h"
#include "App_model.h"

#include "PnPLCompManager.h"
#include "Deviceinformation_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Ism330dhcx_Acc_PnPL.h"
#include "Ism330dhcx_Gyro_PnPL.h"
#include "Iis2mdc_Mag_PnPL.h"
#include "Stts22h_Temp_PnPL.h"
#include "Ilps22qs_Press_PnPL.h"
#include "Imp34dt05_Mic_PnPL.h"
#include "Iis2iclx_Acc_PnPL.h"
#include "Automode_PnPL.h"
#include "parson.h"

#include "Vl53l8cx_Tof_PnPL.h"
#include "Vd6283tx_Als_PnPL.h"
#include "Sths34pf80_Tmos_PnPL.h"

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

static IPnPLComponent_t *pVl53l8cx_Tof_PnPLObj = NULL;
static IPnPLComponent_t *pVd6283tx_Als_PnPLObj = NULL;
static IPnPLComponent_t *pSths34pf80_Tmos_PnPLObj = NULL;

#if (DATALOG2_USE_WIFI == 1)
static IPnPLComponent_t *pWifiConfigPnPLObj = NULL;
#endif

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

static AManagedTaskEx *spVL53L8CXObj = NULL;
static AManagedTaskEx *spSTHS34PF80Obj = NULL;
static AManagedTaskEx *spVD6283TXObj = NULL;
/**
  * DatalogApp
  */
static AManagedTaskEx *sDatalogAppObj = NULL;


/* eLooM framework entry points definition */
/*******************************************/


sys_error_code_t SysLoadApplicationContext(ApplicationContext *pAppContext)
{
  assert_param(pAppContext);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint8_t sths34pf80_address;
  boolean_t ext_pdetect = FALSE;
  hwd_st25dv_version st25dv_version;

  PnPLSetAllocationFunctions(SysAlloc, SysFree);

  /* Check if PDETECT is connected */
  ext_pdetect = HardwareDetection_Check_Ext_PDETECT(&sths34pf80_address);

  /* Check NFC chip version */
  st25dv_version = HardwareDetection_Check_ST25DV();
  if (st25dv_version == ST25DV64KC)
  {
    BoardId = BOARD_ID_BOXB;
    FwId = USB_FW_ID_DATALOG2_BOXB;
  }

  /************ Allocate task objects ************/
  sUtilObj = UtilTaskAlloc(&MX_TIM4InitParams, &MX_GPIO_PA8InitParams, &MX_GPIO_PA0InitParams, &MX_GPIO_PD0InitParams, &MX_TIM5InitParams, &MX_ADC4InitParams,
                           &MX_GPIO_UBInitParams, &MX_GPIO_LED1InitParams, &MX_GPIO_LED2InitParams);
  sDatalogAppObj = DatalogAppTaskAlloc();
  sI2C2BusObj = I2CBusTaskAlloc(&MX_I2C2InitParams);
  sSPI2BusObj = SPIBusTaskAlloc(&MX_SPI2InitParams);
  sIIS2ICLXObj = IIS2ICLXTaskAlloc(&MX_GPIO_INT1_ICLXInitParams, &MX_GPIO_CS_ICLXInitParams);
  sIIS2MDCObj = IIS2MDCTaskAlloc(&MX_GPIO_INT_MAGInitParams, NULL);
  sISM330DHCXObj = ISM330DHCXTaskAlloc(&MX_GPIO_INT1_DHCXInitParams, &MX_GPIO_INT2_DHCXInitParams, &MX_GPIO_CS_DHCXInitParams);
  sSTTS22HObj = STTS22HTaskAlloc(NULL, NULL, STTS22H_I2C_ADD_L);
  sILPS22QSObj = ILPS22QSTaskAlloc(NULL, NULL);
  sIMP34DT05Obj = IMP34DT05TaskAlloc(&MX_ADF1InitParams);
  if (ext_pdetect)
  {
    sI2C3BusObj = I2CBusTaskAlloc(&MX_I2C3InitParams);
    spVL53L8CXObj = VL53L8CXTaskAlloc(NULL, NULL);
    spSTHS34PF80Obj = STHS34PF80TaskAlloc(NULL, NULL);
    spVD6283TXObj = VD6283TXTaskAlloc(NULL, NULL);
  }

  /************ Add the task object to the context ************/
  res = ACAddTask(pAppContext, (AManagedTask *) sUtilObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sDatalogAppObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sI2C2BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSPI2BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2ICLXObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2MDCObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sILPS22QSObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIMP34DT05Obj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSTTS22HObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sISM330DHCXObj);
  if (spSTHS34PF80Obj)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sI2C3BusObj);
    res = ACAddTask(pAppContext, (AManagedTask *)spVL53L8CXObj);
    res = ACAddTask(pAppContext, (AManagedTask *)spSTHS34PF80Obj);
    res = ACAddTask(pAppContext, (AManagedTask *)spVD6283TXObj);
  }

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
  pDeviceInfoPnPLObj = Deviceinformation_PnPLAlloc();
  pAcquisitionInfoPnPLObj = Acquisition_Info_PnPLAlloc();
  pTagsInfoPnPLObj = Tags_Info_PnPLAlloc();
  pFirmwareInfoPnPLObj = Firmware_Info_PnPLAlloc();
  pLogControllerPnPLObj = Log_Controller_PnPLAlloc();
  pAutomodePnPLObj = Automode_PnPLAlloc();

#if (DATALOG2_USE_WIFI == 1)
  pWifiConfigPnPLObj = Wifi_Config_PnPLAlloc();
#endif

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

  /************ Other PnPL Components ************/
  Deviceinformation_PnPLInit(pDeviceInfoPnPLObj);
  Firmware_Info_PnPLInit(pFirmwareInfoPnPLObj);
  Acquisition_Info_PnPLInit(pAcquisitionInfoPnPLObj);
  Tags_Info_PnPLInit(pTagsInfoPnPLObj);
  Log_Controller_PnPLInit(pLogControllerPnPLObj, DatalogAppTask_GetILogControllerIF((DatalogAppTask *) sDatalogAppObj));
  Automode_PnPLInit(pAutomodePnPLObj);

#if (DATALOG2_USE_WIFI == 1)
  Wifi_Config_PnPLInit(pWifiConfigPnPLObj, AppNetXDuo_GetIWifi_ConfigIF());
#endif

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

