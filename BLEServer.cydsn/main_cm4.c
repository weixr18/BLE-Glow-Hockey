#include "project.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>



/******************************GLOBAL VARIABLES***********************************/

QueueHandle_t bleQueueHandle;

/******************************FUNCTION DECLARATIONS***********************************/
void StackEventHandler(uint32 event, void * param);



/******************************FUNCTION DEFINATIONS***********************************/



// BLE task
void bleTask()
{
    uint32 brightVal;
    isUser_0_PosStartNotification = false;
    isCommand_0_StartNotification = false;
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
            //printf("STACK_ON\r\n");
            if(gameState == SERVER_START){
                gameState = WAITING_FOR_DEVICE_0;
                printf("gameState: WAITING_FOR_DEVICE_0\r\n");
                bleapiResult = Cy_BLE_GAPP_StartAdvertisement(
                    CY_BLE_ADVERTISING_FAST,
                    CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX
                );
            }
            else{
                 printf("STACK_ON error.\r\n");
            }
            
            break;
        }
        case CY_BLE_EVT_TIMEOUT:
            printf("CY_BLE_EVT_TIMEOUT\r\n");
        break;
 
        // if there is a device connection, store the connection handle 
        case CY_BLE_EVT_GATT_CONNECT_IND:
        {
            //printf("GATT_CONNECT_IND\r\n");
            
            if(gameState == SERVER_START){
                printf("gameState: SERVER_START\r\n");
            }
            else if(gameState == WAITING_FOR_DEVICE_0){
                bleConnectionHandle0 = *(cy_stc_ble_conn_handle_t*)param;
                gameState = WAITING_FOR_DEVICE_1;
                          
                printf("gameState: WAITING_FOR_DEVICE_1\r\n");
            }
            else if(gameState == WAITING_FOR_DEVICE_1){
                cy_stc_ble_conn_handle_t temp = *(cy_stc_ble_conn_handle_t*)param;
                if(bleConnectionHandle0.bdHandle == temp.attId 
                    && bleConnectionHandle0.attId == temp.attId){
                    printf("Duplicate connection.\r\n");
                }
                else{
                    bleConnectionHandle1 = *(cy_stc_ble_conn_handle_t*)param;

                    gameState = GAME_INITIALIZE;
                    printf("gameState: GAME_INITIALIZE\r\n");
                }
                
            }
            else{
                printf("gameState: %d\r\n", gameState);
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
                // device 0 write request
                if(writeReqPara->connHandle.attId == bleConnectionHandle0.attId
                && writeReqPara->connHandle.bdHandle == bleConnectionHandle0.bdHandle){
                    printf("DEVICE 0 WRITE REQUEST.\r\n");
                }
                else{
                     printf("DEVICE 1 WRITE REQUEST.\r\n");
                }
                
                // Player Position N 
                if(CY_BLE_GH_POSITION_PLAYER_POSITION_N_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE 
                    == writeReqPara->handleValPair.attrHandle
                ){
                    printf("PLAYER_POSITION_N\r\n");

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
                            printf("isUser_0_PosStartNotification:%d\r\n", isUser_0_PosStartNotification);
                        }
                        else{
                            isUser_1_PosStartNotification = writeReqPara->handleValPair.value.val[
                                CY_BLE_GH_POSITION_PLAYER_POSITION_N_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX
                            ];
                            printf("isUser_1_PosStartNotification:%d\r\n", isUser_1_PosStartNotification);
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
                    
                    printf("COMMAND_NOTIFY\r\n");
                    
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
                
                Cy_BLE_GATTS_WriteRsp(writeReqPara->connHandle);
            
           
        
            }
            
            
            
            /*
            
            if(writeReqPara->connHandle.attId == bleConnectionHandle0.attId
                && writeReqPara->connHandle.bdHandle == bleConnectionHandle0.bdHandle){
                
                    
                
                }
            
            else if(writeReqPara->connHandle.attId == bleConnectionHandle1.attId
                && writeReqPara->connHandle.bdHandle == bleConnectionHandle1.bdHandle){
                
                    
                } 
            */
            
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
                        .connHandle = bleConnectionHandle0,
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
                        .connHandle = bleConnectionHandle0,
                        .errInfo = err_info
                    };
                    Cy_BLE_GATTS_ErrorRsp(&errParam);
                }
            }
            */
            
            
            break;
        }
        
        case CY_BLE_EVT_GATTS_WRITE_CMD_REQ:
            isReceivePosition = true;
            writeReqPara = (cy_stc_ble_gatts_write_cmd_req_param_t*)param;
                  
            cy_stc_ble_gatt_handle_value_pair_t tmp = ((cy_stc_ble_gatts_write_cmd_req_param_t*)param)->handleValPair;
            xQueueSendFromISR(bleQueueHandle, (int32*)tmp.value.val, &xHigherPriorityTaskWoken);
            break;
        
        case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
            printf("READ_CHAR_VAL_ACCESS_REQ\r\n");
            break;
            
        case CY_BLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            printf("GAPP_ADVERTISEMENT_START_STOP\r\n");
            break;
            
        case CY_BLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            bleapiResult = Cy_BLE_GAPP_StartAdvertisement(
                    CY_BLE_ADVERTISING_FAST,
                    CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX
            );
            printf("BLE API result: %d\r\n", bleapiResult);
            break;
        
        default:
        {
            printf("DEFAULT : %u\r\n", event);
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
    IPC_STRUCT_Type *myIpcHandle;
    
    __enable_irq(); /* Enable global interrupts. */
        
    /**************Components Startups***************/
    Cy_IPC_Sema_Init(CY_IPC_CHAN_SEMA, (uint32_t)NULL, (uint32_t*)NULL);
    UART_Start();
    CapSense_Start();
    gameState = SERVER_START;
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
