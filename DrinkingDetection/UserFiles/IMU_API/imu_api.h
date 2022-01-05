/*
 ******************************************************************************
 * @file    imu_api.h
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
#ifndef IMU_API_H
#define IMU_API_H
#include "lsm6dsl_reg.h"

#ifdef __cplusplus
  extern "C" {
#endif

extern stmdev_ctx_t lsm6dsl_dev_ctx_t;
      
      
/*Defines*/
#define IMU_FIFO_SIZE 256
#define IMU_NUM_OF_AXIS 6
#define IMU_BUFFER_SIZE (IMU_FIFO_SIZE * IMU_NUM_OF_AXIS) * 2 /*Multiplied by 2,
                                                                because imu outputs 16-bit data 
                                                                but we read 8-bit registers*/

/**
  * \brief           This is imu data struct
  * \note            This structure is used to store converted imu data
  */
typedef struct {
    float acc_x[IMU_FIFO_SIZE]; /*!< X axis accelerometer data */
    float acc_y[IMU_FIFO_SIZE]; /*!< Y axis accelerometer data */
    float acc_z[IMU_FIFO_SIZE]; /*!< Z axis accelerometer data */
    float gyr_x[IMU_FIFO_SIZE]; /*!< X axis gyroscope data */
    float gyr_y[IMU_FIFO_SIZE]; /*!< Y axis gyroscope data */
    float gyr_z[IMU_FIFO_SIZE]; /*!< Z axis gyroscope data */
} imu_data_t;
      
      uint8_t imu_fifo_reset();
void imu_init(stmdev_ctx_t* ctx);
#ifdef __cplusplus
}
#endif

#endif // !IMU_API_H
