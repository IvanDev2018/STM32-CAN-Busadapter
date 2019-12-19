/**
  ******************************************************************************
  * File Name          : CAN.h
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __can_H
#define __can_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan;

/* USER CODE BEGIN Private defines */
//#define CANHACKER_SERIAL_RESPONSE     "N0001\r"
//#define CANHACKER_SW_VERSION_RESPONSE "v0107\r"
//#define CANHACKER_VERSION_RESPONSE    "V1010\r"

#define COMMAND_SET_BITRATE     'S' // Setup with standard CAN bit-rates
#define COMMAND_SET_BTR         's' // Setup with BTR0/BTR1 CAN bit-rates
#define COMMAND_OPEN_CAN_CHAN   'O' // Open the CAN channel in normal mode (sending & receiving)
#define COMMAND_LISTEN_ONLY     'L' // Open the CAN channel in listen only mode (receiving)
#define COMMAND_CLOSE_CAN_CHAN  'C' // Close the CAN channel
#define COMMAND_SEND_11BIT_ID   't' // Transmit a standard (11bit) CAN frame
#define COMMAND_SEND_29BIT_ID   'T' // Transmit an extended (29bit) CAN frame
#define COMMAND_SEND_R11BIT_ID  'r' // Transmit an standard RTR (11bit) CAN frame
#define COMMAND_SEND_R29BIT_ID  'R' // Transmit an extended RTR (29bit) CAN frame
//#define COMMAND_SEND_R29BIT_ID  'P' // Poll incomming FIFO for CAN frames (single poll)
//#define COMMAND_SEND_R29BIT_ID  'A' //Polls incomming FIFO for CAN frames (all pending frames)
#define COMMAND_READ_STATUS     'F' // Read Status Flags
#define COMMAND_SET_POOL_SEND   'X' // Sets Auto Poll/Send ON/OFF for received frames
#define COMMAND_WRITE_REG       'W' // Filter mode setting
#define COMMAND_SET_ACR         'M' // Sets Acceptance Code Register
#define COMMAND_SET_AMR         'm' // Sets Acceptance Mask Register
#define COMMAND_UART_BITRATE    'U' // Setup UART with a new baud rate
#define COMMAND_GET_VERSION     'V' // Get Version number of both CAN232 hardware and software
#define COMMAND_GET_SW_VERSION  'v' // Get software version only
#define COMMAND_GET_SERIAL      'N' // Get Serial number of the CAN232
#define COMMAND_TIME_STAMP      'Z' // Sets Time Stamp ON/OFF for received frames only
#define COMMAND_Startup_feature 'Q' // Auto Startup feature (from power on)
//#define COMMAND_READ_ECR        'E' // read Error Capture Register
//#define COMMAND_READ_ALCR       'A' // read Arbritation Lost Capture Register
//#define COMMAND_READ_REG        'G' // read register conten from SJA1000

//#define RESPONSE_OK		"\r"
//#define RESPONSE_ERROR	"\x7"
/* USER CODE END Private defines */

void MX_CAN_Init(void);

/* USER CODE BEGIN Prototypes */
HAL_StatusTypeDef CAN_Config(uint32_t mode);
HAL_StatusTypeDef CAN_SetBitrate(uint8_t bitrate);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ can_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
