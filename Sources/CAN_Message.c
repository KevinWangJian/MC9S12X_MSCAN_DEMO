/**
  ******************************************************************************
  * @Copyright (C), 1997-2015, Hangzhou Gold Electronic Equipment Co., Ltd.
  * @file name: CAN_Message.c
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

#include "CAN_Message.h"
#include "xgate.h"




#pragma DATA_SEG __GPAGE_SEG PAGED_RAM

volatile CANSendMessagebuffer_TypeDef g_CANx_SendBuffer;

#pragma DATA_SEG DEFAULT




/**
 * @brief   Check the CAN received buffers and judge if there is a valid CAN messages.If yes,get them out.
 * @param   CANx, CAN channel number.
 *          *CAN_RMessage, Store the read CAN message data buffer.
 * @returns 0: There is a valid CAN message and get it out from CAN receive buffer.Calling succeeded.
 * 			-1: There is not a valid CAN message.Calling failed.
 */
int16_t Check_CANReceiveBuffer(MSCAN_ChannelTypeDef CANx, MSCAN_MessageTypeDef* CAN_RMessage) 
{
    uint8_t i;
    
    if ((CANx < MSCAN_Channel0) || (CANx > MSCAN_Channel4))return -1;
    
    if (NULL == CAN_RMessage)return -1;
    
    /* If select CAN0,it means that check system intranet CAN receive buffer. */
    if (MSCAN_Channel0 == CANx) 
    {
        if (g_CANx_RecBuffer.Intranet_RecBuff_RPointer >= INTRANET_RECEIVEBUF_SIZE) 
        {
            g_CANx_RecBuffer.Intranet_RecBuff_RPointer = 0;    
        }
    
        /* If the frame type value of intranet CAN received message is zero,that means the current 
           received buffer is empty. */
        if (g_CANx_RecBuffer.Intranet_RecBuf[g_CANx_RecBuffer.Intranet_RecBuff_RPointer].frametype == (MSCAN_FrameAndIDTypeDef)0)   
        {        
            return -1;        
        } 
        else /* If the frame type value is not zero,that means there is a valid CAN message in the current received buffer. */
        {
            CAN_RMessage->frametype   = g_CANx_RecBuffer.Intranet_RecBuf[g_CANx_RecBuffer.Intranet_RecBuff_RPointer].frametype;
            g_CANx_RecBuffer.Intranet_RecBuf[g_CANx_RecBuffer.Intranet_RecBuff_RPointer].frametype   = (MSCAN_FrameAndIDTypeDef)0;
            
            CAN_RMessage->frame_id    = g_CANx_RecBuffer.Intranet_RecBuf[g_CANx_RecBuffer.Intranet_RecBuff_RPointer].frame_id;
            g_CANx_RecBuffer.Intranet_RecBuf[g_CANx_RecBuffer.Intranet_RecBuff_RPointer].frame_id    = 0;
            
            if ((CAN_RMessage->frametype == DataFrameWithStandardId)
             || (CAN_RMessage->frametype == DataFrameWithExtendedId)) 
            {
                CAN_RMessage->data_length  = g_CANx_RecBuffer.Intranet_RecBuf[g_CANx_RecBuffer.Intranet_RecBuff_RPointer].data_length;
                g_CANx_RecBuffer.Intranet_RecBuf[g_CANx_RecBuffer.Intranet_RecBuff_RPointer].data_length = 0;
                
                for (i = 0; i < CAN_RMessage->data_length; i++) 
                {
                    CAN_RMessage->data[i]  = g_CANx_RecBuffer.Intranet_RecBuf[g_CANx_RecBuffer.Intranet_RecBuff_RPointer].data[i];  
                    g_CANx_RecBuffer.Intranet_RecBuf[g_CANx_RecBuffer.Intranet_RecBuff_RPointer].data[i] = 0;  
                }       
            }
            
            g_CANx_RecBuffer.Intranet_RecBuff_RPointer++;
          
            return 0;
        }        
    } 
    else if (MSCAN_Channel1 == CANx) 
    {
        if (g_CANx_RecBuffer.ECU_RecBuff_RPointer >= ECU_RECEIVEBUF_SIZE) 
        {
            g_CANx_RecBuffer.ECU_RecBuff_RPointer = 0;    
        }
    
        /* If the frame type value of intranet CAN received message is zero,that means the current 
           received buffer is empty. */
        if (g_CANx_RecBuffer.ECU_RecBuf[g_CANx_RecBuffer.ECU_RecBuff_RPointer].frametype == (MSCAN_FrameAndIDTypeDef)0)   
        {        
            return -1;        
        } 
        else /* If the frame type value is not zero,that means there is a valid CAN message in the current received buffer. */
        {
            CAN_RMessage->frametype   = g_CANx_RecBuffer.ECU_RecBuf[g_CANx_RecBuffer.ECU_RecBuff_RPointer].frametype;
            g_CANx_RecBuffer.ECU_RecBuf[g_CANx_RecBuffer.ECU_RecBuff_RPointer].frametype   = (MSCAN_FrameAndIDTypeDef)0;
            
            CAN_RMessage->frame_id    = g_CANx_RecBuffer.ECU_RecBuf[g_CANx_RecBuffer.ECU_RecBuff_RPointer].frame_id;
            g_CANx_RecBuffer.ECU_RecBuf[g_CANx_RecBuffer.ECU_RecBuff_RPointer].frame_id    = 0;
            
            if ((CAN_RMessage->frametype == DataFrameWithStandardId)
             || (CAN_RMessage->frametype == DataFrameWithExtendedId)) 
            {
                CAN_RMessage->data_length  = g_CANx_RecBuffer.ECU_RecBuf[g_CANx_RecBuffer.ECU_RecBuff_RPointer].data_length;
                g_CANx_RecBuffer.ECU_RecBuf[g_CANx_RecBuffer.ECU_RecBuff_RPointer].data_length = 0;
                
                for (i = 0; i < CAN_RMessage->data_length; i++) 
                {
                    CAN_RMessage->data[i]  = g_CANx_RecBuffer.ECU_RecBuf[g_CANx_RecBuffer.ECU_RecBuff_RPointer].data[i];  
                    g_CANx_RecBuffer.ECU_RecBuf[g_CANx_RecBuffer.ECU_RecBuff_RPointer].data[i] = 0;  
                }       
            }
            
            g_CANx_RecBuffer.ECU_RecBuff_RPointer++;
          
            return 0;
        }  
    } 
    else 
    {
        if (g_CANx_RecBuffer.Charger_RecBuff_RPointer >= CHARGER_RECEIVEBUF_SIZE) 
        {
            g_CANx_RecBuffer.Charger_RecBuff_RPointer = 0;    
        }
    
        /* If the frame type value of intranet CAN received message is zero,that means the current 
           received buffer is empty. */
        if (g_CANx_RecBuffer.Charger_RecBuf[g_CANx_RecBuffer.Charger_RecBuff_RPointer].frametype == (MSCAN_FrameAndIDTypeDef)0)   
        {        
            return -1;        
        } 
        else /* If the frame type value is not zero,that means there is a valid CAN message in the current received buffer. */
        {
            CAN_RMessage->frametype   = g_CANx_RecBuffer.Charger_RecBuf[g_CANx_RecBuffer.Charger_RecBuff_RPointer].frametype;
            g_CANx_RecBuffer.Charger_RecBuf[g_CANx_RecBuffer.Charger_RecBuff_RPointer].frametype   = (MSCAN_FrameAndIDTypeDef)0;
            
            CAN_RMessage->frame_id    = g_CANx_RecBuffer.Charger_RecBuf[g_CANx_RecBuffer.Charger_RecBuff_RPointer].frame_id;
            g_CANx_RecBuffer.Charger_RecBuf[g_CANx_RecBuffer.Charger_RecBuff_RPointer].frame_id    = 0;
            
            if ((CAN_RMessage->frametype == DataFrameWithStandardId)
             || (CAN_RMessage->frametype == DataFrameWithExtendedId)) 
            {
                CAN_RMessage->data_length  = g_CANx_RecBuffer.Charger_RecBuf[g_CANx_RecBuffer.Charger_RecBuff_RPointer].data_length;
                g_CANx_RecBuffer.Charger_RecBuf[g_CANx_RecBuffer.Charger_RecBuff_RPointer].data_length = 0;
                
                for (i = 0; i < CAN_RMessage->data_length; i++) 
                {
                    CAN_RMessage->data[i]  = g_CANx_RecBuffer.Charger_RecBuf[g_CANx_RecBuffer.Charger_RecBuff_RPointer].data[i];  
                    g_CANx_RecBuffer.Charger_RecBuf[g_CANx_RecBuffer.Charger_RecBuff_RPointer].data[i] = 0;  
                }       
            }
            
            g_CANx_RecBuffer.Charger_RecBuff_RPointer++;
          
            return 0;
        }   
    }
}



/**
 * @brief   Fill the specified CAN message to the corresponding CAN send buffers.
 * @param   CANx, CAN channel number.
 *          *CAN_WMessage, CAN message which will be filled into CAN send buffers.
 * @returns 0: Calling succeeded.Which means the specified CAN message has filled into CAN send buffers.
 * 			-1: Calling failed.Which means CAN send buffers is full and can't fill new messages.
 */
int16_t Fill_CANSendBuffer(MSCAN_ChannelTypeDef CANx, MSCAN_MessageTypeDef* CAN_WMessage) 
{
    uint8_t i;
    
    if ((CANx < MSCAN_Channel0) || (CANx > MSCAN_Channel4))return -1;
    
    if (NULL == CAN_WMessage)return -1;
    
    /* If select CAN0,it means that fill the specified CAN message into intranet CAN send buffers */
    if (MSCAN_Channel0 == CANx) 
    {
        /* If the write pointer of intranet CAN send buffers reach to maximum value,clear it. */
        if (g_CANx_SendBuffer.Intranet_SendBuff_WPointer >= INTRANET_SENDBUF_SIZE) 
        {
            g_CANx_SendBuffer.Intranet_SendBuff_WPointer = 0;
        }
        
        /* Get the frame type value and judge it.If the value is not zero,that means the 
           pointed buffer is full,and user can not fill the CAN message to it. */    
        if (g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_WPointer].frametype != (MSCAN_FrameAndIDTypeDef)0) 
        {
            return -1;    
        } 
        else /* If the frame type value is zero,that means the pointed buffer is empty,and user can fill the CAN message to it. */
        {   
            g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_WPointer].frame_id  = CAN_WMessage->frame_id;

            if ((CAN_WMessage->frametype == DataFrameWithStandardId)
             || (CAN_WMessage->frametype == DataFrameWithExtendedId)) 
            {
                g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_WPointer].data_length = CAN_WMessage->data_length;
                               
                for (i = 0; i < CAN_WMessage->data_length; i++) 
                {
                    g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_WPointer].data[i] = CAN_WMessage->data[i];
                }
            }
            
            g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_WPointer].frametype = CAN_WMessage->frametype;
            
            g_CANx_SendBuffer.Intranet_SendBuff_WPointer++;

            return 0; 
        }
    }  
    else if (MSCAN_Channel1 == CANx) /* If select CAN1,it means that fill the specified CAN message into ECU CAN send buffers. */ 
    {
        /* If the write pointer of ECU CAN send buffers reach to maximum value,clear it. */
        if (g_CANx_SendBuffer.ECU_SendBuff_WPointer >= ECU_SENDBUF_SIZE) 
        {
            g_CANx_SendBuffer.ECU_SendBuff_WPointer = 0;
        }
        
        /* Get the frame type value and judge it.If the value is not zero,that means the 
           pointed buffer is full,and user can not fill the CAN message to it. */     
        if (g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_WPointer].frametype != (MSCAN_FrameAndIDTypeDef)0) 
        {
            return -1;    
        } 
        else  /* If the frame type value is zero,that means the pointed buffer is empty,and user can fill the CAN message to it. */
        {   
            g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_WPointer].frame_id  = CAN_WMessage->frame_id;

            if ((CAN_WMessage->frametype == DataFrameWithStandardId)
             || (CAN_WMessage->frametype == DataFrameWithExtendedId)) 
            {
                g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_WPointer].data_length = CAN_WMessage->data_length;
                               
                for (i = 0; i < CAN_WMessage->data_length; i++) 
                {
                    g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_WPointer].data[i] = CAN_WMessage->data[i];
                }
            }
            
            g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_WPointer].frametype = CAN_WMessage->frametype;
            
            g_CANx_SendBuffer.ECU_SendBuff_WPointer++;

            return 0; 
        }    
    } 
    else /* If select CAN4,it means that fill the specified CAN message into charger CAN send buffers. */ 
    {
        if (g_CANx_SendBuffer.Charger_SendBuff_WPointer >= CHARGER_SENDBUF_SIZE) 
        {
            g_CANx_SendBuffer.Charger_SendBuff_WPointer = 0;
        }
            
        if (g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_WPointer].frametype != (MSCAN_FrameAndIDTypeDef)0) 
        {
            return -1;    
        } 
        else 
        {   
            g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_WPointer].frame_id  = CAN_WMessage->frame_id;

            if ((CAN_WMessage->frametype == DataFrameWithStandardId)
             || (CAN_WMessage->frametype == DataFrameWithExtendedId)) 
            {
                g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_WPointer].data_length = CAN_WMessage->data_length;
                               
                for (i = 0; i < CAN_WMessage->data_length; i++) 
                {
                    g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_WPointer].data[i] = CAN_WMessage->data[i];
                }
            }
            
            g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_WPointer].frametype = CAN_WMessage->frametype;
            
            g_CANx_SendBuffer.Charger_SendBuff_WPointer++;

            return 0; 
        }    
    }
}



/**
 * @brief   Check the CAN send buffers and judge if there is a valid CAN messages.If yes,get them out.
 * @param   CANx, CAN channel number.
 *          *CAN_RMessage, buffer which will store the read CAN message. 
 * @returns  0: Calling succeeded.Which means the pointed send buffer has a valid CAN message and user
 *              have got it.
 * 			-1: Calling failed.Which means the pointed send buffer does not have a valid CAN message.
 */
int16_t Check_CANSendBuffer(MSCAN_ChannelTypeDef CANx, MSCAN_MessageTypeDef* CAN_RMessage) 
{
    uint8_t i;
    
    if ((CANx < MSCAN_Channel0) || (CANx > MSCAN_Channel4))return -1;
    
    if (NULL == CAN_RMessage)return -1;
    
    /* If select CAN0, that means check the intranet CAN send buffers and read the CAN messages. */
    if (MSCAN_Channel0 == CANx) 
    {
        /* If the read pointer of intranet CAN send buffers reach to maximum value,clear it. */
        if (g_CANx_SendBuffer.Intranet_SendBuff_RPointer >= INTRANET_SENDBUF_SIZE) 
        {
            g_CANx_SendBuffer.Intranet_SendBuff_RPointer = 0;
        }
        
        /* Get the frame type value and judge it.If the value is not zero,that means the 
           read pointed buffer is full,and user can read the CAN messages from it. */ 
        if (g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_RPointer].frametype != (MSCAN_FrameAndIDTypeDef)0) 
        {
            CAN_RMessage->frametype = g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_RPointer].frametype;
            g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_RPointer].frametype = (MSCAN_FrameAndIDTypeDef)0;
            
            CAN_RMessage->frame_id  = g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_RPointer].frame_id;
            g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_RPointer].frame_id = 0;
            
            if ((CAN_RMessage->frametype == DataFrameWithStandardId)
             || (CAN_RMessage->frametype == DataFrameWithExtendedId)) 
            {
                CAN_RMessage->data_length = g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_RPointer].data_length;
                g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_RPointer].data_length = 0;
                
                for (i = 0; i < CAN_RMessage->data_length; i++) 
                {
                    CAN_RMessage->data[i] = g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_RPointer].data[i];
                    g_CANx_SendBuffer.Intranet_SendBuff[g_CANx_SendBuffer.Intranet_SendBuff_RPointer].data[i] = 0;    
                }
            }
            
            g_CANx_SendBuffer.Intranet_SendBuff_RPointer++;
            
            return 0;
        } 
        else /* If the value is zero,that means the pointed buffer is empty,user can not read the CAN messages from it. */
        {
            return -1;
        }
    } 
    else if (MSCAN_Channel1 == CANx) 
    {
        if (g_CANx_SendBuffer.ECU_SendBuff_RPointer >= ECU_SENDBUF_SIZE) 
        {
            g_CANx_SendBuffer.ECU_SendBuff_RPointer = 0;
        }
        
        if (g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_RPointer].frametype != (MSCAN_FrameAndIDTypeDef)0) 
        {
            CAN_RMessage->frametype = g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_RPointer].frametype;
            g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_RPointer].frametype = (MSCAN_FrameAndIDTypeDef)0;
            
            CAN_RMessage->frame_id  = g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_RPointer].frame_id;
            g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_RPointer].frame_id = 0;
            
            if ((CAN_RMessage->frametype == DataFrameWithStandardId)
             || (CAN_RMessage->frametype == DataFrameWithExtendedId)) 
            {
                CAN_RMessage->data_length = g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_RPointer].data_length;
                g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_RPointer].data_length = 0;
                
                for (i = 0; i < CAN_RMessage->data_length; i++) 
                {
                    CAN_RMessage->data[i] = g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_RPointer].data[i];
                    g_CANx_SendBuffer.ECU_SendBuff[g_CANx_SendBuffer.ECU_SendBuff_RPointer].data[i] = 0;    
                }
            }
            
            g_CANx_SendBuffer.ECU_SendBuff_RPointer++;
            
            return 0;
        } 
        else 
        {
            return -1;
        }    
    } 
    else 
    {
        if (g_CANx_SendBuffer.Charger_SendBuff_RPointer >= CHARGER_SENDBUF_SIZE) 
        {
            g_CANx_SendBuffer.Charger_SendBuff_RPointer = 0;
        }
        
        if (g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_RPointer].frametype != (MSCAN_FrameAndIDTypeDef)0) 
        {
            CAN_RMessage->frametype = g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_RPointer].frametype;
            g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_RPointer].frametype = (MSCAN_FrameAndIDTypeDef)0;
            
            CAN_RMessage->frame_id  = g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_RPointer].frame_id;
            g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_RPointer].frame_id = 0;
            
            if ((CAN_RMessage->frametype == DataFrameWithStandardId)
             || (CAN_RMessage->frametype == DataFrameWithExtendedId)) 
            {
                CAN_RMessage->data_length = g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_RPointer].data_length;
                g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_RPointer].data_length = 0;
                
                for (i = 0; i < CAN_RMessage->data_length; i++) 
                {
                    CAN_RMessage->data[i] = g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_RPointer].data[i];
                    g_CANx_SendBuffer.Charger_SendBuff[g_CANx_SendBuffer.Charger_SendBuff_RPointer].data[i] = 0;    
                }
            }
            
            g_CANx_SendBuffer.Charger_SendBuff_RPointer++;
            
            return 0;
        } 
        else 
        {
            return -1;
        }     
    }
}

/*****************************END OF FILE**************************************/
