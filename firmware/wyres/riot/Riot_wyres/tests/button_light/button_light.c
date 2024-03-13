#include "xtimer.h"
#include "board.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include <stdio.h>
#include "flash_utils.h"
#include "phydat.h"
#include "saul_reg.h"

#define ADC_IN_USE 			ADC_LINE(0)
#define ADC_RES				ADC_RES_12BIT


int main(void)
{
	printf("\r\nRead button 1 state\r\n");
	phydat_t res;
	uint8_t btn_state = 0;

    printf("\r\nTest light sensor\r\n");

	LED_GREEN_ON;
	xtimer_sleep(1);
	LED_RED_OFF;
	xtimer_sleep(1);
	LED_RED_OFF;
	xtimer_sleep(1);
	LED_GREEN_OFF;
	xtimer_sleep(1);

	LIGHT_SENSOR_SUPPLY_ON;
	
	/* initialize the ADC line */
    if (adc_init(ADC_IN_USE) < 0) {
        printf("\r\nInitialization of ADC_LINE(%u) failed\r\n", ADC_IN_USE);

    	LED_RED_ON;
    	xtimer_sleep(5);
    	LED_RED_OFF;

        return 1;
    }
    else {
        printf("\r\nSuccessfully initialized ADC_LINE(%u)\r\n", ADC_IN_USE);

    	LED_GREEN_ON;
    	xtimer_sleep(2);
    	LED_GREEN_OFF;

    }
	
	int sample = 0;
	

	while(1){
		saul_reg_t *dev = saul_reg;

        if (dev == NULL) {
            puts("No SAUL devices present");
            return 1;
        }
		while(gpio_read(BTN1_PIN) == btn_state);

        sample = adc_sample(ADC_IN_USE,ADC_RES);

		btn_state = gpio_read(BTN1_PIN);

		if(btn_state && sample > 0){
			printf("Button 1 has been pressed!\r\n");
            printf("ADC_LINE(%u): raw value: %.4i, percent: %.2d %% \r\n", ADC_IN_USE, sample, sample*100/4096);
            printf("\n%d\n", sample);
			// lecture capteur pression / température
			for(int i=0;i<2;i++)
			{
				int dim = saul_reg_read(dev, &res);
            printf("\nDev: %s\tType: %" PRIsflash "\n", dev->name,
                   saul_class_to_str(dev->driver->type));
            phydat_dump(&res, dim);
            dev = dev->next;
			}
			

            LED_GREEN_ON;
		}else{
			printf("Button 1 has been released!\r\n");
            LED_GREEN_OFF;
		}

        
		
		/*if (sample < 0) {
            printf("\r\nADC_LINE(%u): selected resolution not applicable\r\n",ADC_IN_USE);
        }else {
            printf("ADC_LINE(%u): raw value: %.4i, percent: %.2d %% \r\n", ADC_IN_USE, sample, sample*100/4096);
            if(sample > 4096/2) {
            	LED_GREEN_ON;
            	xtimer_msleep(500);
            	LED_GREEN_OFF;
            } else {
            	LED_RED_ON;
            	xtimer_msleep(500);
            	LED_RED_OFF;
            }
        }
		
    	xtimer_msleep(500);*/
	}
	
    return 0;
}
