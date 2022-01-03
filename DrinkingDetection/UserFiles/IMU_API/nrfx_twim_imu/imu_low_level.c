
/*
 ******************************************************************************
 * @file    imu_low_level.c
 * @author  Sensors Software Solution Team
 * @brief   LSM6DSL driver file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
#include "imu_low_level.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrfx_twim.h"
#include "nrfx_gpiote.h"
#include "nrfx_ppi.h"
#include "nrf_gpio.h"
#include "lsm6dsl_reg.h"

/*Initializing TWIM0 instance*/
#define TWIM_INSTANCE_ID    0
#define TWIM_FREQ           NRF_TWIM_FREQ_100K
#define TWIM_IRQ_PRIORITY   APP_IRQ_PRIORITY_HIGH


nrfx_twim_t const twim_t = NRFX_TWIM_INSTANCE(TWIM_INSTANCE_ID);

volatile bool m_xfer_done = true, fifo_transfer_done = false; 

/**
  * @defgroup    IMU_LOW_LEVEL
  * @brief       This file provides a set of low level functions needed to drive the
  *              lsm6dsl enanced inertial module on NRF52.
  * @{
  *
  */

static void merge_register_and_data(uint8_t * new_buffer, uint8_t reg, 
                                    uint8_t * p_data, size_t length)
{
	new_buffer[0] = reg;
	memcpy((new_buffer + 1), p_data, length);
}

/**
  * @defgroup    IMU_LL_interfaces_functions
  * @brief       This section provide an implementation of platform read
  *              and write functions.
  *              MANDATORY: return 0 -> no Error.
  * @{
  *
  */
int32_t imu_ll_platform_write(void*    handle,
                              uint8_t  reg,
                              uint8_t* buffer,
                              size_t length) 
{

    uint8_t twi_tx_buffer[length + 1];
	ret_code_t err_code;
		
    merge_register_and_data(twi_tx_buffer, reg, buffer, length);
	
	m_xfer_done = false;
    nrfx_twim_tx(&twim_t, LSM6DSL_ADDRESS, twi_tx_buffer, length + 1, false);
    if (err_code != NRFX_SUCCESS) 
    {
        return err_code;
    }
    
	while (m_xfer_done == false) 
	{
    	__WFE();    /*Wait for the transfer to finish*/
	}    
	
	m_xfer_done = false;
    
    return 0;
}


int32_t imu_ll_platform_read(void*    handle,
                             uint8_t  reg,
                             uint8_t* buffer,
                             size_t length) 
{

	uint8_t tx_data[1] = { reg };
	ret_code_t err_code;
	
	
	m_xfer_done = false;
    err_code = nrfx_twim_tx(&twim_t, LSM6DSL_ADDRESS, tx_data, sizeof(tx_data), true);
    if (err_code != NRFX_SUCCESS) 
    {
        return err_code;
    }
    
	while (m_xfer_done == false) {__WFE();}    /*Wait for the transfer to finish*/
	
	m_xfer_done = false;	/*Reset the flag for read operation*/
	err_code =  nrfx_twim_rx(&twim_t, LSM6DSL_ADDRESS, buffer, length);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    
	while (m_xfer_done == false)
	{
    	__WFE();	/*Wait for the transfer to finish*/
	}
	m_xfer_done = false;
	
    return 0;
			

}
/**
  * @}
  *
  */

 static void twim_handler(nrfx_twim_evt_t const * p_event, void * p_context) {

	/*Check the event to see what type of event occurred*/
	switch (p_event->type) {
        /*If data transmission or receiving is finished*/
	case NRFX_TWIM_EVT_DONE:
		if (p_event->xfer_desc.type == NRFX_TWIM_XFER_RX) {
			
			m_xfer_done = true;    /*Set the flag*/ 
		}
		else if (p_event->xfer_desc.type == NRFX_TWIM_XFER_TXRX) {

			m_xfer_done = true;    /*Set the flag*/
 		
			fifo_transfer_done = true;    /*Set the flag*/
			
			
		}
		else if (p_event->xfer_desc.type == NRFX_TWIM_XFER_TX)
		{
			m_xfer_done = true;    /*Set the flag*/
		}
		else if (p_event->xfer_desc.type == NRFX_TWIM_XFER_TXTX)
		{
			m_xfer_done = true;
		}
		
		
		break;
    																		   
	default:
		// do nothing
		break;
	}
}


void imu_ll_twi_master_init()
{
    /*TWI init struct*/
    nrfx_twim_config_t config = {
        .scl = IMU_LL_SCL,
        .sda = IMU_LL_SDA,
        .frequency = TWIM_FREQ,
        .interrupt_priority = TWIM_IRQ_PRIORITY,
    };
    APP_ERROR_CHECK(nrfx_twim_init(&twim_t, &config, twim_handler, NULL));
	
    nrfx_twim_enable(&twim_t);
	
}


void imu_ll_twim_dma_init(uint8_t  ppi_trigger_pin,
                          uint8_t  reg,
                          uint8_t* buffer, 
                          size_t   buffer_length)
{
	
    ret_code_t err_code;
    /*Variable to store address od interrupt event*/
    uint32_t event_address_interrupt;
    /*Variable to store address of twi start task*/
    uint32_t task_address_twim_start;
    /*Variable holding ppi channel*/
    nrf_ppi_channel_t ppi_channel_twim_start;
    
	/*Create a transfer description for DMA*/
    nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TXRX(LSM6DSL_ADDRESS,
                                                          &reg, 1,
                                                          buffer, buffer_length);
	
    uint32_t flags =   NRFX_TWIM_FLAG_RX_POSTINC  | NRFX_TWIM_FLAG_HOLD_XFER;
    err_code = nrfx_twim_xfer(&twim_t, &xfer, flags); 
	

    /* Get the address of the TWI start task.*/
    task_address_twim_start = nrfx_twim_start_task_get(&twim_t, xfer.type);
    /*Get the address of the interrupt instance*/
    event_address_interrupt = nrfx_gpiote_in_event_addr_get(ppi_trigger_pin);
	
    /*Allocate a ppi channel*/
    err_code = nrfx_ppi_channel_alloc(&ppi_channel_twim_start);
    APP_ERROR_CHECK(err_code);
	
    /*Connect the interrupt event with twim start using ppi*/
    err_code = nrfx_ppi_channel_assign(ppi_channel_twim_start,
                                       event_address_interrupt,
                                       task_address_twim_start);
    APP_ERROR_CHECK(err_code);
	
    /*Enable the ppi channel*/
    err_code = nrfx_ppi_channel_enable(ppi_channel_twim_start);
    APP_ERROR_CHECK(err_code);
	
}


void imu_ll_prepare_dma(uint8_t  reg,
                        uint8_t* buffer, 
                        size_t   buffer_length)
{
    /*Describe a TXRX transfer where we first write to a register we want to read
      and then point to the recieve buffer*/
    nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TXRX(LSM6DSL_ADDRESS, 
                                                          &reg, sizeof(reg),
                                                          buffer, buffer_length);
	
	/*Set flag to automatically increment recieve buffer index
	  and hold the transfer until the PPI event triggers it*/
    uint32_t flags = NRFX_TWIM_FLAG_RX_POSTINC | NRFX_TWIM_FLAG_HOLD_XFER;
    ret_code_t err_code = nrfx_twim_xfer(&twim_t, &xfer, flags);
    APP_ERROR_CHECK(err_code);  
}

static void int1_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    
}

void imu_ll_gpio_init()
{
    ret_code_t err_code;
    err_code = nrfx_gpiote_init();
    APP_ERROR_CHECK(err_code);
    
    nrfx_gpiote_in_config_t in_config_t = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    
    /*Configure pin to sense INT1 of LSM6DSL*/
    err_code = nrfx_gpiote_in_init(IMU_LL_INT1, &in_config_t, int1_handler);
    APP_ERROR_CHECK(err_code);
    /*Enable interrupt event*/
    nrfx_gpiote_in_event_enable(IMU_LL_INT1, true);
    
    /*Configure pin to sense INT2 of LSM6DSL*/
    err_code = nrfx_gpiote_in_init(IMU_LL_INT2, &in_config_t, NULL);
    APP_ERROR_CHECK(err_code);
    /*Enable interrupt event*/
    nrfx_gpiote_in_event_enable(IMU_LL_INT2, false);
    
   /*Configure BSP LED or wristband LED depending on .h file setup*/
    nrf_gpio_cfg_output(IMU_LL_STATUS_LED);

}
/**
  * @}
  *
  */