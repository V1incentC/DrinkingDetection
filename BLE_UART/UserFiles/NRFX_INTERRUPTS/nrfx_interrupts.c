#include "nrfx_interrupts.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrf_gpio.h"
#include "pca10056.h"
#include "nrf_log.h"
#include "nrfx_twim.h"
#include "nrfx_gpiote.h"
#include "imu_api.h"
#include "imu_low_level.h"

lsm6dsl_all_sources_t lsm6dsl_int_src;


void int2_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    
    static bool setAWT = 0, fifo_enable = 0;

    lsm6dsl_all_sources_get(&lsm6dsl_dev_ctx_t, &lsm6dsl_int_src);
    
    
    if (1 || (lsm6dsl_int_src.a_wrist_tilt_mask.wrist_tilt_mask_xneg == 0)  || (lsm6dsl_int_src.a_wrist_tilt_mask.wrist_tilt_mask_xneg == 1))
    {

   
        /* Set fifo full flag on INT1 pin */
        //int1_route.int1_full_flag =  PROPERTY_ENABLE;
        //lsm6dsl_pin_int1_route_set(ctx, int1_route);
        /* Disable AWT interrupt */

        //imu_fifo_mode(&lsm6dsl_dev_ctx_t, IMU_FIFO_SIZE);
        
        lsm6dsl_int1_route_t int1_route =
        {
            .int1_full_flag = PROPERTY_ENABLE
        };
        lsm6dsl_int2_route_t int2_route =
        { 
            .int2_wrist_tilt = PROPERTY_DISABLE        
        };
    

        /* Set fifo full flag on INT1 pin */
        lsm6dsl_pin_int1_route_set(&lsm6dsl_dev_ctx_t, int1_route);
        /* Disable AWT interrupt */
        lsm6dsl_pin_int2_route_set(&lsm6dsl_dev_ctx_t, int2_route);
    
        imu_fifo_setup(&lsm6dsl_dev_ctx_t, IMU_FIFO_SIZE); /* Configure fifo */
        lsm6dsl_fifo_mode_set(&lsm6dsl_dev_ctx_t, LSM6DSL_BYPASS_MODE); /* resets buffer */
        lsm6dsl_fifo_mode_set(&lsm6dsl_dev_ctx_t, LSM6DSL_FIFO_MODE); /* buffer starts */
        
        NRF_LOG_INFO("AWT detected: IMU in fifo mode");
    }
}

void int1_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    static uint8_t counter = 0;

    
        ++counter;  
        if (counter >= 5)
        {
            NRF_LOG_INFO("FIFO over");
            imu_fifo_disable(&lsm6dsl_dev_ctx_t);

            imu_awt_detection_mode(&lsm6dsl_dev_ctx_t);
            counter = 0;

        }
        else
        {
            imu_read_fifo(&lsm6dsl_dev_ctx_t, &imu_data);
            fifo_transfer_done = true;
        }

    
}