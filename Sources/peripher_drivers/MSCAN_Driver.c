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

/* Includes ------------------------------------------------------------------*/

#include "MSCAN_Driver.h"




/**
 * @brief   Configure the filters about the MSCAN module when receiving CAN frames.
 * &attention  The CAN module is configured to two 32-bit acceptance filters in default.
 * @param   CANx: The pointer which point to MSCAN module number and pins.
 * 			frame_config: The buffer which point to MSCAN module acceptance parameters when receiving CAN frames.
 * @returns 0: Calling succeeded.
 * 			-1: Calling failed.
 */
static int16_t MSCAN_ConfigIDFilter(MSCAN_ModuleConfig* CANx, MSCAN_FilterConfig* CANFilter_Config)
{
    if ((CANx != NULL) && (CANFilter_Config != NULL)) 
    {   
        if (CANx->ch == MSCAN_Channel0) 
        {
            /* Judge whether CAN module filter is enabled or disabled */
            if (CANFilter_Config->Filter_Enable == 0)  
            {
                CAN0IDMR0 = 0xFF;
                CAN0IDMR1 = 0xFF;
                CAN0IDMR2 = 0xFF;
                CAN0IDMR3 = 0xFF;
                CAN0IDMR4 = 0xFF;
                CAN0IDMR5 = 0xFF;
                CAN0IDMR6 = 0xFF;
                CAN0IDMR7 = 0xFF;           /* If CAN filter is disabled,set CAN identifier mask register */
                
                return 0;
            } 
            else 
            {
                CAN0IDAC_IDAM0 = 0;
                CAN0IDAC_IDAM1 = 0;         /* Configure CAN filters as Two 32-bit acceptance filters */
            }
          
            if (CANFilter_Config->filter_channel == 0)     /* Select group 0 filters. */
            {
                /* First,check the received frame ID type. */
                /* Check whether filters only receive standard ID. */ 
                if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                {
                    CAN0IDMR0 = (uint8_t)(CANFilter_Config->id_mask >> 3);
                    CAN0IDAR0 = (uint8_t)(CANFilter_Config->id_accept >> 3);
                    
                    CAN0IDMR1 = (uint8_t)(CANFilter_Config->id_mask) << 5;
                    CAN0IDAR1 = (uint8_t)(CANFilter_Config->id_accept) << 5;
                }
                /* Check whether filters only receive extended ID. */  
                else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                {
                    CAN0IDMR0 = (uint8_t)(CANFilter_Config->id_mask >> 21);
                    CAN0IDAR0 = (uint8_t)(CANFilter_Config->id_accept >> 21);
                    
                    CAN0IDMR1 = ((uint8_t)(CANFilter_Config->id_mask >> 18) << 5)
                                |((uint8_t)(CANFilter_Config->id_mask >> 15) & 0x07);
                    CAN0IDAR1 = ((uint8_t)(CANFilter_Config->id_accept >> 18) << 5) 
                                |((uint8_t)(CANFilter_Config->id_accept >> 15) & 0x07);
                    CAN0IDAR1 |= ((1 << 4) | (1 << 3));  /* Extended id format */          
                                
                    CAN0IDMR2 = (uint8_t)(CANFilter_Config->id_mask >> 7);
                    CAN0IDAR2 = (uint8_t)(CANFilter_Config->id_accept >> 7);
                    
                    CAN0IDMR3 = (uint8_t)(CANFilter_Config->id_mask & 0x7Fu) << 1;
                    CAN0IDAR3 = (uint8_t)(CANFilter_Config->id_accept & 0x7Fu) << 1;
                } 
                else 
                {
                    return -1;   /* The frame id type is wrong,return error. */
                }
                
                /* Second,check the received frame type. */
                /* Check whether filtes only receive remote frame. */
                if (CANFilter_Config->frame_type == OnlyAcceptRemoteFrame) 
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN0IDAR1 |= (1 << 4);
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN0IDAR3 |= (1 << 0);
                    } 
					else
					{
						return -1; /* The frame id type is wrong,return error. */
                    }
                }
                /* Check whether filters only receive data frame. */
                else if (CANFilter_Config->frame_type == OnlyAcceptDataFrame)
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN0IDAR1 &= ~(1 << 4);   
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN0IDAR3 &= ~(1 << 0);
                    }
					else
					{
						return -1; /* The frame id type is wrong,return error. */
                    }
                }
                /* Check whether filters receive both remote and data frame. */
                else if (CANFilter_Config->frame_type == AcceptBothFrame)
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN0IDMR1 |= (1 << 4);
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN0IDMR3 |= (1 << 0);
                    }
					else
					{
						return -1; /* The frame id type is wrong,return error. */
                    }
                } 
				else
				{
					return -1;     /* The frame type is wrong,return error. */
                }
                
                return 0;
            } 
            else                                       /* Select group 1 filters. */
            {
                /* First,check the received frame ID type. */
                /* Check whether filters only receive standard ID. */ 
                if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                {
                    CAN0IDMR4 = (uint8_t)(CANFilter_Config->id_mask >> 3);
                    CAN0IDAR4 = (uint8_t)(CANFilter_Config->id_accept >> 3);
                    
                    CAN0IDMR5 = (uint8_t)(CANFilter_Config->id_mask) << 5;
                    CAN0IDAR5 = (uint8_t)(CANFilter_Config->id_accept) << 5;
                }
                /* Check whether filters only receive extended ID. */  
                else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                {
                    CAN0IDMR4 = (uint8_t)(CANFilter_Config->id_mask >> 21);
                    CAN0IDAR4 = (uint8_t)(CANFilter_Config->id_accept >> 21);
                    
                    CAN0IDMR5 = ((uint8_t)(CANFilter_Config->id_mask >> 18) << 5)
                                |((uint8_t)(CANFilter_Config->id_mask >> 15) & 0x07);
                    CAN0IDAR5 = ((uint8_t)(CANFilter_Config->id_accept >> 18) << 5) 
                                |((uint8_t)(CANFilter_Config->id_accept >> 15) & 0x07);
                    CAN0IDAR5 |= ((1 << 4) | (1 << 3));  /* Extended id format */          
                                
                    CAN0IDMR6 = (uint8_t)(CANFilter_Config->id_mask >> 7);
                    CAN0IDAR6 = (uint8_t)(CANFilter_Config->id_accept >> 7);
                    
                    CAN0IDMR7 = (uint8_t)(CANFilter_Config->id_mask & 0x7Fu) << 1;
                    CAN0IDAR7 = (uint8_t)(CANFilter_Config->id_accept & 0x7Fu) << 1;
                } else {return -1;
                }
                
                /* Second,check the received frame type. */
                /* Check whether filtes only receive remote frame. */
                if (CANFilter_Config->frame_type == OnlyAcceptRemoteFrame) 
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN0IDAR5 |= (1 << 4);
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN0IDAR7 |= (1 << 0);
                    } else{return -1; /* The frame id type is wrong,return error. */
                    }
                }
                /* Check whether filters only receive data frame. */
                else if (CANFilter_Config->frame_type == OnlyAcceptDataFrame)
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN0IDAR5 &= ~(1 << 4);   
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN0IDAR7 &= ~(1 << 0);
                    }else{return -1; /* The frame id type is wrong,return error. */
                    }
                }
                /* Check whether filters receive both remote and data frame. */
                else if (CANFilter_Config->frame_type == AcceptBothFrame)
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN0IDMR5 |= (1 << 4);
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN0IDMR7 |= (1 << 0);
                    }else{return -1; /* The frame id type is wrong,return error. */
                    }
                } else{return -1;
                }
                
                return 0;
            }
        } 
        else if (CANx->ch == MSCAN_Channel1) 
        {
            /* Judge whether CAN module filter is enabled or disabled */
            if (CANFilter_Config->Filter_Enable == 0)  
            {
                CAN1IDMR0 = 0xFF;
                CAN1IDMR1 = 0xFF;
                CAN1IDMR2 = 0xFF;
                CAN1IDMR3 = 0xFF;
                CAN1IDMR4 = 0xFF;
                CAN1IDMR5 = 0xFF;
                CAN1IDMR6 = 0xFF;
                CAN1IDMR7 = 0xFF;
                
                return 0;
            } 
            else 
            {
                CAN1IDAC_IDAM0 = 0;
                CAN1IDAC_IDAM1 = 0;         /* Two 32-bit acceptance filters */
            }
          
            if (CANFilter_Config->filter_channel == 0)     /* Select group 0 filters. */
            {
                /* First,check the received frame ID type. */
                /* Check whether filters only receive standard ID. */ 
                if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                {
                    CAN1IDMR0 = (uint8_t)(CANFilter_Config->id_mask >> 3);
                    CAN1IDAR0 = (uint8_t)(CANFilter_Config->id_accept >> 3);
                    
                    CAN1IDMR1 = (uint8_t)(CANFilter_Config->id_mask) << 5;
                    CAN1IDAR1 = (uint8_t)(CANFilter_Config->id_accept) << 5;
                }
                /* Check whether filters only receive extended ID. */  
                else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                {
                    CAN1IDMR0 = (uint8_t)(CANFilter_Config->id_mask >> 21);
                    CAN1IDAR0 = (uint8_t)(CANFilter_Config->id_accept >> 21);
                    
                    CAN1IDMR1 = ((uint8_t)(CANFilter_Config->id_mask >> 18) << 5)
                                |((uint8_t)(CANFilter_Config->id_mask >> 15) & 0x07);
                    CAN1IDAR1 = ((uint8_t)(CANFilter_Config->id_accept >> 18) << 5) 
                                |((uint8_t)(CANFilter_Config->id_accept >> 15) & 0x07);
                    CAN1IDAR1 |= ((1 << 4) | (1 << 3));  /* Extended id format */          
                                
                    CAN1IDMR2 = (uint8_t)(CANFilter_Config->id_mask >> 7);
                    CAN1IDAR2 = (uint8_t)(CANFilter_Config->id_accept >> 7);
                    
                    CAN1IDMR3 = (uint8_t)(CANFilter_Config->id_mask & 0x7Fu) << 1;
                    CAN1IDAR3 = (uint8_t)(CANFilter_Config->id_accept & 0x7Fu) << 1;
                } 
                else 
                {
                    return -1;   /* The frame id type is wrong,return error. */
                }
                
                /* Second,check the received frame type. */
                /* Check whether filtes only receive remote frame. */
                if (CANFilter_Config->frame_type == OnlyAcceptRemoteFrame) 
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN1IDAR1 |= (1 << 4);
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN1IDAR3 |= (1 << 0);
                    } else{return -1; /* The frame id type is wrong,return error. */
                    }
                }
                /* Check whether filters only receive data frame. */
                else if (CANFilter_Config->frame_type == OnlyAcceptDataFrame)
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN1IDAR1 &= ~(1 << 4);   
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN1IDAR3 &= ~(1 << 0);
                    }else{return -1; /* The frame id type is wrong,return error. */
                    }
                }
                /* Check whether filters receive both remote and data frame. */
                else if (CANFilter_Config->frame_type == AcceptBothFrame)
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN1IDMR1 |= (1 << 4);
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN1IDMR3 |= (1 << 0);
                    }else{return -1; /* The frame id type is wrong,return error. */
                    }
                } else{return -1;     /* The frame type is wrong,return error. */
                }
                
                return 0;
            } 
            else                                       /* Select group 1 filters. */
            {
                /* First,check the received frame ID type. */
                /* Check whether filters only receive standard ID. */ 
                if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                {
                    CAN1IDMR4 = (uint8_t)(CANFilter_Config->id_mask >> 3);
                    CAN1IDAR4 = (uint8_t)(CANFilter_Config->id_accept >> 3);
                    
                    CAN1IDMR5 = (uint8_t)(CANFilter_Config->id_mask) << 5;
                    CAN1IDAR5 = (uint8_t)(CANFilter_Config->id_accept) << 5;
                }
                /* Check whether filters only receive extended ID. */  
                else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                {
                    CAN1IDMR4 = (uint8_t)(CANFilter_Config->id_mask >> 21);
                    CAN1IDAR4 = (uint8_t)(CANFilter_Config->id_accept >> 21);
                    
                    CAN1IDMR5 = ((uint8_t)(CANFilter_Config->id_mask >> 18) << 5)
                                |((uint8_t)(CANFilter_Config->id_mask >> 15) & 0x07);
                    CAN1IDAR5 = ((uint8_t)(CANFilter_Config->id_accept >> 18) << 5) 
                                |((uint8_t)(CANFilter_Config->id_accept >> 15) & 0x07);
                    CAN1IDAR5 |= ((1 << 4) | (1 << 3));  /* Extended id format */          
                                
                    CAN1IDMR6 = (uint8_t)(CANFilter_Config->id_mask >> 7);
                    CAN1IDAR6 = (uint8_t)(CANFilter_Config->id_accept >> 7);
                    
                    CAN1IDMR7 = (uint8_t)(CANFilter_Config->id_mask & 0x7Fu) << 1;
                    CAN1IDAR7 = (uint8_t)(CANFilter_Config->id_accept & 0x7Fu) << 1;
                } else {return -1;
                }
                
                /* Second,check the received frame type. */
                /* Check whether filtes only receive remote frame. */
                if (CANFilter_Config->frame_type == OnlyAcceptRemoteFrame) 
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN1IDAR5 |= (1 << 4);
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN1IDAR7 |= (1 << 0);
                    } else{return -1; /* The frame id type is wrong,return error. */
                    }
                }
                /* Check whether filters only receive data frame. */
                else if (CANFilter_Config->frame_type == OnlyAcceptDataFrame)
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN1IDAR5 &= ~(1 << 4);   
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN1IDAR7 &= ~(1 << 0);
                    }else{return -1; /* The frame id type is wrong,return error. */
                    }
                }
                /* Check whether filters receive both remote and data frame. */
                else if (CANFilter_Config->frame_type == AcceptBothFrame)
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN1IDMR5 |= (1 << 4);
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN1IDMR7 |= (1 << 0);
                    }else{return -1; /* The frame id type is wrong,return error. */
                    }
                } else{return -1;
                }
                
                return 0;
            }
        } 
        else if (CANx->ch == MSCAN_Channel4) 
        {
            /* Judge whether CAN module filter is enabled or disabled */
            if (CANFilter_Config->Filter_Enable == 0)  
            {
                CAN4IDMR0 = 0xFF;
                CAN4IDMR1 = 0xFF;
                CAN4IDMR2 = 0xFF;
                CAN4IDMR3 = 0xFF;
                CAN4IDMR4 = 0xFF;
                CAN4IDMR5 = 0xFF;
                CAN4IDMR6 = 0xFF;
                CAN4IDMR7 = 0xFF;
                
                return 0;
            } 
            else 
            {
                CAN4IDAC_IDAM0 = 0;
                CAN4IDAC_IDAM1 = 0;         /* Two 32-bit acceptance filters */
            }
          
            if (CANFilter_Config->filter_channel == 0)     /* Select group 0 filters. */
            {
                /* First,check the received frame ID type. */
                /* Check whether filters only receive standard ID. */ 
                if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                {
                    CAN4IDMR0 = (uint8_t)(CANFilter_Config->id_mask >> 3);
                    CAN4IDAR0 = (uint8_t)(CANFilter_Config->id_accept >> 3);
                    
                    CAN4IDMR1 = (uint8_t)(CANFilter_Config->id_mask) << 5;
                    CAN4IDAR1 = (uint8_t)(CANFilter_Config->id_accept) << 5;
                }
                /* Check whether filters only receive extended ID. */  
                else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                {
                    CAN4IDMR0 = (uint8_t)(CANFilter_Config->id_mask >> 21);
                    CAN4IDAR0 = (uint8_t)(CANFilter_Config->id_accept >> 21);
                    
                    CAN4IDMR1 = ((uint8_t)(CANFilter_Config->id_mask >> 18) << 5)
                                |((uint8_t)(CANFilter_Config->id_mask >> 15) & 0x07);
                    CAN4IDAR1 = ((uint8_t)(CANFilter_Config->id_accept >> 18) << 5) 
                                |((uint8_t)(CANFilter_Config->id_accept >> 15) & 0x07);
                    CAN4IDAR1 |= ((1 << 4) | (1 << 3));  /* Extended id format */          
                                
                    CAN4IDMR2 = (uint8_t)(CANFilter_Config->id_mask >> 7);
                    CAN4IDAR2 = (uint8_t)(CANFilter_Config->id_accept >> 7);
                    
                    CAN4IDMR3 = (uint8_t)(CANFilter_Config->id_mask & 0x7Fu) << 1;
                    CAN4IDAR3 = (uint8_t)(CANFilter_Config->id_accept & 0x7Fu) << 1;
                } 
                else 
                {
                    return -1;   /* The frame id type is wrong,return error. */
                }
                
                /* Second,check the received frame type. */
                /* Check whether filtes only receive remote frame. */
                if (CANFilter_Config->frame_type == OnlyAcceptRemoteFrame) 
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN4IDAR1 |= (1 << 4);
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN4IDAR3 |= (1 << 0);
                    } else{return -1; /* The frame id type is wrong,return error. */
                    }
                }
                /* Check whether filters only receive data frame. */
                else if (CANFilter_Config->frame_type == OnlyAcceptDataFrame)
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN4IDAR1 &= ~(1 << 4);   
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN4IDAR3 &= ~(1 << 0);
                    }else{return -1; /* The frame id type is wrong,return error. */
                    }
                }
                /* Check whether filters receive both remote and data frame. */
                else if (CANFilter_Config->frame_type == AcceptBothFrame)
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN4IDMR1 |= (1 << 4);
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN4IDMR3 |= (1 << 0);
                    }else{return -1; /* The frame id type is wrong,return error. */
                    }
                } else{return -1;     /* The frame type is wrong,return error. */
                }
                
                return 0;
            } 
            else                                       /* Select group 1 filters. */
            {
                /* First,check the received frame ID type. */
                /* Check whether filters only receive standard ID. */ 
                if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                {
                    CAN4IDMR4 = (uint8_t)(CANFilter_Config->id_mask >> 3);
                    CAN4IDAR4 = (uint8_t)(CANFilter_Config->id_accept >> 3);
                    
                    CAN4IDMR5 = (uint8_t)(CANFilter_Config->id_mask) << 5;
                    CAN4IDAR5 = (uint8_t)(CANFilter_Config->id_accept) << 5;
                }
                /* Check whether filters only receive extended ID. */  
                else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                {
                    CAN4IDMR4 = (uint8_t)(CANFilter_Config->id_mask >> 21);
                    CAN4IDAR4 = (uint8_t)(CANFilter_Config->id_accept >> 21);
                    
                    CAN4IDMR5 = ((uint8_t)(CANFilter_Config->id_mask >> 18) << 5)
                                |((uint8_t)(CANFilter_Config->id_mask >> 15) & 0x07);
                    CAN4IDAR5 = ((uint8_t)(CANFilter_Config->id_accept >> 18) << 5) 
                                |((uint8_t)(CANFilter_Config->id_accept >> 15) & 0x07);
                    CAN4IDAR5 |= ((1 << 4) | (1 << 3));  /* Extended id format */          
                                
                    CAN4IDMR6 = (uint8_t)(CANFilter_Config->id_mask >> 7);
                    CAN4IDAR6 = (uint8_t)(CANFilter_Config->id_accept >> 7);
                    
                    CAN4IDMR7 = (uint8_t)(CANFilter_Config->id_mask & 0x7Fu) << 1;
                    CAN4IDAR7 = (uint8_t)(CANFilter_Config->id_accept & 0x7Fu) << 1;
                } else {return -1;
                }
                
                /* Second,check the received frame type. */
                /* Check whether filtes only receive remote frame. */
                if (CANFilter_Config->frame_type == OnlyAcceptRemoteFrame) 
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN4IDAR5 |= (1 << 4);
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN4IDAR7 |= (1 << 0);
                    } else{return -1; /* The frame id type is wrong,return error. */
                    }
                }
                /* Check whether filters only receive data frame. */
                else if (CANFilter_Config->frame_type == OnlyAcceptDataFrame)
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN4IDAR5 &= ~(1 << 4);   
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN4IDAR7 &= ~(1 << 0);
                    }else{return -1; /* The frame id type is wrong,return error. */
                    }
                }
                /* Check whether filters receive both remote and data frame. */
                else if (CANFilter_Config->frame_type == AcceptBothFrame)
                {
                    if (CANFilter_Config->id_format == OnlyAcceptStandardID) 
                    {
                        CAN4IDMR5 |= (1 << 4);
                    } 
                    else if (CANFilter_Config->id_format == OnlyAcceptExtendedID) 
                    {
                        CAN4IDMR7 |= (1 << 0);
                    }else{return -1; /* The frame id type is wrong,return error. */
                    }
                } else{return -1;
                }
                
                return 0;
            }
        }
    }
	
    return -1;    
}



/**
 * @brief   Initialize the MSCAN module with a given parameters by users.
 * @param   CANx: The pointer which point to MSCAN module number and pins.
 *          CAN_Config: The pointer which point to the MSCAN module initialization features.
 * 			Acceptframe_config: The pointer whicha point to MSCAN module acceptance parameters when receiving CAN frames.
 *          baud: MSCAN module initialze baudrate.
 * @returns 0: Calling succeeded.
 * 			-1: Calling failed.
 */
int16_t MSCAN_Init(MSCAN_ModuleConfig* CANx, MSCAN_ParametersConfig* Para_Config, MSCAN_FilterConfig* Filter_Config)
{
    uint16_t Timeout_Count = 0;
    int ret;
    
    if ((CANx != NULL) && (Para_Config != NULL) && (Filter_Config != NULL)) 
    {
		if (CANx->ch == MSCAN_Channel0)            /* MSCAN0 */
		{       
			if (CANx->pins == MSCAN0_PM0_PM1) 
			{
				/* 
				Configure Module Routing Register(MODRR) and remap 
				TXCAN0 and RXCAN0 signals to PM1 and PM0. 
				*/
				MODRR_MODRR0 = 0;
				MODRR_MODRR1 = 0;
				
				DDRM_DDRM0 = 0;
				DDRM_DDRM1 = 1;
			} 
			else if (CANx->pins == MSCAN0_PM2_PM3)
			{
				/* Configure Module Routing Register(MODRR) and remap 
				   TXCAN0 and RXCAN0 signals to PM3 and PM2. */
				MODRR_MODRR0 = 1;
				MODRR_MODRR1 = 0;
				
				DDRM_DDRM2 = 0;
				DDRM_DDRM3 = 1;
			} 
			else if (CANx->pins == MSCAN0_PM4_PM5)
			{
				/* Configure Module Routing Register(MODRR) and remap 
				   TXCAN0 and RXCAN0 signals to PM5 and PM4. */
				MODRR_MODRR0 = 0;
				MODRR_MODRR1 = 1;
				
				DDRM_DDRM4 = 0;
				DDRM_DDRM5 = 1;
			} 
			else if (CANx->pins == MSCAN0_PJ6_PJ7)
			{
				/* Configure Module Routing Register(MODRR) and remap 
				   TXCAN0 and RXCAN0 signals to PJ7 and PJ6. */
				MODRR_MODRR0 = 1;
				MODRR_MODRR1 = 1;
				
				DDRJ_DDRJ6 = 0;
				DDRJ_DDRJ7 = 1;
			}
			
			/* Enable MSCAN0 module. */
			CAN0CTL1_CANE = Para_Config->MSCAN_ModuleEnable;
			
			/* Enable MSCAN wake-up function,this bit must be configured before sleep mode entry. */
			CAN0CTL0_WUPE = Para_Config->MSCAN_WakeUpEnable;
			
			/* First,make sure CAN0 module is in SLEEP mode and 
			   wait CAN BUS enter idle status. */ 
			CAN0CTL0_SLPRQ = 1;
			
			while (!CAN0CTL1_SLPAK) 
			{
				if (++Timeout_Count == 0xFFFFu)
				{
					return -1;
				}
			}
			
			Timeout_Count = 0;
			
			/* Then,CPU request CAN0 module exit SLEEP mode before enter INIT mode. */
			CAN0CTL0_SLPRQ = 0;
			
			while (CAN0CTL1_SLPAK) 
			{
				if (++Timeout_Count == 0xFFFFu) 
				{
					return -1;
				}
			}
			
			Timeout_Count = 0;
			
			/* Second,CPU request CAN0 module enter INIT mode by writing 
			   a 1 to the INITRQ bit in CAN0CTL0 register. */
			CAN0CTL0_INITRQ = 1;
			
			while (!CAN0CTL1_INITAK)
			{
				 if (++Timeout_Count == 0xFFFFu)
				 {
					return -1;
				 }
			}
			
			Timeout_Count = 0;
			
			CAN0CTL1_CLKSRC = Para_Config->MSCAN_ClockSource;
			CAN0CTL1_LOOPB  = Para_Config->MSCAN_LoopbackMode;
			CAN0CTL1_LISTEN = Para_Config->MSCAN_ListenOnlyMode;
			CAN0CTL1_BORM   = Para_Config->MSCAN_BusoffRecoveryMode;
			CAN0CTL1_WUPM   = Para_Config->MSCAN_WakeUpMode;
			
			/* Initialize the baud rate of MSCAN module.
			   Attention: The following setting MSCAN module baud rate program is only apply to
						  the CANCLK source frequency is 16MHz.If the CANCLK frequency is changed,
						  user should modify the corresponding configure registers. 
			*/
			switch (Para_Config->baudrate) 
			{
				case MSCAN_Baudrate_50K:{
				CAN0BTR0_SJW = 1;
				CAN0BTR1_SAMP = 1;
				CAN0BTR0_BRP = 19;
				CAN0BTR1_TSEG_10 = 9;
				CAN0BTR1_TSEG_20 = 4;   
				}break;
				
				case MSCAN_Baudrate_100K:{
				CAN0BTR0_SJW = 1;
				CAN0BTR1_SAMP = 1;
				CAN0BTR0_BRP = 9;
				CAN0BTR1_TSEG_10 = 9;
				CAN0BTR1_TSEG_20 = 4;
				}break;
				
				case MSCAN_Baudrate_125K: {
				CAN0BTR0_SJW = 1;
				CAN0BTR1_SAMP = 1;
				CAN0BTR0_BRP = 7;
				CAN0BTR1_TSEG_10 = 9;
				CAN0BTR1_TSEG_20 = 4;
				}break;
				
				case MSCAN_Baudrate_250K:{   
				CAN0BTR0_SJW = 1;
				CAN0BTR1_SAMP = 1;
				CAN0BTR0_BRP = 3;
				CAN0BTR1_TSEG_10 = 9;
				CAN0BTR1_TSEG_20 = 4;
				}break;
				
				case MSCAN_Baudrate_500K: {
				CAN0BTR0_SJW = 1;
				CAN0BTR1_SAMP = 1;
				CAN0BTR0_BRP = 1;
				CAN0BTR1_TSEG_10 = 9;
				CAN0BTR1_TSEG_20 = 4;
				}break;
				
				case MSCAN_Baudrate_1M:{
				CAN0BTR0_SJW = 1;
				CAN0BTR1_SAMP = 1;
				CAN0BTR0_BRP = 1;
				CAN0BTR1_TSEG_10 = 4;
				CAN0BTR1_TSEG_20 = 1;
				}break;
				
				default:break;
			}
			
			/* Configure the ID filters for receiving frame. */
			ret = MSCAN_ConfigIDFilter(CANx, Filter_Config);
			
			/* Finally, CPU request CAN0 module to exit INIT mode. */
			CAN0CTL0_INITRQ = 0;
			
			while (CAN0CTL1_INITAK)
			{
				if (++Timeout_Count == 0xFFFFu)
				{
					return -1;
				}
			}
			
			CAN0CTL0_CSWAI = Para_Config->MSCAN_StopInWaitMode;
			CAN0CTL0_TIME  = Para_Config->MSCAN_TimeStampEnable;
			
			CAN0RIER_WUPIE = Para_Config->MSCAN_WakeUpEnable;
			CAN0RIER_CSCIE = Para_Config->MSCAN_StatusChangeINTEnable;
			CAN0RIER_OVRIE = Para_Config->MSCAN_OverrunINTEnable;
			CAN0RIER_RXFIE = Para_Config->MSCAN_ReceiveFullINTEnable;
			
			CAN0TIER_TXEIE0 = Para_Config->MSCAN_Trans0EmptyINTEnable;
			CAN0TIER_TXEIE1 = Para_Config->MSCAN_Trans1EmptyINTEnable;
			CAN0TIER_TXEIE2 = Para_Config->MSCAN_Trans2EmptyINTEnable;
			
			return 0;
		} 
		else if (CANx->ch == MSCAN_Channel1) 
		{
			if (CANx->pins == MSCAN1_PM2_PM3) 
			{
				/* 
				Configure TXCAN1 and RXCAN1 signals to PM3 and PM2. 
				*/
				DDRM_DDRM2 = 0;
				DDRM_DDRM3 = 1;
			} 
			else return -1;
			
			/* Enable MSCAN1 module. */
			CAN1CTL1_CANE = Para_Config->MSCAN_ModuleEnable;
			
			/* Enable MSCAN wake-up function,this bit must be configured before sleep mode entry. */
			CAN1CTL0_WUPE = Para_Config->MSCAN_WakeUpEnable;
			
			/* First,make sure CAN1 module is in SLEEP mode and 
			   wait CAN BUS enter idle status. */ 
			CAN1CTL0_SLPRQ = 1;
			
			while (!CAN1CTL1_SLPAK) 
			{
				if (++Timeout_Count == 0xFFFFu)
				{
					return -1;
				}
			}
			
			Timeout_Count = 0;
			
			/* Then,CPU request CAN1 module exit SLEEP mode before enter INIT mode. */
			CAN1CTL0_SLPRQ = 0;
			
			while (CAN1CTL1_SLPAK) 
			{
				if (++Timeout_Count == 0xFFFFu) 
				{
					return -1;
				}
			}
			
			Timeout_Count = 0;
			
			/* Second,CPU request CAN1 module enter INIT mode by writing 
			   a 1 to the INITRQ bit in CAN1CTL0 register. */
			CAN1CTL0_INITRQ = 1;
			
			while (!CAN1CTL1_INITAK)
			{
				 if (++Timeout_Count == 0xFFFFu)
				 {
					return -1;
				 }
			}
			
			Timeout_Count = 0;
			
			CAN1CTL1_CLKSRC = Para_Config->MSCAN_ClockSource;
			CAN1CTL1_LOOPB  = Para_Config->MSCAN_LoopbackMode;
			CAN1CTL1_LISTEN = Para_Config->MSCAN_ListenOnlyMode;
			CAN1CTL1_BORM   = Para_Config->MSCAN_BusoffRecoveryMode;
			CAN1CTL1_WUPM   = Para_Config->MSCAN_WakeUpMode;
			
			/* Initialize the baud rate of MSCAN module.
			   Attention: The following setting MSCAN module baud rate program is only apply to
						  the CANCLK source frequency is 16MHz.If the CANCLK frequency is changed,
						  user should modify the corresponding configure registers. 
			*/
			switch (Para_Config->baudrate) 
			{
				case MSCAN_Baudrate_50K:{
				CAN1BTR0_SJW = 1;
				CAN1BTR1_SAMP = 1;
				CAN1BTR0_BRP = 19;
				CAN1BTR1_TSEG_10 = 9;
				CAN1BTR1_TSEG_20 = 4;   
				}break;
				
				case MSCAN_Baudrate_100K:{
				CAN1BTR0_SJW = 1;
				CAN1BTR1_SAMP = 1;
				CAN1BTR0_BRP = 9;
				CAN1BTR1_TSEG_10 = 9;
				CAN1BTR1_TSEG_20 = 4;
				}break;
				
				case MSCAN_Baudrate_125K: {
				CAN1BTR0_SJW = 1;
				CAN1BTR1_SAMP = 1;
				CAN1BTR0_BRP = 7;
				CAN1BTR1_TSEG_10 = 9;
				CAN1BTR1_TSEG_20 = 4;
				}break;
				
				case MSCAN_Baudrate_250K:{   
				CAN1BTR0_SJW = 1;
				CAN1BTR1_SAMP = 1;
				CAN1BTR0_BRP = 3;
				CAN1BTR1_TSEG_10 = 9;
				CAN1BTR1_TSEG_20 = 4;
				}break;
				
				case MSCAN_Baudrate_500K: {
				CAN1BTR0_SJW = 1;
				CAN1BTR1_SAMP = 1;
				CAN1BTR0_BRP = 1;
				CAN1BTR1_TSEG_10 = 9;
				CAN1BTR1_TSEG_20 = 4;
				}break;
				
				case MSCAN_Baudrate_1M:{
				CAN1BTR0_SJW = 1;
				CAN1BTR1_SAMP = 1;
				CAN1BTR0_BRP = 1;
				CAN1BTR1_TSEG_10 = 4;
				CAN1BTR1_TSEG_20 = 1;
				}break;
				
				default:break;
			}
			
			/* Configure the ID filters for receive frame. */
			ret = MSCAN_ConfigIDFilter(CANx, Filter_Config);
			
			/* Finally, CPU request CAN1 module to exit INIT mode. */
			CAN1CTL0_INITRQ = 0;
			
			while (CAN1CTL1_INITAK)
			{
				if (++Timeout_Count == 0xFFFFu)
				{
					return -1;
				}
			}
			
			CAN1CTL0_CSWAI = Para_Config->MSCAN_StopInWaitMode;
			CAN1CTL0_TIME  = Para_Config->MSCAN_TimeStampEnable;
			
			CAN1RIER_WUPIE = Para_Config->MSCAN_WakeUpEnable;
			CAN1RIER_CSCIE = Para_Config->MSCAN_StatusChangeINTEnable;
			CAN1RIER_OVRIE = Para_Config->MSCAN_OverrunINTEnable;
			CAN1RIER_RXFIE = Para_Config->MSCAN_ReceiveFullINTEnable;
			
			CAN1TIER_TXEIE0 = Para_Config->MSCAN_Trans0EmptyINTEnable;
			CAN1TIER_TXEIE1 = Para_Config->MSCAN_Trans1EmptyINTEnable;
			CAN1TIER_TXEIE2 = Para_Config->MSCAN_Trans2EmptyINTEnable;
			
			return 0;
		}
		else if (CANx->ch == MSCAN_Channel4)
		{
			if (CANx->pins == MSCAN4_PJ6_PJ7) 
			{
				/* 
				Configure Module Routing Register(MODRR) and remap 
				TXCAN4 and RXCAN4 signals to PJ7 and PJ6. 
				*/
				MODRR_MODRR2 = 0;
				MODRR_MODRR3 = 0;
				
				DDRJ_DDRJ6 = 0;
				DDRJ_DDRJ7 = 1;
			} 
			else if (CANx->pins == MSCAN4_PM4_PM5)
			{
				/* Configure Module Routing Register(MODRR) and remap 
				   TXCAN4 and RXCAN4 signals to PM5 and PM4. */
				MODRR_MODRR2 = 1;
				MODRR_MODRR3 = 0;
				
				DDRM_DDRM4 = 0;
				DDRM_DDRM5 = 1;
			} 
			else if (CANx->pins == MSCAN4_PM6_PM7)
			{
				/* Configure Module Routing Register(MODRR) and remap 
				   TXCAN4 and RXCAN4 signals to PM7 and PM6. */
				MODRR_MODRR2 = 0;
				MODRR_MODRR3 = 1;
				
				DDRM_DDRM6 = 0;
				DDRM_DDRM7 = 1;
			} 
			else return -1;
			
			/* Enable MSCAN4 module. */
			CAN4CTL1_CANE = Para_Config->MSCAN_ModuleEnable;
			
			/* Enable MSCAN wake-up func,this bit must be configured before sleep mode entry. */
			CAN4CTL0_WUPE = Para_Config->MSCAN_WakeUpEnable;
			
			/* First,make sure CAN4 module is in SLEEP mode and 
			   wait CAN BUS enter idle status. */ 
			CAN4CTL0_SLPRQ = 1;
			
			while (!CAN4CTL1_SLPAK) 
			{
				if (++Timeout_Count == 0xFFFFu)
				{
					return -1;
				}
			}
			
			Timeout_Count = 0;
			
			/* Then,CPU request CAN4 module exit SLEEP mode before enter INIT mode. */
			CAN4CTL0_SLPRQ = 0;
			
			while (CAN4CTL1_SLPAK) 
			{
				if (++Timeout_Count == 0xFFFFu) 
				{
					return -1;
				}
			}
			
			Timeout_Count = 0;
			
			/* Second,CPU request CAN4 module enter INIT mode by writing 
			   a 1 to the INITRQ bit in CAN0CTL0 register. */
			CAN4CTL0_INITRQ = 1;
			
			while (!CAN4CTL1_INITAK)
			{
				 if (++Timeout_Count == 0xFFFFu)
				 {
					return -1;
				 }
			}
			
			Timeout_Count = 0;
			
			CAN4CTL1_CLKSRC = Para_Config->MSCAN_ClockSource;
			CAN4CTL1_LOOPB  = Para_Config->MSCAN_LoopbackMode;
			CAN4CTL1_LISTEN = Para_Config->MSCAN_ListenOnlyMode;
			CAN4CTL1_BORM   = Para_Config->MSCAN_BusoffRecoveryMode;
			CAN4CTL1_WUPM   = Para_Config->MSCAN_WakeUpMode;
			
			/* Initialize the baud rate of MSCAN module.
			   Attention: The following setting MSCAN module baud rate program is only apply to
						  the CANCLK source frequency is 16MHz.If the CANCLK frequency is changed,
						  user should modify the corresponding configure registers. 
			*/
			switch (Para_Config->baudrate) 
			{
				case MSCAN_Baudrate_50K:{
				CAN4BTR0_SJW = 1;
				CAN4BTR1_SAMP = 1;
				CAN4BTR0_BRP = 19;
				CAN4BTR1_TSEG_10 = 9;
				CAN4BTR1_TSEG_20 = 4;   
				}break;
				
				case MSCAN_Baudrate_100K:{
				CAN4BTR0_SJW = 1;
				CAN4BTR1_SAMP = 1;
				CAN4BTR0_BRP = 9;
				CAN4BTR1_TSEG_10 = 9;
				CAN4BTR1_TSEG_20 = 4;
				}break;
				
				case MSCAN_Baudrate_125K: {
				CAN4BTR0_SJW = 1;
				CAN4BTR1_SAMP = 1;
				CAN4BTR0_BRP = 7;
				CAN4BTR1_TSEG_10 = 9;
				CAN4BTR1_TSEG_20 = 4;
				}break;
				
				case MSCAN_Baudrate_250K:{   
				CAN4BTR0_SJW = 1;
				CAN4BTR1_SAMP = 1;
				CAN4BTR0_BRP = 3;
				CAN4BTR1_TSEG_10 = 9;
				CAN4BTR1_TSEG_20 = 4;
				}break;
				
				case MSCAN_Baudrate_500K: {
				CAN4BTR0_SJW = 1;
				CAN4BTR1_SAMP = 1;
				CAN4BTR0_BRP = 1;
				CAN4BTR1_TSEG_10 = 9;
				CAN4BTR1_TSEG_20 = 4;
				}break;
				
				case MSCAN_Baudrate_1M:{
				CAN4BTR0_SJW = 1;
				CAN4BTR1_SAMP = 1;
				CAN4BTR0_BRP = 1;
				CAN4BTR1_TSEG_10 = 4;
				CAN4BTR1_TSEG_20 = 1;
				}break;
				
				default:break;
			}
			
			/* Configure the ID filters for receive frame. */
			ret = MSCAN_ConfigIDFilter(CANx, Filter_Config);
			
			/* Finally, CPU request CAN4 module to exit INIT mode. */
			CAN4CTL0_INITRQ = 0;
			
			while (CAN4CTL1_INITAK)
			{
				if (++Timeout_Count == 0xFFFFu)
				{
					return -1;
				}
			}
			
			CAN4CTL0_CSWAI = Para_Config->MSCAN_StopInWaitMode;
			CAN4CTL0_TIME  = Para_Config->MSCAN_TimeStampEnable;
			
			CAN4RIER_WUPIE = Para_Config->MSCAN_WakeUpEnable;
			CAN4RIER_CSCIE = Para_Config->MSCAN_StatusChangeINTEnable;
			CAN4RIER_OVRIE = Para_Config->MSCAN_OverrunINTEnable;
			CAN4RIER_RXFIE = Para_Config->MSCAN_ReceiveFullINTEnable;
			
			CAN4TIER_TXEIE0 = Para_Config->MSCAN_Trans0EmptyINTEnable;
			CAN4TIER_TXEIE1 = Para_Config->MSCAN_Trans1EmptyINTEnable;
			CAN4TIER_TXEIE2 = Para_Config->MSCAN_Trans2EmptyINTEnable;
			
			return 0;
		}
    }
    
    return -1;    
}



/**
 * @brief   MSCAN send a message by a chosen CAN module.
 * @param   CANx, The specified MSCAN module.
 * 			*W_Framebuff: Data buffer that storing one CAN message which will be sent.
 * @returns 0: Calling succeeded.
 * 			-1: Calling failed.
 */
int16_t MSCAN_SendFrame(MSCAN_ModuleConfig* CANx, MSCAN_MessageTypeDef* W_Framebuff)
{
    uint8_t ret_val;
    
    uint8_t i;
    
    uint8_t tempID_H,tempID_M,tempID_L;
    
    if ((CANx != NULL) && (W_Framebuff != NULL))
    {
        if ((CANx->ch < MSCAN_Channel0) || (CANx->ch > MSCAN_Channel4))return -1;
        
        if ((CANx->pins < MSCAN0_PM0_PM1) || (CANx->pins > MSCAN4_PM6_PM7))return -1;
        
        /* Make sure the frame type is correctly. */
        if ((W_Framebuff->frametype < DataFrameWithStandardId) 
         || (W_Framebuff->frametype > RemoteFrameWithExtendedId))return -1;
        
        /* Make sure the length of the data not to be greater than 8 bytes. */
        if (W_Framebuff->data_length > 8)return -1;
        
        if (CANx->ch == MSCAN_Channel0) 		/* MSCAN0 */
        {
            /* Make sure there is at least one Empty Transmission Buffer. */
            ret_val = CAN0TFLG & CAN0TFLG_TXE_MASK;
            if (ret_val == 0)return -1;
            
            /* Select an empty MSCAN transmit buffer and ready to send frame. */
            CAN0TBSEL = ret_val;
            
            /* When transmit a frame with standard ID. */
            if ((W_Framebuff->frametype == RemoteFrameWithStandardId)
                || (W_Framebuff->frametype == DataFrameWithStandardId)) 
            {
                /* Indicate the sent frame is standard id format */
                CAN0TXIDR1_IDE = 0;
                
                /* If caller wants to send a packet with standard id, firstly
			     * ensure the id not to be exceed 11 bits.
			     */
                if (W_Framebuff->frame_id > 0x7FFu)return -1;
                
                CAN0TXIDR0 = (uint8_t)((W_Framebuff->frame_id) >> 3);
                CAN0TXIDR1 = (uint8_t)((W_Framebuff->frame_id) << 5);
                
                if (W_Framebuff->frametype == RemoteFrameWithStandardId) /* Remote Frame with Standard ID */
                {
                    /*
    				 * If caller wants to send a remote packet, set the
    				 * respective bit in CANxTXIDR1, and set the DLR
    				 * to be 0 (the remote packet has no data).
    				 */
    				 CAN0TXIDR1_SRR = 1;
    				
    				 /* The data length should be 0 */ 
    				 CAN0TXDLR = 0;
                } 
                else 													 /* Data Frame with Standard ID */
                {
                     /*
    				 * If caller wants to send a data packet, clear the
    				 * respective bit in CANxTXIDR1.
    				 */
                     CAN0TXIDR1_SRR = 0;
                     
                     /*
				      * If caller wants to send a data packet, set the
				      * TDLR to be equal to the actual data length
				     */
                     CAN0TXDLR = (uint8_t)(W_Framebuff->data_length) & 0x0Fu;
                     
                     /* Load the actual data to the Transmit Data Segment Registers. */
                     for (i = 0;i < W_Framebuff->data_length; i++) 
                     {
                         if (i == 0)CAN0TXDSR0 = (uint8_t)W_Framebuff->data[0];
                         else if (i == 1)CAN0TXDSR1 = (uint8_t)W_Framebuff->data[1];
                         else if (i == 2)CAN0TXDSR2 = (uint8_t)W_Framebuff->data[2];
                         else if (i == 3)CAN0TXDSR3 = (uint8_t)W_Framebuff->data[3];
                         else if (i == 4)CAN0TXDSR4 = (uint8_t)W_Framebuff->data[4];
                         else if (i == 5)CAN0TXDSR5 = (uint8_t)W_Framebuff->data[5];
                         else if (i == 6)CAN0TXDSR6 = (uint8_t)W_Framebuff->data[6];
                         else if (i == 7)CAN0TXDSR7 = (uint8_t)W_Framebuff->data[7];
                     }
                }
            } /* Or when transmit a frame with extended ID. */ 
            else if ((W_Framebuff->frametype == RemoteFrameWithExtendedId)
                     || (W_Framebuff->frametype == DataFrameWithExtendedId))  
            {
                /* Indicate the sent frame is extended id format */
                CAN0TXIDR1_IDE = 1;
                
                /* If caller wants to send a packet with extended id, firstly
			     * ensure the id not to be exceed 28 bits.
			     */
                if (W_Framebuff->frame_id > 0x1FFFFFFFu)return -1;
                
                CAN0TXIDR0 = (uint8_t)((W_Framebuff->frame_id) >> 21);
                
                tempID_H = (uint8_t)((W_Framebuff->frame_id) >> 18) & 0x07;
                tempID_M = (uint8_t)((W_Framebuff->frame_id) >> 15) & 0x07;
                
                CAN0TXIDR1 = (tempID_H << 5) | CAN0TXIDR1_SRR_MASK | CAN0TXIDR1_IDE_MASK | tempID_M;
                
                CAN0TXIDR2 = (uint8_t)((W_Framebuff->frame_id) >> 7);
                
                tempID_L = (uint8_t)W_Framebuff->frame_id & 0x7Fu;
                
                CAN0TXIDR3 = tempID_L << 1;
                
                if (W_Framebuff->frametype == RemoteFrameWithExtendedId)
                {
                    CAN0TXIDR3_RTR = 1;
                    
                    /* The data length should be 0 */ 
    				CAN0TXDLR = 0;    
                } 
                else 
                {
                    CAN0TXIDR3_RTR = 0;
                    
                    CAN0TXDLR = (uint8_t)(W_Framebuff->data_length) & 0x0Fu;
                    
                     /* Load the actual data to the Transmit Data Segment Registers. */
                     for (i = 0;i < W_Framebuff->data_length; i++) 
                     {
                         if (i == 0)CAN0TXDSR0 = (uint8_t)W_Framebuff->data[0];
                         else if (i == 1)CAN0TXDSR1 = (uint8_t)W_Framebuff->data[1];
                         else if (i == 2)CAN0TXDSR2 = (uint8_t)W_Framebuff->data[2];
                         else if (i == 3)CAN0TXDSR3 = (uint8_t)W_Framebuff->data[3];
                         else if (i == 4)CAN0TXDSR4 = (uint8_t)W_Framebuff->data[4];
                         else if (i == 5)CAN0TXDSR5 = (uint8_t)W_Framebuff->data[5];
                         else if (i == 6)CAN0TXDSR6 = (uint8_t)W_Framebuff->data[6];
                         else if (i == 7)CAN0TXDSR7 = (uint8_t)W_Framebuff->data[7];
                     }
                }
            }
            
            /* Clear the respective bits. */
            CAN0TFLG = CAN0TBSEL;
            
            return 0;
        } 
        else if (CANx->ch == MSCAN_Channel1) 
        {
            /* Make sure there is at least one Empty Transmission Buffer. */
            ret_val = CAN1TFLG & CAN1TFLG_TXE_MASK;
            if (ret_val == 0)return -1;
            
            /* Select an empty MSCAN transmit buffer and ready to send frame. */
            CAN1TBSEL = ret_val;
            
            /* When transmit a frame with standard ID. */
            if ((W_Framebuff->frametype == RemoteFrameWithStandardId)
                || (W_Framebuff->frametype == DataFrameWithStandardId)) 
            {
                /* Indicate the sent frame is standard id format */
                CAN1TXIDR1_IDE = 0;
                
                /* If caller wants to send a packet with standard id, firstly
			     * ensure the id not to be exceed 11 bits.
			     */
                if (W_Framebuff->frame_id > 0x7FFu)return -1;
                
                CAN1TXIDR0 = (uint8_t)((W_Framebuff->frame_id) >> 3);
                CAN1TXIDR1 = (uint8_t)((W_Framebuff->frame_id) << 5);
                
                if (W_Framebuff->frametype == RemoteFrameWithStandardId) 
                {
                    /*
    				 * If caller wants to send a remote packet, set the
    				 * respective bit in CANxTXIDR1, and set the DLR
    				 * to be 0 (the remote packet has no data).
    				 */
    				 CAN1TXIDR1_SRR = 1;
    				
    				 /* The data length should be 0 */ 
    				 CAN1TXDLR = 0;
                } 
                else 
                {
                     /*
    				 * If caller wants to send a data packet, clear the
    				 * respective bit in CANxTXIDR1.
    				 */
                     CAN1TXIDR1_SRR = 0;
                     
                     /*
				      * If caller wants to send a data packet, set the
				      * TDLR to be equal to the actual data length
				     */
                     CAN1TXDLR = (uint8_t)(W_Framebuff->data_length) & 0x0Fu;
                     
                     /* Load the actual data to the Transmit Data Segment Registers. */
                     for (i = 0;i < W_Framebuff->data_length; i++) 
                     {
                         if (i == 0)CAN1TXDSR0 = (uint8_t)W_Framebuff->data[0];
                         else if (i == 1)CAN1TXDSR1 = (uint8_t)W_Framebuff->data[1];
                         else if (i == 2)CAN1TXDSR2 = (uint8_t)W_Framebuff->data[2];
                         else if (i == 3)CAN1TXDSR3 = (uint8_t)W_Framebuff->data[3];
                         else if (i == 4)CAN1TXDSR4 = (uint8_t)W_Framebuff->data[4];
                         else if (i == 5)CAN1TXDSR5 = (uint8_t)W_Framebuff->data[5];
                         else if (i == 6)CAN1TXDSR6 = (uint8_t)W_Framebuff->data[6];
                         else if (i == 7)CAN1TXDSR7 = (uint8_t)W_Framebuff->data[7];
                     }
                }
            } /* Or when transmit a frame with extended ID. */ 
            else if ((W_Framebuff->frametype == RemoteFrameWithExtendedId)
                     || (W_Framebuff->frametype == DataFrameWithExtendedId))  
            {
                /* Indicate the sent frame is extended id format */
                CAN1TXIDR1_IDE = 1;
                
                /* If caller wants to send a packet with extended id, firstly
			     * ensure the id not to be exceed 28 bits.
			     */
                if (W_Framebuff->frame_id > 0x1FFFFFFFu)return -1;
                
                CAN1TXIDR0 = (uint8_t)((W_Framebuff->frame_id) >> 21);
                
                tempID_H = (uint8_t)((W_Framebuff->frame_id) >> 18) & 0x07;
                tempID_M = (uint8_t)((W_Framebuff->frame_id) >> 15) & 0x07;
                
                CAN1TXIDR1 = (tempID_H << 5) | CAN1TXIDR1_SRR_MASK | CAN1TXIDR1_IDE_MASK | tempID_M;
                
                CAN1TXIDR2 = (uint8_t)((W_Framebuff->frame_id) >> 7);
                
                tempID_L = (uint8_t)W_Framebuff->frame_id & 0x7Fu;
                
                CAN1TXIDR3 = tempID_L << 1;
                
                if (W_Framebuff->frametype == RemoteFrameWithExtendedId)
                {
                    CAN1TXIDR3_RTR = 1;
                    
                    /* The data length should be 0 */ 
    				CAN1TXDLR = 0;    
                } 
                else 
                {
                    CAN1TXIDR3_RTR = 0;
                    
                    CAN1TXDLR = (uint8_t)(W_Framebuff->data_length) & 0x0Fu;
                    
                     /* Load the actual data to the Transmit Data Segment Registers. */
                     for (i = 0;i < W_Framebuff->data_length; i++) 
                     {
                         if (i == 0)CAN1TXDSR0 = (uint8_t)W_Framebuff->data[0];
                         else if (i == 1)CAN1TXDSR1 = (uint8_t)W_Framebuff->data[1];
                         else if (i == 2)CAN1TXDSR2 = (uint8_t)W_Framebuff->data[2];
                         else if (i == 3)CAN1TXDSR3 = (uint8_t)W_Framebuff->data[3];
                         else if (i == 4)CAN1TXDSR4 = (uint8_t)W_Framebuff->data[4];
                         else if (i == 5)CAN1TXDSR5 = (uint8_t)W_Framebuff->data[5];
                         else if (i == 6)CAN1TXDSR6 = (uint8_t)W_Framebuff->data[6];
                         else if (i == 7)CAN1TXDSR7 = (uint8_t)W_Framebuff->data[7];
                     }
                }
            }
            
            /* Clear the respective bits. */
            CAN1TFLG = CAN1TBSEL;
            
            return 0;
        } 
        else if (CANx->ch == MSCAN_Channel4) 
        {
            /* Make sure there is at least one Empty Transmission Buffer. */
            ret_val = CAN4TFLG & CAN4TFLG_TXE_MASK;
            if (ret_val == 0)return -1;
            
            /* Select an empty MSCAN transmit buffer and ready to send frame. */
            CAN4TBSEL = ret_val;
            
            /* When transmit a frame with standard ID. */
            if ((W_Framebuff->frametype == RemoteFrameWithStandardId)
                || (W_Framebuff->frametype == DataFrameWithStandardId)) 
            {
                /* Indicate the sent frame is standard id format */
                CAN4TXIDR1_IDE = 0;
                
                /* If caller wants to send a packet with standard id, firstly
			     * ensure the id not to be exceed 11 bits.
			     */
                if (W_Framebuff->frame_id > 0x7FFu)return -1;
                
                CAN4TXIDR0 = (uint8_t)((W_Framebuff->frame_id) >> 3);
                CAN4TXIDR1 = (uint8_t)((W_Framebuff->frame_id) << 5);
                
                if (W_Framebuff->frametype == RemoteFrameWithStandardId) 
                {
                    /*
    				 * If caller wants to send a remote packet, set the
    				 * respective bit in CANxTXIDR1, and set the DLR
    				 * to be 0 (the remote packet has no data).
    				 */
    				 CAN4TXIDR1_SRR = 1;
    				
    				 /* The data length should be 0 */ 
    				 CAN4TXDLR = 0;
                } 
                else 
                {
                     /*
    				 * If caller wants to send a data packet, clear the
    				 * respective bit in CANxTXIDR1.
    				 */
                     CAN4TXIDR1_SRR = 0;
                     
                     /*
				      * If caller wants to send a data packet, set the
				      * TDLR to be equal to the actual data length
				     */
                     CAN4TXDLR = (uint8_t)(W_Framebuff->data_length) & 0x0Fu;
                     
                     /* Load the actual data to the Transmit Data Segment Registers. */
                     for (i = 0;i < W_Framebuff->data_length; i++) 
                     {
                         if (i == 0)CAN4TXDSR0 = (uint8_t)W_Framebuff->data[0];
                         else if (i == 1)CAN4TXDSR1 = (uint8_t)W_Framebuff->data[1];
                         else if (i == 2)CAN4TXDSR2 = (uint8_t)W_Framebuff->data[2];
                         else if (i == 3)CAN4TXDSR3 = (uint8_t)W_Framebuff->data[3];
                         else if (i == 4)CAN4TXDSR4 = (uint8_t)W_Framebuff->data[4];
                         else if (i == 5)CAN4TXDSR5 = (uint8_t)W_Framebuff->data[5];
                         else if (i == 6)CAN4TXDSR6 = (uint8_t)W_Framebuff->data[6];
                         else if (i == 7)CAN4TXDSR7 = (uint8_t)W_Framebuff->data[7];
                     }
                }
            } /* Or when transmit a frame with extended ID. */ 
            else if ((W_Framebuff->frametype == RemoteFrameWithExtendedId)
                     || (W_Framebuff->frametype == DataFrameWithExtendedId))  
            {
                /* Indicate the sent frame is extended id format */
                CAN4TXIDR1_IDE = 1;
                
                /* If caller wants to send a packet with extended id, firstly
			     * ensure the id not to be exceed 28 bits.
			     */
                if (W_Framebuff->frame_id > 0x1FFFFFFFu)return -1;
                
                CAN4TXIDR0 = (uint8_t)((W_Framebuff->frame_id) >> 21);
                
                tempID_H = (uint8_t)((W_Framebuff->frame_id) >> 18) & 0x07;
                tempID_M = (uint8_t)((W_Framebuff->frame_id) >> 15) & 0x07;
                
                CAN4TXIDR1 = (tempID_H << 5) | CAN4TXIDR1_SRR_MASK | CAN4TXIDR1_IDE_MASK | tempID_M;
                
                CAN4TXIDR2 = (uint8_t)((W_Framebuff->frame_id) >> 7);
                
                tempID_L = (uint8_t)W_Framebuff->frame_id & 0x7Fu;
                
                CAN4TXIDR3 = tempID_L << 1;
                
                if (W_Framebuff->frametype == RemoteFrameWithExtendedId)
                {
                    CAN4TXIDR3_RTR = 1;
                    
                    /* The data length should be 0 */ 
    				CAN4TXDLR = 0;    
                } 
                else 
                {
                    CAN4TXIDR3_RTR = 0;
                    
                    CAN4TXDLR = (uint8_t)(W_Framebuff->data_length) & 0x0Fu;
                    
                     /* Load the actual data to the Transmit Data Segment Registers. */
                     for (i = 0;i < W_Framebuff->data_length; i++) 
                     {
                         if (i == 0)CAN4TXDSR0 = (uint8_t)W_Framebuff->data[0];
                         else if (i == 1)CAN4TXDSR1 = (uint8_t)W_Framebuff->data[1];
                         else if (i == 2)CAN4TXDSR2 = (uint8_t)W_Framebuff->data[2];
                         else if (i == 3)CAN4TXDSR3 = (uint8_t)W_Framebuff->data[3];
                         else if (i == 4)CAN4TXDSR4 = (uint8_t)W_Framebuff->data[4];
                         else if (i == 5)CAN4TXDSR5 = (uint8_t)W_Framebuff->data[5];
                         else if (i == 6)CAN4TXDSR6 = (uint8_t)W_Framebuff->data[6];
                         else if (i == 7)CAN4TXDSR7 = (uint8_t)W_Framebuff->data[7];
                     }
                }
            }
            
            /* Clear the respective bits. */
            CAN4TFLG = CAN4TBSEL;
            
            return 0;
        }
    }
    
    return -1;   
}



/**
 * @brief   Checking the specified CAN module whether have enough
 *          hard transmission buffer to send CAN messages.
 * @param   CANx, The specified MSCAN module.
 * @returns 0: Calling succeeded.User can load CAN messages to send.
 * 			-1: Calling failed.User can not load CAN messages to send.
 */
int16_t MSCAN_HardTxBufferCheck(MSCAN_ChannelTypeDef CANx) 
{
    uint8_t ret_val;
    
    if ((CANx < MSCAN_Channel0) || (CANx > MSCAN_Channel4))return -1;
    
    if (CANx == MSCAN_Channel0) 
    {
        ret_val = CAN0TFLG & CAN0TFLG_TXE_MASK;
        
        if (ret_val == 0)return -1;   /* All the TX buffer is full,that means user can load CAN messages to send. */
    } 
    else if (CANx == MSCAN_Channel1) 
    {
        ret_val = CAN1TFLG & CAN1TFLG_TXE_MASK;
        
        if (ret_val == 0)return -1;   /* All the TX buffer is full,that means user can load CAN messages to send. */
    } 
    else 
    {
        ret_val = CAN4TFLG & CAN4TFLG_TXE_MASK;
        
        if (ret_val == 0)return -1;   /* All the TX buffer is full,that means user can load CAN messages to send. */
    }
    
    return 0;   /* If CANTFLG register value is not zero,that means there is at least one empty TX buffer,user can load
                   CAN messages to send. */
}

/*****************************END OF FILE**************************************/






