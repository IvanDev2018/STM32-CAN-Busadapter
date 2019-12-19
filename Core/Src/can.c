/**
  ******************************************************************************
  * File Name          : CAN.c
  * Description        : This file provides code for the configuration
  *                      of the CAN instances.
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

/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "tim.h"
#include "usbd_cdc_if.h"

//CAN_TxHeaderTypeDef   TxHeader;
//uint8_t               TxData[8];
//uint32_t              TxMailbox;
CAN_RxHeaderTypeDef   RxHeader;
uint8_t               RxData[8];
uint8_t				  CAN1_Timestamp = 0;
uint8_t halfbyte_to_hexascii(uint8_t _halfbyte);
uint8_t hexascii_to_halfbyte(uint8_t _ascii);

#ifdef DEBUG
uint32_t CAN_Time = 0;
uint32_t CAN_Last_Time = 0;
uint32_t CAN_start_time, CAN_end_time;

uint32_t max_time(uint32_t s, uint32_t e);
#endif
/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  hcan.Instance = CAN;
  hcan.Init.Prescaler = 9;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN)
  {
  /* USER CODE BEGIN CAN_MspInit 0 */

  /* USER CODE END CAN_MspInit 0 */
    /* CAN clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN GPIO Configuration    
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN_RX0_IRQn);
  /* USER CODE BEGIN CAN_MspInit 1 */

  /* USER CODE END CAN_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN)
  {
  /* USER CODE BEGIN CAN_MspDeInit 0 */

  /* USER CODE END CAN_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();
  
    /**CAN GPIO Configuration    
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

    /* CAN interrupt Deinit */
  /* USER CODE BEGIN CAN:USB_LP_CAN_RX0_IRQn disable */
    /**
    * Uncomment the line below to disable the "USB_LP_CAN_RX0_IRQn" interrupt
    * Be aware, disabling shared interrupt may affect other IPs
    */
    /* HAL_NVIC_DisableIRQ(USB_LP_CAN_RX0_IRQn); */
  /* USER CODE END CAN:USB_LP_CAN_RX0_IRQn disable */

  /* USER CODE BEGIN CAN_MspDeInit 1 */

  /* USER CODE END CAN_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
HAL_StatusTypeDef CAN_Config(uint32_t mode){
	CAN_FilterTypeDef sFilterConfig;
	HAL_StatusTypeDef result = HAL_OK;

	hcan.Instance = CAN;
	//hcan.Init.Prescaler = 16;
	hcan.Init.Mode = CAN_MODE_NORMAL;
	hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
	//hcan.Init.TimeSeg1 = CAN_BS1_1TQ;
	//hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
	hcan.Init.TimeTriggeredMode = DISABLE;
	hcan.Init.AutoBusOff = DISABLE;
	hcan.Init.AutoWakeUp = DISABLE;
	hcan.Init.AutoRetransmission = DISABLE;
	hcan.Init.ReceiveFifoLocked = DISABLE;
	hcan.Init.TransmitFifoPriority = DISABLE;

	if (HAL_CAN_Init(&hcan) != HAL_OK)
	{
		result = HAL_ERROR;
	}

	sFilterConfig.FilterBank = 0;
#if 1
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
#else
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
	sFilterConfig.FilterIdHigh = 0x00A0<<5;
	sFilterConfig.FilterIdLow = Id<<5;
	sFilterConfig.FilterMaskIdHigh = 0xFFF0<<5;
	sFilterConfig.FilterMaskIdLow = 0xFFF0<<5;
#endif
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 4;

	if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK) result = HAL_ERROR;

	if (HAL_CAN_Start(&hcan) != HAL_OK) result = HAL_ERROR;

	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK) result = HAL_ERROR;
	//if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING)!=HAL_OK) Error_Handler();
	//if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_ERROR)!=HAL_OK) Error_Handler();

	return result;
}

HAL_StatusTypeDef CAN_SetBitrate(uint8_t bitrate){
	switch (bitrate){
	case '0': // Setup 10Kbit
		hcan.Init.Prescaler = 225;
		hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
		hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
		break;
	case '1': // Setup 20Kbit
		hcan.Init.Prescaler = 90;
		hcan.Init.TimeSeg1 = CAN_BS1_16TQ;
		hcan.Init.TimeSeg2 = CAN_BS2_3TQ;
		break;
	case '2': // Setup 50Kbit
		hcan.Init.Prescaler = 45;
		hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
		hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
		break;
	case '3': // Setup 100Kbit
		hcan.Init.Prescaler = 18;
		hcan.Init.TimeSeg1 = CAN_BS1_16TQ;
		hcan.Init.TimeSeg2 = CAN_BS2_3TQ;
		break;
	case '4': // Setup 125Kbit
		hcan.Init.Prescaler = 18;
		hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
		hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
		break;
	case '5': // Setup 250Kbit
		hcan.Init.Prescaler = 9;
		hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
		hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
		break;
	case '6': // Setup 500Kbit
		hcan.Init.Prescaler = 9;
		hcan.Init.TimeSeg1 = CAN_BS1_5TQ;
		hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
		break;
//	case 7: // Setup 800Kbit
//		hcan1.Init.Prescaler = 5;
//		hcan1.Init.TimeSeg1 = CAN_BS1_7TQ;
//		hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
//		break;
//	case 8: // Setup 1Mbit
//		hcan1.Init.Prescaler = 2;
//		hcan1.Init.TimeSeg1 = CAN_BS1_15TQ;
//		hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
//		break;
	default:
		return HAL_ERROR;
		break;
	}

	return HAL_OK;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
#ifdef DEBUG
	CAN_start_time = HAL_GetTick();
#endif
  /* Get RX message */
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) Error_Handler();

  uint32_t len = 0;
  uint8_t buf[50];
  static uint32_t time;
  time = __HAL_TIM_GET_COUNTER(&htim15);
//  len = sprintf((char*)buf, "t%3.3lX%1.1lX%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X%4.4lX\r",
//		  	  	  RxHeader.StdId, RxHeader.DLC,
//				  RxData[0], RxData[1], RxData[2], RxData[3], RxData[4], RxData[5], RxData[6], RxData[7],
//				  time);
  buf[len++] = 't';
  buf[len++] = halfbyte_to_hexascii((RxHeader.StdId)>>8);
  buf[len++] = halfbyte_to_hexascii((RxHeader.StdId)>>4);
  buf[len++] = halfbyte_to_hexascii((RxHeader.StdId));
  buf[len++] = halfbyte_to_hexascii((RxHeader.DLC));
  for (uint8_t i=0;i<8;i++){
	  buf[len++] = halfbyte_to_hexascii((RxData[i])>>4);
	  buf[len++] = halfbyte_to_hexascii((RxData[i]));
  }
//  buf[len++] = halfbyte_to_hexascii((RxData[0])>>4);
//  buf[len++] = halfbyte_to_hexascii((RxData[0]));
//  buf[len++] = halfbyte_to_hexascii((RxData[1])>>4);
//  buf[len++] = halfbyte_to_hexascii((RxData[1]));
//  buf[len++] = halfbyte_to_hexascii((RxData[2])>>4);
//  buf[len++] = halfbyte_to_hexascii((RxData[2]));
//  buf[len++] = halfbyte_to_hexascii((RxData[3])>>4);
//  buf[len++] = halfbyte_to_hexascii((RxData[3]));
//  buf[len++] = halfbyte_to_hexascii((RxData[4])>>4);
//  buf[len++] = halfbyte_to_hexascii((RxData[4]));
//  buf[len++] = halfbyte_to_hexascii((RxData[5])>>4);
//  buf[len++] = halfbyte_to_hexascii((RxData[5]));
//  buf[len++] = halfbyte_to_hexascii((RxData[6])>>4);
//  buf[len++] = halfbyte_to_hexascii((RxData[6]));
//  buf[len++] = halfbyte_to_hexascii((RxData[7])>>4);
//  buf[len++] = halfbyte_to_hexascii((RxData[7]));
  if (CAN1_Timestamp){
	  buf[len++] = halfbyte_to_hexascii((time)>>12);
	  buf[len++] = halfbyte_to_hexascii((time)>>8);
	  buf[len++] = halfbyte_to_hexascii((time)>>4);
	  buf[len++] = halfbyte_to_hexascii((time)>>0);
  }
  buf[len++] = '\r';
  //CDC_Transmit_FS(buf, len);
  CDC_AddBuf_ToTransmit(buf, len);
#ifdef DEBUG
  CAN_end_time = HAL_GetTick();
  CAN_Last_Time = CAN_end_time - CAN_start_time;
  CAN_Time = max_time(CAN_Time, CAN_Last_Time);
#endif
}

uint8_t halfbyte_to_hexascii(uint8_t _halfbyte) {
	_halfbyte &= 0x0F;
	if (_halfbyte >= 10)
		return ('A' + _halfbyte - 10);
	else
		return ('0' + _halfbyte);
}

uint8_t hexascii_to_halfbyte(uint8_t _ascii) {
	if ((_ascii >= '0') && (_ascii <= '9'))
		return (_ascii - '0');
	if ((_ascii >= 'a') && (_ascii <= 'f'))
		return (_ascii - 'a' + 10);
	if ((_ascii >= 'A') && (_ascii <= 'F'))
		return (_ascii - 'A' + 10);
	return (0xFF);
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
