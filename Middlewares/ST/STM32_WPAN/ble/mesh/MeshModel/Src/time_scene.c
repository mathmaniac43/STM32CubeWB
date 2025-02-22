/**
******************************************************************************
* @file    time_scene.c
* @author  BLE Mesh Team
* @version V1.10.000
* @date    15-Jan-2019
* @brief   Time and Scene model middleware file
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Initial BLE-Mesh is built over Motorola�s Mesh over Bluetooth Low Energy 
* (MoBLE) technology. The present solution is developed and maintained for both 
* Mesh library and Applications solely by STMicroelectronics.
*
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "hal_common.h"
#include "mesh_cfg.h"
#include "common.h"
#include <string.h>
#include "compiler.h"
#include "Math.h"
#include "time_scene.h"


/** @addtogroup Model_Callbacks
*  @{
*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

const MODEL_OpcodeTableParam_t Time_Scene_Opcodes_Table[] = {
  /*MOBLEUINT32 opcode, MOBLEBOOL reliable, MOBLEUINT16 min_payload_size, 
  MOBLEUINT16 max_payload_size;
  Here in this array, Handler is not defined; */
#ifdef ENABLE_TIME_MODEL_SERVER     
  {TIME_GET,                    MOBLE_TRUE,  0, 0,               TIME_STATUS ,             10, 10},
  {TIME_SET,                    MOBLE_TRUE,  10, 10,             TIME_STATUS ,             10, 10},  
  {TIME_ROLE_GET,               MOBLE_TRUE,  0, 0,               TIME_ROLL_STATUS ,        1, 1},    
  {TIME_ROLL_SET,               MOBLE_TRUE,  1, 1,               TIME_ROLL_STATUS ,        1, 1},
  {TIME_ZONE_GET,               MOBLE_TRUE,  0, 0,               TIME_ZONE_STATUS ,        7, 7},
  {TIME_ZONE_SET,               MOBLE_TRUE,  6, 6,               TIME_ZONE_STATUS ,        7, 7},
  {TAI_UTC_DELTA_GET,           MOBLE_TRUE,  0, 0,               TAI_UTC_DELTA_STATUS ,    9, 9},
  {TAI_UTC_DELTA_SET,           MOBLE_TRUE,  7, 7,               TAI_UTC_DELTA_STATUS ,    9, 9}, 
#endif
#ifdef ENABLE_SCENE_MODEL_SERVER
  {SCENE_GET,                   MOBLE_TRUE,  0, 0,               SCENE_STATUS ,            3, 6}, 
  {SCENE_RECALL,                MOBLE_TRUE,  3, 5,               SCENE_STATUS ,            3, 6},   
  {SCENE_RECALL_UNACK,          MOBLE_FALSE, 3, 5,               SCENE_STATUS ,            3, 6},     
  {SCENE_REGISTER_GET,          MOBLE_TRUE,  0, 0,               SCENE_REGISTER_STATUS ,   5, 8},
  {SCENE_STORE,                 MOBLE_TRUE,  2, 2,               SCENE_REGISTER_STATUS ,   5, 8},                  
  {SCENE_STORE_UNACK,           MOBLE_FALSE,  2, 2,              SCENE_REGISTER_STATUS ,   5, 8},
  {SCENE_DELETE,                MOBLE_TRUE,  2, 2,               SCENE_REGISTER_STATUS ,   5, 8},
  {SCENE_DELETE_UNACK,          MOBLE_FALSE, 2, 2,               SCENE_REGISTER_STATUS ,   5, 8},
#endif
  {0}
};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief  Time_SceneModelServer_GetOpcodeTableCb: This function is call-back 
          from the library to send Model Opcode Table info to library
* @param  MODEL_OpcodeTableParam_t:  Pointer to the Light Model opcode array 
* @param  length: Pointer to the Length of Light Model opcode array
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Time_SceneModelServer_GetOpcodeTableCb(const MODEL_OpcodeTableParam_t **data, 
                                               MOBLEUINT16 *length)
{
  *data = Time_Scene_Opcodes_Table;
  *length = sizeof(Time_Scene_Opcodes_Table)/sizeof(Time_Scene_Opcodes_Table[0]);
  
  return MOBLE_RESULT_SUCCESS;
}

/**
* @brief  Time_SceneModelServer_GetStatusRequestCb : This function is call-back 
          from the library to send response to the message from peer
* @param  peer_addr: Address of the peer
* @param  dst_peer: destination send by peer for this node. It can be a
*                                                     unicast or group address 
* @param  opcode: Received opcode of the Status message callback
* @param  pResponsedata: Pointer to the buffer to be updated with status
* @param  plength: Pointer to the Length of the data, to be updated by application
* @param  pRxData: Pointer to the data received in packet.
* @param  dataLength: length of the data in packet.
* @param  response: Value to indicate wheather message is acknowledged meassage or not.
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Time_SceneModelServer_GetStatusRequestCb(MOBLE_ADDRESS peer_addr, 
                                                  MOBLE_ADDRESS dst_peer, 
                                                  MOBLEUINT16 opcode, 
                                                  MOBLEUINT8 *pResponsedata, 
                                                  MOBLEUINT32 *plength, 
                                                  MOBLEUINT8 const *pRxData,
                                                  MOBLEUINT32 dataLength,
                                                  MOBLEBOOL response)
{
  switch(opcode)
  {
  case TIME_STATUS:
    {
      break;
    }
  case TIME_ROLL_STATUS:
    {
      break;
    }
  case TIME_ZONE_STATUS:
    {
      break;
    }
  case TAI_UTC_DELTA_STATUS:
    {
      break;
    }
  case SCENE_STATUS:
    {
      break;
    }
  case SCENE_REGISTER_STATUS:
    {
      break;
    }
    default:
    {
      break;
    }
  }
  return MOBLE_RESULT_SUCCESS;    
}

/**
* @brief  Time_SceneModelServer_ProcessMessageCb: This is a callback function from
          the library whenever a Light Model message is received
* @param  peer_addr: Address of the peer
* @param  dst_peer: destination send by peer for this node. It can be a
*                                                     unicast or group address 
* @param  opcode: Received opcode of the Status message callback
* @param  pData: Pointer to the buffer to be updated with status
* @param  length: Length of the parameters received 
* @param  response: if TRUE, the message is an acknowledged message 
* @param  pRxData: Pointer to the data received in packet.
* @param  dataLength: length of the data in packet.
* @param  response: Value to indicate wheather message is acknowledged meassage or not.
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Time_SceneModelServer_ProcessMessageCb(MOBLE_ADDRESS peer_addr, 
                                               MOBLE_ADDRESS dst_peer, 
                                               MOBLEUINT16 opcode, 
                                               MOBLEUINT8 const *pRxData, 
                                               MOBLEUINT32 dataLength, 
                                               MOBLEBOOL response
                                                 )
{  
  switch(opcode)
  {
  case TIME_SET:
    {
      break;
    }
  case TIME_ROLL_SET:
    {
      break;
    }
  case TIME_ZONE_SET:
    {
      break;
    }
  case TAI_UTC_DELTA_SET:
    {
      break;
    }     
  case SCENE_RECALL:
  case SCENE_RECALL_UNACK:
    {
      break;
    }
  case SCENE_STORE:
  case SCENE_STORE_UNACK:
    {
      break;
    }
  case SCENE_DELETE:
  case SCENE_DELETE_UNACK: 
    {
      break;
    }
  default:
    {
      break;
    }    
  } /* Switch ends */
 return MOBLE_RESULT_SUCCESS;
}


/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/

