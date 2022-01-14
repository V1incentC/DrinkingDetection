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

lsm6dsl_all_sources_t lsm6dsl_int_src;


void int2_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    
    static bool setAWT = 0, fifo_enable = 0;

    lsm6dsl_all_sources_get(&lsm6dsl_dev_ctx_t, &lsm6dsl_int_src);
    
    
    if (lsm6dsl_int_src.a_wrist_tilt_mask.wrist_tilt_mask_xpos == 1)
    {

   
        /* Set fifo full flag on INT1 pin */
        //int1_route.int1_full_flag =  PROPERTY_ENABLE;
        //lsm6dsl_pin_int1_route_set(ctx, int1_route);
        /* Disable AWT interrupt */

        imu_fifo_mode(&lsm6dsl_dev_ctx_t, IMU_FIFO_SIZE);
        NRF_LOG_INFO("AWT detected: IMU in fifo mode");
    }
}
