
#include <stdlib.h>
#include <arm_math.h>
#include "imu_api.h"
#include "imu_low_level.h"
#include "nrf_log.h"
#include "nrf_delay.h"

#define LATENCY_MIN 40

/** \brief   Global imu object */
stmdev_ctx_t lsm6dsl_dev_ctx_t; 
/** \brief   Pointer to the imu object */
 stmdev_ctx_t* p_lsm6dsl_dev_ctx_t;

/** \brief   Array used for storing fifo data */
uint8_t imu_data_buffer[IMU_BUFFER_SIZE];

void imu_activity_inactivity_setup(stmdev_ctx_t *ctx)
{
    lsm6dsl_xl_power_mode_set(ctx, LSM6DSL_XL_NORMAL);
    /* Set Output Data Rate and full scale for accelerometer
     * Disable gyroscope
     **/
    lsm6dsl_xl_data_rate_set(ctx, LSM6DSL_XL_ODR_52Hz);
    lsm6dsl_xl_full_scale_set(ctx, LSM6DSL_2g);
    lsm6dsl_gy_data_rate_set(ctx, LSM6DSL_GY_ODR_OFF);
    /* set duration for inactivity detection */
    lsm6dsl_act_sleep_dur_set(ctx, IMU_SLEEP_DUR); /* Comes out as 10 seconds */

    /* Set Activity/Inactivity threshold */
    lsm6dsl_wkup_threshold_set(ctx, IMU_WKUP_THRESHOLD); //value is 2 which comes out as 62.5mg

    // Inactivity configuration: acc to 12.5 LP, gyro to Power-Down
    lsm6dsl_act_mode_set(ctx, LSM6DSL_XL_12Hz5_GY_PD);
    //enable slope filter
    lsm6dsl_xl_hp_path_internal_set(ctx, LSM6DSL_USE_SLOPE);
}


static uint8_t convert_angle_to_int(double angle)
{
    
    return (uint8_t) 64*(sin(angle * PI / 180));
}


void imu_absolte_wrist_tilt_setup(stmdev_ctx_t* ctx,
                                  uint16_t latency_ms,
                                  lsm6dsl_a_wrist_tilt_mask_t* wrist_tilt_mask,
                                  float trigger_angle_deg)
{
    uint8_t latency_converted;
    
    /* enable embedded functions */
    lsm6dsl_wrist_tilt_sens_set(ctx, PROPERTY_ENABLE);
    nrf_delay_ms(50);
    /* disable embedded functions */ 
    lsm6dsl_wrist_tilt_sens_set(ctx, PROPERTY_DISABLE);

    /* Set latency 1lsb=40ms
     * Divide by LATENCY_MIN(40) to get correct bit value for the latency set function
     **/
    latency_converted = latency_ms / LATENCY_MIN;
    lsm6dsl_tilt_latency_set(ctx, &latency_converted);
    /* set threshold */
    uint8_t threshold = convert_angle_to_int(trigger_angle_deg);
    lsm6dsl_tilt_threshold_set(ctx, &threshold);

    /* Pass the mask pointer to set the trigger plane */
    lsm6dsl_tilt_src_set(ctx, wrist_tilt_mask);

    //enable embedded functions
    lsm6dsl_wrist_tilt_sens_set(ctx, PROPERTY_ENABLE);

    lsm6dsl_data_ready_mode_set(ctx, LSM6DSL_DRDY_PULSED);
}
uint8_t imu_fifo_disable(stmdev_ctx_t*ctx)
{
    lsm6dsl_xl_power_mode_set(ctx, LSM6DSL_XL_NORMAL);
    lsm6dsl_gy_power_mode_set(ctx, LSM6DSL_GY_NORMAL);
    lsm6dsl_fifo_mode_set(ctx, LSM6DSL_BYPASS_MODE);
    lsm6dsl_gy_data_rate_set(ctx, LSM6DSL_GY_ODR_OFF);
    
    return EXIT_SUCCESS;
}


static uint8_t imu_fifo_reset_dev(stmdev_ctx_t* ctx)
{
    lsm6dsl_fifo_mode_set(ctx, LSM6DSL_BYPASS_MODE); /* resets buffer */
    lsm6dsl_fifo_mode_set(ctx, LSM6DSL_FIFO_MODE);   /* buffer starts */

    return EXIT_SUCCESS;
}

uint8_t imu_fifo_reset()
{
    imu_fifo_reset_dev(p_lsm6dsl_dev_ctx_t);
    
    imu_ll_prepare_dma(LSM6DSL_FIFO_DATA_OUT_L,
                    imu_data_buffer, IMU_BUFFER_SIZE);
    
    return EXIT_SUCCESS;
}


uint8_t imu_is_fifo_full(stmdev_ctx_t *ctx)
{
    lsm6dsl_reg_t reg;
    int32_t ret;

    ret = lsm6dsl_read_reg(ctx, LSM6DSL_FIFO_STATUS2, &reg.byte, 1);
    if (ret != 0)
    {
        NRF_LOG_INFO("Error in imu_is_fifo_full function");

    }
    /* Check the fifo full smart flag in status register */
    if (reg.fifo_status2.fifo_full_smart == true)
        return 1;
    else
        return 0;
}

uint8_t imu_fifo_mode(stmdev_ctx_t *ctx, uint16_t fifo_size)
{
    lsm6dsl_int2_route_t int2_route;
    
    imu_fifo_setup(ctx, fifo_size); /* Configure fifo */
    imu_fifo_reset();
    /* Set fifo full flag on INT2 pin */
    int2_route.int2_full_flag =  PROPERTY_ENABLE;
    int2_route.int2_wrist_tilt = PROPERTY_DISABLE;
    int2_route.int2_inact_state = PROPERTY_DISABLE;

    lsm6dsl_pin_int2_route_set(ctx, int2_route);
    
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
    lsm6dsl_int1_route_t int1_route = 
    {
        .int1_full_flag = PROPERTY_ENABLE
       
         
    };
    
    lsm6dsl_pin_int1_route_set(p_lsm6dsl_dev_ctx_t, int1_route); 
    /*TEMPORARY CODE*/
    
    
    /* Set up the dma transfer */
    imu_ll_twim_dma_init(IMU_LL_INT1,
                         LSM6DSL_FIFO_DATA_OUT_L,
                         imu_data_buffer, IMU_BUFFER_SIZE);
                       
}