/**
  ******************************************************************************
  * @file    hci_tl_template.c 
  * @author  AMG RF Application Team
  * @version V1.0.0
  * @date    18-Jan-2018
  * @brief   Contains the basic functions for managing the framework required
  *          for handling the HCI interface
  ******************************************************************************
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics</center></h2>
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/ 
#include "hci_const.h"
#include "hci.h"
#include "hci_tl.h"

static tHciContext    hciContext;

void hci_init(void(* UserEvtRx)(void* pData), void* pConf)
{
  /* USER CODE BEGIN hci_init 1 */
  
  /* USER CODE END hci_init 1 */
  
  if(UserEvtRx != NULL)
  {
    hciContext.UserEvtRx = UserEvtRx;
  }
  
  /* Initialize TL BLE layer */
  hci_tl_lowlevel_init();
  
  /* USER CODE BEGIN hci_init 2 */
  
  /* USER CODE END hci_init 2 */
  
  /* Initialize low level driver */
  if (hciContext.io.Init)  hciContext.io.Init(NULL);
  if (hciContext.io.Reset) hciContext.io.Reset();
}

void hci_register_io_bus(tHciIO* fops)
{
  /* Register bus function */
  hciContext.io.Init    = fops->Init; 
  hciContext.io.Receive = fops->Receive;  
  hciContext.io.Send    = fops->Send;
  hciContext.io.GetTick = fops->GetTick;
  hciContext.io.Reset   = fops->Reset;
}

int hci_send_req(struct hci_request* r, BOOL async)
{
  /* USER CODE BEGIN hci_send_req */
  
  /* USER CODE END hci_send_req */
  
  return 0;
}
 
void hci_user_evt_proc(void)
{
  /* USER CODE BEGIN hci_user_evt_proc */
  
  /* USER CODE END hci_user_evt_proc */
}

int32_t hci_notify_asynch_evt(void* pdata)
{
  /* USER CODE BEGIN hci_notify_asynch_evt */
  
  /* USER CODE END hci_notify_asynch_evt */
  
  return 0;
}

void hci_resume_flow(void)
{
  /* USER CODE BEGIN hci_resume_flow */
  
  /* USER CODE END hci_resume_flow */
}

void hci_cmd_resp_wait(uint32_t timeout)
{
  /* USER CODE BEGIN hci_cmd_resp_wait */
  
  /* USER CODE END hci_cmd_resp_wait */
}

void hci_cmd_resp_release(uint32_t flag)
{
  /* USER CODE BEGIN hci_cmd_resp_release */
  
  /* USER CODE END hci_cmd_resp_release */
}
