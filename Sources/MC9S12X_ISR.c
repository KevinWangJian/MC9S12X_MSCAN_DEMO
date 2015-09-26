/**
  ******************************************************************************
  * @Copyright (C), 1997-2015, Hangzhou Gold Electronic Equipment Co., Ltd.
  * @file name: MC9S12X_ISR.c
  * @author: Wangjian
  * @Descriptiuon: Provides a set of system interrupt service routines.
  * @Others: None
  * @History: 1. Created by Wangjian.
  * @version: V1.0.0
  * @date:    19-Sep-2015

  ******************************************************************************
  * @attention
  *
  * Licensed under GPLv2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.gnu.org/licenses/gpl-2.0.html
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
#include "MC9S12X_ISR.h"
#include "System_Driver.h"
#include "MSCAN_Driver.h"
#include "GPIO_Driver.h"
#include "CAN_Message.h"




  
  
#pragma CODE_SEG __NEAR_SEG NON_BANKED


void interrupt VectorNumber_Vrti RTI_ISR(void)
{
    static uint8_t Time_Count = 0;
    
    int16_t ret_val;
    
    MSCAN_MessageTypeDef S_Message;
    MSCAN_ModuleConfig CANx_Module;
    
    CANx_Module.ch   = MSCAN_Channel0;
    CANx_Module.pins = MSCAN0_PM0_PM1;
   
	if (CRGFLG_RTIF)
	{
		/* Clear the RTI interrupt flag by writing 1 to it */
		CRGFLG_RTIF = 1;
		
		/* Call time delay decrement function */
		TimeDelay_Decrement();
		
		Time_Count++;
		
		if (Time_Count >= 50) /* Delay 500ms time is arrived */
		{
		    Time_Count = 0;
		     
		    ret_val = Check_CANSendBuffer(MSCAN_Channel0, &S_Message);
		    
		    if (ret_val == 0) 
		    {
		        (void)MSCAN_SendFrame(&CANx_Module, &S_Message);
		    } 
		}
	}
}


/* Add your interrupt service routines here. */



#pragma CODE_SEG DEFAULT
