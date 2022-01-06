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

#ifdef __cplusplus
}
#endif

const uint8_t leds_list[LEDS_NUMBER] = LEDS_LIST;

int main(void)
{
	
	APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
	NRF_LOG_DEFAULT_BACKENDS_INIT();
	
	NRF_LOG_INFO("This is a log data from nrf");

    imu_init(&lsm6dsl_dev_ctx_t);
	for (;;)
	{
    	if (fifo_transfer_done)
    	{
        	fifo_transfer_done = false;
        	imu_fifo_reset();
    	}
	}
}
