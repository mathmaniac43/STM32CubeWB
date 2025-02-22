/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : App/app_thread.c
 * Description        : Thread Application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "utilities_common.h"
#include "app_entry.h"
#include "dbg_trace.h"
#include "app_thread.h"
#include "stm32wbxx_core_interface_def.h"
#include "openthread_api_wb.h"
#include "shci.h"
#include "stm_logging.h"
#include "app_conf.h"
#include "stm32_lpm.h"
#include "stm32_seq.h"
#if (CFG_USB_INTERFACE_ENABLE != 0)
#include "vcp.h"
#include "vcp_conf.h"
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/**
  * @brief  APP_THREAD Status structures definition
  */
typedef enum
{
  APP_THREAD_OK       = 0x00,
  APP_THREAD_ERROR    = 0x01,
} APP_THREAD_StatusTypeDef;
/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define C_SIZE_CMD_STRING       256U
#define C_PANID                 0x1111U
#define C_CHANNEL_NB            15U

/* USER CODE BEGIN PD */
#define C_RESSOURCE_FUOTA_PROVISIONING     "FUOTA_PROVISIONING"
#define C_RESSOURCE_FUOTA_PARAMETERS       "FUOTA_PARAMETERS"
#define C_RESSOURCE_FUOTA_SEND             "FUOTA_SEND"

#define FUOTA_NUMBER_WORDS_64BITS                50
#define FUOTA_PAYLOAD_SIZE                FUOTA_NUMBER_WORDS_64BITS * 8

#define LED_TOGGLE_TIMING                  (0.1*1000*1000/CFG_TS_TICK_VAL) /**< 0.5s */

typedef void (*CoapRespHandlerCallback) (otCoapHeader * pHeader, otMessage * pMessage,const otMessageInfo * pMessageInfo,otError Result);
/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
static void APP_THREAD_CheckWirelessFirmwareInfo(void);
static void APP_THREAD_DeviceConfig(void);
static void APP_THREAD_StateNotif(uint32_t NotifFlags, void *pContext);
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode);
#if (CFG_FULL_LOW_POWER == 0)
static void Send_CLI_To_M0(void);
#endif /* (CFG_FULL_LOW_POWER == 0) */
static void Send_CLI_Ack_For_OT(void);
static void HostTxCb( void );
static void Wait_Getting_Ack_From_M0(void);
static void Receive_Ack_From_M0(void);
static void Receive_Notification_From_M0(void);
#if (CFG_HW_LPUART1_ENABLED == 1)
extern void MX_LPUART1_UART_Init(void);
#endif
#if (CFG_HW_USART1_ENABLED == 1)
extern void MX_USART1_UART_Init(void);
#endif
#if (CFG_USB_INTERFACE_ENABLE != 0)
static uint32_t ProcessCmdString(uint8_t* buf , uint32_t len);
#else
#if (CFG_FULL_LOW_POWER == 0)
static void RxCpltCallback(void);
#endif /* (CFG_FULL_LOW_POWER == 0) */
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */

/* USER CODE BEGIN PFP */
static void Delete_Sectors( void );

static void APP_THREAD_DummyReqHandler(void                * p_context,
    otCoapHeader        * pHeader,
    otMessage           * pMessage,
    const otMessageInfo * pMessageInfo);

static void APP_THREAD_CoapReqHandlerFuotaProvisioning(otCoapHeader        * pHeader,
    otMessage           * pMessage,
    const otMessageInfo * pMessageInfo);
static void APP_THREAD_ProvisioningRespSend(otCoapHeader    * pRequestHeader,
    const otMessageInfo * pMessageInfo);

static void APP_THREAD_CoapReqHandlerFuota(otCoapHeader        * pHeader,
    otMessage           * pMessage,
    const otMessageInfo * pMessageInfo);
static void APP_THREAD_CoapSendDataResponseFuota(otCoapHeader    * pRequestHeader,
    const otMessageInfo * pMessageInfo);

static void APP_THREAD_CoapReqHandlerFuotaParameters(otCoapHeader * pHeader,
    otMessage            * pMessage,
    const otMessageInfo  * pMessageInfo);
static void APP_THREAD_CoapSendRespFuotaParameters(otCoapHeader    * pRequestHeader,
    const otMessageInfo * pMessageInfo,
    uint8_t * pData);
static void APP_THREAD_PerformReset(void);
static void APP_THREAD_TimingElapsed(void);
static APP_THREAD_StatusTypeDef APP_THREAD_CheckDeviceCapabilities(void);
/* USER CODE END PFP */

/* Private variables -----------------------------------------------*/
#if (CFG_USB_INTERFACE_ENABLE != 0)
static uint8_t TmpString[C_SIZE_CMD_STRING];
static uint8_t VcpRxBuffer[sizeof(TL_CmdSerial_t)];        /* Received Data over USB are stored in this buffer */
static uint8_t VcpTxBuffer[sizeof(TL_EvtPacket_t) + 254U]; /* Transmit buffer over USB */
#else
#if (CFG_FULL_LOW_POWER == 0)
static uint8_t aRxBuffer[C_SIZE_CMD_STRING];
#endif /* (CFG_FULL_LOW_POWER == 0) */
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */

#if (CFG_FULL_LOW_POWER == 0)
static uint8_t CommandString[C_SIZE_CMD_STRING];
#endif /* (CFG_FULL_LOW_POWER == 0) */
static __IO uint16_t indexReceiveChar = 0;
static __IO uint16_t CptReceiveCmdFromUser = 0;

static TL_CmdPacket_t *p_thread_otcmdbuffer;
static TL_EvtPacket_t *p_thread_notif_M0_to_M4;
static __IO uint32_t  CptReceiveMsgFromM0 = 0;
PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static TL_TH_Config_t ThreadConfigBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t ThreadOtCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ThreadNotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t ThreadCliCmdBuffer;

/* USER CODE BEGIN PV */
static otCoapResource OT_RessourceFuotaProvisioning = {C_RESSOURCE_FUOTA_PROVISIONING, APP_THREAD_DummyReqHandler, (void*)APP_THREAD_CoapReqHandlerFuotaProvisioning, NULL};
static otCoapResource OT_RessourceFuotaParameters = {C_RESSOURCE_FUOTA_PARAMETERS, APP_THREAD_DummyReqHandler, (void*)APP_THREAD_CoapReqHandlerFuotaParameters, NULL};
static otCoapResource OT_RessourceFuotaSend = {C_RESSOURCE_FUOTA_SEND, APP_THREAD_DummyReqHandler, (void*)APP_THREAD_CoapReqHandlerFuota, NULL};

static uint8_t OT_Command = 0;
static otMessageInfo OT_MessageInfo = {0};
static otCoapHeader  OT_Header = {0};
static otMessage* pOT_Message = NULL;

static uint8_t TimerID;
static uint32_t FuotaBinData_index = 0;
static uint64_t FuotaTransferArray[FUOTA_NUMBER_WORDS_64BITS] = {0};
static APP_THREAD_OtaContext_t OtaContext;
static uint32_t flash_current_offset = 0;
/* USER CODE END PV */

/* Functions Definition ------------------------------------------------------*/

void APP_THREAD_Init( void )
{
  /* USER CODE BEGIN APP_THREAD_INIT_1 */
  APP_DBG("Thread_Ota Application");

  /**
   * This is a safe clear in case the engi bytes are not all written
   * The error flag should be cleared before moving forward
   */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

  APP_DBG("Delete_Sectors");
  Delete_Sectors();
  /* USER CODE END APP_THREAD_INIT_1 */

  SHCI_CmdStatus_t ThreadInitStatus;
  
  /* Check the compatibility with the Coprocessor Wireless Firmware loaded */
  APP_THREAD_CheckWirelessFirmwareInfo();

#if (CFG_USB_INTERFACE_ENABLE != 0)
  VCP_Init(&VcpTxBuffer[0], &VcpRxBuffer[0]);
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */

  /* Register cmdbuffer */
  APP_THREAD_RegisterCmdBuffer(&ThreadOtCmdBuffer);

  /**
   * Do not allow standby in the application
   */
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP_THREAD, UTIL_LPM_DISABLE);

  /* Init config buffer and call TL_THREAD_Init */
  APP_THREAD_TL_THREAD_INIT();

  /* Configure UART for sending CLI command from M4 */
  APP_THREAD_Init_UART_CLI();

  /* Send Thread start system cmd to M0 */
  ThreadInitStatus = SHCI_C2_THREAD_Init();
  
  /* Prevent unused argument(s) compilation warning */
  UNUSED(ThreadInitStatus);

  /* Register task */
  /* Create the different tasks */
  UTIL_SEQ_RegTask( 1<<(uint32_t)CFG_TASK_MSG_FROM_M0_TO_M4, UTIL_SEQ_RFU, APP_THREAD_ProcessMsgM0ToM4);

  /* USER CODE BEGIN INIT TASKS */
  UTIL_SEQ_RegTask( 1<<(uint32_t)CFG_TASK_FUOTA_RESET, UTIL_SEQ_RFU, APP_THREAD_PerformReset);
  HW_TS_Create(CFG_TIM_PROC_ID_ISR, &TimerID, hw_ts_Repeated, APP_THREAD_TimingElapsed);
  /* USER CODE END INIT TASKS */

  /* Initialize and configure the Thread device*/
  APP_THREAD_DeviceConfig();

  /* USER CODE BEGIN APP_THREAD_INIT_2 */

  /* USER CODE END APP_THREAD_INIT_2 */
}

/**
  * @brief  Trace the error or the warning reported.
  * @param  ErrId :
  * @param  ErrCode
  * @retval None
  */
void APP_THREAD_Error(uint32_t ErrId, uint32_t ErrCode)
{
  /* USER CODE BEGIN APP_THREAD_Error_1 */

  /* USER CODE END APP_THREAD_Error_1 */
  switch(ErrId)
  {
  case ERR_REC_MULTI_MSG_FROM_M0 :
    APP_THREAD_TraceError("ERROR : ERR_REC_MULTI_MSG_FROM_M0 ", ErrCode);
    break;
  case ERR_THREAD_SET_STATE_CB :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_STATE_CB ",ErrCode);
    break;
  case ERR_THREAD_SET_CHANNEL :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_CHANNEL ",ErrCode);
    break;
  case ERR_THREAD_SET_PANID :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_PANID ",ErrCode);
    break;
  case ERR_THREAD_IPV6_ENABLE :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_IPV6_ENABLE ",ErrCode);
    break;
  case ERR_THREAD_START :
    APP_THREAD_TraceError("ERROR: ERR_THREAD_START ", ErrCode);
    break;
  case ERR_THREAD_ERASE_PERSISTENT_INFO :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_ERASE_PERSISTENT_INFO ",ErrCode);
    break;
  case ERR_THREAD_CHECK_WIRELESS :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_CHECK_WIRELESS ",ErrCode);
    break;
  /* USER CODE BEGIN APP_THREAD_Error_2 */
  case ERR_THREAD_COAP_START :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_START ",ErrCode);
    break;
  case ERR_THREAD_COAP_ADD_RESSOURCE :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_ADD_RESSOURCE ",ErrCode);
    break;
  case ERR_THREAD_MESSAGE_READ :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_MESSAGE_READ ",ErrCode);
    break;
  case ERR_THREAD_COAP_SEND_RESPONSE :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_SEND_RESPONSE ",ErrCode);
    break;
  case ERR_THREAD_COAP_APPEND :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_APPEND ",ErrCode);
    break;
  case ERR_THREAD_COAP_SEND_REQUEST :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_COAP_SEND_REQUEST ",ErrCode);
    break;
  case ERR_THREAD_MSG_COMPARE_FAILED:
    APP_THREAD_TraceError("ERROR : ERR_THREAD_MSG_COMPARE_FAILED ",ErrCode);
    break;
  /* USER CODE END APP_THREAD_Error_2 */
  default :
    APP_THREAD_TraceError("ERROR Unknown ", 0);
    break;
  }
}

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/**
 * @brief Thread initialization.
 * @param  None
 * @retval None
 */
static void APP_THREAD_DeviceConfig(void)
{
  otError error;
  error = otInstanceErasePersistentInfo(NULL);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_ERASE_PERSISTENT_INFO,error);
  }
  otInstanceFinalize(NULL);
  otInstanceInitSingle();
  error = otSetStateChangedCallback(NULL, APP_THREAD_StateNotif, NULL);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_STATE_CB,error);
  }
  error = otLinkSetChannel(NULL, C_CHANNEL_NB);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_CHANNEL,error);
  }
  error = otLinkSetPanId(NULL, C_PANID);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_SET_PANID,error);
  }
  error = otIp6SetEnabled(NULL, true);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_IPV6_ENABLE,error);
  }
  error = otThreadSetEnabled(NULL, true);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_START,error);
  }

  /* USER CODE BEGIN DEVICECONFIG */
  /* Start the COAP server */
  error = otCoapStart(NULL, OT_DEFAULT_COAP_PORT);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_COAP_START,error);
  }
  /* Add COAP resources */
  error = otCoapAddResource(NULL, &OT_RessourceFuotaProvisioning);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_COAP_ADD_RESSOURCE,error);
  }
  error = otCoapAddResource(NULL, &OT_RessourceFuotaParameters);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_COAP_ADD_RESSOURCE,error);
  }
  error = otCoapAddResource(NULL, &OT_RessourceFuotaSend);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error(ERR_THREAD_COAP_ADD_RESSOURCE,error);
  }
  /* USER CODE END DEVICECONFIG */
}

/**
 * @brief Thread notification when the state changes.
 * @param  aFlags  : Define the item that has been modified
 *         aContext: Context
 *
 * @retval None
 */
static void APP_THREAD_StateNotif(uint32_t NotifFlags, void *pContext)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(pContext);

  /* USER CODE BEGIN APP_THREAD_STATENOTIF */
  
  /* USER CODE END APP_THREAD_STATENOTIF */

  if ((NotifFlags & (uint32_t)OT_CHANGED_THREAD_ROLE) == (uint32_t)OT_CHANGED_THREAD_ROLE)
  {
    switch (otThreadGetDeviceRole(NULL))
    {
    case OT_DEVICE_ROLE_DISABLED:
      /* USER CODE BEGIN OT_DEVICE_ROLE_DISABLED */
      BSP_LED_Off(LED2);
      BSP_LED_Off(LED3);
      /* USER CODE END OT_DEVICE_ROLE_DISABLED */
      break;
    case OT_DEVICE_ROLE_DETACHED:
      /* USER CODE BEGIN OT_DEVICE_ROLE_DETACHED */
      BSP_LED_Off(LED2);
      BSP_LED_Off(LED3);
      /* USER CODE END OT_DEVICE_ROLE_DETACHED */
      break;
    case OT_DEVICE_ROLE_CHILD:
      /* USER CODE BEGIN OT_DEVICE_ROLE_CHILD */
      BSP_LED_Off(LED2);
      BSP_LED_On(LED3);
      /* USER CODE END OT_DEVICE_ROLE_CHILD */
      break;
    case OT_DEVICE_ROLE_ROUTER :
      /* USER CODE BEGIN OT_DEVICE_ROLE_ROUTER */
      BSP_LED_Off(LED2);
      BSP_LED_On(LED3);
      /* USER CODE END OT_DEVICE_ROLE_ROUTER */
      break;
    case OT_DEVICE_ROLE_LEADER :
      /* USER CODE BEGIN OT_DEVICE_ROLE_LEADER */
      BSP_LED_On(LED2);
      BSP_LED_Off(LED3);
      /* USER CODE END OT_DEVICE_ROLE_LEADER */
      break;
    default:
      /* USER CODE BEGIN DEFAULT */
      BSP_LED_Off(LED2);
      BSP_LED_Off(LED3);
      /* USER CODE END DEFAULT */
      break;
    }
  }
}

/**
  * @brief  Warn the user that an error has occurred.In this case,
  *         the LEDs on the Board will start blinking.
  *
  * @param  pMess  : Message associated to the error.
  * @param  ErrCode: Error code associated to the module (OpenThread or other module if any)
  * @retval None
  */
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode)
{
  /* USER CODE BEGIN TRACE_ERROR */
  APP_DBG("**** Fatal error = %s (Err = %d)", pMess, ErrCode);
  while(1U == 1U)
  {
    BSP_LED_Toggle(LED1);
    HAL_Delay(500U);
    BSP_LED_Toggle(LED2);
    HAL_Delay(500U);
    BSP_LED_Toggle(LED3);
    HAL_Delay(500U);
  }
  /* USER CODE END TRACE_ERROR */
}

/**
 * @brief Check if the Coprocessor Wireless Firmware loaded supports Thread
 *        and display associated informations
 * @param  None
 * @retval None
 */
static void APP_THREAD_CheckWirelessFirmwareInfo(void)
{
  WirelessFwInfo_t wireless_info_instance;
  WirelessFwInfo_t* p_wireless_info = &wireless_info_instance;

  if (SHCI_GetWirelessFwInfo(p_wireless_info) != SHCI_Success)
  {
    APP_THREAD_Error((uint32_t)ERR_THREAD_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
  }
  else
  {
    APP_DBG("**********************************************************");
    APP_DBG("WIRELESS COPROCESSOR FW:");
    /* Print version */
    APP_DBG("VERSION ID = %d.%d.%d", p_wireless_info->VersionMajor, p_wireless_info->VersionMinor, p_wireless_info->VersionSub);

    switch(p_wireless_info->StackType)
    {
    case INFO_STACK_TYPE_THREAD_FTD :
      APP_DBG("FW Type : Thread FTD");
      break;
    case INFO_STACK_TYPE_THREAD_MTD :
      APP_DBG("FW Type : Thread MTD");
      break;
    case INFO_STACK_TYPE_BLE_THREAD_FTD_STATIC :
      APP_DBG("FW Type : Static Concurrent Mode BLE/Thread");
      break;
    default :
      /* No Thread device supported ! */
      APP_THREAD_Error((uint32_t)ERR_THREAD_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
      break;
    }
    APP_DBG("**********************************************************");
  }
}
/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
static void Delete_Sectors( void )
{
  /**
   * The number of sectors to erase is read from SRAM1.
   * It shall be checked whether the number of sectors to erase does not overlap on the secured Flash
   * The limit can be read from the SFSA option byte which provides the first secured sector address.
   */

  uint32_t page_error;
  FLASH_EraseInitTypeDef p_erase_init;
  uint32_t first_secure_sector_idx;

  first_secure_sector_idx = (READ_BIT(FLASH->SFR, FLASH_SFR_SFSA) >> FLASH_SFR_SFSA_Pos);

  p_erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
  p_erase_init.Page = *((uint8_t*) SRAM1_BASE + 1);
  if(p_erase_init.Page < (CFG_APP_START_SECTOR_INDEX - 1))
  {
    /**
     * Something has been wrong as there is no case we should delete the BLE_Ota application
     * Reboot on the firmware application
     */
    *(uint8_t*)SRAM1_BASE = CFG_REBOOT_ON_FW_APP;
    NVIC_SystemReset();
  }
  p_erase_init.NbPages = *((uint8_t*) SRAM1_BASE + 2);

  if ((p_erase_init.Page + p_erase_init.NbPages) > first_secure_sector_idx)
  {
    p_erase_init.NbPages = first_secure_sector_idx - p_erase_init.Page;
  }

  APP_DBG("SFSA Option Bytes set to sector = %d (0x080%x)", first_secure_sector_idx, first_secure_sector_idx*4096);
  APP_DBG("Erase FLASH Memory from sector %d (0x080%x) to sector %d (0x080%x)", p_erase_init.Page, p_erase_init.Page*4096, p_erase_init.NbPages+p_erase_init.Page, (p_erase_init.NbPages+p_erase_init.Page)*4096);

  HAL_FLASH_Unlock();

  HAL_FLASHEx_Erase(&p_erase_init, &page_error);

  HAL_FLASH_Lock();

  return;
}

/**
 * @brief Dummy request handler
 *
 * @param None
 * @retval None
 */
static void APP_THREAD_DummyReqHandler(void            * p_context,
    otCoapHeader        * pHeader,
    otMessage           * pMessage,
    const otMessageInfo * pMessageInfo)
{
}

/**
 * @brief Handler called when the server receives a COAP request.
 *
 * @param pHeader : Header
 * @param pMessage : Message
 * @param pMessageInfo : Message information
 * @retval None
 */
static void APP_THREAD_CoapReqHandlerFuotaProvisioning(otCoapHeader * pHeader,
    otMessage            * pMessage,
    const otMessageInfo  * pMessageInfo)
{
  APP_DBG(" Received CoAP request on FUOTA_PROVISIONING ressource");

  if (otCoapHeaderGetType(pHeader) == OT_COAP_TYPE_NON_CONFIRMABLE &&
      otCoapHeaderGetCode(pHeader) == OT_COAP_CODE_GET)
  {
    OT_MessageInfo = *pMessageInfo;
    memset(&OT_MessageInfo.mSockAddr, 0, sizeof(OT_MessageInfo.mSockAddr));
    APP_THREAD_ProvisioningRespSend(pHeader, pMessageInfo);
  }
}

/**
 * @brief This function acknowledges the data reception by sending an ACK
 *    back to the sender.
 * @param  pRequestHeader coap header
 * @param  pMessageInfo message info pointer
 * @retval None
 */
static void APP_THREAD_ProvisioningRespSend(otCoapHeader    * pRequestHeader,
    const otMessageInfo * pMessageInfo)
{
  otError  error = OT_ERROR_NONE;

  do{
    APP_DBG("Provisiong: Send CoAP response");
    otCoapHeaderInit(&OT_Header, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_CONTENT);
    otCoapHeaderSetToken(&OT_Header,
        otCoapHeaderGetToken(pRequestHeader),
        otCoapHeaderGetTokenLength(pRequestHeader));
    otCoapHeaderSetPayloadMarker(&OT_Header);

    pOT_Message = otCoapNewMessage(NULL, &OT_Header);
    if (pOT_Message == NULL)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_APPEND_MSG, error);
      break;
    }

    error = otMessageAppend(pOT_Message, &OT_Command, sizeof(OT_Command));
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_APPEND_MSG, error);
    }

    error = otMessageAppend(pOT_Message, otThreadGetMeshLocalEid(NULL), sizeof(otIp6Address));
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_ALLOC_MSG, error);
      break;
    }

    error = otCoapSendResponse(NULL, pOT_Message, pMessageInfo);
    if (error != OT_ERROR_NONE && pOT_Message != NULL)
    {
      otMessageFree(pOT_Message);
      APP_THREAD_Error(ERR_THREAD_COAP_DATA_RESPONSE,error);
    }
  }while(false);
}

/**
 * @brief Handler called when the server receives a COAP request.
 *
 * @param pHeader : Header
 * @param pMessage : Message
 * @param pMessageInfo : Message information
 * @retval None
 */
static void APP_THREAD_CoapReqHandlerFuotaParameters(otCoapHeader * pHeader,
    otMessage            * pMessage,
    const otMessageInfo  * pMessageInfo)
{
  if (otMessageRead(pMessage, otMessageGetOffset(pMessage), &OtaContext, sizeof(OtaContext)) != sizeof(OtaContext))
  {
    APP_THREAD_Error(ERR_THREAD_MESSAGE_READ, 0);
  }

  /* Display Ota_Context values */
  if(OtaContext.file_type == APP_THREAD_OTA_FILE_TYPE_FW_APP)
  {
    APP_DBG("FUOTA_PARAMETERS: File Type set to : FW_APP");
  }
  else  if (OtaContext.file_type == APP_THREAD_OTA_FILE_TYPE_FW_COPRO_WIRELESS)
  {
    APP_DBG("FUOTA_PARAMETERS: File Type set to : FW_COPRO_WIRELESS");
  }
  else
  {
    APP_DBG("FUOTA_PARAMETERS: File Type not recognized");
    APP_THREAD_Error(ERR_THREAD_FUOTA_FILE_TYPE_NOT_RECOGNIZED, 0);
  }

  APP_DBG("FUOTA_PARAMETERS: Binary Size = 0x%x", OtaContext.binary_size);
  APP_DBG("FUOTA_PARAMETERS: Address = 0x%x", OtaContext.base_address);
  APP_DBG("FUOTA_PARAMETERS: Magic Keyword = 0x%x", OtaContext.magic_keyword);

  /* Check if Device can be updated with Fuota Server request */
  if (APP_THREAD_CheckDeviceCapabilities() == APP_THREAD_OK)
  {
    OT_Command = APP_THREAD_OK;
    HW_TS_Start(TimerID, (uint32_t)LED_TOGGLE_TIMING);
  }
  else
  {
    OT_Command = APP_THREAD_ERROR;
    APP_DBG("WARNING: Current Device capabilities cannot handle FUOTA. Check memory size available!");
  }
  /* If Message is Confirmable, send response */
  if (otCoapHeaderGetType(pHeader) == OT_COAP_TYPE_CONFIRMABLE)
  {
    APP_THREAD_CoapSendRespFuotaParameters(pHeader, pMessageInfo, &OT_Command);
  }
}

static APP_THREAD_StatusTypeDef APP_THREAD_CheckDeviceCapabilities(void)
{
  APP_THREAD_StatusTypeDef status = APP_THREAD_OK;

  /* Get Flash memory size available to copy binary from Server */
  uint32_t first_sector_idx;
  uint32_t first_secure_sector_idx;
  uint32_t free_sectors;
  uint32_t free_size;

  APP_DBG("Check Device capabilities");

  first_secure_sector_idx = (READ_BIT(FLASH->SFR, FLASH_SFR_SFSA) >> FLASH_SFR_SFSA_Pos);
  APP_DBG("SFSA Option Bytes set to sector = %d (0x080%x)", first_secure_sector_idx, first_secure_sector_idx*4096);

  first_sector_idx = *((uint8_t*) SRAM1_BASE + 1);
  if (first_sector_idx == 0)
  {
    APP_DBG("ERROR : SRAM1_BASE + 1 == 0");
    first_sector_idx = CFG_APP_START_SECTOR_INDEX;
  }
  APP_DBG("First available sector = %d (0x080%x)", first_sector_idx, first_sector_idx*4096);

  free_sectors = first_secure_sector_idx - first_sector_idx;
  free_size = free_sectors*4096;

  APP_DBG("free_sectors = %d , -> %d bytes of FLASH Free", free_sectors, free_size);

  APP_DBG("Server requests    : %d bytes", OtaContext.binary_size);
  APP_DBG("Client Free memory : %d bytes", free_size);

  if (free_size < OtaContext.binary_size)
  {
    status = APP_THREAD_ERROR;
    APP_DBG("WARNING: Not enough Free Flash Memory available to download binary from Server!");
  }
  else
  {
    APP_DBG("Device contains enough Flash Memory to download binary");
  }

  return status;
}

/**
 * @brief Handler called when the server receives a COAP request.
 *
 * @param pHeader : Header
 * @param pMessage : Message
 * @param pMessageInfo : Message information
 * @retval None
 */
static void APP_THREAD_CoapReqHandlerFuota(otCoapHeader * pHeader,
    otMessage            * pMessage,
    const otMessageInfo  * pMessageInfo)
{
  bool l_end_full_bin_transfer = FALSE;
  uint32_t flash_index = 0;
  uint64_t l_read64 = 0;

  if (otMessageRead(pMessage, otMessageGetOffset(pMessage), &FuotaTransferArray, FUOTA_PAYLOAD_SIZE) != FUOTA_PAYLOAD_SIZE)
  {
    APP_THREAD_Error(ERR_THREAD_MESSAGE_READ, 0);
  }

  /* Test if magic Keyword is in FuotaBinData */
  for (int index = 0; index < FUOTA_NUMBER_WORDS_64BITS; ++index) {
    if((FuotaTransferArray[index] & 0x00000000FFFFFFFF) == OtaContext.magic_keyword)
    {
      APP_DBG("1 - FUOTA_MAGIC_KEYWORD found at flash_current_offset = %d", (FuotaBinData_index + index)*8);
      l_end_full_bin_transfer = TRUE;
    }
    else
    if (((FuotaTransferArray[index] & 0xFFFFFFFF00000000) >> 32) == OtaContext.magic_keyword)
    {
      APP_DBG("2 - FUOTA_MAGIC_KEYWORD found at flash_current_offset = %d", (FuotaBinData_index + index)*8);
      l_end_full_bin_transfer = TRUE;
    }
  }

  FuotaBinData_index += FUOTA_NUMBER_WORDS_64BITS;

  /* Write to Flash Memory */
  for(flash_index = 0; flash_index < FUOTA_NUMBER_WORDS_64BITS; flash_index++)
  {
    while( LL_HSEM_1StepLock( HSEM, CFG_HW_FLASH_SEMID ) );
    HAL_FLASH_Unlock();
    while(LL_FLASH_IsActiveFlag_OperationSuspended());

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
        OtaContext.base_address + flash_current_offset,
        FuotaTransferArray[flash_index]) == HAL_OK)
    {
      /* Read back value for verification */
      l_read64 = *(uint64_t*)(OtaContext.base_address + flash_current_offset);
      if(l_read64 != FuotaTransferArray[flash_index])
      {
        APP_DBG("FLASH: Comparison failed l_read64 = 0x%jx / ram_array = 0x%jx", l_read64, FuotaTransferArray[flash_index])
                  APP_THREAD_Error(ERR_THREAD_MSG_COMPARE_FAILED,0);
      }
    }
    else
    {
      APP_DBG("HAL_FLASH_Program FAILED at flash_index = %d", flash_index)
      APP_THREAD_Error(ERR_THREAD_FLASH_PROGRAM,0);
    }

    HAL_FLASH_Lock();
    LL_HSEM_ReleaseLock( HSEM, CFG_HW_FLASH_SEMID, 0 );

    flash_current_offset += 8;
  }

  /* If Message is Confirmable, send response */
  if (otCoapHeaderGetType(pHeader) == OT_COAP_TYPE_CONFIRMABLE)
  {
    APP_THREAD_CoapSendDataResponseFuota(pHeader, pMessageInfo);
  }

  if(l_end_full_bin_transfer == TRUE)
  {
    UTIL_SEQ_SetTask(TASK_FUOTA_RESET, CFG_SCH_PRIO_0);
  }
}

/**
 * @brief This function acknowledges the data reception by sending an ACK
 *    back to the sender.
 * @param  pRequestHeader coap header
 * @param  pMessageInfo message info pointer
 * @retval None
 */
static void APP_THREAD_CoapSendDataResponseFuota(otCoapHeader    * pRequestHeader,
    const otMessageInfo * pMessageInfo)
{
  otError  error = OT_ERROR_NONE;
  static otCoapHeader  OT_Header = {0};

  do{
    otCoapHeaderInit(&OT_Header, OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_CHANGED);
    otCoapHeaderSetMessageId(&OT_Header, otCoapHeaderGetMessageId(pRequestHeader));
    otCoapHeaderSetToken(&OT_Header,
        otCoapHeaderGetToken(pRequestHeader),
        otCoapHeaderGetTokenLength(pRequestHeader));

    pOT_Message = otCoapNewMessage(NULL, &OT_Header);
    if (pOT_Message == NULL)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_APPEND_MSG, error);
      break;
    }

    error = otCoapSendResponse(NULL, pOT_Message, pMessageInfo);
    if (error != OT_ERROR_NONE && pOT_Message != NULL)
    {
      otMessageFree(pOT_Message);
      APP_THREAD_Error(ERR_THREAD_COAP_DATA_RESPONSE,error);
    }
  }while(false);
}

/**
 * @brief This function acknowledges the data reception by sending an ACK
 *    back to the sender.
 * @param  pRequestHeader coap header
 * @param  pMessageInfo message info pointer
 * @param  pMessage     message pointer
 * @retval None
 */
static void APP_THREAD_CoapSendRespFuotaParameters(otCoapHeader    * pRequestHeader,
    const otMessageInfo * pMessageInfo,
    uint8_t * pData)
{
  otError  error = OT_ERROR_NONE;
  static otCoapHeader  OT_Header = {0};
  uint8_t data = *pData;
  APP_DBG("APP_THREAD_CoapSendRespFuotaParameters data = %d", data);

  do{
    APP_DBG("FUOTA: Send CoAP response for Fuota Parameters");
    otCoapHeaderInit(&OT_Header, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_CONTENT);
    otCoapHeaderSetToken(&OT_Header,
        otCoapHeaderGetToken(pRequestHeader),
        otCoapHeaderGetTokenLength(pRequestHeader));
    otCoapHeaderSetPayloadMarker(&OT_Header);

    pOT_Message = otCoapNewMessage(NULL, &OT_Header);
    if (pOT_Message == NULL)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_APPEND_MSG, error);
      break;
    }

    error = otMessageAppend(pOT_Message, &data, sizeof(data));
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_COAP_APPEND_MSG, error);
    }

    error = otCoapSendResponse(NULL, pOT_Message, pMessageInfo);
    if (error != OT_ERROR_NONE && pOT_Message != NULL)
    {
      otMessageFree(pOT_Message);
      APP_THREAD_Error(ERR_THREAD_COAP_DATA_RESPONSE,error);
    }
  }while(false);
}

/**
 * @brief Task responsible for the reset at the end of OTA transfer.
 * @param  None
 * @retval None
 */
static void APP_THREAD_PerformReset(void)
{
  APP_DBG("*******************************************************");
  APP_DBG(" FUOTA_CLIENT : END OF TRANSFER COMPLETED");
  /* Stop Toggling of the LED */
  HW_TS_Stop(TimerID);
  BSP_LED_On(LED1);

  /* Insert delay to make sure CoAP response has been sent */
  HAL_Delay(200);

  if(OtaContext.file_type == APP_THREAD_OTA_FILE_TYPE_FW_APP)
  {
    APP_DBG("  --> Request to reboot on FW Application");
    APP_DBG("*******************************************************");
    /**
     * Reboot on FW Application
     */
    *(uint8_t*)SRAM1_BASE = CFG_REBOOT_ON_FW_APP;
    NVIC_SystemReset();
  }
  else if(OtaContext.file_type == APP_THREAD_OTA_FILE_TYPE_FW_COPRO_WIRELESS)
  {
    APP_DBG("  --> Request to reboot on FUS");
    APP_DBG("*******************************************************");
    /**
     * Wireless firmware update is requested
     * Request CPU2 to reboot on FUS by sending two FUS command
     */
    SHCI_C2_FUS_GetState( NULL );
    SHCI_C2_FUS_GetState( NULL );
    while(1)
    {
      HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    }
  }
  else
  {
    APP_DBG("APP_THREAD_PerformReset: OtaContext.file_type not recognized");
    APP_THREAD_Error(ERR_THREAD_FUOTA_FILE_TYPE_NOT_RECOGNIZED,0);
  }

}

static void APP_THREAD_TimingElapsed(void)
{
  BSP_LED_Toggle(LED1);
}

/* USER CODE END FD_LOCAL_FUNCTIONS */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

void APP_THREAD_RegisterCmdBuffer(TL_CmdPacket_t* p_buffer)
{
  p_thread_otcmdbuffer = p_buffer;
}

Thread_OT_Cmd_Request_t* THREAD_Get_OTCmdPayloadBuffer(void)
{
  return (Thread_OT_Cmd_Request_t*)p_thread_otcmdbuffer->cmdserial.cmd.payload;
}

Thread_OT_Cmd_Request_t* THREAD_Get_OTCmdRspPayloadBuffer(void)
{
  return (Thread_OT_Cmd_Request_t*)((TL_EvtPacket_t *)p_thread_otcmdbuffer)->evtserial.evt.payload;
}

Thread_OT_Cmd_Request_t* THREAD_Get_NotificationPayloadBuffer(void)
{
  return (Thread_OT_Cmd_Request_t*)(p_thread_notif_M0_to_M4)->evtserial.evt.payload;
}

/**
 * @brief  This function is used to transfer the Ot commands from the
 *         M4 to the M0.
 *
 * @param   None
 * @return  None
 */
void Ot_Cmd_Transfer(void)
{
  /* OpenThread OT command cmdcode range 0x280 .. 0x3DF = 352 */
  p_thread_otcmdbuffer->cmdserial.cmd.cmdcode = 0x280U;
  /* Size = otCmdBuffer->Size (Number of OT cmd arguments : 1 arg = 32bits so multiply by 4 to get size in bytes)
   * + ID (4 bytes) + Size (4 bytes) */
  uint32_t l_size = ((Thread_OT_Cmd_Request_t*)(p_thread_otcmdbuffer->cmdserial.cmd.payload))->Size * 4U + 8U;
  p_thread_otcmdbuffer->cmdserial.cmd.plen = l_size;

  TL_OT_SendCmd();

  /* Wait completion of cmd */
  Wait_Getting_Ack_From_M0();
}

/**
 * @brief  This function is called when acknowledge from OT command is received from the M0+.
 *
 * @param   Otbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_OT_CmdEvtReceived( TL_EvtPacket_t * Otbuffer )
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Otbuffer);

  Receive_Ack_From_M0();
}

/**
 * @brief  This function is called when notification from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_THREAD_NotReceived( TL_EvtPacket_t * Notbuffer )
{
  p_thread_notif_M0_to_M4 = Notbuffer;

  Receive_Notification_From_M0();
}

/**
  * @brief  This function is called before sending any ot command to the M0
  *         core. The purpose of this function is to be able to check if
  *         there are no notifications coming from the M0 core which are
  *         pending before sending a new ot command.
  * @param  None
  * @retval None
  */
void Pre_OtCmdProcessing(void)
{
  UTIL_SEQ_WaitEvt(EVENT_SYNCHRO_BYPASS_IDLE);
}

/**
  * @brief  This function waits for getting an acknowledgment from the M0.
  *
  * @param  None
  * @retval None
  */
static void Wait_Getting_Ack_From_M0(void)
{
  UTIL_SEQ_WaitEvt(EVENT_ACK_FROM_M0_EVT);
}

/**
  * @brief  Receive an acknowledgment from the M0+ core.
  *         Each command send by the M4 to the M0 are acknowledged.
  *         This function is called under interrupt.
  * @param  None
  * @retval None
  */
static void Receive_Ack_From_M0(void)
{
  UTIL_SEQ_SetEvt(EVENT_ACK_FROM_M0_EVT);
}

/**
  * @brief  Receive a notification from the M0+ through the IPCC.
  *         This function is called under interrupt.
  * @param  None
  * @retval None
  */
static void Receive_Notification_From_M0(void)
{
  CptReceiveMsgFromM0++;
  UTIL_SEQ_SetTask(TASK_MSG_FROM_M0_TO_M4,CFG_SCH_PRIO_0);
}

#if (CFG_USB_INTERFACE_ENABLE != 0)
#else
#if (CFG_FULL_LOW_POWER == 0)
static void RxCpltCallback(void)
{
  /* Filling buffer and wait for '\r' char */
  if (indexReceiveChar < C_SIZE_CMD_STRING)
  {
    CommandString[indexReceiveChar++] = aRxBuffer[0];
    if (aRxBuffer[0] == '\r')
    {
      CptReceiveCmdFromUser = 1U;

      /* UART task scheduling*/
      UTIL_SEQ_SetTask(1U << CFG_TASK_SEND_CLI_TO_M0, CFG_SCH_PRIO_0);
    }
  }

  /* Once a character has been sent, put back the device in reception mode */
  HW_UART_Receive_IT(CFG_CLI_UART, aRxBuffer, 1U, RxCpltCallback);
}
#endif /* (CFG_FULL_LOW_POWER == 0) */
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */

#if (CFG_USB_INTERFACE_ENABLE != 0)
/**
 * @brief Process the command strings.
 *        As soon as a complete command string has been received, the task
 *        in charge of sending the command to the M0 is scheduled
 * @param  None
 * @retval None
 */
static uint32_t  ProcessCmdString( uint8_t* buf , uint32_t len )
{
  uint32_t i,j,tmp_start;
  tmp_start = 0;
  uint32_t res = 0;

  i= 0;
  while ((buf[i] != '\r') && (i < len))
  {
    i++;
  }

  if (i != len)
  {
    memcpy(CommandString, buf,(i+1));
    indexReceiveChar = i + 1U; /* Length of the buffer containing the command string */
    UTIL_SEQ_SetTask(1U << CFG_TASK_SEND_CLI_TO_M0, CFG_SCH_PRIO_0);
    tmp_start = i;
    for (j = 0; j < (len - tmp_start - 1U) ; j++)
    {
      buf[j] = buf[tmp_start + j + 1U];
    }
    res = len - tmp_start - 1U;
  }
  else
  {
    res = len;
  }
  return res; /* Remaining characters in the temporary buffer */
}
#endif/* (CFG_USB_INTERFACE_ENABLE != 0) */

#if (CFG_FULL_LOW_POWER == 0)
/**
 * @brief Process sends receive CLI command to M0.
 * @param  None
 * @retval None
 */
static void Send_CLI_To_M0(void)
{
  memset(ThreadCliCmdBuffer.cmdserial.cmd.payload, 0x0U, 255U);
  memcpy(ThreadCliCmdBuffer.cmdserial.cmd.payload, CommandString, indexReceiveChar);
  ThreadCliCmdBuffer.cmdserial.cmd.plen = indexReceiveChar;
  ThreadCliCmdBuffer.cmdserial.cmd.cmdcode = 0x0;

  /* Clear receive buffer, character counter and command complete */
  CptReceiveCmdFromUser = 0;
  indexReceiveChar = 0;
  memset(CommandString, 0, C_SIZE_CMD_STRING);

  TL_CLI_SendCmd();
}
#endif /* (CFG_FULL_LOW_POWER == 0) */

/**
 * @brief Send notification for CLI TL Channel.
 * @param  None
 * @retval None
 */
static void Send_CLI_Ack_For_OT(void)
{

  /* Notify M0 that characters have been sent to UART */
  TL_THREAD_CliSendAck();
}

/**
 * @brief Perform initialization of CLI UART interface.
 * @param  None
 * @retval None
 */
void APP_THREAD_Init_UART_CLI(void)
{
#if (CFG_FULL_LOW_POWER == 0)
  UTIL_SEQ_RegTask( 1<<CFG_TASK_SEND_CLI_TO_M0, UTIL_SEQ_RFU,Send_CLI_To_M0);
#endif /* (CFG_FULL_LOW_POWER == 0) */

#if (CFG_USB_INTERFACE_ENABLE != 0)
#else
#if (CFG_FULL_LOW_POWER == 0)
  MX_USART1_UART_Init();
  HW_UART_Receive_IT(CFG_CLI_UART, aRxBuffer, 1, RxCpltCallback);
#endif /* (CFG_FULL_LOW_POWER == 0) */
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */
}

/**
 * @brief Perform initialization of TL for THREAD.
 * @param  None
 * @retval None
 */
void APP_THREAD_TL_THREAD_INIT(void)
{
  ThreadConfigBuffer.p_ThreadOtCmdRspBuffer = (uint8_t*)&ThreadOtCmdBuffer;
  ThreadConfigBuffer.p_ThreadNotAckBuffer = (uint8_t*)ThreadNotifRspEvtBuffer;
  ThreadConfigBuffer.p_ThreadCliRspBuffer = (uint8_t*)&ThreadCliCmdBuffer;

  TL_THREAD_Init( &ThreadConfigBuffer );
}

/**
 * @brief  This function is called when notification on CLI TL Channel from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_THREAD_CliNotReceived( TL_EvtPacket_t * Notbuffer )
{
  TL_CmdPacket_t* l_CliBuffer = (TL_CmdPacket_t*)Notbuffer;
  uint8_t l_size = l_CliBuffer->cmdserial.cmd.plen;

  /* WORKAROUND: if string to output is "> " then respond directly to M0 and do not output it */
  if (strcmp((const char *)l_CliBuffer->cmdserial.cmd.payload, "> ") != 0)
  {
    /* Write to CLI UART */
#if (CFG_USB_INTERFACE_ENABLE != 0)
    VCP_SendData( l_CliBuffer->cmdserial.cmd.payload, l_size, HostTxCb);
#else
    HW_UART_Transmit_IT(CFG_CLI_UART, l_CliBuffer->cmdserial.cmd.payload, l_size, HostTxCb);
#endif /*USAGE_OF_VCP */
  }
  else
  {
    Send_CLI_Ack_For_OT();
  }
}

/**
 * @brief  End of transfer callback for CLI UART sending.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void HostTxCb(void)
{
  Send_CLI_Ack_For_OT();
}

/**
 * @brief Process the messages coming from the M0.
 * @param  None
 * @retval None
 */
void APP_THREAD_ProcessMsgM0ToM4(void)
{
  if (CptReceiveMsgFromM0 != 0)
  {
    /* If CptReceiveMsgFromM0 is > 1. it means that we did not serve all the events from the radio */
    if (CptReceiveMsgFromM0 > 1U)
    {
      APP_THREAD_Error(ERR_REC_MULTI_MSG_FROM_M0, 0);
    }
    else
    {
      OpenThread_CallBack_Processing();
    }
    /* Reset counter */
    CptReceiveMsgFromM0 = 0;
  }
}

#if (CFG_USB_INTERFACE_ENABLE != 0)
/**
 * @brief  This function is called when thereare some data coming
 *         from the Hyperterminal via the USB port
 *         Data received over USB OUT endpoint are sent over CDC interface
 *         through this function.
 * @param  Buf: Buffer of data received
 * @param  Len: Number of data received (in bytes)
 * @retval Number of characters remaining in the buffer and not yet processed
 */
void VCP_DataReceived(uint8_t* Buf , uint32_t *Len)
{
  uint32_t i,flag_continue_checking = TRUE;
  uint32_t char_remaining = 0;
  static uint32_t len_total = 0;

  /* Copy the characteres in the temporary buffer */
  for (i = 0; i < *Len; i++)
  {
    TmpString[len_total++] = Buf[i];
  }

  /* Process the buffer commands one by one     */
  /* A command is limited by a \r caracaters    */
  while (flag_continue_checking == TRUE)
  {
    char_remaining = ProcessCmdString(TmpString,len_total);
    /* If char_remaining is equal to len_total, it means that the command string is not yet
     * completed.
     * If char_remaining is equal to 0, it means that the command string has
     * been entirely processed.
     */
    if ((char_remaining == 0) || (char_remaining == len_total))
    {
      flag_continue_checking = FALSE;
    }
    len_total = char_remaining;
  }
}
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */

/* USER CODE END FD_WRAP_FUNCTIONS */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
