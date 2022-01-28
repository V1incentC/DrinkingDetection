
#include <stdlib.h>
#include <arm_math.h>
#include "imu_api.h"
#include "imu_low_level.h"
#include "nrf_log.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "machine_learning_features.h"
#include "XGBoost.h"

#define LATENCY_MIN 40

/** \brief   Global imu object */
stmdev_ctx_t lsm6dsl_dev_ctx_t; 
/** \brief   Pointer to the imu object */
 stmdev_ctx_t* p_lsm6dsl_dev_ctx_t;

/** \brief   Array used for storing converted fifo data */
imu_data_t imu_data;
/** \brief   Array used for storing fifo data */
uint8_t imu_data_buffer[IMU_BUFFER_SIZE];

uint8_t imu_activity_inactivity_setup(stmdev_ctx_t *ctx)
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
    lsm6dsl_wkup_threshold_set(ctx, IMU_WKUP_THRESHOLD); /* value is 2 which comes out as 62.5mg */

    /* Inactivity configuration: acc to 12.5 LP, gyro to Power-Down */
    lsm6dsl_act_mode_set(ctx, LSM6DSL_XL_12Hz5_GY_PD);
    /* enable slope filter */
    lsm6dsl_xl_hp_path_internal_set(ctx, LSM6DSL_USE_SLOPE);
    
    return EXIT_SUCCESS;
}


static uint8_t convert_angle_to_int(double angle)
{
    /* Equation extracted from datasheet pg. 55*/
    return (uint8_t) 64*(sin(angle * PI / 180));
}


uint8_t imu_absolte_wrist_tilt_setup(stmdev_ctx_t*                 ctx,
                                  uint16_t                      latency_ms,
                                  lsm6dsl_a_wrist_tilt_mask_t*  wrist_tilt_mask,
                                  float                         trigger_angle_deg)
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
    
    return EXIT_SUCCESS;
}
uint8_t imu_awt_detection_mode(stmdev_ctx_t* ctx)
{
    lsm6dsl_int2_route_t int2_route;
    
    imu_fifo_disable(ctx);
    
    int2_route.int2_wrist_tilt = PROPERTY_ENABLE;
    lsm6dsl_pin_int2_route_set(ctx, int2_route);
    
    imu_activity_inactivity_setup(ctx);
    
    return EXIT_SUCCESS;
}
uint8_t imu_fifo_disable(stmdev_ctx_t* ctx)
{
    lsm6dsl_int1_route_t int1_route =
    { 
        .int1_full_flag =  PROPERTY_DISABLE
    };
    
    
    lsm6dsl_pin_int1_route_set(ctx, int1_route);
    
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
    lsm6dsl_int1_route_t int1_route =
    {
        //.int1_full_flag =  PROPERTY_ENABLE,
        .int1_fth = PROPERTY_ENABLE
    };
    lsm6dsl_int2_route_t int2_route =
    { 
        .int2_wrist_tilt = PROPERTY_DISABLE        
     };
    

    /* Set fifo full flag on INT1 pin */
    lsm6dsl_pin_int1_route_set(ctx, int1_route);
    /* Disable AWT interrupt */
    lsm6dsl_pin_int2_route_set(ctx, int2_route);
    
    imu_fifo_setup(ctx, fifo_size); /* Configure fifo */
    imu_fifo_reset();

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
    lsm6dsl_fifo_watermark_set(ctx, fifo_size * IMU_NUM_OF_AXIS + 3);
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
        NRF_LOG_INFO("whoamI not matching");
        //while (1) ;
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
static void imu_parse_data_buffer(imu_data_t* data)
{
    axis3bit16_t data_raw_acceleration;
    axis3bit16_t data_raw_gyroscope;
    const uint8_t offset = IMU_NUM_OF_AXIS * 2, data_width = IMU_NUM_OF_AXIS;
    char print_buffer[250];
    uint8_t len;
    
    for (uint16_t i = 0; i < IMU_FIFO_SIZE; ++i)
    {

				
        memcpy( data_raw_gyroscope.u8bit,
                &imu_data_buffer[i*offset],
                6);
        
        data->gyr_x[i] = 
        
            lsm6dsl_from_fs500dps_to_mdps(data_raw_gyroscope.i16bit[0]) / 1000;

        data->gyr_y[i] = 
            
            lsm6dsl_from_fs500dps_to_mdps(data_raw_gyroscope.i16bit[1]) / 1000;

        data->gyr_z[i] = 
            
            lsm6dsl_from_fs500dps_to_mdps(data_raw_gyroscope.i16bit[2]) / 1000;

        
        //acc code

        memcpy( data_raw_acceleration.u8bit,
               &imu_data_buffer[i*offset + data_width],
                6);
        
        data->acc_x[i] = 
            
            lsm6dsl_from_fs2g_to_mg(data_raw_acceleration.i16bit[0]) / 1000; 

        data->acc_y[i] = 
            
            lsm6dsl_from_fs2g_to_mg(data_raw_acceleration.i16bit[1]) / 1000;

        data->acc_z[i] = 
            
            lsm6dsl_from_fs2g_to_mg(data_raw_acceleration.i16bit[2]) / 1000;

       len = sprintf(print_buffer," %f, %f, %f, %f, %f, %f \n",
                                                            data->acc_x[i],
                                                            data->acc_y[i],
                                                            data->acc_z[i],
                                                            data->gyr_x[i],
                                                            data->gyr_y[i],
                                                            data->gyr_z[i]);
        
       /* NRF_LOG_INFO("%s",print_buffer); */ 
        ble_send_string(print_buffer, len);                                                            
        //printf("%d", fifo_data[i]);
    } 
    
}


void imu_read_fifo(stmdev_ctx_t* ctx,imu_data_t* data)
{
    axis3bit16_t data_raw_acceleration;
    axis3bit16_t data_raw_gyroscope;
    const uint8_t offset = IMU_NUM_OF_AXIS * 2, data_width = IMU_NUM_OF_AXIS;
    char print_buffer[100];
    uint8_t len;
    
    for (uint16_t i = 0; i < IMU_FIFO_SIZE; ++i)
    {

				
        memset(data_raw_gyroscope.u8bit, 0x00, data_width);
        lsm6dsl_fifo_raw_data_get(ctx, data_raw_gyroscope.u8bit, data_width);
        
        data->gyr_x[i] = 
        
            lsm6dsl_from_fs500dps_to_mdps(data_raw_gyroscope.i16bit[0]) / 1000;

        data->gyr_y[i] = 
            
            lsm6dsl_from_fs500dps_to_mdps(data_raw_gyroscope.i16bit[1]) / 1000;

        data->gyr_z[i] = 
            
            lsm6dsl_from_fs500dps_to_mdps(data_raw_gyroscope.i16bit[2]) / 1000;

        
        //acc code

        memset(data_raw_acceleration.u8bit, 0, data_width);
        lsm6dsl_fifo_raw_data_get(ctx, data_raw_acceleration.u8bit, data_width);
        
        data->acc_x[i] = 
            
            lsm6dsl_from_fs2g_to_mg(data_raw_acceleration.i16bit[0]) / 1000; 

        data->acc_y[i] = 
            
            lsm6dsl_from_fs2g_to_mg(data_raw_acceleration.i16bit[1]) / 1000;

        data->acc_z[i] = 
            
            lsm6dsl_from_fs2g_to_mg(data_raw_acceleration.i16bit[2]) / 1000;
  
                                                         
        //printf("%d", fifo_data[i]);
    }
    imu_fifo_reset_dev(ctx);
   
//    for (uint16_t i = 0; i < IMU_FIFO_SIZE; ++i)
//    {
//        len = sprintf(print_buffer,
//            " %f, %f, %f, %f, %f, %f \n",
//            data->acc_x[i],
//            data->acc_y[i],
//            data->acc_z[i],
//            data->gyr_x[i],
//            data->gyr_y[i],
//            data->gyr_z[i]);
//        
//        /* NRF_LOG_INFO("%s",print_buffer); */ 
//        ble_send_string(print_buffer, len);   
//        
//    }
    
}

void fill_features_array(imu_data_t* data, float* features)
{
    
    float *axis[6] = { data->acc_x, data->acc_y, data->acc_x,
                        data->gyr_x, data->gyr_y, data->gyr_z };
   
    const uint8_t raw_feature_offset      = 13,
                    frequency_feature_offset = 36,
                    all_raw_length           = 78;
        
    for (uint16_t i = 0; i < 6; i++)
    {
        mlf_fill_raw_drinking_features(&features[i * raw_feature_offset],
                                        axis[i],
                                        IMU_FIFO_SIZE);
    }
    for (uint16_t i = 0; i < 6; i++)
    {
        mlf_fill_frequency_eating_features(
            &features[i * frequency_feature_offset + all_raw_length],
            axis[i],
            IMU_FIFO_SIZE);
    }
}

void imu_predict(imu_data_t* data, float* result)
{
    float features[MLF_NUM_OF_FEATURES] = { 0 };
    
    fill_features_array(data, features);
    
    score(features, result);
    
    
}
float result[2];


void imu_handle_fifo_transfer_done()
{
    static uint8_t counter = 0;
    uint8_t len;
    uint8_t buffer[50];
    
    
    if (fifo_transfer_done)
    {
        	
        	
        NRF_LOG_INFO("FIFO full: %d", counter);
        	
        fifo_transfer_done = false;
        
        len =  sprintf(buffer, "FIFO full: %d \n", counter);
        //ble_send_string(buffer, len);
        
        
        ++counter;  
        if (counter >= 5)
        {
            NRF_LOG_INFO("FIFO over");
            imu_fifo_disable(p_lsm6dsl_dev_ctx_t);

            imu_awt_detection_mode(&lsm6dsl_dev_ctx_t);
            counter = 0;
            
            len =  sprintf(buffer, "Fifo over \n");
            //ble_send_string(buffer, len);
        }
        else
        {
            imu_fifo_reset();
        }
        imu_parse_data_buffer(&imu_data);
        
        imu_predict(&imu_data, result);
        
        len =  sprintf(buffer, "res[0] = %f  res[1] = %f \n", result[0], result[1]);
        //ble_send_string(buffer, len);
        NRF_LOG_INFO("%s", buffer);
    }
    /*
    while (!fifo_transfer_done)
    {
        __WFE();
    }*/
}
void imu_init(stmdev_ctx_t* ctx)
{
  
    p_lsm6dsl_dev_ctx_t = ctx;
    p_lsm6dsl_dev_ctx_t->read_reg =  *(stmdev_read_ptr)  imu_ll_platform_read;
    p_lsm6dsl_dev_ctx_t->write_reg = *(stmdev_write_ptr) imu_ll_platform_write;
    
    imu_ll_gpio_init();         /* Initialize inputs and interrupts used for the IMU */
    imu_ll_twi_master_init();   /* Initialize TWI pins and frequency, enable TWI */
    /* Set up the dma transfer */

//      imu_ll_twim_dma_init(IMU_LL_INT1,
//                           LSM6DSL_FIFO_DATA_OUT_L,
//                           imu_data_buffer, IMU_BUFFER_SIZE);
    
    /* Check id and reset device */
    imu_device_check(p_lsm6dsl_dev_ctx_t);
    
    lsm6dsl_block_data_update_set(p_lsm6dsl_dev_ctx_t, PROPERTY_ENABLE);
    lsm6dsl_auto_increment_set(p_lsm6dsl_dev_ctx_t, PROPERTY_ENABLE);
      
    lsm6dsl_int1_route_t int1 =
    { 
        .int1_full_flag = PROPERTY_ENABLE,
        
    };
    lsm6dsl_pin_int1_route_set(p_lsm6dsl_dev_ctx_t, int1);
    
    lsm6dsl_int2_route_t int2_route =
    { 
        .int2_wrist_tilt = PROPERTY_ENABLE,
        .int2_inact_state = PROPERTY_DISABLE,
        .int2_drdy_xl = PROPERTY_DISABLE
            
    };
    lsm6dsl_pin_int2_route_set(p_lsm6dsl_dev_ctx_t, int2_route);
    


    imu_activity_inactivity_setup(p_lsm6dsl_dev_ctx_t);
    
    lsm6dsl_a_wrist_tilt_mask_t wrist_tilt_mask =
    { 
        .wrist_tilt_mask_xpos = PROPERTY_ENABLE,
        .wrist_tilt_mask_xneg = PROPERTY_ENABLE
    };
    imu_absolte_wrist_tilt_setup(p_lsm6dsl_dev_ctx_t, 120, &wrist_tilt_mask, 10);
    /*TEMPORARY CODE*/
    /* Accelerometer - LPF1 path ( LPF2 not used )
    lsm6dsl_xl_filter_analog_set(p_lsm6dsl_dev_ctx_t, LSM6DSL_XL_ANA_BW_400Hz);
    
    lsm6dsl_xl_lp2_bandwidth_set(p_lsm6dsl_dev_ctx_t,
                                 LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100);
    
    lsm6dsl_gy_band_pass_set(p_lsm6dsl_dev_ctx_t, LSM6DSL_HP_260mHz_LP1_STRONG);*/

    

                       
}

void imu_polling_init(stmdev_ctx_t* dev_ctx)
{

    
    static uint8_t whoamI, rst;
    dev_ctx->write_reg = *(stmdev_read_ptr)  imu_ll_platform_write;
    dev_ctx->read_reg = *(stmdev_read_ptr)  imu_ll_platform_read;
    
    imu_ll_gpio_init(); /* Initialize inputs and interrupts used for the IMU */
    imu_ll_twi_master_init(); /* Initialize TWI pins and frequency, enable TWI */
    
    imu_device_check(dev_ctx);

    /* Enable Block Data Update */
    lsm6dsl_block_data_update_set(dev_ctx, PROPERTY_ENABLE);
    /* Set power modes for accelerometer and gyroscope */
    lsm6dsl_xl_power_mode_set(dev_ctx, LSM6DSL_XL_NORMAL);
    lsm6dsl_gy_power_mode_set(dev_ctx, LSM6DSL_GY_NORMAL); 
    
    /* Set Output Data Rate and full scale for accelerometer */
    lsm6dsl_xl_data_rate_set(dev_ctx, LSM6DSL_XL_ODR_12Hz5);
    lsm6dsl_xl_full_scale_set(dev_ctx, LSM6DSL_2g);
    
    /* Set Output Data Rate and full scale for gyroscope */
    lsm6dsl_gy_data_rate_set(dev_ctx, LSM6DSL_GY_ODR_12Hz5);
    lsm6dsl_gy_full_scale_set(dev_ctx, LSM6DSL_500dps);
    
    /* Configure filtering chain(No aux interface) */
    /* Accelerometer - analog filter */
    lsm6dsl_xl_filter_analog_set(dev_ctx, LSM6DSL_XL_ANA_BW_400Hz);
    /* Accelerometer - LPF1 path ( LPF2 not used )*/
    //lsm6dsl_xl_lp1_bandwidth_set(&dev_ctx, LSM6DSL_XL_LP1_ODR_DIV_4);
    /* Accelerometer - LPF1 + LPF2 path */
    lsm6dsl_xl_lp2_bandwidth_set(dev_ctx,
        LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100);
    /* Accelerometer - High Pass / Slope path */
    //lsm6dsl_xl_reference_mode_set(&dev_ctx, PROPERTY_DISABLE);
    //lsm6dsl_xl_hp_bandwidth_set(&dev_ctx, LSM6DSL_XL_HP_ODR_DIV_100);
    /* Gyroscope - filtering chain */
    lsm6dsl_gy_band_pass_set(dev_ctx, LSM6DSL_HP_260mHz_LP1_STRONG);
}


static int16_t data_raw_acceleration[3];
static int16_t data_raw_angular_rate[3];
static int16_t data_raw_temperature;
static float acceleration_mg[3];
static float angular_rate_mdps[3];

void imu_polling_and_ble_transfer(stmdev_ctx_t* dev_ctx)
{
    /* Read output only if new value is available */
    lsm6dsl_reg_t reg;
    lsm6dsl_status_reg_get(dev_ctx, &reg.status_reg);
    char print_buffer[250];
    uint8_t len;

    
    
    if (reg.status_reg.xlda) {
      /* Read magnetic field data */
      memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
      lsm6dsl_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
      acceleration_mg[0] = lsm6dsl_from_fs2g_to_mg(
                             data_raw_acceleration[0]);
      acceleration_mg[1] = lsm6dsl_from_fs2g_to_mg(
                             data_raw_acceleration[1]);
      acceleration_mg[2] = lsm6dsl_from_fs2g_to_mg(
                             data_raw_acceleration[2]);

    }

    if (reg.status_reg.gda) {
      /* Read magnetic field data */
      memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
      lsm6dsl_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);
      angular_rate_mdps[0] = lsm6dsl_from_fs2000dps_to_mdps(
                               data_raw_angular_rate[0]);
      angular_rate_mdps[1] = lsm6dsl_from_fs2000dps_to_mdps(
                               data_raw_angular_rate[1]);
      angular_rate_mdps[2] = lsm6dsl_from_fs2000dps_to_mdps(
                               data_raw_angular_rate[2]);
        len = sprintf(print_buffer,
            " %f, %f, %f, %f, %f, %f \n",
            acceleration_mg[0],
            acceleration_mg[1],
            acceleration_mg[2],
            angular_rate_mdps[0],
            angular_rate_mdps[1],
            angular_rate_mdps[2]);
        
        /* NRF_LOG_INFO("%s",print_buffer); */ 
        ble_send_string(print_buffer, len);  
    }
}