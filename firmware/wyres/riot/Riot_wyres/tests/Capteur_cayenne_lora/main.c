/*
 * Copyright (C) 2024 Université Grenoble Alpes
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Cayenne Low Power Payload of sensors sent with semtech-Loramac 
 *
 * @author      Leo Festoc <leo.festoc@univ-grenoble-alpes.fr>
 *
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


#define ADC_IN_USE 			ADC_LINE(0)
#define ADC_RES				ADC_RES_12BIT
#define M_PI  (3.14159265358979323846)

/* LoRaMac values */
#define JOIN_NEXT_RETRY_TIME            30 // Next join tentative in 1 minute
#define SECONDS_PER_DAY                 24 * 60 * 60

/* Use a fast datarate, e.g. BW125/SF7 in EU868 */
#define DR_INIT                         LORAMAC_DR_5
#include "ztimer.h"

#include "cayenne_lpp.h"
#include <stdio.h>
//#include "net/loramac.h"     /* core loramac definitions */
#include "semtech_loramac.h" /* package API */
#include "loramac_utils.h"
#ifdef MODULE_SEMTECH_LORAMAC_RX
#include "thread.h"
#include "msg.h"
#endif

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

static void _print_buffer(const uint8_t *buffer, size_t len, const char *msg)
{
    printf("%s: ", msg);
    for (uint8_t i = 0; i < len; i++)
    {
        printf("%02X", buffer[i]);
    }
}

/*
 * For emulating a track
 */
static void cassiniOval(double time, double a, double b, double *x, double *y)
{
    // Exemple de paramétrisation (à ajuster en fonction de tes besoins)
    double t = time; // Utilise le temps comme paramètre
    *x = a * cos(t);
    *y = b * sin(t);
}

int main(void)
{      
    puts("LORAMAC CAYENNE SENSORS START ");
    phydat_t res;
    	/* initialize the ADC line */
    if (adc_init(ADC_IN_USE) < 0) {
        printf("\r\nInitialization of ADC_LINE(%u) failed\r\n", ADC_IN_USE);

    	LED_RED_ON;
    	ztimer_sleep(ZTIMER_SEC, 5);
    	LED_RED_OFF;

        return 1;
    }
    else {
        printf("\r\nSuccessfully initialized ADC_LINE(%u)\r\n", ADC_IN_USE);

    	LED_GREEN_ON;
    	ztimer_sleep(ZTIMER_SEC, 2);
    	LED_GREEN_OFF;

    }
    /* initialize the light sensor */
    LIGHT_SENSOR_SUPPLY_ON;
    /* 1. auto-initialize the LoRaMAC MAC layer ( nothing to do) */
    /* 2. set the keys identifying the device */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);
    puts("LORAMAC CAYENNE join procedure ");
      /* 3. join the network */
     /* start the OTAA join procedure (and retries in required) */
    /*uint8_t joinRes = */ loramac_utils_join_retry_loop(&loramac, DR_INIT, JOIN_NEXT_RETRY_TIME, SECONDS_PER_DAY);
    puts("Join procedure succeeded");
    

    double init_latitude = 45.5;
    double init_longitude = 5.5;
    double init_altitude = 10000;  // meter
    double init_pressure = 500;    // hPa
    double init_temperature = -20; // °C
    double init_humidity = 50;
    //double init_luninosity = 500;
    double init_battery_voltage = 3.6; // mV
    int sample = 0;
    
    while (true)
    {   
        LED_GREEN_ON;
        //wait 5 sec before starting mesure
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
        puts("Mesure starts");
        saul_reg_t *dev = saul_reg;
        if (dev == NULL) {
            puts("No SAUL devices present");
            return 1;
        }
        // Lecture capteur luminosité
        sample = adc_sample(ADC_IN_USE,ADC_RES);
        double pressure = init_pressure;      // hPa
        double temperature = init_temperature ; // °C
        if( sample > 0){
            printf("ADC_LINE(%u): raw value: %.4i, percent: %.2d %% \r\n", ADC_IN_USE, sample, sample*100/4096);
            printf("\n%d\n", sample);
			// Lecture capteur pression
			int dim = saul_reg_read(dev, &res);
            printf("\nDev: %s\tType: %" PRIsflash "\n", dev->name,saul_class_to_str(dev->driver->type));
            phydat_dump(&res, dim);
            
            dev = dev->next;
            pressure = res.val[0];
            // Lecture capteur température
            dim = saul_reg_read(dev, &res);
            printf("\nDev: %s\tType: %" PRIsflash "\n", dev->name,saul_class_to_str(dev->driver->type));
           // senml_value_t test_senml;
           // phydat_to_senml_float(&test_senml, &res,dim);
            phydat_dump(&res, dim);
            
            temperature =res.val[0]/100.00;
           // temperature = (double)test_senml.value.value.f;
            dev = dev->next;
        }
        puts("Mesure ends");
        

        double latitude;
        double longitude;
        cassiniOval(0, 1, 2, &latitude, &longitude);
        latitude = init_latitude;
        longitude = init_longitude;
        double altitude = init_altitude ;    // meter
        //double pressure = init_pressure;      // hPa
        //double temperature = init_temperature ; // °C
        double humidity = init_humidity ;
        double luninosity = (double) sample;
        double battery_voltage = init_battery_voltage; // V

        
        cayenne_lpp_reset(&lpp);
        cayenne_lpp_add_temperature(&lpp, 1, temperature);
        cayenne_lpp_add_relative_humidity(&lpp, 2, humidity);
        cayenne_lpp_add_barometric_pressure(&lpp, 3, pressure);
        cayenne_lpp_add_luminosity(&lpp, 4, luninosity);
        cayenne_lpp_add_gps(&lpp, 5, latitude, longitude, altitude);
        cayenne_lpp_add_analog_input(&lpp, 6, battery_voltage);
        

        printf("\n==== Point #test ====\n");

        printf(" temperature:     %.2f °C\n",temperature);
        printf(" humidity:        %.1f RH\n",humidity);
        printf(" pressure:        %.0f hPa\n",pressure);
        printf(" luninosity:      %.1f lux\n",luninosity);
        printf(" position:        lat=%.5f° lon=%.5f° alt=%.0fm\n",latitude, longitude, altitude);
        printf(" battery_voltage: %.1f mV\n",battery_voltage);

       // uint8_t ucnf = LORAMAC_TX_UNCNF;  /* Default: confirmable */
      //  uint8_t port = CONFIG_LORAMAC_DEFAULT_TX_PORT; /* Default: 2 */
       // semtech_loramac_set_tx_mode(&loramac, ucnf);
        //semtech_loramac_set_tx_port(&loramac, port);
        //semtech_loramac_set_dr(&loramac, 5);
        // _print_buffer(lpp.buffer, lpp.cursor, "LPP: ");
       // semtech_loramac_set_class(&lorama, loramac_class_t cls);

        /*Sending of the buffer */
        puts("LORAMAC start sending buffer ");
        printf("tx mode : %d",semtech_loramac_get_tx_mode(&loramac));
        printf("\n dr : %d",semtech_loramac_get_dr(&loramac));

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
        puts("LORAMAC start waiting");
        LED_RED_OFF;
        ztimer_sleep(ZTIMER_SEC, 60);
       
        
    }
}
