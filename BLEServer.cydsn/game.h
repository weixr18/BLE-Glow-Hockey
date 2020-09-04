/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/***********************************MACROS*****************************************/


// canvas parameters
#define PADDING_LEFT_RATE 0.04
#define PADDING_RIGHT_RATE 0.04
#define PADDING_TOP_RATE 0.04
#define PADDING_BOTTOM_RATE 0.04

#define BALL_SIZE_RATIO 0.05
#define BALL_INITIAL_RATE_X 0.5
#define BALL_INITIAL_RATE_Y 0.5
#define DOOR_LEFT_RATE 0.35
#define DOOR_RIGHT_RATE 0.65
#define playerCircleSize_RATE 0.1
#define playerInnerCircleSize_RATE 0.072
#define PLAYER_0_INITIAL_RATE_X 0.5
#define PLAYER_0_INITIAL_RATE_Y 0.8
#define PLAYER_1_INITIAL_RATE_X 0.5
#define PLAYER_1_INITIAL_RATE_Y 0.2

// bounce effect const parameters
#define BALL_MASS 20
#define PLAYER_CIRCLE_MASS 20
#define BALL_SPEED_MAX_RATE 0.6
#define BALL_SPEED_MIN_RATE 0.05
#define BOUNCE_DECAY_RATE 0.2
#define BOUNCE_ACCELERATE_RATE 0.4

// other const parameters
#define USER_INDEX_MASK 1
#define USER_POSITION_MASK 0x0FFF
#define STD_SCREEN_WIDTH 1080u
#define STD_SCREEN_HEIGHT 1920u


/*
bool isOver false
bool isStart false
int scoreA 0
int scoreB 0
*/

#define tableWidth STD_SCREEN_WIDTH
#define tableHeight STD_SCREEN_HEIGHT

#define gamePaddingLeft ((int) (tableWidth * PADDING_LEFT_RATE))
#define gamePaddingRight ((int) (tableWidth * PADDING_RIGHT_RATE))
#define gamePaddingTop ((int) (tableWidth * PADDING_TOP_RATE))
#define gamePaddingBottom ((int) (tableWidth * PADDING_BOTTOM_RATE))

#define doorSideLeft ((int) (tableWidth * DOOR_LEFT_RATE))
#define doorSideRight ((int) (tableWidth * DOOR_RIGHT_RATE))
#define playerCircleSize ((int) (tableWidth * playerCircleSize_RATE))
#define playerInnerCircleSize ((int) (tableWidth * playerInnerCircleSize_RATE))
#define ballSize ((int) (tableWidth * BALL_SIZE_RATIO))


// bluetooth macros
#define ERR_INVALID_PDU 0X04
#define CCCD_VALID_BIT_MASK 0x03
#define NOTIFY_BIT_MASK 0x01
#define MY_IPC_CHANNEL 8u

// game command macros
#define GH_CC_W_PAUSE         0x02
#define GH_CC_W_RESUME        0x03
#define GH_CC_N_READY         0x10
#define GH_CC_N_START         0x11
#define GH_CC_N_PAUSE         0x12

#define GH_CC_N_RESUME        0x13
#define GH_CC_N_SCORE         0x14
#define GH_CC_N_OVER          0x15
#define GH_CC_N_NOTIFY_ID     0x18
#define GH_CC_N_NOTIFY_COLOR  0x19



/******************************STRUCT DEFINATIONS***********************************/

enum GameState{
    INIT_SERVER,
    WAIT_FOR_DEVICE_0,
    INIT_DEVICE_0,
    WAIT_FOR_DEVICE_1,
    INIT_DEVICE_1,
    GAME_START
};


/******************************GLOBAL VARIABLES***********************************/

QueueHandle_t bleQueueHandle;

bool isReceivePosition;
bool isUser_0_PosStartNotification;
bool isCommand_0_StartNotification;
bool isUser_1_PosStartNotification;
bool isCommand_1_StartNotification;

enum GameState gameState;

cy_stc_ble_conn_handle_t bleConnectionHandle0;
cy_stc_ble_conn_handle_t bleConnectionHandle1;

/***********************FUNCTION DECLARATIONS**************************/

void GlowHockeyTask();
void SendBleNotification(cy_ble_gatt_db_attr_handle_t charHandle, uint64* value, cy_stc_ble_conn_handle_t connHandle);
void delayTime(uint16 x);


/* [] END OF FILE */
