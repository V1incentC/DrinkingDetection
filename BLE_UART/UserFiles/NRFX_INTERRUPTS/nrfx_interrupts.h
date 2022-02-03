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
#ifndef NRFX_INTERRUPTS_H
#define NRFX_INTERRUPTS_H

#ifdef __cplusplus
  extern "C" {
#endif
   
#include "nrfx_gpiote.h"
#include "nrf_gpio.h"
      
/**
  * @defgroup    NRFX_INTERRUPTS
  * @brief       This file provides a set of interrupt handler functions.
  * @{
  *
  */   
      
/**
  * \brief           Function for handling the gpio interrupt triggered by the IMU on
  *                  pin INT2.
  * 
  * \param[in]       pin:       Pin that triggered this event.
  * \param[in]       action:    Action that led to triggering this event.
  */       
void int2_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);      
      
void int1_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action); 
      
void push_button_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
    
/**
  * @}
  */               
#ifdef __cplusplus
}
#endif  

#endif // !NRFX_INTERRUPTS_H