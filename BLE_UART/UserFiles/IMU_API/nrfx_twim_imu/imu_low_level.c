/*
 ******************************************************************************
 * @file    imu_low_level.c
 * @author  Vincent Cergolj
 * @brief   LSM6DSL low level driver file
 ******************************************************************************
*/
/*
 * Copyright (c) 2021 Vincent Cergolj
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of IMU API.
 *
 * Author:          Vincent Cergolj
 */
#include "imu_low_level.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf_log.h"
#include "nrfx_twim.h"
#include "nrfx_gpiote.h"
#include "nrfx_ppi.h"
#include "nrf_gpio.h"
#include "lsm6dsl_reg.h"
#include "nrfx_interrupts.h"

#include "imu_api.h"

/*Initializing TWIM0 instance*/
#define TWIM_INSTANCE_ID    0
#define TWIM_FREQ           NRF_TWIM_FREQ_400K
#define TWIM_IRQ_PRIORITY   APP_IRQ_PRIORITY_HIGH
#define IMU_TWIM_TIMEOUT    10000 


nrfx_twim_t const twim_t = NRFX_TWIM_INSTANCE(TWIM_INSTANCE_ID);

volatile bool m_xfer_done = true, fifo_transfer_done = false; 

bool imu_ll_is_fifo_transfer_complete()
{
    return fifo_transfer_done;
}

void imu_ll_set_fifo_transfer_complete()
{
    fifo_transfer_done = true;
}

void imu_ll_clear_fifo_transfer_complete()
{
    fifo_transfer_done = false;
}

static void merge_register_and_data(uint8_t * new_buffer, uint8_t reg, 
                                    uint8_t * p_data, size_t length)
{
	new_buffer[0] = reg;
	memcpy((new_buffer + 1), p_data, length);
}


int32_t imu_ll_platform_write(void*    handle,
                              uint8_t  reg,
                              uint8_t* buffer,
                              size_t length) 
{

    uint8_t twi_tx_buffer[length + 1];
	ret_code_t err_code;
    uint32_t timeout = IMU_TWIM_TIMEOUT;
		
    merge_register_and_data(twi_tx_buffer, reg, buffer, length);
    
	m_xfer_done = false;
    nrfx_twim_tx(&twim_t, LSM6DSL_ADDRESS, twi_tx_buffer, sizeof(twi_tx_buffer), false);
    while ((!m_xfer_done) && --timeout) 
    {
        __WFE(); /*Wait for the transfer to finish*/
    }
    
    if (err_code != NRFX_SUCCESS) 
    {
        //APP_ERROR_CHECK(err_code);
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
    uint32_t timeout = IMU_TWIM_TIMEOUT;
	
	m_xfer_done = false;
    err_code = nrfx_twim_tx(&twim_t, LSM6DSL_ADDRESS, tx_data, sizeof(tx_data), true);
    while ((!m_xfer_done) && --timeout) 
    {
        __WFE(); /*Wait for the transfer to finish*/
    } 
    
    if (err_code != NRFX_SUCCESS) 
    {
        //APP_ERROR_CHECK(err_code);
    }
    
   
	
	m_xfer_done = false;	/*Reset the flag for read operation*/
    timeout = IMU_TWIM_TIMEOUT;
	err_code =  nrfx_twim_rx(&twim_t, LSM6DSL_ADDRESS, buffer, length);
    while ((!m_xfer_done) && --timeout)
    {
        __WFE(); /*Wait for the transfer to finish*/
    }
 
    if (err_code != NRF_SUCCESS) {
        //APP_ERROR_CHECK(err_code);
    }
    

	m_xfer_done = false;
	
    return 0;
			

}


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
		
    	m_xfer_done = true;
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
                                                          &reg, sizeof(reg),
                                                          buffer, buffer_length);
	
    uint32_t flags =   NRFX_TWIM_FLAG_RX_POSTINC  | NRFX_TWIM_FLAG_HOLD_XFER;
    //err_code = nrfx_twim_xfer(&twim_t, &xfer, flags); 
	

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
    err_code = nrfx_gpiote_in_init(IMU_LL_INT2, &in_config_t, int2_handler);
    APP_ERROR_CHECK(err_code);
    /*Enable interrupt event*/
    nrfx_gpiote_in_event_enable(IMU_LL_INT2, true);
    
    in_config_t.pull = NRF_GPIO_PIN_PULLUP;
    /*Configure pin to sense push button of the wristband*/
    err_code = nrfx_gpiote_in_init(IMU_LL_PUSH_BUTTON, &in_config_t, push_button_handler);
    APP_ERROR_CHECK(err_code);
    /*Enable interrupt event*/
    nrfx_gpiote_in_event_enable(IMU_LL_PUSH_BUTTON, true);
    
   /*Configure BSP LED or wristband LED depending on .h file setup*/
    nrf_gpio_cfg_output(IMU_LL_STATUS_LED);
    nrf_gpio_pin_clear(IMU_LL_STATUS_LED);
    NRF_LOG_INFO("GPIO config finished");
}
