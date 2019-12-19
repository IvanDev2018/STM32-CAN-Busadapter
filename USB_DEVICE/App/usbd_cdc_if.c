/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v2.0_Cube
  * @brief          : Usb device for Virtual Com Port.
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
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */
#include "can.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
CAN_TxHeaderTypeDef   TxHeader;
uint8_t               TxData[8];
uint32_t              TxMailbox;
uint8_t				  CDC_LineCoding[7] = {0,0,0,0,0,0,0};

#ifdef DEBUG
uint32_t CDC_Time = 0;
uint32_t CDC_Last_Time = 0;
uint32_t CDC_start_time, CDC_end_time;

uint32_t max_time(uint32_t s, uint32_t e){
	if (e > s)
		return e;
	else
		return s;
}
#endif
/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* Define size for the receive and transmit buffer over CDC */
/* It's up to user to redefine and/or remove those define */
#define APP_RX_DATA_SIZE  1000
#define APP_TX_DATA_SIZE  1000
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */
uint32_t pWriteUserTxBufferFS = 0; //запись в буфер при приёме из CAN
uint32_t pReadUserTxBufferFS = 0;  //чтение из буфера, отправка в USB

const char CANHACKER_SERIAL_RESPONSE[] =     "N0001\r";
const char CANHACKER_SW_VERSION_RESPONSE[] = "v0107\r";
const char CANHACKER_VERSION_RESPONSE[] =    "V1010\r";

const char CANHACKER_OK_RESPONSE[] =	"\r";
const char CANHACKER_ERROR_RESPONSE[] =	"\x7";
/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */
extern uint8_t				  CAN1_Timestamp;
/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */
uint8_t halfbyte_to_hexascii(uint8_t _halfbyte);
uint8_t hexascii_to_halfbyte(uint8_t _ascii);
/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:
    	memcpy(&CDC_LineCoding, pbuf, 7);
    break;

    case CDC_GET_LINE_CODING:
    	memcpy(pbuf, &CDC_LineCoding, 7);
    break;

    case CDC_SET_CONTROL_LINE_STATE:

    break;

    case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will block any OUT packet reception on USB endpoint
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result
  *         in receiving more data while previous ones are still not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
#ifdef DEBUG
	CDC_start_time = HAL_GetTick();
#endif
  //uint32_t len = 1;
  //uint8_t result[10] = {0};
  //uint8_t idx = 1;
  //uint8_t tmp_byte;

  //HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
  //HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);

  switch (Buf[0]) {
  case COMMAND_GET_SERIAL:
	  //len = sprintf((char*)result, CANHACKER_SERIAL_RESPONSE);
	  //len = CDC_WriteResponse((uint8_t*)&result, CANHACKER_SERIAL_RESPONSE, 6);
	  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_SERIAL_RESPONSE[0], 6);
	  break;

  case COMMAND_GET_SW_VERSION:
      //len = sprintf((char*)result, CANHACKER_SW_VERSION_RESPONSE);
	  //len = CDC_WriteResponse((uint8_t*)&result, CANHACKER_SW_VERSION_RESPONSE, 6);
	  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_SW_VERSION_RESPONSE[0], 6);
      break;

  case COMMAND_GET_VERSION:
      //len = sprintf((char*)result, CANHACKER_VERSION_RESPONSE);
	  //len = CDC_WriteResponse((uint8_t*)&result, CANHACKER_VERSION_RESPONSE, 6);
	  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_VERSION_RESPONSE[0], 6);
      break;

  case COMMAND_SEND_11BIT_ID:
	  TxHeader.StdId = (hexascii_to_halfbyte(Buf[1])<<8)|(hexascii_to_halfbyte(Buf[2])<<4)|(hexascii_to_halfbyte(Buf[3]));
	  TxHeader.ExtId = 0;
	  TxHeader.RTR = CAN_RTR_DATA;
	  TxHeader.IDE = CAN_ID_STD;
	  TxHeader.DLC = hexascii_to_halfbyte(Buf[4]);
	  TxHeader.TransmitGlobalTime = DISABLE;
	  for (uint8_t i=0; i<TxHeader.DLC; i++)
		  TxData[i] = (hexascii_to_halfbyte(Buf[5+i*2])<<4)|(hexascii_to_halfbyte(Buf[6+i*2]));
	  if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK){
		  //len = sprintf((char*)result, RESPONSE_ERROR);
		  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_ERROR_RESPONSE[0], 1);
	  } else {
		  //len = sprintf((char*)result, RESPONSE_OK);
		  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_OK_RESPONSE[0], 1);
	  }
	  break;

//  case COMMAND_SEND_29BIT_ID:
//	  TxHeader.StdId = 0;
//	  TxHeader.ExtId = (hexascii_to_halfbyte(Buf[idx++])<<8)|(hexascii_to_halfbyte(Buf[idx++])<<4)|(hexascii_to_halfbyte(Buf[idx++]));
//	  TxHeader.RTR = CAN_RTR_DATA;
//	  TxHeader.IDE = CAN_ID_EXT;
//	  TxHeader.DLC = hexascii_to_halfbyte(Buf[idx++]);
//	  TxHeader.TransmitGlobalTime = DISABLE;
//	  for (uint8_t i=0; i<TxHeader.DLC; i++)
//		  TxData[i] = (hexascii_to_halfbyte(Buf[idx++])<<4)|(hexascii_to_halfbyte(Buf[idx++]));
//	  HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
//	  len = sprintf((char*)UserTxBufferFS, "\r");
//	  break;
//  case COMMAND_SEND_R11BIT_ID:
//	  TxHeader.StdId = (hexascii_to_halfbyte(Buf[idx++])<<8)|(hexascii_to_halfbyte(Buf[idx++])<<4)|(hexascii_to_halfbyte(Buf[idx++]));
//	  TxHeader.ExtId = 0;
//	  TxHeader.RTR = CAN_RTR_REMOTE;
//	  TxHeader.IDE = CAN_ID_STD;
//	  TxHeader.DLC = hexascii_to_halfbyte(Buf[idx++]);
//	  TxHeader.TransmitGlobalTime = DISABLE;
//	  HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
//	  len = sprintf((char*)UserTxBufferFS, RESPONSE_OK);
//	  break;
//  case COMMAND_SEND_R29BIT_ID:
//	  TxHeader.StdId = 0;
//	  TxHeader.ExtId = (hexascii_to_halfbyte(Buf[idx++])<<8)|(hexascii_to_halfbyte(Buf[idx++])<<4)|(hexascii_to_halfbyte(Buf[idx++]));
//	  TxHeader.RTR = CAN_RTR_REMOTE;
//	  TxHeader.IDE = CAN_ID_EXT;
//	  TxHeader.DLC = hexascii_to_halfbyte(Buf[idx++]);
//	  TxHeader.TransmitGlobalTime = DISABLE;
//	  HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
//	  len = sprintf((char*)UserTxBufferFS, "\r");
//	  break;

  case COMMAND_CLOSE_CAN_CHAN:
	  if (HAL_CAN_DeInit(&hcan) != HAL_OK){
		  //len = sprintf((char*)result, RESPONSE_ERROR);
		  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_ERROR_RESPONSE[0], 1);
	  } else {
		  //len = sprintf((char*)result, RESPONSE_OK);
		  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_OK_RESPONSE[0], 1);
		  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
	  }
	  break;

  case COMMAND_LISTEN_ONLY:
	  if (CAN_Config(CAN_MODE_SILENT) != HAL_OK){
		  //len = sprintf((char*)result, RESPONSE_ERROR);
		  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_ERROR_RESPONSE[0], 1);
	  } else {
		  //len = sprintf((char*)result, RESPONSE_OK);
		  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_OK_RESPONSE[0], 1);
		  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	  }
	  break;

  case COMMAND_OPEN_CAN_CHAN:
	  if (CAN_Config(CAN_MODE_NORMAL) != HAL_OK){
		  //len = sprintf((char*)result, RESPONSE_ERROR);
		  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_ERROR_RESPONSE[0], 1);
	  } else {
		  //len = sprintf((char*)result, RESPONSE_OK);
		  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_OK_RESPONSE[0], 1);
		  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	  }
	  break;

  case COMMAND_SET_BITRATE:
	  if (CAN_SetBitrate(Buf[1]) != HAL_OK){
		  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_ERROR_RESPONSE[0], 1);
	  } else {
		  //len = sprintf((char*)result, RESPONSE_OK);
		  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_OK_RESPONSE[0], 1);
	  }
	  break;
//
//  case COMMAND_SET_ACR:
//      return receiveSetAcrCommand(buffer, length);
//
//  case COMMAND_SET_AMR:
//      return receiveSetAmrCommand(buffer, length);
//
//  case COMMAND_SET_BTR:
//      if (isConnected()) {
//              writeStream(BEL);
//              writeDebugStream(F("SET_BTR command cannot be called while connected\n"));
//              return ERROR_CONNECTED;
//      }
//      writeDebugStream(F("SET_BTR not supported\n"));
//      return writeStream(CR);
//
  case COMMAND_TIME_STAMP:
	  CAN1_Timestamp = Buf[1] - '0';
	  //len = sprintf((char*)result, RESPONSE_OK);
	  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_OK_RESPONSE[0], 1);
	  break;
//
//  case COMMAND_WRITE_REG:
//  case COMMAND_READ_REG:
//      return writeStream(CR);
//
//  case COMMAND_READ_STATUS:
//  case COMMAND_READ_ECR:
//  case COMMAND_READ_ALCR:
//      if (!isConnected()) {
//              writeDebugStream(F("Read status, ecr, alcr while not connected\n"));
//              writeStream(BEL);
//              return ERROR_NOT_CONNECTED;
//      }
//      return writeStream(CR);

  default:
      //len = sprintf((char*)result, RESPONSE_OK);
	  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&CANHACKER_OK_RESPONSE[0], 1);
      break;
  }

  //USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)&result[0], len);
  USBD_CDC_TransmitPacket(&hUsbDeviceFS);


  //CDC_Transmit_FS(Buf, *Len);

  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
#ifdef DEBUG
  CDC_end_time = HAL_GetTick();
  CDC_Last_Time = CDC_end_time - CDC_start_time;
  CDC_Time = max_time(CDC_Time, CDC_Last_Time);
#endif
  return (USBD_OK);
  /* USER CODE END 6 */
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  /* USER CODE END 7 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
uint8_t CDC_AddBuf_ToTransmit(uint8_t* Buf, uint16_t Len) //добавление в буфер для передачи
{
	uint16_t _cnt = Len;

	while (_cnt) {
		UserTxBufferFS[pWriteUserTxBufferFS] = *Buf;
		pWriteUserTxBufferFS++;
		Buf++;
		pWriteUserTxBufferFS %= APP_TX_DATA_SIZE; //закольцовываем
		_cnt--;
	}
	return 0;
}

uint8_t CDC_Periodic_Callback(void) // периодически проверяем есть ли данные для передачи
{
	uint32_t buffptr;
	uint32_t buffsize;

	if (pReadUserTxBufferFS != pWriteUserTxBufferFS) {
		__disable_irq(); //при интенсивном обмене по CAN без этого всё «разваливается»
		if (pReadUserTxBufferFS > pWriteUserTxBufferFS){ // сделать кольцо?
			buffsize = APP_TX_DATA_SIZE	- pReadUserTxBufferFS; //этап 1, байт до конца массива
		} else {
			buffsize = pWriteUserTxBufferFS - pReadUserTxBufferFS; //все данные сразу
		}
		__enable_irq();

		buffptr = pReadUserTxBufferFS;

		//if (interface_state != 1)
		//	return (1); //если интерфейс не сконфигурирован, то отправлять не будем
		if (hcan.State != HAL_CAN_STATE_LISTENING)
			return 1;

		USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*) &UserTxBufferFS[buffptr], buffsize);

		if (USBD_CDC_TransmitPacket(&hUsbDeviceFS) == USBD_OK) {
			pReadUserTxBufferFS += buffsize;
			if (pReadUserTxBufferFS == APP_TX_DATA_SIZE) {
				pReadUserTxBufferFS = 0; //закольцовываем
			}
		}
	}
	return 0;
}
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
