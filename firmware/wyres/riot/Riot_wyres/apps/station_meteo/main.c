/**
 * @ingroup     applications
 * @{
 *
 * @file
 * @brief       sen15901 meteo station application
 *
 * @author      Pieau Maxime <maxime.pieau@etu.univ-grenoble-alpes.fr>
 *		Abelkalon Arthur <arthur.anelkalon@etu.univ-grenoble-alpes.fr>
 *		Lailler Tristan <tristan.lailler@etu.univ-grenoble-alpes.fr>
 *
 * @}
 */


#include <stdio.h>
#include <string.h>

#include "semtech_loramac.h"
#include "fmt.h"

#include "xtimer.h"
#include "periph/gpio.h"
#include "periph/adc.h"
#include "periph_conf.h"
#include "cayenne_lpp.h"
#include "sen15901.h"

//TO MODIFY ACCORDING TO YOUR APPLICATION
//#define DEVEUI "CAFEBABE62583ccc"
//#define APPEUI "CAFEBABE00000000"
//#define APPKEY "a171dae26ea40a6fc7c1e3ae21cfe2de"
//WYRES G4 
#define DEVEUI "CAFEBABEC6B0DAF7"
#define APPEUI "CAFEBABE00000000"
#define APPKEY "D69093718BAA94739388FA5B586678B2"

extern semtech_loramac_t loramac;

static uint8_t deveui[LORAMAC_DEVEUI_LEN] ;
static uint8_t appeui[LORAMAC_APPEUI_LEN] ;
static uint8_t appkey[LORAMAC_APPKEY_LEN] ;

static cayenne_lpp_t lpp;

/*static void _print_buffer(const uint8_t *buffer, size_t len, const char *msg)
{
    printf("%s: ", msg);
    for (uint8_t i = 0; i < len; i++)
    {
        printf("%02X", buffer[i]);
    }
}*/

int lora_init(void)
{
	printf("Start init lora\n");
	/* Convert identifiers and application key */
	fmt_hex_bytes(deveui, DEVEUI);
	fmt_hex_bytes(appeui, APPEUI);
	fmt_hex_bytes(appkey, APPKEY);

	/* set the LoRaWAN keys */
	semtech_loramac_set_deveui(&loramac, deveui);
	semtech_loramac_set_appeui(&loramac, appeui);
	semtech_loramac_set_appkey(&loramac, appkey);
	
	semtech_loramac_save_config(&loramac);
	
	semtech_loramac_set_adr(&loramac, true);
	
	semtech_loramac_set_dr(&loramac, 0);
	
	switch (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA)) {
            case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
                puts("Cannot join: dutycycle restriction\n");
                return 1;
            case SEMTECH_LORAMAC_BUSY:
                puts("Cannot join: mac is busy\n");
                return 1;
            case SEMTECH_LORAMAC_JOIN_FAILED:
                puts("Join procedure failed!\n");
                return 1;
            case SEMTECH_LORAMAC_ALREADY_JOINED:
                puts("Warning: already joined!\n");
                return 1;
            case SEMTECH_LORAMAC_JOIN_SUCCEEDED:
                puts("Join procedure succeeded!\n");
                break;
            default: /* should not happen */
                break;
        }
        
        semtech_loramac_set_tx_mode(&loramac, LORAMAC_TX_UNCNF);
        semtech_loramac_set_tx_port(&loramac, 2);
        
        return 0;
}


int main(void)
{
	if (lora_init() == 0){
		printf("Init lora done\n");
		//char payload[26];
		int duration = 60; // time in seconds between two data fetch
		
		sen15901_t dev;
		sen15901_params_t params = { 
		  .adc = ADC_LINE(0), 
		  .res = ADC_RES_12BIT, 
		  .sensor0_pin = GPIO_PIN(PORT_B, 14), 
		  .sensor0_mode = GPIO_IN, 
		  .sensor1_pin = EXTERNAL_GPIO_PIN, 
		  .sensor1_mode = GPIO_IN_PD, 
		  .sensor1_flank = GPIO_RISING, 
		  .sensor2_pin = BTN1_PIN, 
		  .sensor2_mode = GPIO_IN_PD, 
		  .sensor2_flank = GPIO_RISING
		};

		if (sen15901_init(&dev, &params) == SEN15901_OK){
			printf("Ready to get data....\n");
			
			int res;
			uint16_t orientation;
			uint16_t ticks_vent;
			uint16_t wind_speed;
			uint16_t ticks_water;
			uint16_t water_level;
			while (1){	
				xtimer_sleep(duration);
				
				/**** Wind direction ****/
				res = sen15901_get_wind_direction(&dev, &orientation);
				if (res != SEN15901_OK){
					printf("Error %d for wind direction\n", res);
				}else{
					printf("Direction : %d\n", orientation);
				}
				
				/**** Wind speed ****/
				res = sen15901_get_wind_ticks(&dev, &ticks_vent);
				if (res != SEN15901_OK){
					printf("Error %d when fetching wind ticks\n", res);
					wind_speed = 0;
				}else{
					wind_speed = ticks_vent*2.4/duration;
					printf("Vitesse du vent : %d km/h\n", (int) (wind_speed));
				}
				
				/**** Rain ****/
				res = sen15901_get_water_ticks(&dev, &ticks_water);
				if (res != SEN15901_OK){
					printf("Error %d when fetching water ticks\n", res);
					water_level = 0;
				}else{
					water_level = ticks_water*0.2794;
					printf("Precipitation : %d mm\n", (int) (water_level));
				}
				
				/**** Build payload ****/
				//sprintf(payload,"o=%03d ,v=%03d ,p=%03d",orientation, wind_speed, water_level);
				/**** Build cayenne payload ****/
				cayenne_lpp_reset(&lpp);
        		//cayenne_lpp_add_temperature(&lpp, 1, temperature);
        		//cayenne_lpp_add_relative_humidity(&lpp, 2, humidity);
        		//cayenne_lpp_add_barometric_pressure(&lpp, 3, pressure);
        		//cayenne_lpp_add_luminosity(&lpp, 4, luninosity);
        		//cayenne_lpp_add_gps(&lpp, 5, latitude, longitude, altitude);
        		//cayenne_lpp_add_analog_input(&lpp, 6, battery_voltage);
				cayenne_lpp_add_analog_input(&lpp, 7, (double)water_level);
				cayenne_lpp_add_analog_input(&lpp, 8, (double)wind_speed);
				cayenne_lpp_add_analog_input(&lpp, 9, (double)orientation);
				/**** Send payload via LoRaWAN ****/
				xtimer_sleep(10);
				//switch (semtech_loramac_send(&loramac,  (uint8_t *)payload, strlen(payload))) {
				switch (semtech_loramac_send(&loramac,lpp.buffer, lpp.cursor)) {

					case SEMTECH_LORAMAC_NOT_JOINED:
						puts("Cannot send: not joined");
						break;

					case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
						puts("Cannot send: dutycycle restriction");
						break;

					case SEMTECH_LORAMAC_BUSY:
						puts("Cannot send: MAC is busy");
						break;

					case SEMTECH_LORAMAC_TX_ERROR:
						puts("Cannot send: error");
						//_print_buffer(lpp.buffer, lpp.cursor, "Cannot send message LPP: ");
						break;

					case SEMTECH_LORAMAC_TX_CNF_FAILED:
						puts("Fail to send: no ACK received");
						break;
					case SEMTECH_LORAMAC_TX_DONE:
						//_print_buffer(lpp.buffer, lpp.cursor, "Message sent with success: ");
						break;

        
    				}   
				puts("LORAMAC buffer sent");
				puts("LORAMAC start waiting");
			}
		}else{
			printf("Failed sen15901 init...\n\r");
		}
	}else{
		printf("Failed lora init...\n\r");
	}
	
	return 0;
}
