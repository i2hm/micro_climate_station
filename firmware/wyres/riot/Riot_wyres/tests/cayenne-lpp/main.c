/*
 * Copyright (C) 2023 Université Grenoble Alpes
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
 * @brief       Cayenne Low Power Payload example application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#define M_PI  (3.14159265358979323846)

#include "ztimer.h"

#include "cayenne_lpp.h"
#include <stdio.h>
//#include "net/loramac.h"     /* core loramac definitions */
#include "semtech_loramac.h" /* package API */

#ifdef MODULE_SEMTECH_LORAMAC_RX
#include "thread.h"
#include "msg.h"
#endif

  extern semtech_loramac_t loramac;  /* The loramac stack device descriptor */
      /* define the required keys for OTAA, e.g over-the-air activation (the
         null arrays need to be updated with valid LoRa values) */
     static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0xCA, 0xFE, 0xBA, 0xBE, \
                                                         0xC6, 0xB0, 0xDA, 0xF7 };
     /*static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0xCA, 0xFE, 0xBA, 0xBE, \
                                                         0x00, 0x00, 0x00, 0x00 };*/
     static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, \
                                                         0x00, 0x00, 0x00, 0x00 };
     static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0xD6, 0x90, 0x93, 0x71, \
                                                          0x8B, 0xAA, 0x94, 0x73, \
                                                        0x93, 0x88, 0xFA, 0x5B, \
                                                        0x58, 0x66, 0x78, 0xB2 };


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
    puts("LORAMAC CAYENNE START ");

    /* 1. initialize the LoRaMAC MAC layer */

 
    /* 2. set the keys identifying the device */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);
    puts("LORAMAC CAYENNE join procedure ");
      /* 3. join the network */
      
     if (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
         puts("Join procedure failed");
         return 1;
     }
    puts("Join procedure succeeded");
    puts("LORAMAC CAYENNE send data ");
    /* 4. send some data 
        char *message = "This is RIOT";
        if (semtech_loramac_send(&loramac,(uint8_t *)message, strlen(message)) != SEMTECH_LORAMAC_TX_DONE) {
            printf("Cannot send message '%s'\n", message);
            return 1;
        }
 */
    puts("High altitude balloon track simulator");

    double init_latitude = 45.5;
    double init_longitude = 5.5;
    double init_altitude = 10000;  // meter
    double init_pressure = 500;    // hPa
    double init_temperature = -20; // °C
    double init_humidity = 50;
    double init_luninosity = 500;
    double init_battery_voltage = 3600; // mV

    int t = 0;
    while (true)
    {
        double step = t *  M_PI * 2 / 3600;

        double latitude;
        double longitude;
        cassiniOval(step, 1, 2, &latitude, &longitude);
        latitude += init_latitude;
        longitude += init_longitude;
        double altitude = init_altitude + 8000 * cos(step);     // meter
        double pressure = init_pressure + 400 * cos(step);      // hPa
        double temperature = init_temperature + 40 * cos(step); // °C
       // double temperature = 20;
        double humidity = init_humidity + 50 * cos(step);
        double luninosity = init_luninosity + 500 * cos(step);
        double battery_voltage = init_battery_voltage - step; // mV

        
        cayenne_lpp_reset(&lpp);
        cayenne_lpp_add_temperature(&lpp, 1, temperature);
        cayenne_lpp_add_relative_humidity(&lpp, 2, humidity);
        cayenne_lpp_add_barometric_pressure(&lpp, 3, pressure);
        cayenne_lpp_add_luminosity(&lpp, 4, luninosity);
        cayenne_lpp_add_gps(&lpp, 5, latitude, longitude, altitude);
        cayenne_lpp_add_analog_input(&lpp, 6, battery_voltage);
        

        printf("\n==== Point #%d ====\n",t);

        printf(" temperature:     %.2f °C\n",temperature);
        printf(" humidity:        %.1f RH\n",humidity);
        printf(" pressure:        %.0f hPa\n",pressure);
        printf(" luninosity:      %.1f lux\n",luninosity);
        printf(" position:        lat=%.5f° lon=%.5f° alt=%.0fm\n",latitude, longitude, altitude);
        printf(" battery_voltage: %.1f mV\n",battery_voltage);

        uint8_t cnf = LORAMAC_TX_UNCNF;  /* Default: confirmable */
        uint8_t port = CONFIG_LORAMAC_DEFAULT_TX_PORT; /* Default: 2 */
        semtech_loramac_set_tx_mode(&loramac, cnf);
        semtech_loramac_set_tx_port(&loramac, port);
        semtech_loramac_set_dr(&loramac, 5);
        // _print_buffer(lpp.buffer, lpp.cursor, "LPP: ");
       // semtech_loramac_set_class(&lorama, loramac_class_t cls);
        




       puts("LORAMAC start sending buffer ");
        /* 5. send some data buffer */
       /* if (semtech_loramac_send(&loramac,lpp.buffer, sizeof(lpp.buffer)) != SEMTECH_LORAMAC_TX_DONE) {
                _print_buffer(lpp.buffer, lpp.cursor, "Cannot send message LPP: ");
                return 1;
        }
        else {
            puts("LORAMAC buffer sent ");
        }*/
        printf("tx mode :  %d",semtech_loramac_get_tx_mode(&loramac));
        printf("dr : %d",semtech_loramac_get_dr(&loramac));


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

        ztimer_sleep(ZTIMER_SEC, 60);

        t++;
    }
}
