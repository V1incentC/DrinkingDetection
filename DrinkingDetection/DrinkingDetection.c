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

#ifdef __cplusplus
}
#endif

const uint8_t leds_list[LEDS_NUMBER] = LEDS_LIST;

int main(void)
{
	
	APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
	NRF_LOG_DEFAULT_BACKENDS_INIT();
	
	LEDS_CONFIGURE(LEDS_MASK);
	NRF_LOG_INFO("This is a log data from nrf");
	for (;;)
	{
		for (int i = 0; i < LEDS_NUMBER; i++)
		{
			LEDS_INVERT(1 << leds_list[i]);
			NRF_LOG_INFO("LED: %d", leds_list[i]);
			nrf_delay_ms(500);
		}
	}
}
