/*
 * Copyright (C) 2024 Université Grenoble Alpes
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     apps
 * @{
 *
 * @file
 * @brief       I2HM climate station sent with semtech-Loramac 
 *
 * @author      Leo Festoc <leo.festoc@univ-grenoble-alpes.fr>
 * @author      Sarah Sabri <Sarah.sabri@univ-grenoble-alpes.fr>
 * @author      Dorian Poissonnet <Dorian.Poissonnet@univ-grenoble-alpes.fr>
 * @author      Thibault Abry <Thibault.abry@univ-grenoble-alpes.fr>
 * @author      Didier Donsez <didier.donsez@univ-grenoble-alpes.fr>
 * @}
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "board.h"
#include "senml.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "flash_utils.h"
#include "phydat.h"
#include "senml/phydat.h"
#include "saul_reg.h"
#include "sen15901.h"
#include "fmt.h"

// Definition des adc 
#define ADC_MISO 			ADC_LINE(1) // ADC of pin SPI MISO (for wind direction)
#define ADC_LIGHT_SENSOR 			ADC_LINE(0) //Adc of  LIGHT SENSOR

#define ADC_RES				ADC_RES_12BIT
#define M_PI  (3.14159265358979323846)

/* LoRaMac values */
#define JOIN_NEXT_RETRY_TIME            30 // Next join tentative in 0.5 minute
#define SECONDS_PER_DAY                 24 * 60 * 60

/* Use a fast datarate, e.g. BW125/SF7 in EU868 */
#define DR_INIT                         LORAMAC_DR_5
#include "ztimer.h"

#include "cayenne_lpp.h" // Sensors Encoding package 
#include <stdio.h>
//#include "net/loramac.h"     /* core loramac definitions */
#include "semtech_loramac.h" /* package API */
#include "loramac_utils.h"
#ifdef MODULE_SEMTECH_LORAMAC_RX
#include "thread.h"
#include "msg.h"
#endif


#include "periph/pm.h" //Power management 
#include "periph/gpio.h"
#ifdef MODULE_PM_LAYERED

#ifdef MODULE_PERIPH_RTC
#include "periph/rtc.h"
#endif
#include "pm_layered.h"
#endif



#ifndef BTN1_INT_FLANK 
#define BTN1_INT_FLANK  GPIO_RISING
#endif
/**
 * @brief  Interruption with magnetic button for reboot
 */

#if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN1_PIN)
static void btn_cb(void *ctx)
{   
    (void) ctx;
    puts("BTN1 pressed."); 
    puts("Reboot...."); 
    pm_reboot();
    
}
#endif /* MODULE_PERIPH_GPIO_IRQ */



  extern semtech_loramac_t loramac;  /* The loramac stack device descriptor */
      /* define the required keys for OTAA, e.g over-the-air activation (the
         null arrays need to be updated with valid LoRa values) */
   //Wyres Groupe 4
 /*    static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0xCA, 0xFE, 0xBA, 0xBE, \
                                                         0xC6, 0xB0, 0xDA, 0xF7 };
     static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0xCA, 0xFE, 0xBA, 0xBE, \
                                                         0x00, 0x00, 0x00, 0x00 };
     static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0xD6, 0x90, 0x93, 0x71, \
                                                          0x8B, 0xAA, 0x94, 0x73, \
                                                        0x93, 0x88, 0xFA, 0x5B, \
                                                      0x58, 0x66, 0x78, 0xB2 };
*/  
     //Wyres Groupe 5
    static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0xCA, 0xFE, 0xBA, 0xBE, \
                                                         0x62, 0x58, 0x3C, 0xCC };
                                                    
     static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, \
                                                         0x00, 0x00, 0x00, 0x00 };
     static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0xa1, 0x71, 0xda, 0xe2, \
                                                          0x6e, 0xa4, 0x0a, 0x6f, \
                                                        0xc7, 0xc1, 0xe3, 0xae, \
                                                        0x21, 0xcf, 0xe2, 0xde };
                                                                       


static cayenne_lpp_t lpp;

static void cb_rtc_puts(void *arg)
{
    puts(arg);
}
/**
 * @brief  print the lpp buffer 
 */
static void _print_buffer(const uint8_t *buffer, size_t len, const char *msg)
{
    printf("%s: ", msg);
    for (uint8_t i = 0; i < len; i++)
    {
        printf("%02X", buffer[i]);
    }
}

/**
 * @brief  Initialization of adc if connected
 */

int initialization_adc(void)
{
    // initialize the ADC line 
#ifdef SEN15901_FUNCTIONS_ENABLED
   if ( (adc_init(ADC_MISO) < 0)) { // adc pour girouette 
        printf("\r\nInitialization of ADC_LINE(%u) \r\n", ADC_MISO);
    }
    else  {
        printf("\r\nSuccessfully initialized ADC_LINE(%u)\r\n", ADC_MISO);
    }
#endif
    if (adc_init(ADC_LIGHT_SENSOR) < 0) {
        printf("\r\nInitialization of ADC_LINE(%u) failed\r\n", ADC_LIGHT_SENSOR);
    }
    else {
        printf("\r\nSuccessfully initialized ADC_LINE(%u)\r\n", ADC_LIGHT_SENSOR);
        LIGHT_SENSOR_SUPPLY_ON;
    }
    return 0;

    
}

/**
 * @brief  Initialization and join procedure LoRa
 */

void initialization_join_LoRa(void) //mettre int pour return 1 ou 0
{
    
    /* 1. auto-initialize the LoRaMAC MAC layer ( nothing to do) */
    /* 2. set the keys identifying the device */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);
    puts("I2HM LoRa join procedure ");
      /* 3. join the network */
     /* start the OTAA join procedure (and retries in required) */
    /*uint8_t joinRes = */ loramac_utils_join_retry_loop(&loramac, DR_INIT, JOIN_NEXT_RETRY_TIME, SECONDS_PER_DAY);
    puts("Join procedure succeeded");
}

/**
 * @brief  Read and display luminosity sensor datas 
 */

void display_luminosity(int * sample)
{
    *sample = adc_sample(ADC_LIGHT_SENSOR,ADC_RES);

    if( *sample > 0){

        // Luminosity
        printf("ADC_LINE(%u): raw value: %.4i, percent: %.2d %% \r\n", ADC_LIGHT_SENSOR, *sample, *sample*100/4096);
        printf("\n%d\n", *sample);

    }
}
#ifdef SEN15901_FUNCTIONS_ENABLED

  typedef struct {
    uint16_t wind_speed;
    uint16_t orientation;
    uint16_t water_level;
} sen15901_values;

/**
 * @brief  Initialization of meteo station (Wind speed, wind direction, rain)
 */

int initialization_sen15901(sen15901_t dev_sen15901) 
        {
            sen15901_params_t params = { 
		  .adc = ADC_LINE(1), 
		  .res = ADC_RES_12BIT, 
          // Girouette 
		  .sensor0_pin = GPIO_PIN(PORT_B, 14), //PB14, Port MISO en bas de la carte 
		  .sensor0_mode = GPIO_IN, 
          // anémomètre 
		  .sensor1_pin = EXTERNAL_GPIO_PIN, // PA8, en bas à droite de la carte
		  .sensor1_mode = GPIO_IN_PD, 
		  .sensor1_flank = GPIO_RISING, 
		 // Pluviomètre
          .sensor2_pin = GPIO_PIN(PORT_B, 15), // PB15, Port SD en bas de la carte 
		  .sensor2_mode = GPIO_IN_PD, 
		  .sensor2_flank = GPIO_RISING
          };

            if (sen15901_init(&dev_sen15901, &params) == SEN15901_OK){
			    printf("Ready to get data from sen15901....\n");
                return 1;
            }
            else 
            {
                printf("init sen15901 failed\n");
                return -1;
            }
		
        }
/**
 * @brief  return value from meteo station (Wind speed, wind direction, rain)
 */

sen15901_values display_sen15901(sen15901_t dev_sen15901, int duration)
{   
    
    ztimer_sleep(ZTIMER_SEC, 60);
    int res;
    sen15901_values vals;
    vals.orientation = -1;
    vals.water_level=-1;
    vals.wind_speed=-1;
    uint16_t ticks_vent=-1;
    uint16_t ticks_water = -1;
    
    /**** Wind direction ****/
    res = sen15901_get_wind_direction(&dev_sen15901, &vals.orientation);
    if (res != SEN15901_OK){    //pas besoin condition vérifiée dans initialization_sen15901
        printf("Error %d for wind direction\n", res);
    }else{
        printf("Direction : %d\n", vals.orientation);
    }
    
    /**** Wind speed ****/
    res = sen15901_get_wind_ticks(&dev_sen15901, &ticks_vent);
    if (res != SEN15901_OK){
        printf("Error %d when fetching wind ticks\n", res);
        vals.wind_speed = 0;
    }else{
        vals.wind_speed = ticks_vent*2.4/duration;
        printf("Vitesse du vent : %d km/h\n", (int) (vals.wind_speed));
    }
    
    /**** Rain ****/
    res = sen15901_get_water_ticks(&dev_sen15901, &ticks_water);
    if (res != SEN15901_OK){
        printf("Error %d when fetching water ticks\n", res);
        vals.water_level = 0;
    }else{
        vals.water_level = ticks_water*0.2794;
        printf("Precipitation : %d mm\n", (int) (vals.water_level));
    }
    return vals;
}
    
#endif
  

int main(void)
{      // definition of BTN1 interuption 
    #if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN1_PIN)
    gpio_init_int(BTN1_PIN, GPIO_IN_PD, BTN1_INT_FLANK, btn_cb, NULL);
    #endif
    //time to get the time later 
    struct tm time;
    //Constants and variables declarations

    
    int sample = 0;

    phydat_t res;


/*-----------------------------------------------------------------*/    

    puts("I2HM LORAMAC CAYENNE SENSORS START ");
    
    //Sensors initializations

    initialization_adc();
    
    //Join procedure Lora
    initialization_join_LoRa();
    

/*-----------------------------------------------------------------*/   
    
    while (true)
    {   
        double pressure = 0;        // hPa  
        double temperature = 0;     // °C 
        double humidity = 0;        // % humidty
        float x_accel = 0;          // m.s-2
        float y_accel = 0;          // m.s-2
        float z_accel = 0;          // m.s-2
        saul_reg_t *dev = saul_reg;
#ifdef SEN15901_FUNCTIONS_ENABLED 
        sen15901_t dev_sen15901; //voir à mettre hors loop
        sen15901_values vals;
        int duration = 60; // time in seconds between two data fetch
#endif
        LED_GREEN_ON;

        //wait 10 sec before starting mesure
        puts("\n mesure starts in 10");
        for(int i=0;i<5;i++)
			{
                ztimer_sleep(ZTIMER_SEC, 1);   
                LED_RED_ON;
                LED_GREEN_OFF;
                ztimer_sleep(ZTIMER_SEC, 1);   
                LED_RED_OFF;
                LED_GREEN_ON; 
            }
        puts("Mesure starts TEST");

        if (dev == NULL) {
            puts("No SAUL devices present");
        
        }
       
        // Reading luminosity sensors 
        display_luminosity(&sample);

        // Reading loop of sensors : bme680, lis2dh12, lpsxx
        
        int test_bme=0;
      while (dev) { 
            int dim = saul_reg_read(dev, &res);
            char dev_sens_name[12];
            strcpy(dev_sens_name,saul_class_to_str(dev->driver->type)) ;
            printf("\nDev: %s\tType: %" PRIsflash "\n", dev->name,
                  dev_sens_name);
            
                #ifdef BME680_FUNCTIONS_ENABLED // if bm680 enabled, we take its values for temerature and pressure 
                if (strcmp(dev->name,"bme680")==1)
                {
                    test_bme =1;
                }
                #endif
                
                if ( strcmp(dev_sens_name,"SENSE_PRESS")==0 && test_bme == 0) 
                {
                    pressure = res.val[0];
                       
                }
                if (strcmp(dev_sens_name,"SENSE_TEMP")==0 && test_bme == 0) 
                {
                    temperature =res.val[0]/100.00;    
                }
                if (strcmp(dev_sens_name,"SENSE_HUM")==0) 
                {
                    humidity =res.val[0]/100.00;    
                }
                if (strcmp(dev_sens_name,"SENSE_ACCEL")==0) 
                {
                    x_accel = res.val[0]; 
                    y_accel = res.val[1];  
                    z_accel = res.val[2];   
                }
                
            phydat_dump(&res, dim);
            dev = dev->next;
        }
        puts("\n##########################");

#ifdef SEN15901_FUNCTIONS_ENABLED

        // Lecture station météo ( pluviomètre, anémomètre, gyrouette)
			if (initialization_sen15901(dev_sen15901) == 1) //Si init sen est OK
            {   
               /* rtc_get_time(&time);
                time.tm_sec += 60;
                rtc_set_alarm(&time, cb_rtc_puts, "The alarm rang");
                // setting low power mode 
                pm_set(1);*/
                vals = display_sen15901(dev_sen15901, duration);  
                  
	        }
#endif
				
        puts("Mesure ends");
        

     
       /* double latitude = 45.5;
        double longitude = 5.5;
        double altitude = 10000;  */// meter
        double luminosity = (double) sample;
        double battery_voltage = 3.6; // V

        /**** Build cayenne payload ****/
        cayenne_lpp_reset(&lpp);
        cayenne_lpp_add_temperature(&lpp, 1, temperature);
        #ifdef BME680_FUNCTIONS_ENABLED
        cayenne_lpp_add_relative_humidity(&lpp, 2, humidity);
        #endif
        cayenne_lpp_add_barometric_pressure(&lpp, 3, pressure);
        cayenne_lpp_add_luminosity(&lpp, 4, luminosity);
        cayenne_lpp_add_accelerometer(&lpp, 5,
                                    x_accel, y_accel, z_accel);
        //cayenne_lpp_add_gps(&lpp, 5, latitude, longitude, altitude);
        cayenne_lpp_add_analog_input(&lpp, 6, battery_voltage);
#ifdef SEN15901_FUNCTIONS_ENABLED
        cayenne_lpp_add_analog_input(&lpp, 7, (double)vals.water_level);
		cayenne_lpp_add_analog_input(&lpp, 8, (double)vals.wind_speed);
		cayenne_lpp_add_analog_input(&lpp, 9, (double)vals.orientation);
#endif
        printf("\n==== Point #test ====\n");

        printf(" temperature:     %.2f °C\n",temperature);
        printf(" humidity:        %.1f RH\n",humidity);
        printf(" pressure:        %.0f hPa\n",pressure);
        printf(" luminosity:      %.1f lux\n",luminosity);
        printf(" accelerometer:        x=%fmg y=%fmg z=%fmg\n",x_accel, y_accel, z_accel);
      //  printf(" position:        lat=%.5f° lon=%.5f° alt=%.0fm\n",latitude, longitude, altitude);
        printf(" battery_voltage: %.1f mV\n",battery_voltage);

        /*Sending of the buffer */
        puts("LORAMAC start sending buffer ");
        printf("\ntx mode : %d",semtech_loramac_get_tx_mode(&loramac));
        printf("\n dr : %d\n",semtech_loramac_get_dr(&loramac));

    LED_GREEN_OFF;
    LED_RED_ON;

    ztimer_sleep(ZTIMER_SEC, 10);


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
            _print_buffer(lpp.buffer, lpp.cursor, "Cannot send message LPP: ");
            break;

        case SEMTECH_LORAMAC_TX_CNF_FAILED:
            puts("Fail to send: no ACK received");
            break;
        case SEMTECH_LORAMAC_TX_DONE:
            _print_buffer(lpp.buffer, lpp.cursor, "Message sent with success: ");
            break;

        
    }   
        puts("LORAMAC buffer sent");
        // clear GPIO for consumption ( not efficient for the moment) 
        /*
        gpio_clear(LED_RED_PIN);                // PA15 
        gpio_clear(LED_GREEN_PIN);              // PA0 
        gpio_clear(LIGHT_SENSOR_SUPPLY_PIN);    // PB6 
        gpio_clear(BTN1_PIN);                   // PB3 
        gpio_clear(SX127X_PARAM_SPI_NSS );      //SPI1_CS 
        gpio_set(SX127X_PARAM_RESET);
        gpio_clear(SX127X_PARAM_RESET);         // PA2 
        gpio_clear(SX127X_PARAM_DIO0);          // PA10 
        gpio_clear(SX127X_PARAM_DIO1);          // PB10 
        gpio_clear(SX127X_PARAM_DIO2);          // PB11 
        gpio_clear(SX127X_PARAM_DIO3);          // PB7 
        gpio_clear(SX127X_PARAM_DIO4) ;         // PB5 
        gpio_clear(SX127X_PARAM_DIO5) ;         // PB4 
        */
        puts("LORAMAC start waiting");
        LED_RED_OFF;
        ztimer_sleep(ZTIMER_SEC, 1);
        
        // getting the time of the real time clock and setting alarm 
        rtc_get_time(&time);
       // time.tm_sec += 60;
        time.tm_sec += 600; // ten minutes between wake up 
        rtc_set_alarm(&time, cb_rtc_puts, "The alarm rang");
        // setting low power mode 
        pm_set(0);
       
    }
}
