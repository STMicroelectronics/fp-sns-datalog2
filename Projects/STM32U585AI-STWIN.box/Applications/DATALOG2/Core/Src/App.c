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
#include "IIS3DWBTask.h"
#include "IIS3DWB10ISTask.h"
#include "ISM330BXTask.h"
#include "ISM330DHCXTask.h"
#include "ISM330ISTask.h"
#include "ISM6HG256XTask.h"
#include "IIS2MDCTask.h"
#include "IMP23ABSUTask.h"
#include "IIS2DLPCTask.h"
#include "IIS2DULPXTask.h"
#include "ILPS22QSTask.h"
#include "ILPS28QSWTask.h"
#include "STTS22HTask.h"
#include "IMP34DT05Task.h"
#include "IIS2ICLXTask.h"
#include "TSC1641Task.h"

#include "DatalogAppTask.h"
#include "App_model.h"
#include "app_netxduo.h"

#include "PnPLCompManager.h"
#include "Iis2dlpc_Acc_PnPL.h"
#include "Iis2iclx_Acc_PnPL.h"
#include "Iis2iclx_Mlc_PnPL.h"
#include "Iis2mdc_Mag_PnPL.h"
#include "Iis3dwb_Acc_PnPL.h"
#include "Ilps22qs_Press_PnPL.h"
#include "Imp23absu_Mic_PnPL.h"
#include "Imp34dt05_Mic_PnPL.h"
#include "Ism330dhcx_Acc_PnPL.h"
#include "Ism330dhcx_Gyro_PnPL.h"
#include "Ism330dhcx_Mlc_PnPL.h"
#include "Stts22h_Temp_PnPL.h"
#include "Iis2dulpx_Acc_PnPL.h"
#include "Iis2dulpx_Mlc_PnPL.h"
#include "Iis3dwb_Ext_Acc_PnPL.h"
#include "Iis3dwb10is_Ext_Acc_PnPL.h"
#include "Iis3dwb10is_Ext_Ispu_PnPL.h"
#include "Ilps28qsw_Press_PnPL.h"
#include "Ism330bx_Acc_PnPL.h"
#include "Ism330bx_Gyro_PnPL.h"
#include "Ism330bx_Mlc_PnPL.h"
#include "Ism330is_Acc_PnPL.h"
#include "Ism330is_Gyro_PnPL.h"
#include "Ism330is_Ispu_PnPL.h"
#include "Ism6hg256x_H_Acc_PnPL.h"
#include "Ism6hg256x_L_Acc_PnPL.h"
#include "Ism6hg256x_Gyro_PnPL.h"
#include "Ism6hg256x_Mlc_PnPL.h"
#include "Stts22h_Ext_Temp_PnPL.h"
#include "Tsc1641_Pow_PnPL.h"
#include "Automode_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Wifi_Config_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Deviceinformation_PnPL.h"

static uint8_t BoardId = BOARD_ID_BOXA;
static uint8_t FwId = USB_FW_ID_DATALOG2_BOXA;

static IPnPLComponent_t *pLogControllerPnPLObj = NULL;
static IPnPLComponent_t *pDeviceInfoPnPLObj = NULL;
static IPnPLComponent_t *pFirmwareInfoPnPLObj = NULL;
static IPnPLComponent_t *pAcquisitionInfoPnPLObj = NULL;
static IPnPLComponent_t *pTagsInfoPnPLObj = NULL;
static IPnPLComponent_t *pIIS3DWB_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS3DWB_Ext_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS3DWB10IS_Ext_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS3DWB10IS_Ext_ISPU_PnPLObj = NULL;
static IPnPLComponent_t *pISM330BX_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330BX_GYRO_PnPLObj = NULL;
static IPnPLComponent_t *pISM330BX_MLC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330DHCX_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330DHCX_GYRO_PnPLObj = NULL;
static IPnPLComponent_t *pISM330DHCX_MLC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330IS_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pISM330IS_GYRO_PnPLObj = NULL;
static IPnPLComponent_t *pISM330IS_ISPU_PnPLObj = NULL;
static IPnPLComponent_t *pISM6HG256X_H_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pISM6HG256X_L_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pISM6HG256X_GYRO_PnPLObj = NULL;
static IPnPLComponent_t *pISM6HG256X_MLC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS2MDC_MAG_PnPLObj = NULL;
static IPnPLComponent_t *pIMP23ABSU_MIC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS2DLPC_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS2DULPX_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pSTTS22H_TEMP_PnPLObj = NULL;
static IPnPLComponent_t *pSTTS22H_Ext_TEMP_PnPLObj = NULL;
static IPnPLComponent_t *pILPS22QS_PRESS_PnPLObj = NULL;
static IPnPLComponent_t *pILPS28QSW_PRESS_PnPLObj = NULL;
static IPnPLComponent_t *pIMP34DT05_MIC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS2ICLX_ACC_PnPLObj = NULL;
static IPnPLComponent_t *pIIS2ICLX_MLC_PnPLObj = NULL;
static IPnPLComponent_t *pAutomodePnPLObj = NULL;
static IPnPLComponent_t *pTSC1641_POW_PnPLObj = NULL;

static IPnPLComponent_t *pWifiConfigPnPLObj = NULL;

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
static AManagedTaskEx *sIIS3DWB10ISExtObj = NULL;
static AManagedTaskEx *sISM330BXObj = NULL;
static AManagedTaskEx *sISM330DHCXObj = NULL;
static AManagedTaskEx *sISM6HG256XObj = NULL;
static AManagedTaskEx *sIIS2MDCObj = NULL;
static AManagedTaskEx *sIMP23ABSUObj = NULL;
static AManagedTaskEx *sIIS2DLPCObj = NULL;
static AManagedTaskEx *sIIS2DULPXObj = NULL;
static AManagedTaskEx *sILPS22QSObj = NULL;
static AManagedTaskEx *sILPS28QSWObj = NULL;
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
  boolean_t ext_iis2dulpx = FALSE;
  boolean_t ext_iis3dwb = FALSE;
  boolean_t ext_iis3dwb10is = FALSE;
  boolean_t ext_ilps28qsw = FALSE;
  boolean_t ext_ism330bx = FALSE;
  boolean_t ext_ism6hg256x = FALSE;
  boolean_t ext_iis330is = FALSE;
  boolean_t ext_stts22h = FALSE;
  boolean_t ext_tsc1641 = FALSE;

  /* PnPL thread safe mutex creation */
  tx_mutex_create(&pnpl_mutex, "PnPL Mutex", TX_INHERIT);

  /* PnPL thread safe function registration */
  PnPL_SetLockUnlockCallbacks(PnPL_lock_fp, PnPL_unlock_fp);

  PnPLSetAllocationFunctions(SysAlloc, SysFree);

  /* Check availability of external sensors */
  ext_iis3dwb10is = HardwareDetection_Check_Ext_IIS3DWB10IS();
  ext_iis2dulpx = HardwareDetection_Check_Ext_IIS2DULPX();
  ext_iis3dwb = HardwareDetection_Check_Ext_IIS3DWB();
  ext_ilps28qsw = HardwareDetection_Check_Ext_ILPS28QSW();
  ext_ism330bx = HardwareDetection_Check_Ext_ISM330BX();
  ext_ism6hg256x = HardwareDetection_Check_Ext_ISM6HG256X();
  ext_iis330is = HardwareDetection_Check_Ext_ISM330IS();
  ext_stts22h = HardwareDetection_Check_Ext_STTS22H(&stts22h_address);
  ext_tsc1641 = HardwareDetection_Check_Ext_TSC1641();

  /************ Allocate task objects ************/
  sDatalogAppObj = DatalogAppTaskAlloc();
  sI2C2BusObj = I2CBusTaskAlloc(&MX_I2C2InitParams);
  if (ext_iis3dwb10is)
  {
    sSPI2BusObj = SPIBusTaskAlloc(&MX_SPI2_20MHzInitParams);
  }
  else
  {
    sSPI2BusObj = SPIBusTaskAlloc(&MX_SPI2InitParams);
  }
  sIIS2DLPCObj = IIS2DLPCTaskAlloc(&MX_GPIO_INT2_DLPCInitParams, &MX_GPIO_CS_DLPCInitParams);
  sIIS2ICLXObj = IIS2ICLXTaskAlloc(&MX_GPIO_INT1_ICLXInitParams, NULL, &MX_GPIO_CS_ICLXInitParams);
  sIIS2MDCObj = IIS2MDCTaskAlloc(&MX_GPIO_INT_MAGInitParams, NULL);
  sIIS3DWBObj = IIS3DWBTaskAlloc(&MX_GPIO_INT1_DWBInitParams, &MX_GPIO_CS_DWBInitParams);
  sISM330DHCXObj = ISM330DHCXTaskAlloc(&MX_GPIO_INT1_DHCXInitParams, &MX_GPIO_INT2_DHCXInitParams, &MX_GPIO_CS_DHCXInitParams);

  if (ext_iis3dwb)
  {
    sIIS3DWBExtObj = IIS3DWBTaskAllocSetName(&MX_GPIO_INT1_EXTERNAL_InitParams, &MX_GPIO_CS_EXTERNALInitParams, "iis3dwb_ext");
  }
  if (ext_iis3dwb10is)
  {
    sIIS3DWB10ISExtObj = IIS3DWB10ISTaskAlloc(&MX_GPIO_INT1_EXTERNAL_InitParams, NULL, &MX_GPIO_CS_EXTERNALInitParams);
  }
  if (ext_ism330bx)
  {
    sISM330BXObj = ISM330BXTaskAlloc(&MX_GPIO_INT1_EXTERNAL_InitParams, NULL, &MX_GPIO_CS_EXTERNALInitParams);
  }
  if (ext_ism6hg256x)
  {
    sISM6HG256XObj = ISM6HG256XTaskAlloc(&MX_GPIO_INT1_EXTERNAL_InitParams, NULL, &MX_GPIO_CS_EXTERNALInitParams);
  }
  if (ext_iis330is)
  {
    sISM330ISObj = ISM330ISTaskAlloc(&MX_GPIO_INT2_EXInitParams, NULL, &MX_GPIO_CS_EXTERNALInitParams);
  }
  if (ext_iis2dulpx)
  {
    sIIS2DULPXObj = IIS2DULPXTaskAlloc(&MX_GPIO_INT1_EXTERNAL_InitParams, NULL, &MX_GPIO_CS_EXTERNALInitParams);
  }
  if (ext_ilps28qsw)
  {
    sILPS28QSWObj = ILPS28QSWTaskAlloc(NULL, NULL);
    sI2C3BusObj = I2CBusTaskAlloc(&MX_I2C3InitParams);
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
    /* Enable HW TAG pins available on STEVAL-C34KPM1 board only */
    sUtilObj = UtilTaskAlloc(&MX_TIM4InitParams, &MX_GPIO_PA8InitParams, &MX_GPIO_PA0InitParams, &MX_GPIO_PD0InitParams, &MX_TIM5InitParams, &MX_ADC4InitParams,
                             &MX_GPIO_UBInitParams, &MX_GPIO_LED1InitParams, &MX_GPIO_LED2InitParams, &MX_GPIO_HWTAG0InitParams,
                             &MX_GPIO_HWTAG1InitParams);
  }
  else
  {
    /* Avoid initializing HW TAG pins */
    sUtilObj = UtilTaskAlloc(&MX_TIM4InitParams, &MX_GPIO_PA8InitParams, &MX_GPIO_PA0InitParams, &MX_GPIO_PD0InitParams, &MX_TIM5InitParams, &MX_ADC4InitParams,
                             &MX_GPIO_UBInitParams, &MX_GPIO_LED1InitParams, &MX_GPIO_LED2InitParams, NULL, NULL);
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
  res = ACAddTask(pAppContext, (AManagedTask *) sILPS22QSObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIMP23ABSUObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sIMP34DT05Obj);
  res = ACAddTask(pAppContext, (AManagedTask *) sSTTS22HObj);
  res = ACAddTask(pAppContext, (AManagedTask *) sISM330DHCXObj);
  if (ext_iis2dulpx)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sIIS2DULPXObj);
  }
  if (ext_iis3dwb)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sIIS3DWBExtObj);
  }
  if (ext_iis3dwb10is)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sIIS3DWB10ISExtObj);
  }
  if (ext_ilps28qsw)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sILPS28QSWObj);
  }
  if (ext_ism330bx)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sISM330BXObj);
  }
  if (ext_ism6hg256x)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sISM6HG256XObj);
  }
  if (ext_iis330is)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sISM330ISObj);
  }
  if (ext_stts22h)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sSTTS22HExtObj);
  }
  if (ext_tsc1641)
  {
    res = ACAddTask(pAppContext, (AManagedTask *) sTSC1641Obj);
  }

  pIIS2DLPC_ACC_PnPLObj = Iis2dlpc_Acc_PnPLAlloc();
  pIIS2ICLX_ACC_PnPLObj = Iis2iclx_Acc_PnPLAlloc();
  pIIS2ICLX_MLC_PnPLObj = Iis2iclx_Mlc_PnPLAlloc();
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
  if (sIIS3DWB10ISExtObj)
  {
    pIIS3DWB10IS_Ext_ACC_PnPLObj = Iis3dwb10is_Ext_Acc_PnPLAlloc();
    pIIS3DWB10IS_Ext_ISPU_PnPLObj = Iis3dwb10is_Ext_Ispu_PnPLAlloc();
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
  if (sISM6HG256XObj)
  {
    pISM6HG256X_H_ACC_PnPLObj = Ism6hg256x_H_Acc_PnPLAlloc();
    pISM6HG256X_L_ACC_PnPLObj = Ism6hg256x_L_Acc_PnPLAlloc();
    pISM6HG256X_GYRO_PnPLObj = Ism6hg256x_Gyro_PnPLAlloc();
    pISM6HG256X_MLC_PnPLObj = Ism6hg256x_Mlc_PnPLAlloc();
  }
  if (sTSC1641Obj)
  {
    pTSC1641_POW_PnPLObj = Tsc1641_Pow_PnPLAlloc();
  }
  if (sIIS2DULPXObj)
  {
    pIIS2DULPX_ACC_PnPLObj = Iis2dulpx_Acc_PnPLAlloc();
  }
  if (sILPS28QSWObj)
  {
    pILPS28QSW_PRESS_PnPLObj = Ilps28qsw_Press_PnPLAlloc();
  }

  pDeviceInfoPnPLObj = Deviceinformation_PnPLAlloc();
  pAcquisitionInfoPnPLObj = Acquisition_Info_PnPLAlloc();
  pTagsInfoPnPLObj = Tags_Info_PnPLAlloc();
  pFirmwareInfoPnPLObj = Firmware_Info_PnPLAlloc();
  pLogControllerPnPLObj = Log_Controller_PnPLAlloc();
  pAutomodePnPLObj = Automode_PnPLAlloc();

  pWifiConfigPnPLObj = Wifi_Config_PnPLAlloc();

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
  if (sIIS3DWB10ISExtObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj,
                            (SPIBusIF *)IIS3DWB10ISTaskGetSensorIF((IIS3DWB10ISTask *) sIIS3DWB10ISExtObj));
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
  if (sISM6HG256XObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj,
                            (SPIBusIF *)ISM6HG256XTaskGetSensorIF((ISM6HG256XTask *) sISM6HG256XObj));
  }
  if (sTSC1641Obj)
  {
    I2CBusTaskConnectDevice((I2CBusTask *) sI2C3BusObj, (I2CBusIF *)TSC1641TaskGetSensorIF((TSC1641Task *) sTSC1641Obj));
  }
  if (sIIS2DULPXObj)
  {
    SPIBusTaskConnectDevice((SPIBusTask *) sSPI2BusObj,
                            (SPIBusIF *)IIS2DULPXTaskGetSensorIF((IIS2DULPXTask *) sIIS2DULPXObj));
  }
  if (sILPS28QSWObj)
  {
    I2CBusTaskConnectDevice((I2CBusTask *) sI2C3BusObj,
                            (I2CBusIF *)ILPS28QSWTaskGetSensorIF((ILPS28QSWTask *) sILPS28QSWObj));
  }


  /************ Connect the Sensor events to the DatalogAppTask ************/
  IEventListener *DatalogAppListener = DatalogAppTask_GetEventListenerIF((DatalogAppTask *) sDatalogAppObj);
  IEventSrcAddEventListener(IIS2DLPCTaskGetEventSrcIF((IIS2DLPCTask *) sIIS2DLPCObj), DatalogAppListener);
  IEventSrcAddEventListener(IIS2ICLXTaskGetEventSrcIF((IIS2ICLXTask *) sIIS2ICLXObj), DatalogAppListener);
  IEventSrcAddEventListener(IIS2ICLXTaskGetMlcEventSrcIF((IIS2ICLXTask *) sIIS2ICLXObj), DatalogAppListener);
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
  if (sIIS3DWB10ISExtObj)
  {
    IEventSrcAddEventListener(IIS3DWB10ISTaskGetAccEventSrcIF((IIS3DWB10ISTask *) sIIS3DWB10ISExtObj), DatalogAppListener);
    IEventSrcAddEventListener(IIS3DWB10ISTaskGetIspuEventSrcIF((IIS3DWB10ISTask *) sIIS3DWB10ISExtObj), DatalogAppListener);
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
  if (sISM6HG256XObj)
  {
    IEventSrcAddEventListener(ISM6HG256XTaskGetAccEventSrcIF((ISM6HG256XTask *) sISM6HG256XObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM6HG256XTaskGetHgAccEventSrcIF((ISM6HG256XTask *) sISM6HG256XObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM6HG256XTaskGetGyroEventSrcIF((ISM6HG256XTask *) sISM6HG256XObj), DatalogAppListener);
    IEventSrcAddEventListener(ISM6HG256XTaskGetMlcEventSrcIF((ISM6HG256XTask *) sISM6HG256XObj), DatalogAppListener);
  }
  if (sTSC1641Obj)
  {
    IEventSrcAddEventListener(TSC1641TaskGetEventSrcIF((TSC1641Task *) sTSC1641Obj), DatalogAppListener);
  }
  if (sIIS2DULPXObj)
  {
    IEventSrcAddEventListener(IIS2DULPXTaskGetEventSrcIF((IIS2DULPXTask *)sIIS2DULPXObj), DatalogAppListener);
  }
  if (sILPS28QSWObj)
  {
    IEventSrcAddEventListener(ILPS28QSWTaskGetPressEventSrcIF((ILPS28QSWTask *) sILPS28QSWObj), DatalogAppListener);
  }

  /************ Connect Sensor LL to be used for ucf management to the DatalogAppTask ************/
  if (sISM330BXObj)
  {
    DatalogAppTask_SetExtMLCIF((AManagedTask *) sISM330BXObj);
  }
  else if (sISM6HG256XObj)
  {
    DatalogAppTask_SetExtMLCIF((AManagedTask *) sISM6HG256XObj);
  }
  else if (sISM330ISObj)
  {
    DatalogAppTask_SetIspuIF((AManagedTask *) sISM330ISObj);
  }
  else if (sIIS3DWB10ISExtObj)
  {
    DatalogAppTask_SetIspuIF((AManagedTask *) sIIS3DWB10ISExtObj);
  }
  else
  {
    /* No external MLC capable sensor */
  }
  DatalogAppTask_Set_ICLX_MLCIF((AManagedTask *) sIIS2ICLXObj);
  DatalogAppTask_SetMLCIF((AManagedTask *) sISM330DHCXObj);

  /************ Other PnPL Components ************/
  Deviceinformation_PnPLInit(pDeviceInfoPnPLObj);
  Firmware_Info_PnPLInit(pFirmwareInfoPnPLObj);
  Acquisition_Info_PnPLInit(pAcquisitionInfoPnPLObj);
  Tags_Info_PnPLInit(pTagsInfoPnPLObj);
  Log_Controller_PnPLInit(pLogControllerPnPLObj);
  Automode_PnPLInit(pAutomodePnPLObj);

  Wifi_Config_PnPLInit(pWifiConfigPnPLObj);

  /************ Sensor PnPL Components ************/
  Iis2dlpc_Acc_PnPLInit(pIIS2DLPC_ACC_PnPLObj);
  Iis2iclx_Acc_PnPLInit(pIIS2ICLX_ACC_PnPLObj);
  Iis2iclx_Mlc_PnPLInit(pIIS2ICLX_MLC_PnPLObj);
  Iis2mdc_Mag_PnPLInit(pIIS2MDC_MAG_PnPLObj);
  Ilps22qs_Press_PnPLInit(pILPS22QS_PRESS_PnPLObj);
  Imp23absu_Mic_PnPLInit(pIMP23ABSU_MIC_PnPLObj);
  Imp34dt05_Mic_PnPLInit(pIMP34DT05_MIC_PnPLObj);
  Stts22h_Temp_PnPLInit(pSTTS22H_TEMP_PnPLObj);
  Iis3dwb_Acc_PnPLInit(pIIS3DWB_ACC_PnPLObj);

  if (sSTTS22HExtObj)
  {
    Stts22h_Ext_Temp_PnPLInit(pSTTS22H_Ext_TEMP_PnPLObj);
    stts22h_ext_temp_set_enable(false, NULL);
  }
  if (sIIS3DWBExtObj)
  {
    Iis3dwb_Ext_Acc_PnPLInit(pIIS3DWB_Ext_ACC_PnPLObj);
    iis3dwb_ext_acc_set_enable(false, NULL);
  }
  if (sISM330BXObj)
  {
    Ism330bx_Acc_PnPLInit(pISM330BX_ACC_PnPLObj);
    ism330bx_acc_set_enable(false, NULL);
    Ism330bx_Gyro_PnPLInit(pISM330BX_GYRO_PnPLObj);
    ism330bx_gyro_set_enable(false, NULL);
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
    ism330is_acc_set_enable(false, NULL);
    Ism330is_Gyro_PnPLInit(pISM330IS_GYRO_PnPLObj);
    ism330is_gyro_set_enable(false, NULL);
    Ism330is_Ispu_PnPLInit(pISM330IS_ISPU_PnPLObj);
  }
  if (sISM6HG256XObj)
  {
    Ism6hg256x_H_Acc_PnPLInit(pISM6HG256X_H_ACC_PnPLObj);
    ism6hg256x_h_acc_set_enable(false, NULL);
    Ism6hg256x_L_Acc_PnPLInit(pISM6HG256X_L_ACC_PnPLObj);
    ism6hg256x_l_acc_set_enable(false, NULL);
    Ism6hg256x_Gyro_PnPLInit(pISM6HG256X_GYRO_PnPLObj);
    ism6hg256x_gyro_set_enable(false, NULL);
    Ism6hg256x_Mlc_PnPLInit(pISM6HG256X_MLC_PnPLObj);
  }
  if (sTSC1641Obj)
  {
    Tsc1641_Pow_PnPLInit(pTSC1641_POW_PnPLObj);
    /* Enable HW tags with STEVAL-CP34KPM1 only */
    tags_info_set_hw_tag0__label("DIN1", NULL);
    tags_info_set_hw_tag1__label("DIN2", NULL);
    tags_info_set_hw_tag0__enabled(true, NULL);
    tags_info_set_hw_tag1__enabled(true, NULL);
    tags_info_set_hw_tag0__status(false, NULL);
    tags_info_set_hw_tag1__status(false, NULL);
  }
  if (sIIS2DULPXObj)
  {
    Iis2dulpx_Acc_PnPLInit(pIIS2DULPX_ACC_PnPLObj);
    iis2dulpx_acc_set_enable(false, NULL);
  }
  if (sILPS28QSWObj)
  {
    Ilps28qsw_Press_PnPLInit(pILPS28QSW_PRESS_PnPLObj);
    ilps28qsw_press_set_enable(false, NULL);
  }
  if (sIIS3DWB10ISExtObj)
  {
    Iis3dwb10is_Ext_Acc_PnPLInit(pIIS3DWB10IS_Ext_ACC_PnPLObj);
    iis3dwb10is_ext_acc_set_enable(true, NULL);
    Iis3dwb10is_Ext_Ispu_PnPLInit(pIIS3DWB10IS_Ext_ISPU_PnPLObj);

    iis2dlpc_acc_set_enable(false, NULL);
    iis2iclx_acc_set_enable(false, NULL);
    iis2mdc_mag_set_enable(false, NULL);
    ilps22qs_press_set_enable(false, NULL);
    imp23absu_mic_set_enable(false, NULL);
    imp34dt05_mic_set_enable(false, NULL);
    ism330dhcx_acc_set_enable(false, NULL);
    ism330dhcx_gyro_set_enable(false, NULL);
    stts22h_temp_set_enable(false, NULL);
    iis3dwb_acc_set_enable(false, NULL);
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


void EXT_INT1_EXTI_Callback(uint16_t nPin)
{
  if (sIIS3DWBExtObj)
  {
    IIS3DWBTask_EXTI_Callback(nPin);
  }
  else if (sIIS3DWB10ISExtObj)
  {
    IIS3DWB10ISTask_EXTI_Callback(nPin);
  }
  else if (sISM330BXObj)
  {
    ISM330BXTask_EXTI_Callback(nPin);
  }
  else if (sISM330ISObj)
  {
    ISM330ISTask_EXTI_Callback(nPin);
  }
  else if (sISM6HG256XObj)
  {
    ISM6HG256XTask_EXTI_Callback(nPin);
  }
  else if (sIIS2DULPXObj)
  {
    IIS2DULPXTask_EXTI_Callback(nPin);
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
  else if (sISM330BXObj)
  {
    INT2_ISM330BX_EXTI_Callback(nPin);
  }
  else if (sIIS3DWB10ISExtObj)
  {
    INT2_IIS3DWB10IS_EXTI_Callback(nPin);
  }
  else
  {
    INT2_HG256X_EXTI_Callback(nPin);
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
