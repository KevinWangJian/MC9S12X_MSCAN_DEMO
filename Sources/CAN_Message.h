/**
  ******************************************************************************
  * @Copyright (C), 1997-2015, Hangzhou Gold Electronic Equipment Co., Ltd.
  * @file name: CAN_Message.h
  * @author: Wangjian
  * @Descriptiuon: Provides a set of reading and writting soft CAN send and receive
  *                buffers functions.
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


#ifndef  __CAN_MESSAGE_H
#define  __CAN_MESSAGE_H

#include <MC9S12XEQ512.h>
#include <hidef.h>
#include "derivative.h"

#include "common.h"
#include "MSCAN_Driver.h"


/* Exported types ------------------------------------------------------------*/

/* Global intranet, ECU, charger receive buffer size definition which will be used by CPU core and XGATE */
#define   INTRANET_RECEIVEBUF_SIZE    (70)
#define   ECU_RECEIVEBUF_SIZE         (10)
#define   CHARGER_RECEIVEBUF_SIZE     (10)

/* Global intranet, ECU, charger receive buffer size definition which will just be used by CPU core */
#define   INTRANET_SENDBUF_SIZE       (30)
#define   ECU_SENDBUF_SIZE            (30)
#define   CHARGER_SENDBUF_SIZE        (10)




typedef struct 
{
    uint8_t Charger_SendBuff_WPointer;
    uint8_t Charger_SendBuff_RPointer;
    
    uint8_t Intranet_SendBuff_WPointer;
    uint8_t Intranet_SendBuff_RPointer;
    
    uint8_t ECU_SendBuff_WPointer;
    uint8_t ECU_SendBuff_RPointer;

    MSCAN_MessageTypeDef Charger_SendBuff[CHARGER_SENDBUF_SIZE]; 
    
    MSCAN_MessageTypeDef Intranet_SendBuff[INTRANET_SENDBUF_SIZE]; 
    
    MSCAN_MessageTypeDef ECU_SendBuff[ECU_SENDBUF_SIZE];  
        
}CANSendMessagebuffer_TypeDef;




typedef struct 
{
    uint8_t Intranet_RecBuff_WPointer;
    uint8_t Intranet_RecBuff_RPointer;
    
    uint8_t ECU_RecBuff_WPointer;
    uint8_t ECU_RecBuff_RPointer;
    
    uint8_t Charger_RecBuff_WPointer;
    uint8_t Charger_RecBuff_RPointer;
    
    MSCAN_MessageTypeDef Intranet_RecBuf[INTRANET_RECEIVEBUF_SIZE];
    
    MSCAN_MessageTypeDef ECU_RecBuf[ECU_RECEIVEBUF_SIZE];
    
    MSCAN_MessageTypeDef Charger_RecBuf[CHARGER_RECEIVEBUF_SIZE];
    
}CANReceiveMessageBuffer_TypeDef;



#pragma DATA_SEG __GPAGE_SEG PAGED_RAM

extern volatile CANSendMessagebuffer_TypeDef g_CANx_SendBuffer;

#pragma DATA_SEG DEFAULT


#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions ------------------------------------------------------- */

int16_t Check_CANReceiveBuffer(MSCAN_ChannelTypeDef CANx, MSCAN_MessageTypeDef* CAN_RMessage);


int16_t Fill_CANSendBuffer(MSCAN_ChannelTypeDef CANx, MSCAN_MessageTypeDef* CAN_WMessage);


int16_t Check_CANSendBuffer(MSCAN_ChannelTypeDef CANx, MSCAN_MessageTypeDef* CAN_RMessage);




#ifdef __cplusplus
}
#endif

#endif

/*****************************END OF FILE**************************************/
