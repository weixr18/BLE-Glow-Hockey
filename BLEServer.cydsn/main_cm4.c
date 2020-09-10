#include "project.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "game.h"
#include "I2CHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


unsigned char playerRed = 63;
unsigned char playerGreen = 237;
unsigned char playerBlue = 228;

double accelerateX = 0.0;
double accelerateY = 0.0;



typedef enum UartState{
    WAIT,
    LEAD_0,
    LEAD_1,
    RED,
    GREEN,
    BLUE
} UartState;




void delayTime(uint16 x){
    double temp = 12.87346;
    for(uint16 i = 0; i < x; i++){
        temp = sqrt(pow(98765.234, 0.5)+pow(temp, 0.8));
    }
}


uint32 PIOData = 1;


double doubleAbs(double x){
    if(x > 0.0){
        return x;
    }
    else{
        return -x;
    }
}

void outputPulse(int thisSound){
    
    static int LastSound = 6;
    int num;
    int i = 0;
    
    /*
    if(LastSound == 6){
        num = thisSound;
    }
    else{
        
    }
    */
    
    num = (6 - LastSound + thisSound) % 6;
    printf("OUTPUT: Last:%d, this: %d\r\n", LastSound, thisSound);
    LastSound = thisSound;
    
    
    
    for(;i < num; i++){
    
        PIOData = 0;
        delayTime(2000); 
        Cy_GPIO_Write(GPIO_PRT9, 0, PIOData);
        
        PIOData = 1;
        delayTime(3000);
        Cy_GPIO_Write(GPIO_PRT9, 0, PIOData);
    }   
}

void UartTask(void* arg){

    (void)arg;
    int count = 1;
    char charReceive;
    UartState state = WAIT;
    char buffer[3];
    
    printf("Starting UART Task.\r\n");
    setvbuf(stdin,0,_IONBF,0);
    
    while(1)
    {
        count = Cy_SCB_UART_GetNumInRxFifo(UART_HW);
        
        if(count > 0){
            charReceive = (uint32_t) getchar();
            
            if(state == WAIT){
                if(charReceive == (char)0x02){
                    state = LEAD_0;
                }
            }
            else if(state == LEAD_0){
                if(charReceive == (char)0x03){
                    state = LEAD_1;
                }
                else{
                    state = WAIT;
                }
            }
            else if(state == LEAD_1){
            
                buffer[0] = charReceive;
                state = RED;
            }
            else if(state == RED){
                buffer[1] = charReceive;
                state = GREEN;
            }
            else if(state == GREEN){
                buffer[2] = charReceive;
                state = BLUE;
                if(gameState == PLAYER_0_CHOOSE_COLOR || gameState == PLAYER_1_CHOOSE_COLOR){
                  
                    /*
                    msg = 0;
                    msg |= ((uint32_t)buffer[0]) << 8;      // R
                    msg |= ((uint32_t)buffer[1]) << 16;     // G
                    msg |= ((uint32_t)buffer[2]) << 24;     // B
                    xQueueSend(colorQueueHandle, &msg, portMAX_DELAY);
                    */
                    
                    playerRed = buffer[0];
                    playerGreen = buffer[1];
                    playerBlue = buffer[2];
                    
                    printf("UART: Send!\r\n");
                }

                state = WAIT;
            }

        }
        
        
        taskYIELD();    
    } 
}


// BLE task
void bleTask()
{
    
    int isUser_0_notified = 10;         // send 10 times to ensure client get it.
    int isUser_1_notified = 10;
    
    uint64 bytesToSend_0 = 0;
    uint64 bytesToSend_1 = 0;
    
    uint32_t receivedColor;
    uint32_t receivedColorLast;
    char buffer[3] = {0};
    
    isUser_0_PosStartNotification = false;
    isCommand_0_StartNotification = false;
    isReceivePosition = false;
    
    //colorQueueHandle = xQueueCreate(3, sizeof(uint32));
    positionQueueHandle = xQueueCreate(1, sizeof(uint32));
    
    cy_en_ble_api_result_t apiResult;
    apiResult = Cy_BLE_Start(StackEventHandler);
    if(apiResult == CY_BLE_SUCCESS){
        printf("Core 4 BLE start success.\r\n");
    }
 
    for(;;){
        
        if(gameState == PLAYER_0_CHOOSE_COLOR || gameState == PLAYER_1_CHOOSE_COLOR){
            //xQueueReceive(colorQueueHandle, &receivedColor, portTICK_PERIOD_MS);
            receivedColor = 0;
            receivedColor |= ((uint32_t) playerRed) << 8;
            receivedColor |= ((uint32_t) playerGreen) << 16;
            receivedColor |= ((uint32_t) playerBlue) << 24;

            // device 0 choose color
            if(gameState == PLAYER_0_CHOOSE_COLOR){

                if(receivedColor != receivedColorLast){
                    buffer[2] = (char)((receivedColor >> 24) & 0xff); //B
                    buffer[1] = (char)((receivedColor >> 16) & 0xff); //G
                    buffer[0] = (char)((receivedColor >> 8) & 0xff);  //R
                    
                    //printf("BLE: R %d G %d B %d\r\n", buffer[0], buffer[1], buffer[2]);
                    
                    uint64 operand = GH_CC_N_NOTIFY_COLOR;
                    bytesToSend_0 = (operand) & (uint64)0x000007ff;
                    bytesToSend_0 |= (uint64)(receivedColor & (uint32_t)0xffffff00);
                    
                    SendBleNotification(
                        CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                        (uint64*)&bytesToSend_0,
                        bleConnectionHandle0
                    );
                    Cy_BLE_ProcessEvents();
                    delayTime(500);
                    
                    bytesToSend_1 = bytesToSend_0;
                    SendBleNotification(
                        CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                        (uint64*)&bytesToSend_1,
                        bleConnectionHandle1
                    );
                    Cy_BLE_ProcessEvents();
                    delayTime(500);
                    
                    receivedColorLast = receivedColor;           
                }
                else{
                    Cy_BLE_ProcessEvents();
                }

            }
            
            // device 1 choose color
            else if(gameState == PLAYER_1_CHOOSE_COLOR){
                
                if(receivedColor != receivedColorLast){
                    buffer[2] = (char)((receivedColor >> 24) & 0xff);
                    buffer[1] = (char)((receivedColor >> 16) & 0xff);
                    buffer[0] = (char)((receivedColor >> 8) & 0xff);
                    
                    //printf("BLE: R %d G %d B %d\r\n", buffer[0], buffer[1], buffer[2]);
                    
                    uint64 operand = GH_CC_N_NOTIFY_COLOR;
                    bytesToSend_0 = (operand) & (uint64)0x000007ff;
                    bytesToSend_0 |= (uint64)(receivedColor & (uint32_t)0xffffff00);
                    
                    SendBleNotification(
                        CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                        (uint64*)&bytesToSend_0,
                        bleConnectionHandle0
                    );
                    Cy_BLE_ProcessEvents();
                    delayTime(500);
                    
                    bytesToSend_1 = bytesToSend_0;
                    SendBleNotification(
                        CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                        (uint64*)&bytesToSend_1,
                        bleConnectionHandle1
                    );
                    Cy_BLE_ProcessEvents();
                    delayTime(500);
                    
                    receivedColorLast = receivedColor;           
                }
                else{
                    Cy_BLE_ProcessEvents();
                }
            }
        
            taskYIELD();
        
        }
             
        // notify command for device 0
        else if((gameState == INIT_DEVICE_0) &&
            (isCommand_0_StartNotification & NOTIFY_BIT_MASK)){ 
            
            // send device ID
            if(isUser_0_notified > 0) {
                
                uint64 id = 0;
                uint64 operand = GH_CC_N_NOTIFY_ID;
                bytesToSend_0 = ((id << 5) | operand) & (uint64)0x000007ff;
                //printf("BLE: user 0. Notify ID. Bytes: %d\r\n", bytesToSend_0);
                
                SendBleNotification(
                    CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                    (uint64*)&bytesToSend_0,
                    bleConnectionHandle0
                );
                isUser_0_notified--;
                Cy_BLE_ProcessEvents();
                delayTime(1000);
            }
            
            // send "Ready" signal, change state
            if (isUser_0_notified == 0 && isUser_0_PosStartNotification){
                
                gameState = WAIT_FOR_DEVICE_1;
                printf("BLE: GameState: WAIT_FOR_DEVICE_1\r\n");
                
                uint64 operand = GH_CC_N_READY;
                bytesToSend_0 = (operand) & (int)0x000007ff;
                printf("BLE: user 0. READY. Bytes: %d\r\n", bytesToSend_0);
                SendBleNotification(
                    CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                    (uint64*)&bytesToSend_0,
                    bleConnectionHandle0
                );
                Cy_BLE_ProcessEvents();
            }
        }
            
        // notify command for device 1
        else if((gameState == INIT_DEVICE_1) && 
            (isCommand_1_StartNotification & NOTIFY_BIT_MASK)){
                
            // send device ID
            if(isUser_1_notified > 0){
                uint64 id = 1;
                uint64 operand = GH_CC_N_NOTIFY_ID;
                bytesToSend_1 = ((id << 5) | operand) & (int)0x000007ff;
                //printf("BLE: user 1. Notify iD. Bytes: %d\r\n", bytesToSend_1);
                SendBleNotification(
                    CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                    (uint64*)&bytesToSend_1,
                    bleConnectionHandle1
                );
                isUser_1_notified--;
                Cy_BLE_ProcessEvents();
                delayTime(1000);
            }
            
            // change state
            if (isUser_1_notified == 0 && isUser_1_PosStartNotification){
               
                // change game state
                gameState = PLAYER_0_CHOOSE_COLOR;
                printf("STACK: GameState: PLAYER_0_CHOOSE_COLOR\r\n");
          
                
                // player 0 choose color
                uint64 id = 0;
                uint64 operand = GH_CC_N_CHOOSE_COLOR;
                bytesToSend_0 = ((id << 5) | operand) & (uint64)0x000007ff;
                SendBleNotification(
                    CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                    (uint64*)&bytesToSend_0,
                    bleConnectionHandle0
                );
                Cy_BLE_ProcessEvents();
                delayTime(1000);
                
                bytesToSend_1 = ((id << 5) | operand) & (uint64)0x000007ff;
                SendBleNotification(
                    CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                    (uint64*)&bytesToSend_1,
                    bleConnectionHandle1
                );
                Cy_BLE_ProcessEvents();
                delayTime(1000);
                
                /*
                // change game state
                gameState = GAME_START;
                printf("BLE: GameState: GAME_START\r\n");
          
                
                // send start signal
                
                uint64 id = 0;
                uint64 operand = GH_CC_N_START;
                bytesToSend_0 = ((id << 5) | operand) & (uint64)0x000007ff;
                SendBleNotification(
                    CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                    (uint64*)&bytesToSend_0,
                    bleConnectionHandle0
                );
                Cy_BLE_ProcessEvents();
                delayTime(1000);
                
                bytesToSend_1 = ((id << 5) | operand) & (uint64)0x000007ff;
                SendBleNotification(
                    CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                    (uint64*)&bytesToSend_1,
                    bleConnectionHandle1
                );
                Cy_BLE_ProcessEvents();
                
                */
            }
        }
        
        else{
            Cy_BLE_ProcessEvents();
            taskYIELD();
        }
        
    }
}

void StackEventHandler(uint32 event, void *param){
    
    cy_stc_ble_gatts_write_cmd_req_param_t* writeReqPara;
    cy_en_ble_api_result_t bleapiResult;
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    
 
    switch((cy_en_ble_event_t) event){
 
        // if the ble stack on, start advertisement so other devices can find it
        case CY_BLE_EVT_STACK_ON:
        {
            if(gameState == INIT_SERVER){
                gameState = WAIT_FOR_DEVICE_0;
                printf("STACK: GameState: WAIT_FOR_DEVICE_0\r\n");
                bleapiResult = Cy_BLE_GAPP_StartAdvertisement(
                    CY_BLE_ADVERTISING_FAST,
                    CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX
                );
            }
            else{
                 printf("STACK: STACK_ON error.\r\n");
            }
            break;
        }
        
 
        // if there is a device connection, store the connection handle 
        case CY_BLE_EVT_GATT_CONNECT_IND:
        {
            //printf("GATT_CONNECT_IND\r\n");
            
            if(gameState == INIT_SERVER){
                printf("STACK: GameState: INIT_SERVER\r\n");
            }
            else if(gameState == WAIT_FOR_DEVICE_0){
                bleConnectionHandle0 = *(cy_stc_ble_conn_handle_t*)param;
                gameState = INIT_DEVICE_0;
                INIT_SOUND
                          
                printf("STACK: GameState: INIT_DEVICE_0\r\n");
            }
            else if(gameState == WAIT_FOR_DEVICE_1){
                cy_stc_ble_conn_handle_t temp = *(cy_stc_ble_conn_handle_t*)param;
                if(bleConnectionHandle0.bdHandle == temp.attId 
                    && bleConnectionHandle0.attId == temp.attId){
                    printf("STACK: Duplicate connection.\r\n");
                }
                else{
                    bleConnectionHandle1 = *(cy_stc_ble_conn_handle_t*)param;
                    gameState = INIT_DEVICE_1;
                    INIT_SOUND
                    
                    printf("STACK: GameState: INIT_DEVICE_1\r\n");
                }
                
            }
            else{
                printf("STACK: GameState: %d\r\n", gameState);
            }
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
            bool flag = (writeReqPara->connHandle.attId == bleConnectionHandle0.attId
                && writeReqPara->connHandle.bdHandle == bleConnectionHandle0.bdHandle) 
            || (writeReqPara->connHandle.attId == bleConnectionHandle1.attId
                && writeReqPara->connHandle.bdHandle == bleConnectionHandle1.bdHandle);
            
            if(flag){
                
                if(writeReqPara->connHandle.attId == bleConnectionHandle0.attId
                && writeReqPara->connHandle.bdHandle == bleConnectionHandle0.bdHandle){
                    // device 0 write request
                    printf("STACK: DEVICE 0 WRITE REQUEST.\r\n");
                }
                else{
                    // device 1 write request
                     printf("STACK: DEVICE 1 WRITE REQUEST.\r\n");
                }
                
                // Player Position N 
                if(CY_BLE_GH_POSITION_PLAYER_POSITION_N_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE 
                    == writeReqPara->handleValPair.attrHandle
                ){
                    printf("STACK: PLAYER_POSITION_N\r\n");

                    if(false == (writeReqPara->handleValPair.value.val[
                        CY_BLE_GH_POSITION_PLAYER_POSITION_N_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX
                    ] & (~CCCD_VALID_BIT_MASK))
                    ){
                        // set the flag for application to know whether the notificatio is allowed
                        
                        if(writeReqPara->connHandle.attId == bleConnectionHandle0.attId
                        && writeReqPara->connHandle.bdHandle == bleConnectionHandle0.bdHandle){
                            isUser_0_PosStartNotification = writeReqPara->handleValPair.value.val[
                                CY_BLE_GH_POSITION_PLAYER_POSITION_N_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX
                            ];
                            printf("STACK: User 0 Pos Start Notification: %d\r\n", isUser_0_PosStartNotification);
                        }
                        else{
                            isUser_1_PosStartNotification = writeReqPara->handleValPair.value.val[
                                CY_BLE_GH_POSITION_PLAYER_POSITION_N_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX
                            ];
                            printf("STACK: User 1 Pos Start Notification: %d\r\n", isUser_1_PosStartNotification);
                            //gameState = GAME_START;
                            //printf("STACK: GameState: GAME_START\r\n");
                        }
                        
                        cy_stc_ble_gatts_db_attr_val_info_t info = {
                            .handleValuePair = writeReqPara -> handleValPair,
                            .offset = false,
                            .connHandle = writeReqPara->connHandle,
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
                            .connHandle =  writeReqPara->connHandle,
                            .errInfo = err_info
                        };
                        Cy_BLE_GATTS_ErrorRsp(&errParam);
                    }
                }
                
                // Command Notify
                else if(CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE
                    == writeReqPara->handleValPair.attrHandle
                ){
                    
                    printf("STACK: COMMAND_NOTIFY\r\n");
                    
                    if(false == (writeReqPara->handleValPair.value.val[
                        CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX
                        
                    ] & (~CCCD_VALID_BIT_MASK))
                    ){
                        // set the flag for application to know whether the notificatio is allowed
                        if(writeReqPara->connHandle.attId == bleConnectionHandle0.attId
                        && writeReqPara->connHandle.bdHandle == bleConnectionHandle0.bdHandle){
                            isCommand_0_StartNotification = writeReqPara->handleValPair.value.val[
                            CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX
                        ];
                        }
                        else{
                            isCommand_1_StartNotification = writeReqPara->handleValPair.value.val[
                            CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX
                        ];
                        }
                        
                        cy_stc_ble_gatts_db_attr_val_info_t info = {
                            .handleValuePair = writeReqPara -> handleValPair,
                            .offset = false,
                            .connHandle =  writeReqPara->connHandle,
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
                            .connHandle =  writeReqPara->connHandle,
                            .errInfo = err_info
                        };
                        Cy_BLE_GATTS_ErrorRsp(&errParam);
                    }
                
                }
                
                // Choose color
                else if(CY_BLE_GH_COMMAND_COMMAND_WRITE_CHAR_HANDLE == writeReqPara->handleValPair.attrHandle){
                    printf("STACK: COMMAND_WRITE_CHAR_HANDLE\r\n");
                    
                    // device 0 write request
                    if(writeReqPara->connHandle.attId == bleConnectionHandle0.attId
                    && writeReqPara->connHandle.bdHandle == bleConnectionHandle0.bdHandle){
                        
                        // player 1 choose color
                        gameState = PLAYER_1_CHOOSE_COLOR;
                        
                        uint64 id = 1;
                        uint64 operand = GH_CC_N_CHOOSE_COLOR;
                        uint64 bytesToSend_0 = ((id << 5) | operand) & (uint64)0x000007ff;
                        SendBleNotification(
                            CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                            (uint64*)&bytesToSend_0,
                            bleConnectionHandle0
                        );
                        Cy_BLE_ProcessEvents();
                        delayTime(1000);
                        
                        uint64 bytesToSend_1 = ((id << 5) | operand) & (uint64)0x000007ff;
                        SendBleNotification(
                            CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                            (uint64*)&bytesToSend_1,
                            bleConnectionHandle1
                        );
                        Cy_BLE_ProcessEvents();
                        delayTime(1000);
                        
                    }
                    
                    // device 1 write request
                    else{
                        // change game state
                        gameState = GAME_START;
                     
                        // send start signal
                        uint64 operand = GH_CC_N_START;
                        uint64 bytesToSend_0 = (operand) & (uint64)0x000007ff;
                        SendBleNotification(
                            CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                            (uint64*)&bytesToSend_0,
                            bleConnectionHandle0
                        );
                        Cy_BLE_ProcessEvents();
                        delayTime(1000);
                        
                        uint64 bytesToSend_1 = (operand) & (uint64)0x000007ff;
                        SendBleNotification(
                            CY_BLE_GH_COMMAND_COMMAND_NOTIFY_CHAR_HANDLE,
                            (uint64*)&bytesToSend_1,
                            bleConnectionHandle1
                        );
                        Cy_BLE_ProcessEvents();
                        delayTime(1000);
                        
                        START_SOUND
                        
                        printf("BLE: GameState: GAME_START\r\n");
                    }
                    
                }
                
                Cy_BLE_GATTS_WriteRsp(writeReqPara->connHandle);       
            }

            break;
        }
        
        
        case CY_BLE_EVT_GATTS_WRITE_CMD_REQ:
        {
            writeReqPara = (cy_stc_ble_gatts_write_cmd_req_param_t*)param;
            
            // player position
            if(CY_BLE_GH_POSITION_PLAYER_POSITION_W_DECL_HANDLE == writeReqPara->handleValPair.attrHandle
                 || CY_BLE_GH_POSITION_PLAYER_POSITION_W_CHAR_HANDLE == writeReqPara->handleValPair.attrHandle){
                isReceivePosition = true;
                cy_stc_ble_gatt_handle_value_pair_t tmp = ((cy_stc_ble_gatts_write_cmd_req_param_t*)param)->handleValPair;
                xQueueSendFromISR(positionQueueHandle, (int32*)tmp.value.val, &xHigherPriorityTaskWoken);
            }
            // command w
            else if(CY_BLE_GH_COMMAND_COMMAND_WRITE_CHAR_HANDLE == writeReqPara->handleValPair.attrHandle
            || CY_BLE_GH_COMMAND_COMMAND_WRITE_DECL_HANDLE == writeReqPara->handleValPair.attrHandle){
                printf("STACK: command write!!\r\n");
                bool flag = (writeReqPara->connHandle.attId == bleConnectionHandle0.attId
                    && writeReqPara->connHandle.bdHandle == bleConnectionHandle0.bdHandle) 
                || (writeReqPara->connHandle.attId == bleConnectionHandle1.attId
                    && writeReqPara->connHandle.bdHandle == bleConnectionHandle1.bdHandle);
                
                if(flag){                    
                    uint32 val = *((int32*)writeReqPara->handleValPair.value.val);

                    // device 0 write command
                    if(writeReqPara->connHandle.attId == bleConnectionHandle0.attId
                    && writeReqPara->connHandle.bdHandle == bleConnectionHandle0.bdHandle){
                        printf("STACK: DEVICE 0 WRITE COMMAND %x.\r\n", val);
                    }
                    
                    // device 1 write command
                    else{
                         printf("STACK: DEVICE 1 WRITE COMMAND %x.\r\n", val);
                    }
                }
                
            }
            else{
                printf("STACK: command write!!\r\n");
            }
            
            break;
        }
            
        case CY_BLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            printf("STACK: GAPP_ADVERTISEMENT_START_STOP\r\n");
            break;
            
        case CY_BLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            bleapiResult = Cy_BLE_GAPP_StartAdvertisement(
                    CY_BLE_ADVERTISING_FAST,
                    CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX
            );
            printf("STACK: Restart Advertisement.\r\n");
            break;
            
        case CY_BLE_EVT_TIMEOUT:
            printf("STACK: CY_BLE_EVT_TIMEOUT\r\n");
            break;
        
        
        default:
        {
            printf("STACK: event code: %u\r\n", event);
            break;
        }
    } 
    
}


void SendBleNotification(cy_ble_gatt_db_attr_handle_t charHandle, uint64* value, cy_stc_ble_conn_handle_t connHandle){
    
    volatile cy_en_ble_api_result_t bleapiresult;
    cy_stc_ble_gatts_handle_value_ntf_t ntf;
 
    // fill the cy_stc_ble_gatts_handle_value_ntf_t struct 
    ntf.handleValPair.attrHandle = charHandle;
    ntf.handleValPair.value.val = (uint8*)value;
    ntf.handleValPair.value.len = sizeof(*value);
    ntf.connHandle = connHandle;
    // call the function to notify the client
    bleapiresult = Cy_BLE_GATTS_Notification(&ntf);
    if(bleapiresult == CY_BLE_SUCCESS){
        Cy_BLE_ProcessEvents();
    } 
}



int main(void)
{    
    
    __enable_irq(); /* Enable global interrupts. */
    
    /**************Components Startups***************/
    Cy_IPC_Sema_Init(CY_IPC_CHAN_SEMA, (uint32_t)NULL, (uint32_t*)NULL);
    UART_Start();
    delayTime(2000);
    
    I2C_1_Start();
    ADXL345_INI();
    int value1 = ADXL345_ReadRegister(0x31);
    nap();
    int value2 = ADXL345_ReadRegister(0x2D);
    nap();
    int value3 = ADXL345_ReadRegister(0x2E);
    nap();
    ADXL345_SETUP();
    
    
    gameState = INIT_SERVER;
       
    /*******************Task creates**********************/
    
    xTaskCreate(UartTask, "UART task", configMINIMAL_STACK_SIZE * 3, 0, 3, 0);
    xTaskCreate(bleTask, "BLE task", configMINIMAL_STACK_SIZE * 20, 0, 3, 0);
    xTaskCreate(GlowHockeyTask, "Game task", configMINIMAL_STACK_SIZE * 10, 0, 3, 0);
    
    
    vTaskStartScheduler();      //never return.
   
}

/* [] END OF FILE */
