#include "project.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdio.h>

/***********************************MACROS*****************************************/

// bluetooth macros
#define ERR_INVALID_PDU 0X04
#define CCCD_VALID_BIT_MASK 0x03
#define NOTIFY_BIT_MASK 0x01
#define MY_IPC_CHANNEL 8u

/******************************GLOBAL VARIABLES***********************************/

QueueHandle_t bleQueueHandle;

bool isReceivePosition = true;
bool isStartNotification = false;

static uint32_t *sharedBrightNessVal = NULL;

static cy_stc_ble_conn_handle_t connectionHandle;

/******************************FUNCTION DECLARATIONS***********************************/
void StackEventHandler(uint32 event, void * param);
void SendBleNotification(cy_ble_gatt_db_attr_handle_t charHandle, uint32* value);

/******************************FUNCTION DEFINATIONS***********************************/

// Capsense task (only for debug)
void CapsenseTask(){
    uint32_t sliderpos; 
    uint32_t lastSliderPos = 0;
     
    // start the capsense component
    CapSense_Start();
    CapSense_ScanAllWidgets();
    
    while(1){
        // if capsense is not scanning 
        if(!CapSense_IsBusy()){
            
            //get the capsense slider pos, the value is between 0 and 100.
            CapSense_ProcessAllWidgets();
            sliderpos = CapSense_GetCentroidPos(CapSense_LINEARSLIDER0_WDGT_ID);
            
            if(sliderpos != CapSense_SLIDER_NO_TOUCH && sliderpos != lastSliderPos)
            {                
                
                uint32 val = sliderpos ;
                printf("Slider value: %u\r\n", val);
                
                // Send Message To BLE Queue to test if sending data is valid
                /*
                isBleNeedUpdate = true;
                xQueueSend(bleQueueHandle, &val, portMAX_DELAY);
                */
                
                lastSliderPos = sliderpos;
            }
            /*
            else{
                uint32 val = 120 ;
                xQueueSend(bleQueueHandle, &val, portMAX_DELAY);
            }
            */
            
            // update the capsense baselines and scan the sensors again
            CapSense_UpdateAllBaselines();
            CapSense_ScanAllWidgets();
        }
        else{
            taskYIELD();
        }
    }
}

// Game task
void GlowHockeyTask(){
    
    uint8 USER_INDEX_MASK = 1;
    uint16 USER_POSITION_MASK = 0x0FFF;

    uint16 ballx;
    uint16 bally;
    uint16 ballSpeedx;
    uint16 ballSpeedy;
    
    uint16 player0x;
    uint16 player0y;
    uint16 player0Speedx;
    uint16 player0Speedy;
    
    uint16 player1x;
    uint16 player1y;
    uint16 player1Speedx;
    uint16 player1Speedy;
    
    uint32 receivedPosition;
    uint32 sendUserPosition;
    uint32 sendOppositePosition;
    
    for(;;){
            
        if(isReceivePosition){
            xQueueReceive(bleQueueHandle, &receivedPosition, portMAX_DELAY);
            isReceivePosition = false;
            
            // receive position
            if(0 == (((uint8)(receivedPosition >> 24)) & USER_INDEX_MASK))
            {
                // player 0
                player0x = ((uint16) receivedPosition) & USER_POSITION_MASK;
                player0y = ((uint16)(receivedPosition >> 12)) & USER_POSITION_MASK;
                printf("User 0 : (%u, %u)\r\n", player0x, player0y);
                
            }
            else{
                // player 1
                player1x = ((uint16) receivedPosition) & USER_POSITION_MASK;
                player1y = ((uint16)(receivedPosition >> 12)) & USER_POSITION_MASK;
                printf("User 1 : (%u, %u)\r\n", player1x, player1y);
            }
            
            // game logic
            
            
            
            // send positions
            
            if(0 == (((uint8)(receivedPosition >> 24)) & USER_INDEX_MASK))
            {
                // player 0
                sendUserPosition = 0;
                sendUserPosition |= (uint32)(player0x & USER_POSITION_MASK);
                sendUserPosition |= ((uint32)(player0y & USER_POSITION_MASK) << 12);
                
                // if the notification is allowed , send the notification
                if((isStartNotification & NOTIFY_BIT_MASK)){
                    SendBleNotification(CY_BLE_GH_POSITION_PLAYER_POSITION_N_CHAR_HANDLE, &sendUserPosition);
                }  
                else{
                    printf("Not send notification user 0.\r\n");
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
    uint32 lastBrightVal = 0;
    isStartNotification = false;
    isReceivePosition = false;
    bleQueueHandle = xQueueCreate(1, sizeof(brightVal));
 
    printf("Starting BLE Task.\r\n");
    cy_en_ble_api_result_t apiResult;
    apiResult = Cy_BLE_Start(StackEventHandler);
    if(apiResult == CY_BLE_SUCCESS){
        printf("BLE Start success.\r\n");
    }
 
    for(;;){
        Cy_BLE_ProcessEvents();
        taskYIELD();
    }
}

void StackEventHandler(uint32 event, void *param){
    param = param;
    cy_stc_ble_gatts_write_cmd_req_param_t* writeReqPara;
    cy_en_ble_api_result_t bleapiResult;
    uint32 val;
 
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
                    isStartNotification = writeReqPara->handleValPair.value.val[
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
            else{
                 
            }
            
            
            printf("GATTS_WRITE_REQ value:%u \r\n", *((int32*) (writeReqPara->handleValPair.value.val)));
            val = *((int32*) (writeReqPara->handleValPair.value.val));
            isReceivePosition = true;
            xQueueSend(bleQueueHandle, &val, portMAX_DELAY);
            
            //send response 
            Cy_BLE_GATTS_WriteRsp(connectionHandle);
            break;
        }
        
        case CY_BLE_EVT_GATTS_WRITE_CMD_REQ:
            printf("WRITE_CMD_REQ\r\n");
            break;
        
        case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
            printf("READ_CHAR_VAL_ACCESS_REQ\r\n");
            break;
        
        default:
        {
            printf("DEFAULT\r\n");
            break;
        }
    } 
    
}


void SendBleNotification(cy_ble_gatt_db_attr_handle_t charHandle, uint32* value){
    
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
    
    /*******************Task creates**********************/
    
    xTaskCreate(CapsenseTask, "capsense task", configMINIMAL_STACK_SIZE * 15, 0, 3, 0);
    xTaskCreate(bleTask, "ble task", configMINIMAL_STACK_SIZE * 20, 0, 3, 0);
    xTaskCreate(GlowHockeyTask, "Game task", configMINIMAL_STACK_SIZE * 20, 0, 3, 0);
    
    vTaskStartScheduler();

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
