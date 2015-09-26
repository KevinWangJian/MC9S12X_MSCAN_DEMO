/**
  ******************************************************************************
  * @Copyright (C), 1997-2015, Hangzhou Gold Electronic Equipment Co., Ltd.
  * @file name: MSCAN_Driver.c
  * @author: Wangjian
  * @Descriptiuon: Provides a set of functions to help users to use MC9S12 MSCAN
  *                module.This functions can initialize CAN controller,send CAN
  *                messages by CAN module,receive CAN messages by CAN module.
  *                Also,user can configure CAN module filters that will filter
  *                some CAN messages.
  * @Others: None
  * @History: 1. Created by Wangjian.                                       (V1.0.0)
  *           2. Add a select chance to decide whether CAN filter is enable or 
  *              disable in configure CAN id filter function.
  *              Also,move the CAN receive frame function to the XGATE.     (V1.0.1)
  *           3. Add a functon which is Checking the specified CAN module whether 
  *              have enough hard transmission buffer to send CAN messages. (V1.0.2)
  * @version: V1.0.2
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
 
#ifndef  __MSCAN_DRIVER_H
#define  __MSCAN_DRIVER_H

#include <MC9S12XEQ512.h>
#include <hidef.h>
#include "derivative.h"

#include "common.h"


/* Exported types ------------------------------------------------------------*/

/* Declaration MSCAN driver version */
#define   MSCAN_DRIVER_VERSION     (102)		/* Rev1.0.2 */




/* MSCAN baud rate enumeration */
typedef enum 
{
    MSCAN_Baudrate_50K = 0,                    /* Baud rate 50Kbps */
    MSCAN_Baudrate_100K,                       /* Baud rate 100Kbps */
    MSCAN_Baudrate_125K,                       /* Baud rate 125Kbps */
    MSCAN_Baudrate_250K,                       /* Baud rate 250Kbps */
    MSCAN_Baudrate_500K,                       /* Baud rate 500Kbps */
    MSCAN_Baudrate_1M,                         /* Baud rate 1Mbps */
}MSCAN_BaudRateTypeDef;



/* MSCAN module enumeration */
typedef enum 
{
    MSCAN_Channel0,                            /* MSCAN0 */
    MSCAN_Channel1,                            /* MSCAN1 */
    MSCAN_Channel4,                            /* MSCAN4 */
}MSCAN_ChannelTypeDef;



/* MSCAN signal pins remap enumeration */
typedef enum
{
    MSCAN0_PM0_PM1,                            /* PM0=CAN0RX, PM1=CAN0TX */
    MSCAN0_PM2_PM3,                            /* PM2=CAN0RX, PM3=CAN0TX */
    MSCAN0_PM4_PM5,                            /* PM4=CAN0RX, PM5=CAN0TX */
    MSCAN0_PJ6_PJ7,                            /* PJ6=CAN0RX, PJ7=CAN0TX */
    MSCAN1_PM2_PM3,                            /* PM2=CAN1RX, PM3=CAN1TX */
    MSCAN4_PJ6_PJ7,                            /* PJ6=CAN4RX, PJ7=CAN4TX */
    MSCAN4_PM4_PM5,                            /* PM4=CAN4RX, PM5=CAN4TX */
    MSCAN4_PM6_PM7,                            /* PM6=CAN4RX, PM7=CAN4TX */
}MSCAN_PinsRemapTypeDef;



/* MSCAN module channel and pins remap declaration */
typedef struct
{
    MSCAN_ChannelTypeDef ch;                   /* MSCAN module number selection. */
    MSCAN_PinsRemapTypeDef pins;               /* MSCAN pins remap selection. */
}MSCAN_ModuleConfig;



/* MSCAN module properties parameter declaration */
typedef struct
{
    uint8_t MSCAN_StopInWaitMode          :1;  /* 0:MSCAN module is normal in wait mode; 1:MSCAN module is stop in wait mode. */
    uint8_t MSCAN_TimeStampEnable         :1;  /* 0:Disable internal MSCAN timer; 1:Enable internal MSCAN timer. */
    uint8_t MSCAN_WakeUpEnable            :1;  /* 0:Wake-up disabled; 1:Wake-up enabled. */
    uint8_t MSCAN_ModuleEnable            :1;  /* 0:MSCAN module is disabled; 1:MSCAN module is enabled. */
    uint8_t MSCAN_ClockSource             :1;  /* 0:MSCAN clock source is the oscillator clock; 1:MSCAN clock is the bus clock. */
    uint8_t MSCAN_LoopbackMode            :1;  /* 0:Loop back self test disabled. 1:Loop back self test enabled. */
    uint8_t MSCAN_ListenOnlyMode          :1;  /* 0:Normal operation;1:Listen only mode. */
    uint8_t MSCAN_BusoffRecoveryMode      :1;  /* 0:Automatic bus-off recovery; 1:bus-off recovery upon user request. */
    uint8_t MSCAN_WakeUpMode              :1;  /* 0:Wakes up on any dominant level on CAN bus;1:Wakes up only in case of a dominant pulse on the CAN bus that has a delay time. */
    uint8_t MSCAN_WakeUpINTEnable         :1;  /* 0:Wake-up interrupt disable; 1:Wake-up interrupt enable. */
    uint8_t MSCAN_StatusChangeINTEnable   :1;  /* 0:CAN status change interrupt disable; 1:CAN status change interrupt enable. */
    uint8_t MSCAN_OverrunINTEnable        :1;  /* 0:Overrun interrupt disable; 1:Overrun interrupt enable. */
    uint8_t MSCAN_ReceiveFullINTEnable    :1;  /* 0:Receive full interrupt disable; 1: Receive full interrupt enable. */
    uint8_t MSCAN_Trans0EmptyINTEnable    :1;  /* 0:Transmitter0 empty interrupt disable; 1:Transmitter0 empty interrupt enable. */
    uint8_t MSCAN_Trans1EmptyINTEnable    :1;  /* 0:Transmitter1 empty interrupt disable; 1:Transmitter1 empty interrupt enable. */
    uint8_t MSCAN_Trans2EmptyINTEnable    :1;  /* 0:Transmitter2 empty interrupt disable; 1:Transmitter2 empty interrupt enable. */
	MSCAN_BaudRateTypeDef  baudrate;           /* MSCAN baud rate. */
}MSCAN_ParametersConfig;



/* CAN Frame type and ID format enumeration */
typedef enum
{
    DataFrameWithStandardId = 1,                
    RemoteFrameWithStandardId,
    DataFrameWithExtendedId,
    RemoteFrameWithExtendedId,
}MSCAN_FrameAndIDTypeDef;



/* CAN message format declaration */
typedef struct 
{
    MSCAN_FrameAndIDTypeDef frametype;			/* CAN frame type and ID format */
    uint16_t  data[8];							/* 8 bytes data */
    uint16_t  data_length;						/* CAN frame data length */
    uint32_t frame_id;							/* CAN frame ID value */
}MSCAN_MessageTypeDef;



/* CAN filters accept ID format enumeration */ 
typedef enum
{
    OnlyAcceptStandardID,
    OnlyAcceptExtendedID,
}AcceptIDFormat;



/* CAN filters accept frame type enumeration */
typedef enum
{
    OnlyAcceptRemoteFrame,
    OnlyAcceptDataFrame,
    AcceptBothFrame,
}AcceptFrameType;



/* CAN filters parameters declaration */
typedef struct
{
    uint8_t Filter_Enable    :1;   /* 0,filter closed; 1,filter opened. If user close CAN filter,the following parameters can't work. */
    uint8_t filter_channel   :1;   /* Filter channel selection.0: channel0; 1: channel1; */
    AcceptIDFormat id_format;	   /* CAN filters accept ID format. */
    AcceptFrameType frame_type;	   /* CAN filters accept frame type. */
    uint32_t id_mask;              /* This variable is mask received ID value. This variable value should be inverted of id_accept value
                                      if user assign the special received ID value.If user set this variable to 0xFFFFFFFF,it means that
                                      the filter identifier mask register can't work. */    
    uint32_t id_accept;            /* This variable is identifier acceptance received ID value,user can assign it */
}MSCAN_FilterConfig;



#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions ------------------------------------------------------- */

/* Initialize the MSCAN module with a given parameters by users. */
int16_t MSCAN_Init(MSCAN_ModuleConfig* CANx, MSCAN_ParametersConfig* Para_Config, MSCAN_FilterConfig* Filter_Config);


/* MSCAN send a frame by a chosen CAN module. */
int16_t MSCAN_SendFrame(MSCAN_ModuleConfig* CANx, MSCAN_MessageTypeDef* W_Framebuff);


/* Checking the specified CAN module whether have enough hard transmission buffer to send CAN messages. */
int16_t MSCAN_HardTxBufferCheck(MSCAN_ChannelTypeDef CANx);


/* MSCAN receive a frame by a chosen CAN module. */
//int16_t MSCAN_ReceiveFrame(MSCAN_ModuleConfig* CANx, MSCAN_MessageTypeDef* R_Framebuff);



#ifdef __cplusplus
}
#endif

#endif

/*****************************END OF FILE**************************************/

