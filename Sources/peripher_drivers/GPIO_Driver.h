/**
  ******************************************************************************
  * @Copyright (C), 1997-2015, Hangzhou Gold Electronic Equipment Co., Ltd.
  * @file name: GPIO_Driver.h
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
  
#ifndef  __GPIO_DRIVER_H
#define  __GPIO_DRIVER_H


#include <MC9S12XEQ512.h>
#include <hidef.h>
#include "derivative.h"

#include "common.h"


/* Exported types ------------------------------------------------------------*/

typedef enum
{
	GPIOA = 0,
	GPIOB,
	GPIOE,
	GPIOH,
	GPIOJ,
	GPIOK,
	GPIOM,
	GPIOP,
	GPIOS,
	GPIOT,
}GPIOPorts_TypeDef;


typedef enum
{
	GPIO_Output = 0,
	GPIO_Input,
}GPIODirection_TypeDef;


typedef enum
{
	GPIO_Pin0 = 0x01,
	GPIO_Pin1 = 0x02,
	GPIO_Pin2 = 0x04,
	GPIO_Pin3 = 0x08,
	GPIO_Pin4 = 0x10,
	GPIO_Pin5 = 0x20,
	GPIO_Pin6 = 0x40,
	GPIO_Pin7 = 0x80,
}GPIOPinNum_TypeDef;


#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions ------------------------------------------------------- */

void GPIO_Init(GPIOPorts_TypeDef port, GPIOPinNum_TypeDef pin_num, GPIODirection_TypeDef mode);


void GPIO_SetBit(GPIOPorts_TypeDef port, GPIOPinNum_TypeDef pin_num);


void GPIO_ClearBit(GPIOPorts_TypeDef port, GPIOPinNum_TypeDef pin_num);


void GPIO_ToggleBit(GPIOPorts_TypeDef port, GPIOPinNum_TypeDef pin_num);




#ifdef __cplusplus
}
#endif

#endif

/*****************************END OF FILE**************************************/