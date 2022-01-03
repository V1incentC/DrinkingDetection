
/*
 ******************************************************************************
 * @file    lsm6dsl_reg.c
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
#ifndef IMU_LOW_LEVEL_H
#define IMU_LOW_LEVEL_H

#include "nrfx_twim.h"
#include "nrf_gpio.h"
#include "pca10056.h"

#define DEVKIT /*Comment out if you're using the wristband*/

#if defined(DEVKIT)
#define IMU_LL_SCL        NRF_GPIO_PIN_MAP(1, 5)         //clock line
#define IMU_LL_SDA        NRF_GPIO_PIN_MAP(1, 6)         //data line
#define IMU_LL_INT1       NRF_GPIO_PIN_MAP(1, 7)         //for interrupts
#define IMU_LL_INT2       NRF_GPIO_PIN_MAP(1, 8)         //for interuppts
#define IMU_LL_STATUS_LED LED_1                     //led
#else
#define IMU_LL_SCL NRF_GPIO_PIN_MAP(0, 24)          //clock line
#define IMU_LL_SDA NRF_GPIO_PIN_MAP(0, 19)          //data line
#define IMU_LL_INT1 NRF_GPIO_PIN_MAP(0, 22)         //interrupts
#define IMU_LL_INT2 NRF_GPIO_PIN_MAP(0, 25)         //interuppts
#define IMU_LL_STATUS_LED NRF_GPIO_PIN_MAP(1, 14)    //led
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
 * \brief          Function for initializing input pins required to
 *                 sense imu interrupts and output pins for status LED
 */
void imu_ll_gpio_init();
#endif // !NRFX_TWIM_IMU_H
