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
    int counter = 0;
    
	for (;;)
	{
    	if (nrf_gpio_pin_read(BUTTON) == 0)
    	{
        	NRF_LOG_INFO("Button");
        	imu_fifo_mode(&lsm6dsl_dev_ctx_t, IMU_FIFO_SIZE);
        	while (nrf_gpio_pin_read(BUTTON) == 0) ;
        	
    	}
    	if (fifo_transfer_done)
    	{
        	
        	
        	NRF_LOG_INFO("FIFO full: %d", counter);
        	nrf_delay_ms(2);
        	
        	fifo_transfer_done = false;
        	//features
            //predict
        	++counter;
        	if (counter >= 5)
        	{
            	NRF_LOG_INFO("FIFO over");
            	nrf_delay_ms(2);
            	imu_fifo_disable(p_lsm6dsl_dev_ctx_t);
            	counter = 0;
        	}
        	else
        	{
            	imu_fifo_reset();
        	}
//        	if (counter >= 5)
//        	{
//            	
//            	//NRF_LOG_INFO("FIFO disabled");
//            	imu_fifo_disable(&lsm6dsl_dev_ctx_t);
//            	lsm6dsl_a_wrist_tilt_mask_t wrist_tilt_mask =
//            	{ 
//                	.wrist_tilt_mask_xpos = PROPERTY_DISABLE,
//                	.wrist_tilt_mask_xneg = PROPERTY_DISABLE 
//            	};
//            	lsm6dsl_tilt_src_set(&lsm6dsl_dev_ctx_t, &wrist_tilt_mask);
//            	imu_awt_detection_mode(&lsm6dsl_dev_ctx_t);
//            	counter = 0;
//        	}
    	}

    	//nrf_delay_ms(100);
    	//nrf_gpio_pin_toggle(IMU_LL_STATUS_LED);
	}
}
