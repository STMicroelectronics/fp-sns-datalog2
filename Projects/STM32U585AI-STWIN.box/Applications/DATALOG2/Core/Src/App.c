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
#include "ISM330BXTask.h"
#include "ISM330DHCXTask.h"
#include "ISM330ISTask.h"
#include "IIS2MDCTask.h"
#include "IMP23ABSUTask.h"
#include "IIS2DLPCTask.h"
#include "ILPS22QSTask.h"
#include "STTS22HTask.h"
#include "IMP34DT05Task.h"
#include "IIS2ICLXTask.h"
#include "TSC1641Task.h"

#include "DatalogAppTask.h"
#include "App_model.h"
#include "app_netxduo.h"

#include "PnPLCompManager.h"
#include "Deviceinformation_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Iis3dwb_Acc_PnPL.h"
#include "Iis3dwb_Ext_Acc_PnPL.h"
#include "Ism330bx_Acc_PnPL.h"
#include "Ism330bx_Gyro_PnPL.h"
#include "Ism330bx_Mlc_PnPL.h"
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
#include "Wifi_Config_PnPL.h"
#include "Tsc1641_Pow_PnPL.h"

static uint8_t BoardId = BOARD_ID_BOXA;
static uint8_t FwId = USB_FW_ID_DATALOG2_BOXA;

static IPnPLComponent_t *pLogControllerPnPLObj = NULL;
static IPnPLComponent_t *pDeviceInfoPnPLObj = NULL;
static IPnPLComponent_t *pFirmwareInfoPnPLObj = NULL;
static IPnPLComponent_t *pAcquisitionInfoPnPLObj = NULL;
static IPnPLComponent_t *pTagsInfoPnPLObj = NULL;
static IPnPLComponent_t *pIIS3DWB_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS3DWB_Ext_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330BX_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330BX_GYRO_PnPLObj = NULL;
static IPnPLComponent_t *pISM330BX_MLC_PnPLObj = NULL;
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
static IPnPLComponent_t *pTSC1641_POW_PnPLObj = NULL;

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
static AManagedTaskEx *sIIS3DWBObj = NULL;
static AManagedTaskEx *sIIS3DWBExtObj = NULL;
static AManagedTaskEx *sISM330BXObj = NULL;
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
static AManagedTaskEx *sTSC1641Obj = NULL;

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
  uint8_t stts22h_address;
  boolean_t ext_iis3dwb = FALSE;
  boolean_t ext_ism330bx = FALSE;
  boolean_t ext_iis330is = FALSE;
  boolean_t ext_stts22h = FALSE;
  boolean_t ext_tsc1641 = FALSE;
  hwd_st25dv_version st25dv_version;

  /* PnPL thread safe mutex creation */
  tx_mutex_create(&pnpl_mutex, "PnPL Mutex", TX_INHERIT);

  /* PnPL thread safe function registration */
  PnPL_SetLockUnlockCallbacks(PnPL_lock_fp, PnPL_unlock_fp);

  PnPLSetAllocationFunctions(SysAlloc, SysFree);

  /* Check availability of external sensors */
  ext_iis3dwb = HardwareDetection_Check_Ext_IIS3DWB();
  ext_ism330bx = HardwareDetection_Check_Ext_ISM330BX();
  ext_iis330is = HardwareDetection_Check_Ext_ISM330IS();
  ext_stts22h = HardwareDetection_Check_Ext_STTS22H(&stts22h_address);
  ext_tsc1641 = HardwareDetection_Check_Ext_TSC1641();

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
  sIIS2DLPCObj = IIS2DLPCTaskAlloc(&MX_GPIO_INT2_DLPCInitParams, &MX_GPIO_CS_DLPCInitParams);
  sIIS2ICLXObj = IIS2ICLXTaskAlloc(&MX_GPIO_INT1_ICLXInitParams, &MX_GPIO_CS_ICLXInitParams);
  sIIS2MDCObj = IIS2MDCTaskAlloc(&MX_GPIO_INT_MAGInitParams, NULL);
  sIIS3DWBObj = IIS3DWBTaskAlloc(&MX_GPIO_INT1_DWBInitParams, &MX_GPIO_CS_DWBInitParams);

  if (ext_iis3dwb)
  {
    /* Use the external IIS3DWB and onboard ISM330DHCX  */
    sIIS3DWBExtObj = IIS3DWBTaskAllocSetName(&MX_GPIO_INT1_EXTERNAL_InitParams, &MX_GPIO_CS_EXTERNALInitParams, "iis3dwb_ext");
    sISM330DHCXObj = ISM330DHCXTaskAlloc(&MX_GPIO_INT1_DHCXInitParams, &MX_GPIO_INT2_DHCXInitParams, &MX_GPIO_CS_DHCXInitParams);
  }
  else if (ext_ism330bx)
  {
    sISM330BXObj = ISM330BXTaskAlloc(&MX_GPIO_INT1_EXTERNAL_InitParams, NULL, &MX_GPIO_CS_EXTERNALInitParams);
  }
  else if (ext_iis330is)
  {
    /* Use the onboard IIS3DWB and the external ISM330IS */
    sISM330ISObj = ISM330ISTaskAlloc(&MX_GPIO_INT2_EXInitParams, &MX_GPIO_INT1_EXTERNAL_InitParams, &MX_GPIO_CS_EXTERNALInitParams);
  }
  else
  {
    /* Use the onboard IIS3DWB and ISM330DHCX */
    sISM330DHCXObj = ISM330DHCXTaskAlloc(&MX_GPIO_INT1_DHCXInitParams, &MX_GPIO_INT2_DHCXInitParams, &MX_GPIO_CS_DHCXInitParams);
  }

  /* Use the onboard STTS22H (address low) */
  sSTTS22HObj = STTS22HTaskAlloc(NULL, NULL, STTS22H_I2C_ADD_L);
  if (ext_stts22h)
  {
    /* Use the external STTS22H on I2C3 */
    sSTTS22HExtObj = STTS22HTaskAllocSetName(NULL, NULL, stts22h_address, "stts22h_ext");
    sI2C3BusObj = I2CBusTaskAlloc(&MX_I2C3InitParams);
  }

  sILPS22QSObj = ILPS22QSTaskAlloc(NULL, NULL);
  sIMP23ABSUObj = IMP23ABSUTaskAlloc(&MX_MDF1InitParams, &MX_ADC1InitParams);
  sIMP34DT05Obj = IMP34DT05TaskAlloc(&MX_ADF1InitParams);

  if (ext_tsc1641)
  {
    sTSC1641Obj = TSC1641TaskAlloc(&MX_GPIO_INT_POW_InitParams, NULL);
    sI2C3BusObj = I2CBusTaskAlloc(&MX_I2C3InitParams);
  }

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
  res = ACAddTask(pAppContext, (AManagedTask *) sISM330BXObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sISM330DHCXObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sISM330ISObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sTSC1641Obj);

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
  if (sISM330BXObj)
  {
    pISM330BX_ACC_PnPLObj = Ism330bx_Acc_PnPLAlloc();
    pISM330BX_GYRO_PnPLObj = Ism330bx_Gyro_PnPLAlloc();
    pISM330BX_MLC_PnPLObj = Ism330bx_Mlc_PnPLAlloc();
  }
  if (sISM330DHCXObj)
  {
    pISM330DHCX_ACC_PnPLObj = Ism330dhcx_Acc_PnPLAlloc();
    pISM330DHCX_GYRO_PnPLObj = Ism330dhcx_Gyro_PnPLAlloc();
    pISM330DHCX_MLC_PnPLObj = Ism330dhcx_Mlc_PnPLAlloc();
  }
  if (sISM330ISObj)
  {
    pISM330IS_ACC_PnPLObj = Ism330is_Acc_PnPLAlloc();
    pISM330IS_GYRO_PnPLObj = Ism330is_Gyro_PnPLAlloc();
    pISM330IS_ISPU_PnPLObj = Ism330is_Ispu_PnPLAlloc();
  }
  if (sTSC1641Obj)
  {
    pTSC1641_POW_PnPLObj = Tsc1641_Pow_PnPLAlloc();
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
  if (sSTTS22HExtObj)
  {
    /* Use I2C3 for External STTS22H */
    I2CBusTaskConnectDevice((I2CBusTask *) sI2C3BusObj, (I2CBusIF *)STTS22HTaskGetSensorIF((STTS22HTask *) sSTTS22HExtObj));
  }

  SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)IIS2DLPCTaskGetSensorIF((IIS2DLPCTask *) sIIS2DLPCObj));
  SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)IIS2ICLXTaskGetSensorIF((IIS2ICLXTask *) sIIS2ICLXObj));

  SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)IIS3DWBTaskGetSensorIF((IIS3DWBTask *) sIIS3DWBObj));
  if (sIIS3DWBExtObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)IIS3DWBTaskGetSensorIF((IIS3DWBTask *) sIIS3DWBExtObj));
  }
  if (sISM330DHCXObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj,
                            (SPIBusIF *)ISM330DHCXTaskGetSensorIF((ISM330DHCXTask *) sISM330DHCXObj));
  }
  if (sISM330BXObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj,
                            (SPIBusIF *)ISM330BXTaskGetSensorIF((ISM330BXTask *) sISM330BXObj));
  }
  if (sISM330ISObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj, (SPIBusIF *)ISM330ISTaskGetSensorIF((ISM330ISTask *) sISM330ISObj));
  }
  if (sTSC1641Obj)
  {
    I2CBusTaskConnectDevice((I2CBusTask *) sI2C3BusObj, (I2CBusIF *)TSC1641TaskGetSensorIF((TSC1641Task *) sTSC1641Obj));
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
  if (sIIS3DWBExtObj)
  {
    IEventSrcAddEventListener(IIS3DWBTaskGetEventSrcIF((IIS3DWBTask *) sIIS3DWBExtObj), DatalogAppListener);
  }
  if (sISM330BXObj)
  {
    IEventSrcAddEventListener(ISM330BXTaskGetAccEventSrcIF((ISM330BXTask *) sISM330BXObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM330BXTaskGetGyroEventSrcIF((ISM330BXTask *) sISM330BXObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM330BXTaskGetMlcEventSrcIF((ISM330BXTask *) sISM330BXObj), DatalogAppListener);
  }
  if (sISM330DHCXObj)
  {
    IEventSrcAddEventListener(ISM330DHCXTaskGetAccEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM330DHCXTaskGetGyroEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM330DHCXTaskGetMlcEventSrcIF((ISM330DHCXTask *) sISM330DHCXObj), DatalogAppListener);
  }
  if (sISM330ISObj)
  {
    IEventSrcAddEventListener(ISM330ISTaskGetAccEventSrcIF((ISM330ISTask *) sISM330ISObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM330ISTaskGetGyroEventSrcIF((ISM330ISTask *) sISM330ISObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM330ISTaskGetMlcEventSrcIF((ISM330ISTask *) sISM330ISObj), DatalogAppListener);
  }
  if (sTSC1641Obj)
  {
    IEventSrcAddEventListener(TSC1641TaskGetEventSrcIF((TSC1641Task *) sTSC1641Obj), DatalogAppListener);
  }

  /************ Connect Sensor LL to be used for ucf management to the DatalogAppTask ************/
  if (sISM330DHCXObj)
  {
    DatalogAppTask_SetMLCIF((AManagedTask *) sISM330DHCXObj);
  }
  if (sISM330BXObj)
  {
    DatalogAppTask_SetMLCIF((AManagedTask *) sISM330BXObj);
  }
  DatalogAppTask_SetIspuIF((AManagedTask *) sISM330ISObj);

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
    stts22h_temp_set_enable(false, NULL);
  }
  if (sIIS3DWBExtObj)
  {
    Iis3dwb_Ext_Acc_PnPLInit(pIIS3DWB_Ext_ACC_PnPLObj);
    iis3dwb_acc_set_enable(false, NULL);
  }
  if (sISM330BXObj)
  {
    Ism330bx_Acc_PnPLInit(pISM330BX_ACC_PnPLObj);
    Ism330bx_Gyro_PnPLInit(pISM330BX_GYRO_PnPLObj);
    Ism330bx_Mlc_PnPLInit(pISM330BX_MLC_PnPLObj);
  }
  if (sISM330DHCXObj)
  {
    Ism330dhcx_Acc_PnPLInit(pISM330DHCX_ACC_PnPLObj);
    Ism330dhcx_Gyro_PnPLInit(pISM330DHCX_GYRO_PnPLObj);
    Ism330dhcx_Mlc_PnPLInit(pISM330DHCX_MLC_PnPLObj);
  }
  if (sISM330ISObj)
  {
    Ism330is_Acc_PnPLInit(pISM330IS_ACC_PnPLObj);
    Ism330is_Gyro_PnPLInit(pISM330IS_GYRO_PnPLObj);
    Ism330is_Ispu_PnPLInit(pISM330IS_ISPU_PnPLObj);
  }
  if (sTSC1641Obj)
  {
    Tsc1641_Pow_PnPLInit(pTSC1641_POW_PnPLObj);
  }

  /************ Other PnPL Components ************/
  Deviceinformation_PnPLInit(pDeviceInfoPnPLObj);
  Firmware_Info_PnPLInit(pFirmwareInfoPnPLObj);
  Acquisition_Info_PnPLInit(pAcquisitionInfoPnPLObj);
  Tags_Info_PnPLInit(pTagsInfoPnPLObj);
  Log_Controller_PnPLInit(pLogControllerPnPLObj);
  Automode_PnPLInit(pAutomodePnPLObj);

#if (DATALOG2_USE_WIFI == 1)
  Wifi_Config_PnPLInit(pWifiConfigPnPLObj);
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


void EXT_INT1_EXTI_Callback(uint16_t nPin)
{
  if (sIIS3DWBExtObj)
  {
    IIS3DWBTask_EXTI_Callback(nPin);
  }
  else if (sISM330BXObj)
  {
    ISM330BXTask_EXTI_Callback(nPin);
  }
  else if (sISM330ISObj)
  {
    ISM330ISTask_EXTI_Callback(nPin);
  }
  else
  {
    TSC1641Task_EXTI_Callback(nPin);
  }
}

void EXT_INT2_EXTI_Callback(uint16_t nPin)
{
  if (sISM330ISObj)
  {
    INT2_ISM330IS_EXTI_Callback(nPin);
  }
  else
  {
    INT2_ISM330BX_EXTI_Callback(nPin);
  }
}

static void PnPL_lock_fp(void)
{
  tx_mutex_get(&pnpl_mutex, TX_NO_WAIT);
}

static void PnPL_unlock_fp(void)
{
  tx_mutex_put(&pnpl_mutex);
}
