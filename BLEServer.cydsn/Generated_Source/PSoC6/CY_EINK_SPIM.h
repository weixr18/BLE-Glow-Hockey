/***************************************************************************//**
* \file CY_EINK_SPIM.h
* \version 2.0
*
*  This file provides constants and parameter values for the SPI component.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_EINK_SPIM_CY_SCB_SPI_PDL_H)
#define CY_EINK_SPIM_CY_SCB_SPI_PDL_H

#include "cyfitter.h"
#include "scb/cy_scb_spi.h"

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
*        Function Prototypes
***************************************/
/**
* \addtogroup group_general
* @{
*/
/* Component specific functions. */
void CY_EINK_SPIM_Start(void);

/* Basic functions. */
__STATIC_INLINE cy_en_scb_spi_status_t CY_EINK_SPIM_Init(cy_stc_scb_spi_config_t const *config);
__STATIC_INLINE void CY_EINK_SPIM_DeInit(void);
__STATIC_INLINE void CY_EINK_SPIM_Enable(void);
__STATIC_INLINE void CY_EINK_SPIM_Disable(void);

/* Register callback. */
__STATIC_INLINE void CY_EINK_SPIM_RegisterCallback(cy_cb_scb_spi_handle_events_t callback);

/* Bus state. */
__STATIC_INLINE bool CY_EINK_SPIM_IsBusBusy(void);

/* Slave select control. */
__STATIC_INLINE void CY_EINK_SPIM_SetActiveSlaveSelect(cy_en_scb_spi_slave_select_t slaveSelect);
__STATIC_INLINE void CY_EINK_SPIM_SetActiveSlaveSelectPolarity(cy_en_scb_spi_slave_select_t slaveSelect, 
                                                                   cy_en_scb_spi_polarity_t polarity);

/* Low level: read. */
__STATIC_INLINE uint32_t CY_EINK_SPIM_Read(void);
__STATIC_INLINE uint32_t CY_EINK_SPIM_ReadArray(void *buffer, uint32_t size);
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetRxFifoStatus(void);
__STATIC_INLINE void     CY_EINK_SPIM_ClearRxFifoStatus(uint32_t clearMask);
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetNumInRxFifo(void);
__STATIC_INLINE void     CY_EINK_SPIM_ClearRxFifo(void);

/* Low level: write. */
__STATIC_INLINE uint32_t CY_EINK_SPIM_Write(uint32_t data);
__STATIC_INLINE uint32_t CY_EINK_SPIM_WriteArray(void *buffer, uint32_t size);
__STATIC_INLINE void     CY_EINK_SPIM_WriteArrayBlocking(void *buffer, uint32_t size);
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetTxFifoStatus(void);
__STATIC_INLINE void     CY_EINK_SPIM_ClearTxFifoStatus(uint32_t clearMask);
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetNumInTxFifo(void);
__STATIC_INLINE bool     CY_EINK_SPIM_IsTxComplete(void);
__STATIC_INLINE void     CY_EINK_SPIM_ClearTxFifo(void);

/* Master/slave specific status. */
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetSlaveMasterStatus(void);
__STATIC_INLINE void     CY_EINK_SPIM_ClearSlaveMasterStatus(uint32_t clearMask);

/* High level: transfer functions. */
__STATIC_INLINE cy_en_scb_spi_status_t CY_EINK_SPIM_Transfer(void *txBuffer, void *rxBuffer, uint32_t size);
__STATIC_INLINE void     CY_EINK_SPIM_AbortTransfer(void);
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetTransferStatus(void);
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetNumTransfered(void);

/* Interrupt handler */
__STATIC_INLINE void CY_EINK_SPIM_Interrupt(void);
/** @} group_general */


/***************************************
*    Variables with External Linkage
***************************************/
/**
* \addtogroup group_globals
* @{
*/
extern uint8_t CY_EINK_SPIM_initVar;
extern cy_stc_scb_spi_config_t const CY_EINK_SPIM_config;
extern cy_stc_scb_spi_context_t CY_EINK_SPIM_context;
/** @} group_globals */


/***************************************
*         Preprocessor Macros
***************************************/
/**
* \addtogroup group_macros
* @{
*/
/** The pointer to the base address of the hardware */
#define CY_EINK_SPIM_HW     ((CySCB_Type *) CY_EINK_SPIM_SCB__HW)

/** The slave select line 0 constant which takes into account pin placement */
#define CY_EINK_SPIM_SPI_SLAVE_SELECT0    ( (cy_en_scb_spi_slave_select_t) CY_EINK_SPIM_SCB__SS0_POSITION)

/** The slave select line 1 constant which takes into account pin placement */
#define CY_EINK_SPIM_SPI_SLAVE_SELECT1    ( (cy_en_scb_spi_slave_select_t) CY_EINK_SPIM_SCB__SS1_POSITION)

/** The slave select line 2 constant which takes into account pin placement */
#define CY_EINK_SPIM_SPI_SLAVE_SELECT2    ( (cy_en_scb_spi_slave_select_t) CY_EINK_SPIM_SCB__SS2_POSITION)

/** The slave select line 3 constant which takes into account pin placement */
#define CY_EINK_SPIM_SPI_SLAVE_SELECT3    ((cy_en_scb_spi_slave_select_t) CY_EINK_SPIM_SCB__SS3_POSITION)
/** @} group_macros */


/***************************************
*    In-line Function Implementation
***************************************/

/*******************************************************************************
* Function Name: CY_EINK_SPIM_Init
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Init() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_spi_status_t CY_EINK_SPIM_Init(cy_stc_scb_spi_config_t const *config)
{
    return Cy_SCB_SPI_Init(CY_EINK_SPIM_HW, config, &CY_EINK_SPIM_context);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_DeInit
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_DeInit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_DeInit(void)
{
    Cy_SCB_SPI_DeInit(CY_EINK_SPIM_HW);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_Enable
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Enable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_Enable(void)
{
    Cy_SCB_SPI_Enable(CY_EINK_SPIM_HW);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_Disable
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Disable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_Disable(void)
{
    Cy_SCB_SPI_Disable(CY_EINK_SPIM_HW, &CY_EINK_SPIM_context);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_RegisterCallback
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_RegisterCallback() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_RegisterCallback(cy_cb_scb_spi_handle_events_t callback)
{
    Cy_SCB_SPI_RegisterCallback(CY_EINK_SPIM_HW, callback, &CY_EINK_SPIM_context);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_IsBusBusy
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_IsBusBusy() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE bool CY_EINK_SPIM_IsBusBusy(void)
{
    return Cy_SCB_SPI_IsBusBusy(CY_EINK_SPIM_HW);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_SetActiveSlaveSelect
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_SetActiveSlaveSelect() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_SetActiveSlaveSelect(cy_en_scb_spi_slave_select_t slaveSelect)
{
    Cy_SCB_SPI_SetActiveSlaveSelect(CY_EINK_SPIM_HW, slaveSelect);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_SetActiveSlaveSelectPolarity
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_SetActiveSlaveSelectPolarity() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_SetActiveSlaveSelectPolarity(cy_en_scb_spi_slave_select_t slaveSelect, 
                                                                   cy_en_scb_spi_polarity_t polarity)
{
    Cy_SCB_SPI_SetActiveSlaveSelectPolarity(CY_EINK_SPIM_HW, slaveSelect, polarity);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_Read
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Read() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t CY_EINK_SPIM_Read(void)
{
    return Cy_SCB_SPI_Read(CY_EINK_SPIM_HW);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_ReadArray
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_ReadArray() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t CY_EINK_SPIM_ReadArray(void *buffer, uint32_t size)
{
    return Cy_SCB_SPI_ReadArray(CY_EINK_SPIM_HW, buffer, size);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_GetRxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_GetRxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetRxFifoStatus(void)
{
    return Cy_SCB_SPI_GetRxFifoStatus(CY_EINK_SPIM_HW);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_ClearRxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_ClearRxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_ClearRxFifoStatus(uint32_t clearMask)
{
    Cy_SCB_SPI_ClearRxFifoStatus(CY_EINK_SPIM_HW, clearMask);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_GetNumInRxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_GetNumInRxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetNumInRxFifo(void)
{
    return Cy_SCB_GetNumInRxFifo(CY_EINK_SPIM_HW);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_ClearRxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_ClearRxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_ClearRxFifo(void)
{
    Cy_SCB_SPI_ClearRxFifo(CY_EINK_SPIM_HW);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_Write
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Write() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t CY_EINK_SPIM_Write(uint32_t data)
{
    return Cy_SCB_SPI_Write(CY_EINK_SPIM_HW, data);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_WriteArray
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_WriteArray() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t CY_EINK_SPIM_WriteArray(void *buffer, uint32_t size)
{
    return Cy_SCB_SPI_WriteArray(CY_EINK_SPIM_HW, buffer, size);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_WriteArrayBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_WriteArrayBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_WriteArrayBlocking(void *buffer, uint32_t size)
{
    Cy_SCB_SPI_WriteArrayBlocking(CY_EINK_SPIM_HW, buffer, size);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_GetTxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_GetTxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetTxFifoStatus(void)
{
    return Cy_SCB_SPI_GetTxFifoStatus(CY_EINK_SPIM_HW);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_ClearTxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_ClearTxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_ClearTxFifoStatus(uint32_t clearMask)
{
    Cy_SCB_SPI_ClearTxFifoStatus(CY_EINK_SPIM_HW, clearMask);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_GetNumInTxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_GetNumInTxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetNumInTxFifo(void)
{
    return Cy_SCB_SPI_GetNumInTxFifo(CY_EINK_SPIM_HW);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_IsTxComplete
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_IsTxComplete() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE bool CY_EINK_SPIM_IsTxComplete(void)
{
    return Cy_SCB_SPI_IsTxComplete(CY_EINK_SPIM_HW);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_ClearTxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_ClearTxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_ClearTxFifo(void)
{
    Cy_SCB_SPI_ClearTxFifo(CY_EINK_SPIM_HW);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_GetSlaveMasterStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_GetSlaveMasterStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetSlaveMasterStatus(void)
{
    return Cy_SCB_SPI_GetSlaveMasterStatus(CY_EINK_SPIM_HW);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_ClearSlaveMasterStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_ClearSlaveMasterStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_ClearSlaveMasterStatus(uint32_t clearMask)
{
    Cy_SCB_SPI_ClearSlaveMasterStatus(CY_EINK_SPIM_HW, clearMask);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_Transfer
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Transfer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_spi_status_t CY_EINK_SPIM_Transfer(void *txBuffer, void *rxBuffer, uint32_t size)
{
    return Cy_SCB_SPI_Transfer(CY_EINK_SPIM_HW, txBuffer, rxBuffer, size, &CY_EINK_SPIM_context);
}

/*******************************************************************************
* Function Name: CY_EINK_SPIM_AbortTransfer
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_AbortTransfer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_AbortTransfer(void)
{
    Cy_SCB_SPI_AbortTransfer(CY_EINK_SPIM_HW, &CY_EINK_SPIM_context);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_GetTransferStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_GetTransferStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetTransferStatus(void)
{
    return Cy_SCB_SPI_GetTransferStatus(CY_EINK_SPIM_HW, &CY_EINK_SPIM_context);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_GetNumTransfered
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_GetNumTransfered() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t CY_EINK_SPIM_GetNumTransfered(void)
{
    return Cy_SCB_SPI_GetNumTransfered(CY_EINK_SPIM_HW, &CY_EINK_SPIM_context);
}


/*******************************************************************************
* Function Name: CY_EINK_SPIM_Interrupt
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Interrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void CY_EINK_SPIM_Interrupt(void)
{
    Cy_SCB_SPI_Interrupt(CY_EINK_SPIM_HW, &CY_EINK_SPIM_context);
}


#if defined(__cplusplus)
}
#endif

#endif /* CY_EINK_SPIM_CY_SCB_SPI_PDL_H */


/* [] END OF FILE */
