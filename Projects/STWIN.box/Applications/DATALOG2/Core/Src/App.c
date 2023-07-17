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
#include "IIS3DWBTask.h"
#include "ISM330DHCXTask.h"
#include "ISM330ISTask.h"
#include "IIS2MDCTask.h"
#include "IMP23ABSUTask.h"
#include "IIS2DLPCTask.h"
#include "ILPS22QSTask.h"
#include "STTS22HTask.h"
#include "IMP34DT05Task.h"
#include "IIS2ICLXTask.h"

#include "DatalogAppTask.h"
#include "App_model.h"

#include "PnPLCompManager.h"
#include "Deviceinformation_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Iis3dwb_Acc_PnPL.h"
#include "Iis3dwb_Ext_Acc_PnPL.h"
#include "Ism330dhcx_Acc_PnPL.h"
#include "Ism330dhcx_Gyro_PnPL.h"
#include "Ism330dhcx_Mlc_PnPL.h"
#include "Ism330is_Acc_PnPL.h"
#include "Ism330is_Gyro_PnPL.h"
#include "Ism330is_Ispu_PnPL.h"
#include "Iis2mdc_Mag_PnPL.h"
#include "Imp23absu_Mic_PnPL.h"
#include "Iis2dlpc_Acc_PnPL.h"
#include "Stts22h_Temp_PnPL.h"
#include "Stts22h_Ext_Temp_PnPL.h"
#include "Ilps22qs_Press_PnPL.h"
#include "Imp34dt05_Mic_PnPL.h"
#include "Iis2iclx_Acc_PnPL.h"
#include "Automode_PnPL.h"
#include "parson.h"

static uint8_t FW_ID = FW_ID_DATALOG2;

static IPnPLComponent_t *pLogControllerPnPLObj = NULL;
static IPnPLComponent_t *pDeviceInfoPnPLObj = NULL;
static IPnPLComponent_t *pFirmwareInfoPnPLObj = NULL;
static IPnPLComponent_t *pAcquisitionInfoPnPLObj = NULL;
static IPnPLComponent_t *pTagsInfoPnPLObj = NULL;
static IPnPLComponent_t *pIIS3DWB_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS3DWB_Ext_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330DHCX_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330DHCX_GYRO_PnPLObj = NULL;
static IPnPLComponent_t *pISM330DHCX_MLC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330IS_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330IS_GYRO_PnPLObj = NULL;
static IPnPLComponent_t *pISM330IS_ISPU_PnPLObj = NULL;
static IPnPLComponent_t *pIIS2MDC_MAG_PnPLObj = NULL;
static IPnPLComponent_t *pIMP23ABSU_MIC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS2DLPC_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pSTTS22H_TEMP_PnPLObj = NULL;
static IPnPLComponent_t *pSTTS22H_Ext_TEMP_PnPLObj = NULL;
static IPnPLComponent_t *pILPS22QS_PRESS_PnPLObj = NULL;
static IPnPLComponent_t *pIMP34DT05_MIC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS2ICLX_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pAutomodePnPLObj = NULL;

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
static AManagedTaskEx *sIIS3DWBObj = NULL;
static AManagedTaskEx *sIIS3DWBExtObj = NULL;
static AManagedTaskEx *sISM330DHCXObj = NULL;
static AManagedTaskEx *sIIS2MDCObj = NULL;
static AManagedTaskEx *sIMP23ABSUObj = NULL;
static AManagedTaskEx *sIIS2DLPCObj = NULL;
static AManagedTaskEx *sILPS22QSObj = NULL;
static AManagedTaskEx *sSTTS22HObj = NULL;
static AManagedTaskEx *sSTTS22HExtObj = NULL;
static AManagedTaskEx *sIMP34DT05Obj = NULL;
static AManagedTaskEx *sIIS2ICLXObj = NULL;
static AManagedTaskEx *sISM330ISObj = NULL;

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
  uint8_t stts22h_address;
  boolean_t sExtIis3dwb = FALSE;
  boolean_t sExtIis330is = FALSE;
  boolean_t sExtStts22h = FALSE;

  /* Workaround to set malloc/free function even if BLE Init fails */
  json_set_allocation_functions(SysAlloc, SysFree);

  /************ Check availability of external sensors ************/
  sExtIis3dwb = HardwareDetection_Check_Ext_IIS3DWB();
  sExtIis330is = HardwareDetection_Check_Ext_ISM330IS();
  sExtStts22h = HardwareDetection_Check_Ext_STTS22H(&stts22h_address);

  /************ Allocate task objects ************/
  sUtilObj = UtilTaskAlloc(&MX_TIM4InitParams, &MX_GPIO_PA8InitParams, &MX_GPIO_PA0InitParams, &MX_GPIO_PD0InitParams, &MX_TIM5InitParams, &MX_ADC4InitParams,
		  	  	  	  	  	  &MX_GPIO_UBInitParams, &MX_GPIO_LED1InitParams, &MX_GPIO_LED2InitParams);
  sDatalogAppObj = DatalogAppTaskAlloc();
  sI2C2BusObj = I2CBusTaskAlloc(&MX_I2C2InitParams);
  sSPI2BusObj = SPIBusTaskAlloc(&MX_SPI2InitParams);
  sIIS2DLPCObj = IIS2DLPCTaskAlloc(&MX_GPIO_INT2_DLPCInitParams, &MX_GPIO_CS_DLPCInitParams);
  sIIS2ICLXObj = IIS2ICLXTaskAlloc(&MX_GPIO_INT1_ICLXInitParams, &MX_GPIO_CS_ICLXInitParams);
  sIIS2MDCObj = IIS2MDCTaskAlloc(&MX_GPIO_INT_MAGInitParams, NULL);
  sIIS3DWBObj = IIS3DWBTaskAlloc(&MX_GPIO_INT1_DWBInitParams, &MX_GPIO_CS_DWBInitParams);

  if(sExtIis3dwb)
  {
    /* Use the external IIS3DWB and onboard ISM330DHCX  */
    sIIS3DWBExtObj = IIS3DWBTaskAllocSetName(&MX_GPIO_INT1_EXTERNAL_DWBInitParams, &MX_GPIO_CS_EXTERNALInitParams, "iis3dwb_ext");
    sISM330DHCXObj = ISM330DHCXTaskAlloc(&MX_GPIO_INT1_DHCXInitParams, &MX_GPIO_INT2_DHCXInitParams, &MX_GPIO_CS_DHCXInitParams);
  }
  else if (sExtIis330is)
  {
	  FW_ID = FW_ID_DATALOG2_ISPU;
    /* Use the onboard IIS3DWB and the external ISM330IS */
    sISM330ISObj = ISM330ISTaskAlloc(&MX_GPIO_INT2_EXInitParams, &MX_GPIO_INT1_EXTERNAL_ISPUInitParams, &MX_GPIO_CS_EXTERNALInitParams);
  }
  else
  {
    /* Use the onboard IIS3DWB and ISM330DHCX */
    sISM330DHCXObj = ISM330DHCXTaskAlloc(&MX_GPIO_INT1_DHCXInitParams, &MX_GPIO_INT2_DHCXInitParams, &MX_GPIO_CS_DHCXInitParams);
  }

  /* Use the onboard STTS22H (address low) */
  sSTTS22HObj = STTS22HTaskAlloc(NULL, NULL, STTS22H_I2C_ADD_L);
  if(sExtStts22h)
  {
    /* Use the external STTS22H on I2C3 */
    sSTTS22HExtObj = STTS22HTaskAllocSetName(NULL, NULL, stts22h_address, "stts22h_ext");
    sI2C3BusObj = I2CBusTaskAlloc(&MX_I2C3InitParams);
  }

  sILPS22QSObj = ILPS22QSTaskAlloc(NULL, NULL);
  sIMP23ABSUObj = IMP23ABSUTaskAlloc(&MX_MDF1InitParams, &MX_ADC1InitParams);
  sIMP34DT05Obj = IMP34DT05TaskAlloc(&MX_ADF1InitParams);

  /************ Add the task object to the context ************/
  res = ACAddTask(pAppContext, (AManagedTask *) sUtilObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sDatalogAppObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sI2C2BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sI2C3BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSPI2BusObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2DLPCObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2ICLXObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS2MDCObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS3DWBObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIIS3DWBExtObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sILPS22QSObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIMP23ABSUObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIMP34DT05Obj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSTTS22HObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSTTS22HExtObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sISM330DHCXObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sISM330ISObj);

  pIIS2DLPC_ACC_PnPLObj = Iis2dlpc_Acc_PnPLAlloc();
  pIIS2ICLX_ACC_PnPLObj = Iis2iclx_Acc_PnPLAlloc();
  pIIS2MDC_MAG_PnPLObj = Iis2mdc_Mag_PnPLAlloc();
  pILPS22QS_PRESS_PnPLObj = Ilps22qs_Press_PnPLAlloc();
  pIMP23ABSU_MIC_PnPLObj = Imp23absu_Mic_PnPLAlloc();
  pIMP34DT05_MIC_PnPLObj = Imp34dt05_Mic_PnPLAlloc();
  pSTTS22H_TEMP_PnPLObj = Stts22h_Temp_PnPLAlloc();
  pIIS3DWB_ACC_PnPLObj = Iis3dwb_Acc_PnPLAlloc();

  if (sSTTS22HExtObj)
  {
    pSTTS22H_Ext_TEMP_PnPLObj = Stts22h_Ext_Temp_PnPLAlloc();
  }

  if (sIIS3DWBExtObj)
  {
    pIIS3DWB_Ext_ACC_PnPLObj = Iis3dwb_Ext_Acc_PnPLAlloc();
  }
  if(sISM330DHCXObj)
  {
    pISM330DHCX_ACC_PnPLObj = Ism330dhcx_Acc_PnPLAlloc();
    pISM330DHCX_GYRO_PnPLObj = Ism330dhcx_Gyro_PnPLAlloc();
    pISM330DHCX_MLC_PnPLObj = Ism330dhcx_Mlc_PnPLAlloc();
  }
  if(sISM330ISObj)
  {
    pISM330IS_ACC_PnPLObj = Ism330is_Acc_PnPLAlloc();
    pISM330IS_GYRO_PnPLObj = Ism330is_Gyro_PnPLAlloc();
    pISM330IS_ISPU_PnPLObj = Ism330is_Ispu_PnPLAlloc();
  }

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

  /************ Re-map the state machine of the Integrated tasks ************/
  SensorManagerStateMachineRemap(spAppPMState2SMPMStateMap);

  /************ Connect the sensor task to the bus ************/
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C2BusObj, (I2CBusIF *)IIS2MDCTaskGetSensorIF((IIS2MDCTask *) sIIS2MDCObj));
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C2BusObj, (I2CBusIF *)ILPS22QSTaskGetSensorIF((ILPS22QSTask *) sILPS22QSObj));

  /* Use I2C2 for Internal STTS22H */
  I2CBusTaskConnectDevice((I2CBusTask *) sI2C2BusObj, (I2CBusIF *)STTS22HTaskGetSensorIF((STTS22HTask *) sSTTS22HObj));
  if (sSTTS22HExtObj)
  {
    /* Use I2C3 for External STTS22H */
    I2CBusTaskConnectDevice((I2CBusTask *) sI2C3BusObj, (I2CBusIF *)STTS22HTaskGetSensorIF((STTS22HTask *) sSTTS22HExtObj));
  }

  SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)IIS2DLPCTaskGetSensorIF((IIS2DLPCTask *) sIIS2DLPCObj));
  SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)IIS2ICLXTaskGetSensorIF((IIS2ICLXTask *) sIIS2ICLXObj));

  SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)IIS3DWBTaskGetSensorIF((IIS3DWBTask *) sIIS3DWBObj));
  if(sIIS3DWBExtObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)IIS3DWBTaskGetSensorIF((IIS3DWBTask *) sIIS3DWBExtObj));
  }
  if(sISM330DHCXObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)ISM330DHCXTaskGetSensorIF((ISM330DHCXTask *) sISM330DHCXObj));
  }
  if(sISM330ISObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)ISM330ISTaskGetSensorIF((ISM330ISTask *) sISM330ISObj));
  }

  /************ Connect the Sensor events to the DatalogAppTask ************/
  IEventListener *DatalogAppListener = DatalogAppTask_GetEventListenerIF((DatalogAppTask *) sDatalogAppObj);
  IEventSrcAddEventListener(IIS2DLPCTaskGetEventSrcIF((IIS2DLPCTask *) sIIS2DLPCObj), DatalogAppListener);
  IEventSrcAddEventListener(IIS2ICLXTaskGetEventSrcIF((IIS2ICLXTask *) sIIS2ICLXObj), DatalogAppListener);
  IEventSrcAddEventListener(IIS2MDCTaskGetMagEventSrcIF((IIS2MDCTask *) sIIS2MDCObj), DatalogAppListener);
  IEventSrcAddEventListener(ILPS22QSTaskGetPressEventSrcIF((ILPS22QSTask *) sILPS22QSObj), DatalogAppListener);
  IEventSrcAddEventListener(IMP23ABSUTaskGetEventSrcIF((IMP23ABSUTask *) sIMP23ABSUObj), DatalogAppListener);
  IEventSrcAddEventListener(IMP34DT05TaskGetEventSrcIF((IMP34DT05Task *) sIMP34DT05Obj), DatalogAppListener);
  IEventSrcAddEventListener(STTS22HTaskGetTempEventSrcIF((STTS22HTask *) sSTTS22HObj), DatalogAppListener);
  IEventSrcAddEventListener(IIS3DWBTaskGetEventSrcIF((IIS3DWBTask *) sIIS3DWBObj), DatalogAppListener);
  if (sSTTS22HExtObj)
  {
    IEventSrcAddEventListener(STTS22HTaskGetTempEventSrcIF((STTS22HTask *) sSTTS22HExtObj), DatalogAppListener);
  }
  if(sIIS3DWBExtObj)
  {
    IEventSrcAddEventListener(IIS3DWBTaskGetEventSrcIF((IIS3DWBTask *) sIIS3DWBExtObj), DatalogAppListener);
  }
  if(sISM330DHCXObj)
  {
    IEventSrcAddEventListener(ISM330DHCXTaskGetAccEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM330DHCXTaskGetGyroEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM330DHCXTaskGetMlcEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
  }
  if(sISM330ISObj)
  {
    IEventSrcAddEventListener(ISM330ISTaskGetAccEventSrcIF((ISM330ISTask *) sISM330ISObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM330ISTaskGetGyroEventSrcIF((ISM330ISTask *) sISM330ISObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM330ISTaskGetMlcEventSrcIF((ISM330ISTask *) sISM330ISObj), DatalogAppListener);
  }

  /************ Sensor PnPL Components ************/
  Iis2dlpc_Acc_PnPLInit(pIIS2DLPC_ACC_PnPLObj);
  Iis2iclx_Acc_PnPLInit(pIIS2ICLX_ACC_PnPLObj);
  Iis2mdc_Mag_PnPLInit(pIIS2MDC_MAG_PnPLObj);
  Ilps22qs_Press_PnPLInit(pILPS22QS_PRESS_PnPLObj);
  Imp23absu_Mic_PnPLInit(pIMP23ABSU_MIC_PnPLObj);
  Imp34dt05_Mic_PnPLInit(pIMP34DT05_MIC_PnPLObj);
  Stts22h_Temp_PnPLInit(pSTTS22H_TEMP_PnPLObj);
  Iis3dwb_Acc_PnPLInit(pIIS3DWB_ACC_PnPLObj);

  if (sSTTS22HExtObj)
  {
    Stts22h_Ext_Temp_PnPLInit(pSTTS22H_Ext_TEMP_PnPLObj);
    stts22h_temp_set_enable(false);
  }
  if(sIIS3DWBExtObj)
  {
    Iis3dwb_Ext_Acc_PnPLInit(pIIS3DWB_Ext_ACC_PnPLObj);
    iis3dwb_acc_set_enable(false);
  }
  if(sISM330DHCXObj)
  {
    Ism330dhcx_Acc_PnPLInit(pISM330DHCX_ACC_PnPLObj);
    Ism330dhcx_Gyro_PnPLInit(pISM330DHCX_GYRO_PnPLObj);
    Ism330dhcx_Mlc_PnPLInit(pISM330DHCX_MLC_PnPLObj, DatalogAppTask_GetIMLCControllerIF((DatalogAppTask*) sDatalogAppObj, (AManagedTask*) sISM330DHCXObj));
  }
  if(sISM330ISObj)
  {
    Ism330is_Acc_PnPLInit(pISM330IS_ACC_PnPLObj);
    Ism330is_Gyro_PnPLInit(pISM330IS_GYRO_PnPLObj);
    Ism330is_Ispu_PnPLInit(pISM330IS_ISPU_PnPLObj, DatalogAppTask_GetIIspuControllerIF((DatalogAppTask*) sDatalogAppObj, (AManagedTask*) sISM330ISObj));
  }

  /************ Other PnPL Components ************/
  Deviceinformation_PnPLInit(pDeviceInfoPnPLObj);
  Firmware_Info_PnPLInit(pFirmwareInfoPnPLObj);
  Acquisition_Info_PnPLInit(pAcquisitionInfoPnPLObj);
  Tags_Info_PnPLInit(pTagsInfoPnPLObj);
  Log_Controller_PnPLInit(pLogControllerPnPLObj, DatalogAppTask_GetILogControllerIF((DatalogAppTask *) sDatalogAppObj));
  Automode_PnPLInit(pAutomodePnPLObj);

  /************ Set GUI label for external components ************/
//  if(sIIS3DWBExtObj)
//  {
//    iis3dwb_ext_acc_set_sensor_annotation("[EXTERN]");
//  }
//
//  if(sSTTS22HExtObj)
//  {
//    stts22h_temp_set_sensor_annotation("[EXTERN]");
//  }

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

  AMTSetPMStateRemapFunc((AManagedTask *) sI2C2BusObj, pPMState2PMStateMap);
  if(sI2C3BusObj)
  {
    AMTSetPMStateRemapFunc((AManagedTask*) sI2C3BusObj, pPMState2PMStateMap);
  }
  AMTSetPMStateRemapFunc((AManagedTask *) sSPI2BusObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sIIS2DLPCObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sIIS2ICLXObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sIIS2MDCObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sILPS22QSObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sIMP23ABSUObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sIMP34DT05Obj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sSTTS22HObj, pPMState2PMStateMap);
  AMTSetPMStateRemapFunc((AManagedTask *) sIIS3DWBObj, pPMState2PMStateMap);

  if (sSTTS22HExtObj)
  {
    AMTSetPMStateRemapFunc((AManagedTask *) sSTTS22HExtObj, pPMState2PMStateMap);
  }
  if (sIIS3DWBExtObj)
  {
    AMTSetPMStateRemapFunc((AManagedTask *) sIIS3DWBExtObj, pPMState2PMStateMap);
  }
  if(sISM330DHCXObj)
  {
    AMTSetPMStateRemapFunc((AManagedTask *) sISM330DHCXObj, pPMState2PMStateMap);
  }
  if(sISM330ISObj)
  {
    AMTSetPMStateRemapFunc((AManagedTask *) sISM330ISObj, pPMState2PMStateMap);
  }

  return SYS_NO_ERROR_CODE;
}


void EXT_INT1_EXTI_Callback(uint16_t nPin)
{
  if(sIIS3DWBExtObj)
  {
    IIS3DWBTask_EXTI_Callback(nPin);
  }
  else
  {
    ISM330ISTask_EXTI_Callback(nPin);
  }
}

