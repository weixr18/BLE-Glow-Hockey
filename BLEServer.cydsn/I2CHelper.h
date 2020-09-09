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

#ifndef I2CM_SUPPORT_H
#define I2CM_SUPPORT_H
    
    
#define I2CM_TIMEOUT        100
#include "project.h"

unsigned int ADXL345_INI();
unsigned int ADXL345_SETUP();
int ADXL345_ReadRegister(uint8_t RegisterAddr);
unsigned int ADXL345_WriteRegister(uint8_t RegisterAddr, uint8_t RegisterValue);
int AccProcessing(int raw0, int raw1);
void ReadAccDecoding();
void nap();
double* SetBase();
int* AlterValue();


#endif //I2CM_SUPPORT_H
/* [] END OF FILE */    