/**
  ******************************************************************************
  * @file    SensorTileBoxPro_nfctag_ex.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.1.0
  * @date    20-July-2023
  * @brief   This file provides a set of functions needed to manage a nfc dual 
  *          interface eeprom memory.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "SensorTileBoxPro_nfctag.h"
#include "SensorTileBoxPro_nfctag_ex.h"

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Configures the ITtime duration for the GPO pulse.
  * @param  uint32_t MsbPasswd MSB 32-bit password
  * @param  uint32_t LsbPasswd LSB 32-bit password
  * @param  ITtime Coefficient for the Pulse duration to be written (Pulse duration = 302,06 us - ITtime * 512 / fc)
  * @retval int32_t enum status.
  */
int32_t BSP_NFCTAG_ChangeITPulse(uint32_t MsbPasswd,uint32_t LsbPasswd,const uint8_t ITtime)
{
  int32_t ret;
  ST25DV_PASSWD Passwd;
  /* Present password to open session  */
  Passwd.MsbPasswd = MsbPasswd;
  Passwd.LsbPasswd = LsbPasswd;
  ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, &Passwd );

  /* Change the Mailbox configuration value */
  ret = BSP_NFCTAG_WriteITPulse( BSP_NFCTAG_INSTANCE, ITtime );

  /* present wrong password for closing the session */
  Passwd.MsbPasswd = ~MsbPasswd;
  Passwd.LsbPasswd = ~LsbPasswd;
  BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, &Passwd );

  return ret;
}

/**
  * @brief  Change MailboxConfiguration
  * @param  uint32_t MsbPasswd MSB 32-bit password
  * @param  uint32_t LsbPasswd LSB 32-bit password
  * @param  uint8_t MB_mode Enable Disable the Mailbox
  * @retval int32_t enum status
  */
int32_t BSP_NFCTAG_ChangeMBMode(uint32_t MsbPasswd,uint32_t LsbPasswd,const uint8_t MB_mode)
{
  int32_t ret;
  ST25DV_PASSWD Passwd;
  /* Present password to open session  */
  Passwd.MsbPasswd = MsbPasswd;
  Passwd.LsbPasswd = LsbPasswd;
  ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, &Passwd );

  /* Change the Mailbox configuration value */
  ret = BSP_NFCTAG_WriteMBMode( BSP_NFCTAG_INSTANCE, MB_mode );

  /* present wrong password for closing the session */
  Passwd.MsbPasswd = ~MsbPasswd;
  Passwd.LsbPasswd = ~LsbPasswd;
  BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, &Passwd );

  return ret;
}

/**
  * @brief  Change the I2C Password protection
  * @param  uint32_t MsbPasswd MSB 32-bit password
  * @param  uint32_t LsbPasswd LSB 32-bit password
  * @retval int32_t enum status
  */
int32_t BSP_NFCTAG_ChangeI2CPassword(uint32_t MsbPasswd,uint32_t LsbPasswd)
{
  int32_t ret = NFCTAG_OK;
  ST25DV_I2CSSO_STATUS i2csso;
  ST25DV_PASSWD Passwd;
  BSP_NFCTAG_ReadI2CSecuritySession_Dyn( BSP_NFCTAG_INSTANCE, &i2csso );
  if( i2csso == ST25DV_SESSION_CLOSED ) {
    /* if I2C session is closed, present default password to open session */
    Passwd.MsbPasswd = 0;
    Passwd.LsbPasswd = 0;
    ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, &Passwd );
    if(ret==NFCTAG_OK) {
      /* Ok we could Change the default Password */
      Passwd.MsbPasswd = MsbPasswd;
      Passwd.LsbPasswd = LsbPasswd;
      ret = BSP_NFCTAG_WriteI2CPassword(BSP_NFCTAG_INSTANCE, &Passwd);
      if(ret==NFCTAG_OK) {
        /* Present a wrong password for closing the session we have alredy setted the new one here */
        Passwd.MsbPasswd = ~MsbPasswd;
        Passwd.LsbPasswd = ~LsbPasswd;
        BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, &Passwd );
      }
    }
  }
  return ret;
}

/**
  * @brief  Write GPO configuration:
  *      GPO managed by user             = ST25DV_GPO_ENABLE_MASK | ST25DV_GPO_RFUSERSTATE_MASK
  *      GPO sensible to RF activity     = ST25DV_GPO_ENABLE_MASK | ST25DV_GPO_RFACTIVITY_MASK
  *      GPO sensible to RF Field change = ST25DV_GPO_ENABLE_MASK | ST25DV_GPO_FIELDCHANGE_MASK
  *
  * @param  uint32_t MsbPasswd MSB 32-bit password
  * @param  uint32_t LsbPasswd LSB 32-bit password
  * @param  uint16_t ITConfig Provides the GPO configuration to apply
  * @retval int32_t enum status
  */
int32_t BSP_NFCTAG_WriteConfigIT(uint32_t MsbPasswd,uint32_t LsbPasswd,const uint16_t ITConfig)
{
  int32_t ret;
  ST25DV_PASSWD Passwd;
  /* Present password to open session  */
  Passwd.MsbPasswd = MsbPasswd;
  Passwd.LsbPasswd = LsbPasswd;
  ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, &Passwd );

  if(ret==NFCTAG_OK) {
    /* Change the GPO configuration value */
    ret = BSP_NFCTAG_ConfigIT( BSP_NFCTAG_INSTANCE, ITConfig );
    
    if(ret==NFCTAG_OK) {
       /* present wrong password for closing the session */
    Passwd.MsbPasswd = ~MsbPasswd;
    Passwd.LsbPasswd = ~LsbPasswd;
    ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, &Passwd );
    }
  }
  return ret;
}

/**
  * @brief  Change the Mailbox watchdog coefficient delay
  * @param  uint32_t MsbPasswd MSB 32-bit password
  * @param  uint32_t LsbPasswd LSB 32-bit password
  * @param  WdgDelay Watchdog duration coefficient to be written (Watch dog duration = MB_WDG*30 ms +/- 6%).
  * @return int32_t enum status.
  */
int32_t BSP_NFCTAG_ChangeMBWDG(uint32_t MsbPasswd,uint32_t LsbPasswd,const uint8_t WdgDelay)
{
  int32_t ret;
  ST25DV_PASSWD Passwd;
  /* Present password to open session  */
  Passwd.MsbPasswd = MsbPasswd;
  Passwd.LsbPasswd = LsbPasswd;
  ret = BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, &Passwd );

  /* Change the Mailbox configuration value */
  ret = BSP_NFCTAG_WriteMBWDG( BSP_NFCTAG_INSTANCE, WdgDelay );

  /* present wrong password for closing the session */
  Passwd.MsbPasswd = ~MsbPasswd;
  Passwd.LsbPasswd = ~LsbPasswd;
  BSP_NFCTAG_PresentI2CPassword( BSP_NFCTAG_INSTANCE, &Passwd );

  return ret;
}

