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
#include <stdio.h>


#define MY_IPC_CHANNEL 8u

static uint32_t array[4];

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    
    Cy_IPC_Sema_Init(CY_IPC_CHAN_SEMA, sizeof(array) * 8,array) ;
 
    // Enable CM4. CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. 
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR);  

    
    if(Cy_BLE_Start(NULL) == CY_BLE_SUCCESS)
    {
        printf("Core 0 BLE start success.\r\n");
    }

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        Cy_BLE_ProcessEvents();
    }
}

/* [] END OF FILE */
