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
  
#include "System_Driver.h"
  

/* Define a global variable which indicate time delaying */
static volatile uint32_t g_TimingDelay = 0;


  
  
 /**
 * @brief   Configure MC9S12X MCU system bus clock and PLL clock.
 * @param   Bus_Clk, system bus clock frequency.
 *          Users can configure bus clock frequency to be 16MHz,20MHz,32MHz,40MHz,
 *          48MHz and 60MHz.
 * @returns 0: Calling succeeded.
 * 			-1: Calling failed.
 */
int16_t SystemClock_Init(BusClockFrequency_TypeDef Bus_Clk)
{
	/* First, judge whether the specified bus clock is invalid.
	 If it is not,just return error value. */
	if ((Bus_Clk < BusClock_16MHz) || (Bus_Clk > BusClock_60MHz))return -1;

	/* Disable total interrupt */
	DisableInterrupts;

	/* Clear CLKSEL register PLLSEL bit to deselect PLLCLK clock as system clock */
	CLKSEL = 0x00u;

	/* Turn off IPLL first */
	PLLCTL &= 0x81u;

	/* Disable RTI,LOCK,SCM interrupt */
	CRGINT = 0x00u;

	/* PLLCLK = 2 ¡Á OSCCLK ¡Á (SYNR + 1)/(REFDV + 1) */
	switch (Bus_Clk)
	{
	  case BusClock_16MHz:
	  {
		  SYNR = 0x00 | 0x07;
	  }
	  break;
	  
	  case BusClock_20MHz:
	  {
		  SYNR = 0x00 | 0x09;
	  }
	  break;
	  
	  case BusClock_32MHz:
	  {
		  SYNR = 0x40 | 0x0F;
	  }
	  break;
	  
	  case BusClock_40MHz:
	  {
		  SYNR = 0x40 | 0x13;
	  }
	  break;
	  
	  case BusClock_48MHz:
	  {
		  SYNR = 0xC0 | 0x17;
	  }
	  break;
	  
	  case BusClock_60MHz:
	  {
		  SYNR = 0xC0 | 0x1D;
	  }
	  break;
	  
	  default:break;
	}

	REFDV = 0x07;

	/* Turn on IPLL */
	PLLCTL_PLLON = 1;

	_asm(nop);

	_asm(nop);

	/* Wait PLLCLK lock bit is set */
	while (!(CRGFLG & 0x08));

	POSTDIV = 0x00;

	/* Select PLLCLK as system clock */
	CLKSEL |= 0x80u;  

	/* Disable Watch dog module */
	COPCTL = 0x00u;

	/* Enable total interrupt */
	EnableInterrupts;

	return 0;
}



 /**
 * @brief   Configure watch dog over flow cycle with different value.
 * @param   Cycle: User specified over flow cycle value.
 * @returns 0: Calling succeeded.
 * 			-1: Calling failed.
 */
int16_t SystemWatchDog_Init(WatchDogOverFlowCycle_TypeDef Cycle)
{
	/* Judge whether the specified parameter is invalid */
	if ((Cycle < WatchDog_Cycle_1024us) || (Cycle > WatchDog_Cycle_1048576us))return -1;

	/* With different kind of over flow cycle,configure the COPCTL register with different value */
	switch (Cycle)
	{
	  case WatchDog_Cycle_1024us: COPCTL = 0x01u;break;
	  
	  case WatchDog_Cycle_4096us: COPCTL = 0x02u;break;
	  
	  case WatchDog_Cycle_8192us: COPCTL = 0x03u;break;
	  
	  case WatchDog_Cycle_65536us: COPCTL = 0x04u;break;
	  
	  case WatchDog_Cycle_262144us: COPCTL = 0x05u;break;
	  
	  case WatchDog_Cycle_524288us: COPCTL = 0x06u;break;
	  
	  case WatchDog_Cycle_1048576us: COPCTL = 0x07u;break;
	  
	  default:break;
	}

	return 0;
}



 /**
 * @brief   Configure Real Time Interrupt Module with a given over flow cycle time.
 * @param   Cycle: User specified over flow cycle time value.
 * @attention  User can configure the RTI timer with 1ms,10ms,100ms over flow cycle.
 * @returns 0: Calling succeeded.
 * 			-1: Calling failed.
 */
int16_t SystemRTI_Init(RTIOverFlowCycle_TypeDef Cycle)
{
	if ((Cycle < RTI_Cycle_1ms) || (Cycle > RTI_Cycle_100ms))return -1;
	
	/* First,disable Real Time Interrupt Enable Bit */
	CRGINT &= 0x7Fu;
	
	/* Select decimal based divider value for RTI */
	RTICTL_RTDEC = 1;
	
	switch (Cycle)
	{
		case RTI_Cycle_1ms:
		{
			RTICTL &= 0x80u;		/* Clear the previous divider value */
			RTICTL |= 0x8Fu;		/* Set the proper divider value with the corresponding cycle */
		}
		break;
		
		case RTI_Cycle_10ms:
		{
			RTICTL &= 0x80u;		/* Clear the previous divider value */
			RTICTL |= 0xBFu;		/* Set the proper divider value with the corresponding cycle */
		}
		break;
		
		case RTI_Cycle_100ms:
		{
			RTICTL &= 0x80u;		/* Clear the previous divider value */
			RTICTL |= 0xF8u;        /* Set the proper divider value with the corresponding cycle */
		}
		break;
		
		default:break;
	}
	
	/* Clear RTI time out interrupt flag */
	CRGFLG_RTIF = 1;
	
	/* Enable RTI interrupt */
	CRGINT |= 0x80u;
	
	return 0;
}  



 /**
 * @brief   Global time delay variable decrement function.  
 * @param   None
 * @returns None
 */
void TimeDelay_Decrement(void)
{
	if (g_TimingDelay != 0)
	{
		g_TimingDelay--;
	}
}



 /**
 * @brief   Delay functions.
 * @param   Cycle: User specified over flow cycle time value.
 * @attention If user wants to use the following three delay functions,user
 *            must add RTI_ISR function to your project!
 * @returns None
 */
#ifdef _1MS_PERTICKS
void Delay1ms(volatile uint32_t nTime)
{
	g_TimingDelay = nTime;
	
	while (g_TimingDelay != 0);
}
#endif

#ifdef  _10MS_PERTICKS
void Delay10ms(volatile uint32_t nTime)
{
	g_TimingDelay = nTime;
	
	while (g_TimingDelay != 0);
}
#endif

#ifdef _100MS_PERTICKS
void Delay100ms(volatile uint32_t nTime)
{
	g_TimingDelay = nTime;
	
	while (g_TimingDelay != 0);
}
#endif

/*****************************END OF FILE**************************************/
  