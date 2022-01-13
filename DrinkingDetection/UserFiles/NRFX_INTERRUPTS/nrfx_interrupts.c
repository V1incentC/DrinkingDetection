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
/*
lsm6dsl_all_sources_t lsm6dsl_int_src;
uint16_t counter = 0;
lsm6dsl_int2_route_t int2_route;

void int2_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    
    static bool setAWT = 0, fifo_enable = 0;
#if DEVKIT
    //bsp_board_led_invert(1);
#endif
   
    
    lsm6dsl_all_sources_get(&lsm6dsl_dev_ctx_t, &lsm6dsl_int_src);

    if (lsm6dsl_int_src.wrist_tilt_ia.wrist_tilt_ia_xpos == 1)
    {
        NRF_LOG_INFO("AWT detected: IMU in fifo mode");
        imu_fifo_mode(&lsm6dsl_dev_ctx_t, IMU_FIFO_SIZE);


#if DEVKIT
            //bsp_board_led_invert(1);
            printf("%d wrist tilt\n", counter);
#endif
    }
}*/
