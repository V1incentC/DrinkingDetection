#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
	
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "imu_api.h"
#include "imu_low_level.h"
#include "lsm6dsl/lsm6dsl_reg.h"
#include "bsp.h"
#include "calculation_features.h"
#include "machine_learning_features.h"
#include  "arm_math.h"
#include "XGBoost.h"

#ifdef __cplusplus
}
#endif
#define BUTTON NRF_GPIO_PIN_MAP(0,24)
const uint8_t leds_list[LEDS_NUMBER] = LEDS_LIST;

int main(void)
{

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
	
    NRF_LOG_INFO("This is a log data from nrf");
    nrf_gpio_cfg_input(BUTTON, NRF_GPIO_PIN_PULLUP);
    imu_init(&lsm6dsl_dev_ctx_t);

	for (;;)
	{
    	if (nrf_gpio_pin_read(BUTTON) == 0)
    	{
        	NRF_LOG_INFO("Button");
        	imu_fifo_mode(&lsm6dsl_dev_ctx_t, IMU_FIFO_SIZE);
        	while (nrf_gpio_pin_read(BUTTON) == 0) ;
        	
    	}
    	imu_handle_fifo_transfer_done();
                             
	}
}
