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
#include "DatalogAppTask.h"
#include "IMP23ABSUTask.h"
#include "FDM_ACOTask.h"

#include "PnPLCompManager.h"
#include "Deviceinformation_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Fft_Dpu_PnPL.h"

#include "Imp23absu_Mic_PnPL.h"
#include "parson.h"

static uint8_t FW_ID = 0x23;

static IPnPLComponent_t *pLogControllerPnPLObj = NULL;
static IPnPLComponent_t *pIMP23ABSU_MIC_PnPLObj = NULL;
static IPnPLComponent_t *pAcquisition_Info_PnPLObj = NULL;
static IPnPLComponent_t *pDeviceinformation_PnPLObj = NULL;
static IPnPLComponent_t *Firmware_Info_PnPLObj = NULL;
static IPnPLComponent_t *Fft_Dpu_PnPLObj = NULL;

/**
  * Utility task object.
  */
static AManagedTaskEx *sUtilObj = NULL;

/**
  * Sensor task object.
  */
static AManagedTaskEx *sIMP23ABSUObj = NULL;

/**
  * DPU container task object.
  */
static AManagedTaskEx *sFFTAudioObj = NULL;

/**
  * DatalogApp task object.
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
  E_POWER_MODE_STATE1,
  E_POWER_MODE_RESERVED
};

/* Private functions declaration */
/*********************************/

/**
  * Re-map the PM State Machine of the Sensor Manager managed tasks used in the application according to the following map:
  *
  * | App State                      | Sensor Manager State         | BLE task State               |
  * | :----------------------------- | ---------------------------: | ---------------------------: |
  * | E_POWER_MODE_STATE1            | E_POWER_MODE_STATE1          | E_POWER_MODE_STATE1          |
  * | E_POWER_MODE_SLEEP_1           | E_POWER_MODE_SLEEP_1         | E_POWER_MODE_STATE1          |
  * | E_POWER_MODE_SENSORS_ACTIVE    | E_POWER_MODE_SENSORS_ACTIVE  | E_POWER_MODE_STATE1          |
  * | E_POWER_MODE_STARTING          | E_POWER_MODE_STATE1          | E_POWER_MODE_STATE1          |
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
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  PnPLSetAllocationFunctions(SysAlloc, SysFree);

  /* Allocate the task objects */
  sFFTAudioObj = FDM_ACOTaskAlloc();
  sDatalogAppObj = DatalogAppTaskAlloc();
  sUtilObj = UtilTaskAlloc(&MX_TIM5InitParams);
  sIMP23ABSUObj = IMP23ABSUTaskAlloc(&MX_MDF1InitParams, &MX_ADC1InitParams);

  /* Add the task object to the context. */
  xRes = ACAddTask(pAppContext, (AManagedTask *) sFFTAudioObj);
  xRes = ACAddTask(pAppContext, (AManagedTask *) sDatalogAppObj);
  xRes = ACAddTask(pAppContext, (AManagedTask *) sUtilObj);
  xRes = ACAddTask(pAppContext, (AManagedTask *) sIMP23ABSUObj);

  pIMP23ABSU_MIC_PnPLObj = Imp23absu_Mic_PnPLAlloc();
  pLogControllerPnPLObj = Log_Controller_PnPLAlloc();
  pAcquisition_Info_PnPLObj = Acquisition_Info_PnPLAlloc();
  pDeviceinformation_PnPLObj = Deviceinformation_PnPLAlloc();
  Firmware_Info_PnPLObj = Firmware_Info_PnPLAlloc();
  Fft_Dpu_PnPLObj = Fft_Dpu_PnPLAlloc();

  PnPLSetFWID(FW_ID);

  return xRes;
}

sys_error_code_t SysOnStartApplication(ApplicationContext *pAppContext)
{
  UNUSED(pAppContext);

  /* Re-map the state machine of the Integrated tasks */
  SensorManagerStateMachineRemap(spAppPMState2SMPMStateMap);

  /** Get microphone observable interface from IMP23ABSUTask
    *  mic_sensor_obsv_interface (type: ISourceObservable) is a subset of methods exposed by the sensor (type: ISensor_t)
    **/
  ISourceObservable *mic_sensor_obsv_interface = IMP23ABSUTaskGetMicSensorIF((IMP23ABSUTask *) sIMP23ABSUObj);

  /* Set sFFTAudioObj source observable object */
  FDM_ACOTaskSetSourceIF((FDM_ACOTask *) sFFTAudioObj, mic_sensor_obsv_interface);

  /* Get DatalogApp IDataEventListener_t interface:  DatalogAppListener*/
  IDataEventListener_t *DatalogAppListener = DatalogAppTask_GetEventListenerIF((DatalogAppTask *) sDatalogAppObj);

  IEventListener *MicrophoneSensorListener = (IEventListener *) DatalogAppTask_GetSensorEventListenerIF((DatalogAppTask *) sDatalogAppObj);
  IEventSrcAddEventListener(IMP23ABSUTaskGetEventSrcIF((IMP23ABSUTask *) sIMP23ABSUObj), MicrophoneSensorListener);

  /* Connect DatalogAppListener as DPU listener */
  FDM_ACOTaskAddDPUListener((FDM_ACOTask *) sFFTAudioObj, DatalogAppListener);

  /* Sensor PnPL Components */
  Imp23absu_Mic_PnPLInit(pIMP23ABSU_MIC_PnPLObj);

  /* Other PnPL Components */
  Log_Controller_PnPLInit(pLogControllerPnPLObj, DatalogAppTask_GetILogControllerIF((DatalogAppTask *) sDatalogAppObj));

  Acquisition_Info_PnPLInit(pAcquisition_Info_PnPLObj);

  Deviceinformation_PnPLInit(pDeviceinformation_PnPLObj);

  Firmware_Info_PnPLInit(Firmware_Info_PnPLObj);

  Fft_Dpu_PnPLInit(Fft_Dpu_PnPLObj);

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

/* Private function definition */
/*******************************/

static sys_error_code_t SensorManagerStateMachineRemap(EPowerMode *pPMState2PMStateMap)
{
  assert_param(pPMState2PMStateMap != NULL);

  AMTSetPMStateRemapFunc((AManagedTask *) sIMP23ABSUObj, pPMState2PMStateMap);

  return SYS_NO_ERROR_CODE;
}
