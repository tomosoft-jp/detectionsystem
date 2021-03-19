/*==========================================================
 * Copyright 2020 QuickLogic Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *==========================================================*/

#include "Fw_global_config.h"
#include <stdint.h>
#include "sml_output.h"
#include "kb.h"
#include "eoss3_hal_uart.h"
#include "ql_time.h"
#include <stdio.h>

#include "eoss3_hal_gpio.h"
#include "dbg_uart.h"

#define SERIAL_OUT_CHARS_MAX 512

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#define WATERTIMER 7
int wateralarm = 0;
extern uint8_t sensor_rate_debug_gpio_val;

static char serial_out_buf[SERIAL_OUT_CHARS_MAX];

static void sml_output_led(uint16_t model, uint16_t classification)
{
    //Unused for right now.
}

#define SENSOR_SSSS_RESULT_BUFLEN    (512)
uint8_t sensor_ssss_ai_fv_arr[MAX_VECTOR_SIZE];
uint8_t sensor_ssss_ai_fv_len;
char    sensor_ssss_ai_result_buf[SENSOR_SSSS_RESULT_BUFLEN];

static void sml_output_serial(uint16_t model, uint16_t classification)
{
    int count;
    int wbytes = 0;
    int buflen = sizeof(sensor_ssss_ai_result_buf)-1;
	int ret;
    kb_get_feature_vector(model, sensor_ssss_ai_fv_arr, &sensor_ssss_ai_fv_len);
	
	//**tomo
	//dbg_str_now();
	switch((int)classification){
	  case 1:
		if(wateralarm < WATERTIMER){
		    wateralarm ++;
    		HAL_GPIO_Write(GPIO_5, 0); //green LED
    		HAL_GPIO_Write(GPIO_6, 0); //red LED
        	HAL_GPIO_Write(GPIO_4, 1); //blue LED
		}
		else{
    		HAL_GPIO_Write(GPIO_5, 0); //green LED
        	HAL_GPIO_Write(GPIO_4, 0); //blue LED
    		HAL_GPIO_Write(GPIO_6, 1); //red LED
      		HAL_GPIO_Write(GPIO_2, sensor_rate_debug_gpio_val);
      		sensor_rate_debug_gpio_val ^= 1;
		}
	    break;
	  case 2:
		wateralarm = 0;
    	HAL_GPIO_Write(GPIO_6, 0); //red LED
        HAL_GPIO_Write(GPIO_4, 0); //blue LED
    	HAL_GPIO_Write(GPIO_5, 1); //green LED
	    break;
	  default:
		wateralarm = 0;
	    break;
	}
	
    count = snprintf(sensor_ssss_ai_result_buf, buflen,
             "{\"ModelNumber\":%d,\"Classification\":%d,\"FeatureLength\":%d,\"FeatureVector\":[",(int)model,(int)classification, (int)sensor_ssss_ai_fv_len);
    wbytes += count;
    buflen -= count;
    for(int j=0; j < (int)(sensor_ssss_ai_fv_len-1); j++)
    {
    	count = snprintf(&sensor_ssss_ai_result_buf[wbytes], buflen, "%d,", sensor_ssss_ai_fv_arr[j]);
        wbytes += count;
	    buflen -= count;
    }
	count = snprintf(&sensor_ssss_ai_result_buf[wbytes], buflen, "%d]}\n", sensor_ssss_ai_fv_arr[sensor_ssss_ai_fv_len-1]);
    wbytes += count;
    buflen -= count;
    uart_tx_raw_buf(UART_ID_SSI, sensor_ssss_ai_result_buf, wbytes);
}

static intptr_t last_output;

uint32_t sml_output_results(uint16_t model, uint16_t classification)
{
//	dbg_str("sml_output_results\n");
//	dbg_str_int("audio", adc_data);


    //kb_get_feature_vector(model, recent_fv_result.feature_vector, &recent_fv_result.fv_len);

    /* LIMIT output to 100hz */

    if( last_output == 0 ){
        last_output = ql_lw_timer_start();
    }

    if( ql_lw_timer_is_expired( last_output, 10 ) ){
        last_output = ql_lw_timer_start();
    	sml_output_serial(model, classification);
    }
    return 0;
}

uint32_t sml_output_init(void * p_module)
{
	//unused for now
    return 0;
}
