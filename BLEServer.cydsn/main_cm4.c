#include "project.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/***********************************MACROS*****************************************/

// bluetooth macros
#define ERR_INVALID_PDU 0X04
#define CCCD_VALID_BIT_MASK 0x03
#define NOTIFY_BIT_MASK 0x01
#define MY_IPC_CHANNEL 8u

/******************************GLOBAL VARIABLES***********************************/

QueueHandle_t bleQueueHandle;

bool isReceivePosition = true;
bool isUserPosStartNotification = false;
bool isBallPosStartNotification = false;

static cy_stc_ble_conn_handle_t connectionHandle;

/******************************FUNCTION DECLARATIONS***********************************/
void StackEventHandler(uint32 event, void * param);
void SendBleNotification(cy_ble_gatt_db_attr_handle_t charHandle, uint64* value);

/******************************CONST VARIABLES***********************************/

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
#define PLAYER_A_INITIAL_RATE_X 0.5
#define PLAYER_A_INITIAL_RATE_Y 0.8

// bounce effect const parameters
#define BALL_MASS 20
#define PLAYER_CIRCLE_MASS 30
#define BALL_SPEED_MAX_RATE 1.3
#define BALL_SPEED_MIN_RATE 0.05
#define BOUNCE_DECAY_RATE 0.5
#define BOUNCE_ACCELERATE_RATE 0.4

// other const parameters
#define USER_INDEX_MASK 1
#define USER_POSITION_MASK 0x0FFF
#define STD_SCREEN_WIDTH 1080
#define STD_SCREEN_HEIGHT 1920


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


/******************************FUNCTION DEFINATIONS***********************************/


// Game task
void GlowHockeyTask(){
   
    uint16 ballPositionX = (uint16) (tableWidth * BALL_INITIAL_RATE_X);
    uint16 ballPositionY = (uint16) (tableHeight * BALL_INITIAL_RATE_Y);
    int16 ballSpeedX = 0;
    int16 ballSpeedY = 0;
    
    uint16 player0X = (int) (tableWidth * PLAYER_A_INITIAL_RATE_X);
    uint16 player0Y = (int) (tableHeight * PLAYER_A_INITIAL_RATE_Y);
    uint16 player0XLast = player0X;
    uint16 player0YLast = player0Y;
    int16 player0SpeedX = 0;
    int16 player0SpeedY = 0;
    
    uint16 player1X;
    uint16 player1Y;
    int16 player1SpeedX;
    int16 player1SpeedY;
    
    uint32 receivedPosition;
    uint32 sendUserPosition;
    uint32 sendBallPosition;
    uint32 sendOppositePosition;
    uint64 bytesToSend;
    
    int counter = 1000;
    int buffer = 0;
    
    printf("gamePaddingBottom ,tableHeight , ballSize: %d, %d, %d\r\n", gamePaddingBottom ,tableHeight , ballSize);
    
    for(;;){
        if(isReceivePosition){

            // receive position
            xQueueReceive(bleQueueHandle, &receivedPosition, portTICK_PERIOD_MS);
            isReceivePosition = false;
            
            if(0 == (((uint8)(receivedPosition >> 24)) & USER_INDEX_MASK))
            {
                // player 0
                player0X = ((uint16) receivedPosition) & USER_POSITION_MASK;
                player0Y = ((uint16)(receivedPosition >> 12)) & USER_POSITION_MASK;
                //printf("User 0 : (%u, %u)\r\n", player0X, player0Y);
                
                player0SpeedX = player0X - player0XLast;
                player0SpeedY = player0Y - player0YLast;
                player0XLast = player0X;
                player0YLast = player0Y;
                
            }
            else{
                // player 1
                player1X = ((uint16) receivedPosition) & USER_POSITION_MASK;
                player1Y = ((uint16)(receivedPosition >> 12)) & USER_POSITION_MASK;
                printf("User 1 : (%u, %u)\r\n", player1X, player1Y);
            }
            
            // game logic
            
            
            
             // 小球左右边框碰撞事件
            if (ballPositionX - gamePaddingLeft <= ballSize) {
                ballSpeedX = abs(ballSpeedX);
            } else if (ballPositionX + gamePaddingRight >= tableWidth - ballSize) {
                ballSpeedX = - abs(ballSpeedX);
            }

            // 小球上下边框碰撞事件
            if (ballPositionY - gamePaddingTop <= ballSize) {
                // 己方得分
                if (ballPositionX > doorSideLeft && ballPositionX < doorSideRight) {
                    //scoreA += 1;
                    //printf("You Scored! current score : %d -- %d", scoreA, scoreB);
                    //if (scoreA >= 7) {
                    //    isOver = true;
                    //} else {
                        //isStart = false;
                    printf("You scored!!\r\n");
                    
                        ballPositionX = (int16) (tableWidth * BALL_INITIAL_RATE_X);
                        ballPositionY = (int16) (tableHeight * BALL_INITIAL_RATE_Y);
                        ballSpeedX = 0;
                        ballSpeedY = 0;
                        player0X = (int16) (tableWidth * PLAYER_A_INITIAL_RATE_X);
                        player0Y = (int16) (tableHeight * PLAYER_A_INITIAL_RATE_Y);
                        
                    //}
                }
                ballSpeedY = abs(ballSpeedY);
            } 
            else if (ballPositionY + gamePaddingBottom >= tableHeight - ballSize) {
                printf("Reach bottom.\r\n");
                // 对方得分
                if (ballPositionX > doorSideLeft && ballPositionX < doorSideRight) {
                    //scoreB += 1;
                    //Log.d(TAG, String.format("Your rival Scored! current score : %d -- %d", scoreA, scoreB));
                    //if (scoreB >= 7) {
                    //    isOver = true;
                    //} else {
                        //isStart = false;
                    printf("Your rival scored!!\r\n");
                    
                        ballPositionX = (int16) (tableWidth * BALL_INITIAL_RATE_X);
                        ballPositionY = (int16) (tableHeight * BALL_INITIAL_RATE_Y);
                        ballSpeedX = 0;
                        ballSpeedY = 0;
                        player0X = (int16) (tableWidth * PLAYER_A_INITIAL_RATE_X);
                        player0Y = (int16) (tableHeight * PLAYER_A_INITIAL_RATE_Y);
                        
                    //}
                }
                ballSpeedY = - abs(ballSpeedY);
            }


            // 小球和玩家碰撞事件
            double ball_playerA_dis = sqrt(
                    pow(ballPositionX - player0X, 2)
                            + pow(ballPositionY - player0Y, 2)
            );
            if (ball_playerA_dis < ballSize + playerCircleSize) {

                printf("Bounce!!\r\n");
                //if (!isStart) {
                //    isStart = true;
                //}
                printf("player speed: (%d, %d)\r\n", player0SpeedX, player0SpeedY);
                printf("speed before:(%d, %d)\r\n", ballSpeedX, ballSpeedX);

                ballSpeedX += (int16) (2 * PLAYER_CIRCLE_MASS * (player0SpeedX - ballSpeedX)
                        / (PLAYER_CIRCLE_MASS + BALL_MASS) * BOUNCE_DECAY_RATE);
                ballSpeedX += (int16) (ballPositionX - player0X) * BOUNCE_ACCELERATE_RATE *
                        (ballSize + playerCircleSize - ball_playerA_dis) / (ballSize + playerCircleSize);

                ballSpeedY += (int16) (2 * PLAYER_CIRCLE_MASS * (player0SpeedY - ballSpeedY)
                        / (PLAYER_CIRCLE_MASS + BALL_MASS) * BOUNCE_DECAY_RATE);
                ballSpeedY += (int16) (ballPositionY - player0Y) * BOUNCE_ACCELERATE_RATE *
                        (ballSize + playerCircleSize - ball_playerA_dis) / (ballSize + playerCircleSize);
                
                printf("speed after:(%d, %d)\r\n", ballSpeedX, ballSpeedY);
            }
            

            // 防止speed过大
            if (ballSpeedX > ballSize * BALL_SPEED_MAX_RATE) {
                ballSpeedX = (int16) (ballSize * BALL_SPEED_MAX_RATE);
            } else if (ballSpeedX < -ballSize * BALL_SPEED_MAX_RATE) {
                ballSpeedX = -(int16) (ballSize * BALL_SPEED_MAX_RATE);
            }

            if (ballSpeedY > ballSize * BALL_SPEED_MAX_RATE) {
                ballSpeedY = (int16) (ballSize * BALL_SPEED_MAX_RATE);
            } else if (ballSpeedY < -ballSize * BALL_SPEED_MAX_RATE) {
                ballSpeedY = -(int16) (ballSize * BALL_SPEED_MAX_RATE);
            }

            //防止speed过小
            //if (isStart) {
                if (ballSpeedX >= 0 && ballSpeedX < ballSize * BALL_SPEED_MIN_RATE) {
                    ballSpeedX = (int16) (ballSize * BALL_SPEED_MIN_RATE);
                } else if (ballSpeedX < 0 && ballSpeedX > -ballSize * BALL_SPEED_MIN_RATE) {
                    ballSpeedX = -(int16) (ballSize * BALL_SPEED_MIN_RATE);
                }
                if (ballSpeedY >= 0 && ballSpeedY < ballSize * BALL_SPEED_MIN_RATE) {
                    ballSpeedY = (int16) (ballSize * BALL_SPEED_MIN_RATE);
                } else if (ballSpeedY < 0 && ballSpeedY > -ballSize * BALL_SPEED_MIN_RATE) {
                    ballSpeedY = -(int16) (ballSize * BALL_SPEED_MIN_RATE);
                }
            //}

            ballPositionY += ballSpeedY;
            ballPositionX += ballSpeedX;
            
    
            // send user position
            
            if(0 == (((uint8)(receivedPosition >> 24)) & USER_INDEX_MASK))
            {
                // player 0
                sendUserPosition = 0;
                sendUserPosition |= (uint32)(player0X & USER_POSITION_MASK);
                sendUserPosition |= ((uint32)(player0Y & USER_POSITION_MASK) << 12);
                
                sendBallPosition = 0;
                sendBallPosition |= (uint32)(ballPositionX & USER_POSITION_MASK);
                sendBallPosition |= ((uint32)(ballPositionY & USER_POSITION_MASK) << 12);
                
                bytesToSend = 0;
                bytesToSend |= sendUserPosition;
                bytesToSend |= ((uint64)sendBallPosition) << 32;
                //printf("User 0 : (%u, %u)\r\n", player0X, player0Y);
                //printf("Ball : (%u, %u)\r\n", ballPositionX, ballPositionY);
                
                // if the notification is allowed , send the notification
                if((isUserPosStartNotification & NOTIFY_BIT_MASK)){
                    SendBleNotification(CY_BLE_GH_POSITION_PLAYER_POSITION_N_CHAR_HANDLE, &bytesToSend);
                }  
                else{
                    printf("Not send user 0 position.\r\n");
                }
                Cy_BLE_ProcessEvents();
            }
            else{
                // player 1

            }
        }
        else{
            taskYIELD();
        }
        
    }
}


// BLE task
void bleTask()
{
    uint32 brightVal;
    isUserPosStartNotification = false;
    isReceivePosition = false;
    bleQueueHandle = xQueueCreate(1, sizeof(brightVal));
 
    cy_en_ble_api_result_t apiResult;
    apiResult = Cy_BLE_Start(StackEventHandler);
    if(apiResult == CY_BLE_SUCCESS){
        printf("Core 4 BLE Start success.\r\n");
    }
 
    for(;;){
        Cy_BLE_ProcessEvents();
        taskYIELD();
    }
}

void StackEventHandler(uint32 event, void *param){
    
    cy_stc_ble_gatts_write_cmd_req_param_t* writeReqPara;
    cy_en_ble_api_result_t bleapiResult;
    uint32 val;
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
 
    switch((cy_en_ble_event_t) event){
 
        // if the ble stack on, start advertisement so other devices can find it
        case CY_BLE_EVT_STACK_ON:
        {
            printf("STACK_ON\r\n");
            bleapiResult = Cy_BLE_GAPP_StartAdvertisement(
                CY_BLE_ADVERTISING_FAST,
                CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX
            );
            break;
        }
        case CY_BLE_EVT_TIMEOUT:
        break;
 
        // if there is a device connection, store the connection handle 
        case CY_BLE_EVT_GATT_CONNECT_IND:
        {
            connectionHandle = *(cy_stc_ble_conn_handle_t*)param;
            break;
        }
 
        // if disconnected, start advertisement again
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
        {
            Cy_BLE_GAPP_StartAdvertisement(
                CY_BLE_ADVERTISING_FAST,
                CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX
            );
            break;
        }
 
        //if receive the "write request" from the gatt client
        case CY_BLE_EVT_GATTS_WRITE_REQ:
        {      
            writeReqPara = (cy_stc_ble_gatts_write_cmd_req_param_t*)param;
            // check if the handle is matching to capslider handle
            if(CY_BLE_GH_POSITION_PLAYER_POSITION_N_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE 
                == writeReqPara->handleValPair.attrHandle
            ){

                if(false == (writeReqPara->handleValPair.value.val[
                    CY_BLE_GH_POSITION_PLAYER_POSITION_N_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX
                    
                ] & (~CCCD_VALID_BIT_MASK))
                ){
                    // set the flag for application to know whether the notificatio is allowed
                    isUserPosStartNotification = writeReqPara->handleValPair.value.val[
                        CY_BLE_GH_POSITION_PLAYER_POSITION_N_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX
                    ];

                    cy_stc_ble_gatts_db_attr_val_info_t info = {
                        .handleValuePair = writeReqPara -> handleValPair,
                        .offset = false,
                        .connHandle = connectionHandle,
                        .flags = CY_BLE_GATT_DB_PEER_INITIATED
                    };
 
                    // update the gatt database
                    Cy_BLE_GATTS_WriteAttributeValue(&info);
                }
                else{
                    cy_stc_ble_gatt_err_info_t err_info;
                    err_info.opCode = CY_BLE_GATT_WRITE_REQ;
                    err_info.attrHandle = writeReqPara->handleValPair.attrHandle;
                    err_info.errorCode = ERR_INVALID_PDU;

                    cy_stc_ble_gatt_err_param_t errParam = {
                        .connHandle = connectionHandle,
                        .errInfo = err_info
                    };
                    Cy_BLE_GATTS_ErrorRsp(&errParam);
                }
            }
            
            /*
            
            else if(CY_BLE_GH_POSITION_BALL_POSITION_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE 
                == writeReqPara->handleValPair.attrHandle
            ){

                if(false == (writeReqPara->handleValPair.value.val[
                    CY_BLE_GH_POSITION_BALL_POSITION_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX
                    
                ] & (~CCCD_VALID_BIT_MASK))
                ){
                    // set the flag for application to know whether the notificatio is allowed
                    isBallPosStartNotification = writeReqPara->handleValPair.value.val[
                        CY_BLE_GH_POSITION_BALL_POSITION_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX
                    ];

                    cy_stc_ble_gatts_db_attr_val_info_t info = {
                        .handleValuePair = writeReqPara -> handleValPair,
                        .offset = false,
                        .connHandle = connectionHandle,
                        .flags = CY_BLE_GATT_DB_PEER_INITIATED
                    };
 
                    // update the gatt database
                    Cy_BLE_GATTS_WriteAttributeValue(&info);
                }
                else{
                    cy_stc_ble_gatt_err_info_t err_info;
                    err_info.opCode = CY_BLE_GATT_WRITE_REQ;
                    err_info.attrHandle = writeReqPara->handleValPair.attrHandle;
                    err_info.errorCode = ERR_INVALID_PDU;

                    cy_stc_ble_gatt_err_param_t errParam = {
                        .connHandle = connectionHandle,
                        .errInfo = err_info
                    };
                    Cy_BLE_GATTS_ErrorRsp(&errParam);
                }
            }
            */
            
            
            //send response 
            Cy_BLE_GATTS_WriteRsp(connectionHandle);
            break;
        }
        
        case CY_BLE_EVT_GATTS_WRITE_CMD_REQ:
            isReceivePosition = true;
            cy_stc_ble_gatt_handle_value_pair_t tmp = ((cy_stc_ble_gatts_write_cmd_req_param_t*)param)->handleValPair;
            xQueueSendFromISR(bleQueueHandle, (int32*)tmp.value.val, &xHigherPriorityTaskWoken);
            break;
        
        case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
            printf("READ_CHAR_VAL_ACCESS_REQ\r\n");
            break;
        
        default:
        {
            printf("DEFAULT : %u\r\n", event);
            break;
        }
    } 
    
}


void SendBleNotification(cy_ble_gatt_db_attr_handle_t charHandle, uint64* value){
    
    volatile cy_en_ble_api_result_t bleapiresult;
    cy_stc_ble_gatts_handle_value_ntf_t ntf;
 
    // fill the cy_stc_ble_gatts_handle_value_ntf_t struct 
    ntf.handleValPair.attrHandle = charHandle;
    ntf.handleValPair.value.val = (uint8*)value;
    ntf.handleValPair.value.len = sizeof(*value);
    ntf.connHandle = connectionHandle;
     
    // call the function to notify the client
    bleapiresult = Cy_BLE_GATTS_Notification(&ntf);
    if(bleapiresult == CY_BLE_SUCCESS){
        Cy_BLE_ProcessEvents();
    } 
}



int main(void)
{
    IPC_STRUCT_Type *myIpcHandle;
    
    __enable_irq(); /* Enable global interrupts. */
        
    /**************Components Startups***************/
    Cy_IPC_Sema_Init(CY_IPC_CHAN_SEMA, (uint32_t)NULL, (uint32_t*)NULL);
    UART_Start();
    CapSense_Start();
    //CapSense_ScanAllWidgets();
    
    /*******************Task creates**********************/
    
    xTaskCreate(bleTask, "ble task", configMINIMAL_STACK_SIZE * 20, 0, 3, 0);
    xTaskCreate(GlowHockeyTask, "Game task", configMINIMAL_STACK_SIZE * 7, 0, 3, 0);
    vTaskStartScheduler();

    for(;;)
    {
        /* Place your application code here. */
    }
   
}

/* [] END OF FILE */
