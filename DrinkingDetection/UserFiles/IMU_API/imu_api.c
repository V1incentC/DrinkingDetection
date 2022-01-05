
#include <stdlib.h>
#include "imu_api.h"
#include "imu_low_level.h"

/* Global imu object */
stmdev_ctx_t lsm6dsl_dev_ctx_t; 
/* Pointer to the imu object */
static stmdev_ctx_t* p_lsm6dsl_dev_ctx_t;

uint8_t imu_data_buffer[IMU_BUFFER_SIZE];

static uint8_t imu_fifo_reset_dev(stmdev_ctx_t* ctx)
{
    lsm6dsl_fifo_mode_set(ctx, LSM6DSL_BYPASS_MODE); /* resets buffer */
    lsm6dsl_fifo_mode_set(ctx, LSM6DSL_FIFO_MODE);   /* buffer starts */
    
    imu_ll_prepare_dma(LSM6DSL_FIFO_DATA_OUT_L,
                       imu_data_buffer, IMU_BUFFER_SIZE);
    return EXIT_SUCCESS;
}

uint8_t imu_fifo_reset()
{
    imu_fifo_reset_dev(p_lsm6dsl_dev_ctx_t);
    return EXIT_SUCCESS;
}

uint8_t imu_fifo_setup(stmdev_ctx_t* ctx, uint16_t fifo_size)
{
    /*
     *  Enable Block Data Update
     */
    lsm6dsl_block_data_update_set(ctx, PROPERTY_ENABLE);
    lsm6dsl_auto_increment_set(ctx, PROPERTY_ENABLE);
    
    /* Before changing FIFO settings the device must be in bypass mode */
    lsm6dsl_fifo_mode_set(ctx, LSM6DSL_BYPASS_MODE);
    
    /* Set power modes for accelerometer and gyroscope */
    lsm6dsl_xl_power_mode_set(ctx, LSM6DSL_XL_NORMAL);
    lsm6dsl_gy_power_mode_set(ctx, LSM6DSL_GY_NORMAL); 
    
    /* Set Output Data Rate and full scale for accelerometer */
    lsm6dsl_xl_data_rate_set(ctx, LSM6DSL_XL_ODR_52Hz);
    lsm6dsl_xl_full_scale_set(ctx, LSM6DSL_2g);
    
    /* Set Output Data Rate and full scale for gyroscope */
    lsm6dsl_gy_data_rate_set(ctx, LSM6DSL_GY_ODR_52Hz);
    lsm6dsl_gy_full_scale_set(ctx, LSM6DSL_500dps);
    
    /* Set FIFO trigger - acc data ready signal */
    lsm6dsl_fifo_write_trigger_set(ctx, LSM6DSL_TRG_XL_GY_DRDY);
    /* Set watermark level(size of FIFO) */
    lsm6dsl_fifo_watermark_set(ctx, fifo_size * IMU_NUM_OF_AXIS);
    /* Enable FIFO threshold */
    lsm6dsl_fifo_stop_on_wtm_set(ctx, PROPERTY_ENABLE);
    
    /* Decimation - data to be stored in FIFO */
    lsm6dsl_fifo_gy_batch_set(ctx, LSM6DSL_FIFO_GY_NO_DEC);
    lsm6dsl_fifo_xl_batch_set(ctx, LSM6DSL_FIFO_XL_NO_DEC);
    lsm6dsl_fifo_dataset_3_batch_set(ctx, LSM6DSL_FIFO_DS3_DISABLE);
    lsm6dsl_fifo_dataset_4_batch_set(ctx, LSM6DSL_FIFO_DS4_DISABLE);
    
    /* FIFO data rate and mode */
    lsm6dsl_fifo_data_rate_set(ctx, LSM6DSL_FIFO_52Hz);
    lsm6dsl_fifo_mode_set(ctx, LSM6DSL_FIFO_MODE);
    
    return EXIT_SUCCESS;
}


uint8_t imu_device_check(stmdev_ctx_t* ctx)
{
    /* Check device ID */
    uint8_t whoamI;
    lsm6dsl_device_id_get(ctx, &whoamI);
    if (whoamI != LSM6DSL_ID)
    {
        while (1) ;
    }
    /* Restore default configuration */    
    uint8_t reset;
    lsm6dsl_reset_set(ctx, reset);
    do 
    {
        lsm6dsl_reset_get(ctx, &reset);
    } while (reset);
    
    return EXIT_SUCCESS;
}


uint8_t imu_restore_default_configuration(stmdev_ctx_t* ctx)
{
    /* Restore default configuration */    
    uint8_t reset;
    lsm6dsl_reset_set(ctx, reset);
    do 
    {
        lsm6dsl_reset_get(ctx, &reset);
    } while (reset);
    
    return EXIT_SUCCESS;
}

void imu_init(stmdev_ctx_t* ctx)
{
  
    p_lsm6dsl_dev_ctx_t = ctx;
    p_lsm6dsl_dev_ctx_t->read_reg =  *(stmdev_read_ptr)  imu_ll_platform_read;
    p_lsm6dsl_dev_ctx_t->write_reg = *(stmdev_write_ptr) imu_ll_platform_write;
    
    imu_ll_gpio_init();         /* Initialize inputs and interrupts used for the IMU */
    imu_ll_twi_master_init();   /* Initialize TWI pins and frequency, enable TWI */
    
    /* Check id and reset device */
    imu_device_check(p_lsm6dsl_dev_ctx_t);
    /* FIFO mode setup */
    imu_fifo_setup(p_lsm6dsl_dev_ctx_t, IMU_FIFO_SIZE);
    
    
    /*TEMPORARY CODE*/
    //INTERRUPT 2
    lsm6dsl_int2_route_t int2_route = 
    {
        .int2_full_flag = PROPERTY_ENABLE
    };
    
	
    lsm6dsl_pin_int2_route_set(p_lsm6dsl_dev_ctx_t, int2_route); 
    /*TEMPORARY CODE*/
    
    
    /* Set up the dma transfer */
    imu_ll_prepare_dma(LSM6DSL_FIFO_DATA_OUT_L,
                       imu_data_buffer, IMU_BUFFER_SIZE);
}