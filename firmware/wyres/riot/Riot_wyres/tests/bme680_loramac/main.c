/*
 * Copyright (C) 2018 Mesotic SAS
 *               2020 Gunar Schorcht
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
 * @brief       Test application for the bme680_driver package.
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "bme680.h"
#include "bme680_params.h"
#include "mutex.h"
#include "timex.h"
#include "ztimer.h"

#define BME680_TEST_PERIOD_MS   (5 * MS_PER_SEC)    /* 5s */

ztimer_t timer;


/* LoRaMac values */
#define JOIN_NEXT_RETRY_TIME            30 // Next join tentative in 0.5 minute
#define SECONDS_PER_DAY                 24 * 60 * 60

/* Use a fast datarate, e.g. BW125/SF7 in EU868 */
#define DR_INIT                         LORAMAC_DR_5

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

static void _timer_cb(void *arg)
{
    ztimer_set(ZTIMER_MSEC, &timer, BME680_TEST_PERIOD_MS);
    mutex_unlock(arg);
}

int main(void)
{
    puts("BME680 LORAMAC START ");
    /* 1. auto-initialize the LoRaMAC MAC layer ( nothing to do) */
    /* 2. set the keys identifying the device */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);
    puts("BME680 LORAMAC join procedure ");
      /* 3. join the network */
     /* start the OTAA join procedure (and retries in required) */
    /*uint8_t joinRes = */ loramac_utils_join_retry_loop(&loramac, DR_INIT, JOIN_NEXT_RETRY_TIME, SECONDS_PER_DAY);
    puts("Join procedure succeeded");
    
    mutex_t timer_mtx = MUTEX_INIT_LOCKED;
    bme680_t dev[BME680_NUMOF];

    for (unsigned i = 0; i < BME680_NUMOF; i++) {
        /*
         * We use a fix temperature here. The ambient temperature could be
         * determined by performing a few temperature readings without
         * operating the gas sensor or by another temperature sensor. Function
         * bme680_set_ambient_temp can be used at any time to change it.
         */
        BME680_SENSOR(&dev[i]).amb_temp = 19;
        printf("Initialize BME680 sensor %u ... ", i);
        if (bme680_init(&dev[i], &bme680_params[i]) != BME680_OK) {
            puts("failed");
            return -1;
        }
        else {
            puts("OK");
        }
    }

    timer.callback = _timer_cb;
    timer.arg = &timer_mtx;
    ztimer_set(ZTIMER_MSEC, &timer, BME680_TEST_PERIOD_MS);

    while (1)
    {  
        struct bme680_field_data data;
        
        for (unsigned i = 0; i < BME680_NUMOF; i++) {
            /* trigger one measuerment */
            bme680_force_measurement(&dev[i]);
            /* get the duration for the measurement */
            int duration = bme680_get_duration(&dev[i]);
            /* wait for the duration */
            ztimer_sleep(ZTIMER_MSEC, duration);
            /* read the data */
            int res = bme680_get_data(&dev[i], &data);

            if (res == 0 && dev[i].sensor.new_fields) {
#ifndef MODULE_BME680_FP
                printf("[bme680]: dev=%u, "
                       "T = %02d.%02d degC, "
                       "P = %" PRIu32 " Pa, H = %02" PRIu32 ".%03" PRIu32 " %%",
                       i, data.temperature / 100, data.temperature % 100,
                       data.pressure,
                       data.humidity / 1000, data.humidity % 1000);
                      // read_temp(dev[i], &TEST_data);
                       printf("temperature : %d", BME680_SENSOR(&dev[0]).amb_temp);
                /* Avoid using measurements from an unstable heating setup */
                if (data.status & BME680_GASM_VALID_MSK) {
                    printf(", G = %" PRIu32 " ohms", data.gas_resistance);
                }
#else
                printf("[bme680]: dev=%u T = %.2f degC, P = %.2f Pa, H %.3f %%",
                       i, data.temperature, data.pressure, data.humidity);
                /* Avoid using measurements from an unstable heating setup */
                if (data.status & BME680_GASM_VALID_MSK) {
                    printf(", G = %.0f ohms", data.gas_resistance);
                }
#endif
                printf("\n");
            }
            else if (res == 0) {
                printf("[bme680]: no new data\n");
            }
            else {
                printf("[bme680]: read data failed with reason %d\n", res);
            }
        }
        printf("+-----------------------------------------+\n");
        mutex_lock(&timer_mtx);
        double pressure = data.pressure/100.00;      // hPa
        double temperature = data.temperature/100.00 ; // °C
        double humidity = data.humidity / 1000.00;


        /**** Build cayenne payload ****/
        cayenne_lpp_reset(&lpp);
        cayenne_lpp_add_temperature(&lpp, 1, temperature);
        cayenne_lpp_add_relative_humidity(&lpp, 2, humidity);
        cayenne_lpp_add_barometric_pressure(&lpp, 3, pressure);
     
        printf("\n==== Point #test ====\n");
        printf(" temperature:     %.2f °C\n",temperature);
        printf(" humidity:        %.1f RH\n",humidity);
        printf(" pressure:        %.0f hPa\n",pressure);

      


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
    }
    /* Should never reach here */
    return 0;
}
