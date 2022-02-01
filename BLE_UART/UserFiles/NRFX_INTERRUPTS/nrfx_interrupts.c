#include "nrfx_interrupts.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "pca10056.h"
#include "nrf_log.h"
#include "nrfx_twim.h"
#include "nrfx_gpiote.h"
#include "imu_api.h"
#include "imu_low_level.h"



void int2_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{

    imu_handle_int2();
}

void int1_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    imu_handle_int1(); 
}