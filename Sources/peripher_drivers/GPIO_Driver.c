/**
  ******************************************************************************
  * @Copyright (C), 1997-2015, Hangzhou Gold Electronic Equipment Co., Ltd.
  * @file name: GPIO_Driver.c
  * @author: Wangjian
  * @Descriptiuon: Provide a set of functions about initialize GPIO ports,
  *                Setting, clearing and toggling the specified GPIO pins.
  * @Others: None
  * @History: 1. Created by Wangjian.
  * @version: V1.0.0
  * @date:    18-Sep-2015

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

/* Includes ------------------------------------------------------------------*/
#include "GPIO_Driver.h"




/**
 * @brief   Initialzie GPIO ports with a given properties.
 * @param   port: The specified GPIO port number.
 * 		    pin_num: The specified GPIO pin number.
 *          mode: The specified work mode.It can be GPIO_Input or GPIO_Output.
 * @returns None.
 */
void GPIO_Init(GPIOPorts_TypeDef port, GPIOPinNum_TypeDef pin_num, GPIODirection_TypeDef mode)
{
	switch (port)
	{
		case GPIOA:
		{
			if (mode == GPIO_Output)					/* Configure the GPIO pin as general output mode */
			{
				DDRA |= pin_num;
			}
			else										/* Configure the GPIO pin as high-impedance input mode */
			{
				DDRA &= ~pin_num;
			}
		}
		break;
		
		case GPIOB:
		{
			if (mode == GPIO_Output)					/* Configure the GPIO pin as general output mode */
			{
				DDRB |= pin_num;
			}
			else										/* Configure the GPIO pin as high-impedance input mode */
			{
				DDRB &= ~pin_num;
			}
		}
		break;
		
		case GPIOE:
		{
			if (mode == GPIO_Output)					/* Configure the GPIO pin as general output mode */
			{
				DDRE |= pin_num;
			}
			else										/* Configure the GPIO pin as high-impedance input mode */
			{
				DDRE &= ~pin_num;
			}
		}
		break;
		
		case GPIOH:
		{
			if (mode == GPIO_Output)					/* Configure the GPIO pin as general output mode */
			{
				RDRH &= ~pin_num;						/* Full drive strength enabled */
				DDRH |= pin_num;
			}
			else										/* Configure the GPIO pin as input mode */
			{
				DDRH &= ~pin_num;
				PERH |= pin_num;						/* Pull device enabled */
				PPSH &= ~pin_num;                       /* A pull-up device is connected to the associated Port H pin */
			}
		}
		break;
		
		case GPIOJ:
		{
			if (mode == GPIO_Output)
			{
				RDRJ &= ~pin_num;
				DDRJ |= pin_num;
			}
			else
			{
				DDRJ &= ~pin_num;
				PERJ |= pin_num;
				PPSJ &= ~pin_num;
			}
		}
		break;
		
		case GPIOK:
		{
			if (mode == GPIO_Output)
			{
				DDRK |= pin_num;
			}
			else
			{
				DDRK &= ~pin_num;
			}
		}
		break;
		
		case GPIOM:
		{
			if (mode == GPIO_Output)
			{
				RDRM &= ~pin_num;
				DDRM |= pin_num;
			}
			else
			{
				DDRM &= ~pin_num;
				PERM |= pin_num;
				PPSM &= ~pin_num;
			}
		}
		break;
		
		case GPIOP:
		{
			if (mode == GPIO_Output)
			{
				RDRP &= ~pin_num;
				DDRP |= pin_num;
			}
			else
			{
				DDRP &= ~pin_num;
				PERP |= pin_num;
				PPSP &= ~pin_num;
			}
		}
		break;
		
		case GPIOS:
		{
			if (mode == GPIO_Output)
			{
				RDRS &= ~pin_num;
				DDRS |= pin_num;
			}
			else
			{
				DDRS &= ~pin_num;
				PERS |= pin_num;
				PPSS &= ~pin_num;
			}
		}
		break;
		
		case GPIOT:
		{
			if (mode == GPIO_Output)
			{
				RDRT &= ~pin_num;
				DDRT |= pin_num;
			}
			else
			{
				DDRT &= ~pin_num;
				PERT |= pin_num;
				PPST &= ~pin_num;
			}
		}
		break;
		
		default:break;
	}
}




/**
 * @brief   Setting the specified GPIO pins.
 * @param   port: The specified GPIO port number.
 * 		    pin_num: The specified GPIO pin number.
 * @returns None.
 */
void GPIO_SetBit(GPIOPorts_TypeDef port, GPIOPinNum_TypeDef pin_num)
{
	switch (port)
	{
		case GPIOA: PORTA |= pin_num;break;
		case GPIOB: PORTB |= pin_num;break;
		case GPIOE: PORTE |= pin_num;break;
		case GPIOH: PTH   |= pin_num;break;
		case GPIOJ: PTJ   |= pin_num;break;
		case GPIOK: PORTK |= pin_num;break;
		case GPIOM: PTM	  |= pin_num;break;
		case GPIOP: PTP   |= pin_num;break;
		case GPIOS: PTS   |= pin_num;break;
		case GPIOT: PTT   |= pin_num;break;
		default:break;
	}
}




/**
 * @brief   Clearing the specified GPIO pins.
 * @param   port: The specified GPIO port number.
 * 		    pin_num: The specified GPIO pin number.
 * @returns None.
 */
void GPIO_ClearBit(GPIOPorts_TypeDef port, GPIOPinNum_TypeDef pin_num)
{
	switch (port)
	{
		case GPIOA: PORTA &= ~pin_num;break;
		case GPIOB: PORTB &= ~pin_num;break;
		case GPIOE: PORTE &= ~pin_num;break;
		case GPIOH: PTH   &= ~pin_num;break;
		case GPIOJ: PTJ   &= ~pin_num;break;
		case GPIOK: PORTK &= ~pin_num;break;
		case GPIOM: PTM	  &= ~pin_num;break;
		case GPIOP: PTP   &= ~pin_num;break;
		case GPIOS: PTS   &= ~pin_num;break;
		case GPIOT: PTT   &= ~pin_num;break;
		default:break;
	}	
}




/**
 * @brief   Toggling the specified GPIO pins.
 * @param   port: The specified GPIO port number.
 * 		    pin_num: The specified GPIO pin number.
 * @returns None.
 */
void GPIO_ToggleBit(GPIOPorts_TypeDef port, GPIOPinNum_TypeDef pin_num)
{
	switch (port)
	{
		case GPIOA: 
		{
			if (PORTA & pin_num)PORTA &= ~pin_num;
			else PORTA |= pin_num;
		}
		break;
		
		case GPIOB: 
		{
			if (PORTB & pin_num)PORTB &= ~pin_num;
			else PORTB |= pin_num;
		}
		break;
		
		case GPIOE: 
		{
			if (PORTE & pin_num)PORTB &= ~pin_num;
			else PORTB |= pin_num;
		}
		break;
		
		case GPIOH: 
		{
			if (PTH & pin_num)PTH &= ~pin_num;
			else PTH |= pin_num;
		}
		break;
		
		case GPIOJ: 
		{
			if (PTJ & pin_num)PTJ &= ~pin_num;
			else PTJ |= pin_num;
		}
		break;
		
		case GPIOK: 
		{
			if (PORTK & pin_num)PORTK &= ~pin_num;
			else PORTK |= pin_num;
		}
		break;
		
		case GPIOM: 
		{
			if (PTM & pin_num)PTM &= ~pin_num;
			else PTM |= pin_num;
		}
		break;
		
		case GPIOP: 
		{
			if (PTP & pin_num)PTP &= ~pin_num;
			else PTP |= pin_num;
		}
		break;
		
		case GPIOS:
		{
			if (PTS & pin_num)PTS &= ~pin_num;
			else PTS |= pin_num;
		}
		break;
		
		case GPIOT:
		{
			if (PTT & pin_num)PTT &= ~pin_num;
			else PTT |= pin_num;
		}
		break;
		
		default:break;
	}	
}

/*****************************END OF FILE**************************************/
