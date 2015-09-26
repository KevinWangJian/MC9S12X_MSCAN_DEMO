/**
  ******************************************************************************
  * @Copyright (C), 1997-2015, Hangzhou Gold Electronic Equipment Co., Ltd.
  * @file name: System_Driver.c
  * @author: Wangjian
  * @Descriptiuon: Provides a set of functions about system clock initialization,
  *                watch dog initialization and system real time interrupt initialization.
  *                Actually,the RTI is system tick for users.Users can use it for RTOS and
  *                delay functions.
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
  
  
#ifndef  __SYSTEM_DRIVER_H
#define  __SYSTEM_DRIVER_H


#include <MC9S12XEQ512.h>
#include <hidef.h>
#include "derivative.h"

#include "common.h"


/* Exported types ------------------------------------------------------------*/

/* Declaration System clock driver version */
#define   SYSTEM_DRIVER_VERSION     (100)		/* Rev1.0.0 */


/* Time ticks macro which can chose different delay function */
//#define   _1MS_PERTICKS
#define   _10MS_PERTICKS
//#define   _100MS_PERTICKS



/* System bus clock enumeration */
typedef enum
{
	BusClock_16MHz = 0,
	BusClock_20MHz,
	BusClock_32MHz,
	BusClock_40MHz,
	BusClock_48MHz,
	BusClock_60MHz,
}BusClockFrequency_TypeDef;



/* System watch dog timer over flow cycle enumeration */
typedef enum
{
	WatchDog_Cycle_1024us = 0,				/* 1.024ms */
	WatchDog_Cycle_4096us,					/* 4.096ms */
	WatchDog_Cycle_8192us,					/* 8.192ms */
	WatchDog_Cycle_65536us,					/* 65.536ms */
	WatchDog_Cycle_262144us,				/* 262.144ms */
	WatchDog_Cycle_524288us,				/* 524.288ms */
	WatchDog_Cycle_1048576us,				/* 1.048576s */
}WatchDogOverFlowCycle_TypeDef;



/* RTI timer over flow cycle enumeration */
typedef enum
{
	RTI_Cycle_1ms = 0,
	RTI_Cycle_10ms,
	RTI_Cycle_100ms,
}RTIOverFlowCycle_TypeDef;



#ifdef __cplusplus
extern "C" {
#endif


/* Exported functions ------------------------------------------------------- */


int16_t SystemClock_Init(BusClockFrequency_TypeDef Bus_Clk);


int16_t SystemWatchDog_Init(WatchDogOverFlowCycle_TypeDef Cycle);


int16_t SystemRTI_Init(RTIOverFlowCycle_TypeDef Cycle);


void TimeDelay_Decrement(void);


void Delay1ms(volatile uint32_t nTime);

void Delay10ms(volatile uint32_t nTime);

void Delay100ms(volatile uint32_t nTime);



#ifdef __cplusplus
}
#endif

#endif

/*****************************END OF FILE**************************************/

