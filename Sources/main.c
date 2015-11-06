/**
  ******************************************************************************
  * @Copyright (C), 1997-2015, Hangzhou Gold Electronic Equipment Co., Ltd.
  * @file name: main.c
  * @author: Wangjian
  * @Descriptiuon: System main loop function and intialize XGATE function.
  * @Others: None
  * @History: 1. Created by Wangjian.
  * @version: V1.0.0
  * @date:    26-Sep-2015

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
#include <hidef.h>           /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

#include <string.h>
#include "xgate.h"

#include "common.h"
#include "System_Driver.h"
#include "GPIO_Driver.h"
#include "MSCAN_Driver.h"
#include "CAN_Message.h"




#pragma push

/* this variable definition is to demonstrate how to share data between XGATE and S12X */
#pragma DATA_SEG SHARED_DATA
volatile int shared_counter; /* volatile because both cores are accessing it. */

#pragma DATA_SEG __GPAGE_SEG PAGED_RAM
/* A volatile CAN receive buffer because both cores and XGATE are accessing it */
volatile CANReceiveMessageBuffer_TypeDef g_CANx_RecBuffer;  
#pragma DATA_SEG DEFAULT


/* Two stacks in XGATE core3 */ 
#pragma DATA_SEG XGATE_STK_L
word XGATE_STACK_L[1]; 
#pragma DATA_SEG XGATE_STK_H
word XGATE_STACK_H[1];

#pragma pop


#define   ROUTE_INTERRUPT(vec_adr, cfdata)                \
  INT_CFADDR = (vec_adr) & 0xF0;                          \
  INT_CFDATA_ARR[((vec_adr) & 0x0F) >> 1] = (cfdata)




#define   SOFTWARETRIGGER0_VEC  0x72      /* vector address= 2 * channel id */

#define   MSCAN0RECEIVE_VEC     0xB2      /* MSCAN0 Receive interrupt vector address.(0x59 * 2 = 0xB2) */

#define   MSCAN1RECEIVE_VEC     0xAA      /* MSCAN1 receive interrupt vector address.(0x55 * 2 = 0xAA) */

#define   MSCAN4RECEIVE_VEC     0x92      /* MSCAN4 receive interrupt vector address.(0x49 * 2 = 0x92) */





/**
 * @brief   Initialize XGATE.
 * @param   None
 * @returns None
 * @attention User should add different vector address and priority depend on different applications.
 */
static void SetupXGATE(void) 
{
    /* initialize the XGATE vector block and
     set the XGVBR register to its start address */
    XGVBR= (unsigned int)(void*__far)(XGATE_VectorTable - XGATE_VECTOR_OFFSET);

    /* switch software trigger 0 interrupt to XGATE */
    ROUTE_INTERRUPT(SOFTWARETRIGGER0_VEC, 0x81); /* RQST=1 and PRIO=1 */
    
    ROUTE_INTERRUPT(MSCAN0RECEIVE_VEC, 0x81); /* Configure CAN0 receive interrupt vector and priority in XGATE */
    
    ROUTE_INTERRUPT(MSCAN1RECEIVE_VEC, 0x81); /* Configure CAN1 receive interrupt vector and priority in XGATE */
    
    ROUTE_INTERRUPT(MSCAN4RECEIVE_VEC, 0x81); /* Configure CAN4 receive interrupt vector and priority in XGATE */

    /* when changing your derivative to non-core3 one please remove next five lines */
    XGISPSEL= 1;
    XGISP31= (unsigned int)(void*__far)(XGATE_STACK_L + 1);
    XGISPSEL= 2;
    XGISP74= (unsigned int)(void*__far)(XGATE_STACK_H + 1);
    XGISPSEL= 0;

    /* enable XGATE mode and interrupts */
    XGMCTL= 0xFBC1; /* XGE | XGFRZ | XGIE */

    /* force execution of software trigger 0 handler */
    XGSWT= 0x0101;
}





/**
 * @brief   System main loop function.
 * @param   None
 * @returns None
 */
void main(void) 
{
/* Local variable definition which will be used in the following program */
    int16_t ret_val,k;

    MSCAN_ParametersConfig CAN_Property;
    MSCAN_FilterConfig CAN_Filter;
  
    MSCAN_ModuleConfig CAN_Module;
    MSCAN_MessageTypeDef Send_Buf;
    MSCAN_MessageTypeDef T_ReceiveBuf;
    
    DisableInterrupts;                               /* Disable total interrupt */
    
    SetupXGATE();                                    /* Initialize XGATE */
   
    g_CANx_RecBuffer.Intranet_RecBuff_WPointer = 0;
    g_CANx_RecBuffer.Intranet_RecBuff_RPointer = 0;
    g_CANx_RecBuffer.ECU_RecBuff_WPointer      = 0;
    g_CANx_RecBuffer.ECU_RecBuff_RPointer      = 0;
    g_CANx_RecBuffer.Charger_RecBuff_WPointer  = 0;
    g_CANx_RecBuffer.Charger_RecBuff_RPointer  = 0;
    
    for (k = 0; k < INTRANET_RECEIVEBUF_SIZE; k++) 
    {
        g_CANx_RecBuffer.Intranet_RecBuf[k].frametype = (MSCAN_FrameAndIDTypeDef)0;  
    }
    
    for (k = 0; k < ECU_RECEIVEBUF_SIZE; k++) 
    {
        g_CANx_RecBuffer.ECU_RecBuf[k].frametype      = (MSCAN_FrameAndIDTypeDef)0;
    }
    
    for (k = 0; k < CHARGER_RECEIVEBUF_SIZE; k++) 
    {   
        g_CANx_RecBuffer.Charger_RecBuf[k].frametype  = (MSCAN_FrameAndIDTypeDef)0;
    }
    
    g_CANx_SendBuffer.Intranet_SendBuff_WPointer = 0;
    g_CANx_SendBuffer.Intranet_SendBuff_RPointer = 0;
    g_CANx_SendBuffer.ECU_SendBuff_WPointer      = 0;
    g_CANx_SendBuffer.ECU_SendBuff_RPointer      = 0;
    g_CANx_SendBuffer.Charger_SendBuff_WPointer  = 0;
    g_CANx_SendBuffer.Charger_SendBuff_RPointer  = 0;
    
    for (k = 0; k < INTRANET_SENDBUF_SIZE; k++) 
    {
        g_CANx_SendBuffer.Intranet_SendBuff[k].frametype = (MSCAN_FrameAndIDTypeDef)0; 
    }
    
    for (k = 0; k < ECU_RECEIVEBUF_SIZE; k++) 
    {
        g_CANx_SendBuffer.ECU_SendBuff[k].frametype      = (MSCAN_FrameAndIDTypeDef)0;
    }
    
    for (k = 0; k < CHARGER_RECEIVEBUF_SIZE; k++) 
    {   
        g_CANx_SendBuffer.Charger_SendBuff[k].frametype  = (MSCAN_FrameAndIDTypeDef)0;
    }
    

    
    /* Configure CAN module trnasfer property parameters */
    CAN_Property.baudrate                    = MSCAN_Baudrate_250K;
    CAN_Property.MSCAN_StopInWaitMode        = 0;
    CAN_Property.MSCAN_TimeStampEnable       = 0;
    CAN_Property.MSCAN_WakeUpEnable          = 1;
    CAN_Property.MSCAN_ModuleEnable          = 1;
    CAN_Property.MSCAN_ClockSource           = 0;
    CAN_Property.MSCAN_LoopbackMode          = 0;
    CAN_Property.MSCAN_ListenOnlyMode        = 0;
    CAN_Property.MSCAN_BusoffRecoveryMode    = 0;
    CAN_Property.MSCAN_WakeUpMode            = 0;
    CAN_Property.MSCAN_WakeUpINTEnable       = 0;
    CAN_Property.MSCAN_StatusChangeINTEnable = 0;
    CAN_Property.MSCAN_OverrunINTEnable      = 0;
    CAN_Property.MSCAN_ReceiveFullINTEnable  = 1;
    CAN_Property.MSCAN_Trans0EmptyINTEnable  = 0;
    CAN_Property.MSCAN_Trans1EmptyINTEnable  = 0;
    CAN_Property.MSCAN_Trans2EmptyINTEnable  = 0;
    
    /* Configure CAN module filters */
    CAN_Filter.Filter_Enable  = 1;    /* If this value is cleared,the following parameters does not work */
    CAN_Filter.filter_channel = 0;
    CAN_Filter.id_format      = OnlyAcceptExtendedID;
    CAN_Filter.frame_type     = OnlyAcceptDataFrame;
    CAN_Filter.id_mask        = 0xFFFFFFFFu; /* If this value is set to 0xFFFFFFFF,the id_accept value does not work */
    CAN_Filter.id_accept      = 0x18901212u;
    
    /* Configure CAN module Send data */ 
    Send_Buf.frametype   = DataFrameWithExtendedId;
    Send_Buf.frame_id    = 0x18f09234;
    Send_Buf.data_length = 8;
    Send_Buf.data[0]     = 0x29;
    Send_Buf.data[1]     = 0x29;
    Send_Buf.data[2]     = 0x29;
    Send_Buf.data[3]     = 0x29;
    Send_Buf.data[4]     = 0x29;
    Send_Buf.data[5]     = 0x29;
    Send_Buf.data[6]     = 0x29;
    Send_Buf.data[7]     = 0x29;
    
    /* Initialize sysytem clock and Bus clock frequency */
    ret_val = SystemClock_Init(BusClock_32MHz);
    
    /* Select CAN module number and signal pins remap */
    CAN_Module.ch = MSCAN_Channel0;
    CAN_Module.pins = MSCAN0_PM0_PM1;
    
    /* Initialize MSCAN module by the specified property */
    ret_val = MSCAN_Init(&CAN_Module, &CAN_Property, &CAN_Filter);
    
    
    /* Select CAN module number and signal pins remap */
    CAN_Module.ch = MSCAN_Channel1;
    CAN_Module.pins = MSCAN1_PM2_PM3;
    
    /* Initialize MSCAN module by the specified property */
    ret_val = MSCAN_Init(&CAN_Module, &CAN_Property, &CAN_Filter);
    
    
    /* Select CAN module number and signal pins remap */
    CAN_Module.ch = MSCAN_Channel4;
    CAN_Module.pins = MSCAN4_PM4_PM5;
    
    /* Initialize MSCAN module by the specified property */
    ret_val = MSCAN_Init(&CAN_Module, &CAN_Property, &CAN_Filter);
    
    /* Initialize System RTI timer to generate systick interrupt */
    ret_val = SystemRTI_Init(RTI_Cycle_10ms);
    
    GPIO_Init(GPIOT, GPIO_Pin6, GPIO_Output);
    
    EnableInterrupts;                                 /* Enable total interrupt */

    for(;;) 
    {  
        Delay10ms(25);
        
       // ret_val = Fill_CANSendBuffer(MSCAN_Channel0, &Send_Buf);
        
        ret_val = Fill_CANSendBuffer(MSCAN_Channel4, &Send_Buf);

        if (Check_CANReceiveBuffer(MSCAN_Channel4, &T_ReceiveBuf) == 0) 
        {
            if (T_ReceiveBuf.frame_id == 0x18901212u) 
            {
                T_ReceiveBuf.frame_id = 0;
                
                GPIO_ToggleBit(GPIOT, GPIO_Pin6);
            }
        }
    }
}
