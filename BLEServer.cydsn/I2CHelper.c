/* ========================================
 *
 * Copyright MA ZHENGHUA, 2020
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */
#include "FreeRTOS.h"
#include "task.h"
#include "I2CHelper.h"
#include "game.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


unsigned int ADXL345_INI(){
    cy_en_scb_i2c_status_t initStatus;
    initStatus = Cy_SCB_I2C_Init(I2C_1_HW, &I2C_1_config, &I2C_1_context);
    if (initStatus == CY_SCB_I2C_SUCCESS){
        //printf("I2C: Step 0!\n");
    }
    uint32_t dataRateSet;
    dataRateSet = Cy_SCB_I2C_SetDataRate(I2C_1_HW, I2C_1_DATA_RATE_HZ, I2C_1_CLK_FREQ_HZ);
    if( dataRateSet <= I2C_1_DATA_RATE_HZ ){
        //printf("I2C: Step 01!\n");
    }
    Cy_SCB_I2C_Enable(I2C_1_HW);
    //printf("I2C: Step 02!\n");
    return 0;
}

unsigned int ADXL345_SETUP(){
    //cy_en_scb_i2c_status_t initStatus;
    //uint8_t Address = 0x53;
        //uint8_t RegisterAddr = 0x31;
    //uint8_t mm = 0;
    cy_en_scb_i2c_status_t status;

    status = ADXL345_WriteRegister(0x31, 0x83);
    nap();
    status |= ADXL345_WriteRegister(0x2D, 0x28);
    nap();
    status |= ADXL345_WriteRegister(0x2E, 0x08);
    nap();
    if(status == CY_SCB_I2C_SUCCESS){
        //printf("I2C: Final - 4!\n");
    }
    return status;
}

int ADXL345_ReadRegister(uint8_t RegisterAddr){
    cy_en_scb_i2c_status_t status;
    uint8_t Address = 0x53;
    uint8_t mm = 0;
    uint8_t *RegisterValue = &mm;
    
    status = I2C_1_MasterSendStart((uint32)Address, CY_SCB_I2C_WRITE_XFER, I2CM_TIMEOUT);
    status |= I2C_1_MasterWriteByte((uint32)RegisterAddr, I2CM_TIMEOUT);
    status |= I2C_1_MasterSendReStart((uint32)Address, CY_SCB_I2C_READ_XFER, I2CM_TIMEOUT);
    status |= I2C_1_MasterReadByte(CY_SCB_I2C_ACK, RegisterValue, I2CM_TIMEOUT);
    nap();
    
    status |= I2C_1_MasterSendStop(I2CM_TIMEOUT);
    int result = (int) *RegisterValue;
    //printf("rValue: %d\n", result);
    return result;
}

unsigned int ADXL345_WriteRegister(uint8_t RegisterAddr, uint8_t RegisterValue){
    cy_en_scb_i2c_status_t status;
    uint8_t Address = 0x53;
    status = I2C_1_MasterSendStart((uint32)Address, CY_SCB_I2C_WRITE_XFER, I2CM_TIMEOUT);
    if(status == CY_SCB_I2C_SUCCESS){
        //printf("I2C: Final - 0!\n");
    }
    status |= I2C_1_MasterWriteByte((uint32)RegisterAddr, I2CM_TIMEOUT);
    if(status == CY_SCB_I2C_SUCCESS){
        //printf("I2C: Final - 1!\n");
    }
    status |= I2C_1_MasterWriteByte((uint32)RegisterValue, I2CM_TIMEOUT);
    if(status == CY_SCB_I2C_SUCCESS){
        //printf("I2C: Final - 2!\n");
    }
    nap();
    status |= I2C_1_MasterSendStop(I2CM_TIMEOUT);
    if(status == CY_SCB_I2C_SUCCESS){
        //printf("I2C: Final - 3!\n");
    }
    return status;
}

int AccProcessing(int raw0, int raw1){
    int result;
    if(raw1 > 127){
        result = -((int) pow(16, 4)-raw1*16*16-raw0);
    }
    else{
        result = raw1*16*16+raw0;
    }
    return result;
}

void ReadAccDecoding(){
    int AccX0;
    int AccX1;
    int AccY0;
    int AccY1;
    int AccZ0;
    int AccZ1;
    
    double Acc_m1_X;
    double Acc_m1_Y;
    double Acc_m1_Z;
    
    double Acc_m2_X;
    double Acc_m2_Y;
    double Acc_m2_Z;
    
    double Acc_m3_X;
    double Acc_m3_Y;
    double Acc_m3_Z;
    
    
    AccX1 = ADXL345_ReadRegister(0x33);
    nap();
    AccX0 = ADXL345_ReadRegister(0x32);
    nap();
    AccY1 = ADXL345_ReadRegister(0x35);
    nap();
    AccY0 = ADXL345_ReadRegister(0x34);
    nap();
    AccZ1 = ADXL345_ReadRegister(0x37);
    nap();
    AccZ0 = ADXL345_ReadRegister(0x36);
    nap();
    //printf("%d %d %d %d %d %d\r\n", AccX0, AccX1, AccY0, AccY1, AccZ0, AccZ1);
    
    
    Acc_m1_X = AccProcessing(AccX0, AccX1)*0.04;
    Acc_m1_Y = AccProcessing(AccY0, AccY1)*0.04;
    Acc_m1_Z = AccProcessing(AccZ0, AccZ1)*0.04;
    
    Acc_m2_X = (419-Acc_m1_Y)/3.42+0.2339;
    Acc_m2_Y = (409+Acc_m1_Z)/3.64+0.1648;
    Acc_m2_Z = (1024-Acc_m1_X)/4.1+2.4975;
    
    
    Acc_m3_X = ((Acc_m2_X+5.989)*128/90);
    Acc_m3_Y = ((Acc_m2_Y+5.627)*128/90);
    Acc_m3_Z = ((Acc_m2_Z+4.996)*128/90);
    
    Acc_m3_X = (doubleAbs(Acc_m3_X) > 15) ? Acc_m3_X : 0;
    Acc_m3_Y = (doubleAbs(Acc_m3_Y) > 15) ? Acc_m3_Y : 0;
    Acc_m3_Z = (doubleAbs(Acc_m3_Z) > 15) ? Acc_m3_Z : 0;
    
    accelerateX = Acc_m3_X;
    accelerateY = Acc_m3_Y;
    
    //printf("Game: AccX %lf Accy %lf\r\n", accelerateX, accelerateY);
    
}


void nap(){
    for(int x = 5; x>0; x--)
        for(int y=200; y>0; y--);
}



/*
double* SetBase(){
    double *Base = (double*)malloc(sizeof(double)*3);
    double XSum = 0;
    double YSum = 0;
    double ZSum = 0;
    double *result = (double*)malloc(sizeof(double)*3);
    for(int i=0; i<10; i--){
        result = ReadAccDecoding();
        XSum += result[0];
        YSum += result[1];
        ZSum += result[2];
    }
    Base[0] = XSum/10;
    Base[1] = YSum/10;
    Base[2] = ZSum/10;
    return Base;
}

int* AlterValue(double* Base){
    double XBase = Base[0];
    double YBase = Base[1];
    double ZBase = Base[2];
    double *raw = (double*)malloc(sizeof(double)*3);
    raw = ReadAccDecoding();
    int *result = (int*)malloc(sizeof(int)*3);
    result[0] = (int) raw[0]-XBase;
    result[1] = (int) raw[1]-YBase;
    result[2] = (int) raw[2]-ZBase;
    return result;
}

*/


