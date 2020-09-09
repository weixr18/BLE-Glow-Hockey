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
#include <stdio.h>
#include <stdlib.h>
#include "I2CHelper.h"

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    UART_1_Start();
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    //printf("Les ami!\n");
    I2C_1_Start();
    /*
    cy_en_scb_i2c_status_t initStatus;
    initStatus = Cy_SCB_I2C_Init(I2C_1_HW, &I2C_1_config, &I2C_1_context);
    if (initStatus == CY_SCB_I2C_SUCCESS)
        printf("Step 0!\n");
    uint32_t dataRateSet;
    dataRateSet = Cy_SCB_I2C_SetDataRate(I2C_1_HW, I2C_1_DATA_RATE_HZ, I2C_1_CLK_FREQ_HZ);
    if( dataRateSet <= I2C_1_DATA_RATE_HZ ){
        printf("Step 01!\n");
    }
    Cy_SCB_I2C_Enable(I2C_1_HW);
    printf("Step 02!\n");
    uint8_t Address = 0x53;
    cy_en_scb_i2c_status_t status;
    //status = I2C_1_MasterSendStart((uint32)Address, CY_SCB_I2C_READ_XFER, I2CM_TIMEOUT); 
    uint8_t RegisterAddr = 0x0D;
    uint8_t mm = 0;
    uint8_t *RegisterValue = &mm;
    status = I2C_1_MasterSendStart((uint32)Address, CY_SCB_I2C_WRITE_XFER, I2CM_TIMEOUT);
    //printf("%d\n", status);
    //printf("%d\n", CY_SCB_I2C_SUCCESS);
    
    if(status == CY_SCB_I2C_SUCCESS){
        printf("Step 03!\n");
        status |= I2C_1_MasterWriteByte((uint32)RegisterAddr, I2CM_TIMEOUT);
    }
    
    //status |= I2C_1_MasterReadByte(CY_SCB_I2C_ACK, RegisterValue, I2CM_TIMEOUT);
    if(status == CY_SCB_I2C_SUCCESS){
        printf("Step 04!\n");
        status |= I2C_1_MasterSendReStart((uint32)Address, CY_SCB_I2C_READ_XFER, I2CM_TIMEOUT);
    }
    
    if(status == CY_SCB_I2C_SUCCESS){
        printf("Step 05!\n");
        status |= I2C_1_MasterReadByte(CY_SCB_I2C_ACK, RegisterValue, I2CM_TIMEOUT);
        //vTaskDelay(10u);
    }
    
    if(status == CY_SCB_I2C_SUCCESS){
        printf("Step 06!\n");
        int result = (int) *RegisterValue;
        printf("%d\n", result);
    }
    else{
        printf("%d\n", status);
    }
    //printf("Step 07!\n");
    status |= I2C_1_MasterSendStop(I2CM_TIMEOUT);
    if(status == CY_SCB_I2C_SUCCESS){
        printf("Final!\n");
    }
    */
    /*
    cy_en_scb_i2c_status_t initStatus;
    initStatus = Cy_SCB_I2C_Init(I2C_1_HW, &I2C_1_config, &I2C_1_context);
    if (initStatus == CY_SCB_I2C_SUCCESS)
        printf("Step 0!\n");
    uint32_t dataRateSet;
    dataRateSet = Cy_SCB_I2C_SetDataRate(I2C_1_HW, I2C_1_DATA_RATE_HZ, I2C_1_CLK_FREQ_HZ);
    if( dataRateSet <= I2C_1_DATA_RATE_HZ ){
        printf("Step 01!\n");
    }
    Cy_SCB_I2C_Enable(I2C_1_HW);
    printf("Step 02!\n");
    uint8_t Address = 0x53;
    cy_en_scb_i2c_status_t status;
    uint8_t RegisterAddr = 0x00;
    uint8_t mm = 0;
    uint8_t *RegisterValue = &mm;
    status = I2C_1_MasterSendStart((uint32)Address, CY_SCB_I2C_WRITE_XFER, I2CM_TIMEOUT);
    
    if(status == CY_SCB_I2C_SUCCESS){
        printf("Step 03!\n");
        status |= I2C_1_MasterWriteByte((uint32)RegisterAddr, I2CM_TIMEOUT);
    }
    
    //status |= I2C_1_MasterReadByte(CY_SCB_I2C_ACK, RegisterValue, I2CM_TIMEOUT);
    if(status == CY_SCB_I2C_SUCCESS){
        printf("Step 04!\n");
        status |= I2C_1_MasterSendReStart((uint32)Address, CY_SCB_I2C_READ_XFER, I2CM_TIMEOUT);
    }
    
    if(status == CY_SCB_I2C_SUCCESS){
        printf("Step 05!\n");
        status |= I2C_1_MasterReadByte(CY_SCB_I2C_ACK, RegisterValue, I2CM_TIMEOUT);
        status |= I2C_1_MasterReadByte(CY_SCB_I2C_ACK, RegisterValue+1, I2CM_TIMEOUT);
        //vTaskDelay(10u);
    }
    
    if(status == CY_SCB_I2C_SUCCESS){
        printf("Step 06!\n");
        int result = (int) *RegisterValue;
        printf("%d\n", result);
        //result = (int) *(RegisterValue+1);
        //printf("%d\n", result);
        status |= I2C_1_MasterSendReStart((uint32)Address, CY_SCB_I2C_WRITE_XFER, I2CM_TIMEOUT);
    }
    
    status |= I2C_1_MasterSendStop(I2CM_TIMEOUT);
    */
    ADXL345_INI();
    int value1 = ADXL345_ReadRegister(0x31);
    nap();
    int value2 = ADXL345_ReadRegister(0x2D);
    nap();
    int value3 = ADXL345_ReadRegister(0x2E);
    nap();
    //printf("va1: %d, va2: %d, va3: %d\n", value1, value2, value3);
    ADXL345_SETUP();
    //int *result = (int*)malloc(sizeof(int)*3);
    double* resultd = (double*)malloc(sizeof(double)*3);
    
    value1 = ADXL345_ReadRegister(0x31);
    nap();
    value2 = ADXL345_ReadRegister(0x2D);
    nap();
    value3 = ADXL345_ReadRegister(0x2E);
    nap();
    //while(value1!=0x83 || value2!=0x08 || value3!=0x80){
    //printf("va1: %d, va2: %d, va3: %d\n", value1, value2, value3);
    ADXL345_SETUP();
    //}
    
    //double *Base = (double*)malloc(sizeof(double)*3);
    //Base = SetBase();
    //int x;
    for(;;){
        //result = AlterValue(Base);
        resultd = ReadAccDecoding();
        //printf("X: %d, Y: %d, Z: %d\n", *result, *(result+1), *(result+2));
        int x = (int) (resultd[0]+5.989)*128/90;
        int y = (int) (resultd[1]+5.627)*128/90;
        int z = (int) (resultd[2]+4.996)*128/90;
        x = abs(x)>15 ? x:0;
        y = abs(y)>15 ? y:0;
        z = abs(z)>15 ? z:0;
        //printf("X: %4lf, Y: %4lf, Z: %4lf\n", *resultd, *(resultd+1), *(resultd+2));
        printf("X: %d, Y: %d, Z: %d\n", x, y, z);
        for(int x = 5000; x>0; x--)
            for(int y=1000; y>0; y--);
    }
}

/* [] END OF FILE */
