/*
 ******************************************************************************
 * @file    imu_low_level.h
 * @author  Sensors Software Solution Team
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
#ifndef IMU_LOW_LEVEL_H
#define IMU_LOW_LEVEL_H

#ifdef __cplusplus
  extern "C" {
#endif
      
#include "nrfx_twim.h"
#include "nrf_gpio.h"
#include "pca10056.h"

#define DEVKIT /*Comment out if you're using the wristband*/

#if defined(DEVKIT)
#define IMU_LL_SCL        NRF_GPIO_PIN_MAP(1, 5)         //clock line
#define IMU_LL_SDA        NRF_GPIO_PIN_MAP(1, 6)         //data line
#define IMU_LL_INT1       NRF_GPIO_PIN_MAP(1, 7)         //for interrupts
#define IMU_LL_INT2       NRF_GPIO_PIN_MAP(1, 8)         //for interuppts
#define IMU_LL_STATUS_LED LED_1                          //led
#else
#define IMU_LL_SCL NRF_GPIO_PIN_MAP(0, 24)               //clock line
#define IMU_LL_SDA NRF_GPIO_PIN_MAP(0, 19)               //data line
#define IMU_LL_INT1 NRF_GPIO_PIN_MAP(0, 22)              //interrupts
#define IMU_LL_INT2 NRF_GPIO_PIN_MAP(0, 25)              //interuppts
#define IMU_LL_STATUS_LED NRF_GPIO_PIN_MAP(1, 14)        //led
#endif 

#if defined(DEVKIT)
#define LSM6DSL_ADDRESS 0x6B                 /*TWI address of sensor*/
#else
#define LSM6DSL_ADDRESS 0x6A                 /*TWI address of sensor*/
#endif
extern const nrfx_twim_t twim_t;
extern volatile bool fifo_transfer_done; /*!< Flag for indicating DMA complete transfer event*/
extern volatile bool m_xfer_done; /*!< Flag for indicating TWIM transfer event done*/
      
/**
  * @defgroup    IMU_LOW_LEVEL
  * @brief       This file provides a set of low level functions needed to drive the
  *              lsm6dsl enanced inertial module on NRF52.
  * @{
  *
  */
      
/**
  * @defgroup    IMU_LL_interfaces_functions
  * @brief       This section provide an implementation of platform read
  *              and write functions.
  *              MANDATORY: return 0 -> no Error.
  * @{
  *
  */
      
/**
 * \brief           Hardware specific function for TWI interface writing
 * 
 * \param[in]       handle: Pointer to lsm6dsl object
 * \param[in]       reg:    Register we want to adress
 * \param[in]       buffer: Pointer to a buffer containing data we want to write to the device
 * \param[in]       len:    Length of the buffer
 * \return          ret_code_t error code
 */
int32_t imu_ll_platform_write(void*    handle,
                              uint8_t  reg,
                              uint8_t* buffer,
                              size_t len); 


/**
 * \brief           Hardware specific function for TWI interface writing
 * 
 * \param[in]       handle: Pointer to lsm6dsl object
 * \param[in]       reg:    Register we want to adress
 * \param[in]       buffer: Pointer to a buffer for storing the read data
 * \param[in]       len:    Length of the buffer
 * \return          ret_code_t error code
 */
int32_t imu_ll_platform_read(void*    handle,
                             uint8_t  reg,
                             uint8_t* buffer,
                             size_t len);

/**
  * @}
  *
  */
      
/**
  * @defgroup    IMU_LL_TWI_and_DMA_functions
  * @brief       This section groups implementation of TWI and DMA setup
  *              functions.
  *              
  * @{
  *
  */    
      
/**
 * \brief          Function for initializing and enabling TWI 
 *
 */
void imu_ll_twi_master_init();


/**
 * \brief           Function for initializing TWIM DMA reading triggered by PPI and GPIO
 * 
 * \param[in]       ppi_trigger_pin: GPIO pin we want to use for triggering the PPI
 * \param[in]       reg:             Register we want to read data from
 * \param[in]       buffer:          Pointer to a buffer for storing the read data
 * \param[in]       buffer_length:   Length of the buffer
 */
void imu_ll_twim_dma_init(uint8_t  ppi_trigger_pin,
                          uint8_t  reg,
                          uint8_t* buffer, 
                          size_t   buffer_length);


/**
 * \brief           Function for preparing TWIM for a new DMA transfer
 * 
 * \param[in]       reg:             Register we want to read data from
 * \param[in]       buffer:          Pointer to a buffer for storing the read data
 * \param[in]       buffer_length:   Length of the buffer
 */
void imu_ll_prepare_dma(uint8_t  reg,
                        uint8_t* buffer, 
                        size_t   buffer_length);
/**
  * @}
  *
  */
      
/**
  * @defgroup    IMU_LL_GPIO_functions
  * @brief       This section groups GPIO specific functions.
  *               
  * @{ 
  *
  */ 
      
/**
 * \brief          Function for initializing input pins required to
 *                 sense imu interrupts and output pins for status LED
 */
void imu_ll_gpio_init();
    
/**
  * @}
  * 
  */  
      
/**
  * @}
  */  
#ifdef __cplusplus
}
#endif  

#endif // !NRFX_TWIM_IMU_H
