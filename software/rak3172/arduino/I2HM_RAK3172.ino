/***
 *  @brief This example demonstrates LoRaWan protocol joining the network in OTAA mode, class A, region EU868.
 *  Device will send uplink every 20 seconds.
***/

/* Configuration RAK3272S
#define OTAA_PERIOD   (10000)
#define OTAA_BAND     (RAK_REGION_EU433)
#define OTAA_DEVEUI   {0xac, 0x1f, 0x09, 0xff, 0xfe, 0x09, 0x83, 0xb0}
#define OTAA_APPEUI   {0xac, 0x1f, 0x09, 0xff, 0xfe, 0x09, 0x83, 0xb0}
#define OTAA_APPKEY   {0x7A, 0x54, 0xA3, 0x15, 0x79, 0xD0, 0x15, 0x54, 0xD6, 0xFF, 0xCB, 0x83, 0xB2, 0x47, 0x14, 0x70}
*/

// Configuration RAK3272S-SiP
#define OTAA_PERIOD   (10000)
#define OTAA_BAND     (RAK_REGION_EU868)
#define OTAA_DEVEUI   {0xfa, 0xd0, 0x01, 0x23, 0x94, 0x7e, 0xc6, 0xb3}
#define OTAA_APPEUI   {0xfa, 0xd0, 0x01, 0x23, 0x94, 0x7e, 0xc6, 0xb3}
#define OTAA_APPKEY   {0x09, 0x18, 0x0B, 0x8C, 0xEE, 0xF3, 0x0E, 0x96, 0xF6, 0x30, 0x48, 0xA4, 0x6D, 0x19, 0xB1, 0x3E}

#include <Wire.h>
#include "Arduino.h"
#include "Adafruit_BME680.h"
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

/** @brief Packet buffer for sending */
uint8_t collected_data[64] = { 0 };

// Declaration of sensors
Adafruit_BME680 bme;
Adafruit_MMA8451 mma = Adafruit_MMA8451();

// Current state definition of sensors
uint8_t state_bme680 = 0;
uint8_t state_accel = 0;

/** @brief Rounds a value to a given precision */
float arrondir(float value, float precision) 
{
    return round(value * (1.0 / precision)) * precision;
}

/** @brief Callback function for receiving data */
void recvCallback(SERVICE_LORA_RECEIVE_T * data)
{
    if (data->BufferSize > 0) {
        Serial.println("Something received!");
        for (int i = 0; i < data->BufferSize; i++) {
            Serial.printf("%x", data->Buffer[i]);
        }
        Serial.print("\r\n");
    }
}

/** @brief Callback function for joining the network */
void joinCallback(int32_t status)
{
    Serial.printf("Join status: %d\r\n", status);
}

/** @brief Callback function for sending data */
void sendCallback(int32_t status)
{
    if (status == RAK_LORAMAC_STATUS_OK) {
        Serial.println("Successfully sent");
    } else {
        Serial.println("Sending failed");
    }
}

/** @brief Initializes the system */
void setup()
{
    Serial.begin(115200, RAK_AT_MODE);
    delay(2000);
  
    Serial.println("RAKwireless LoRaWan OTAA - I2HM");
    Serial.println("------------------------------------------------------");
  
    if(api.lorawan.nwm.get() != 1)
    {
        Serial.printf("Set Node device work mode %s\r\n",
            api.lorawan.nwm.set(1) ? "Success" : "Fail");
        api.system.reboot();
    }

    uint8_t node_device_eui[8] = OTAA_DEVEUI;
    uint8_t node_app_eui[8] = OTAA_APPEUI;
    uint8_t node_app_key[16] = OTAA_APPKEY;
  
    if (!api.lorawan.appeui.set(node_app_eui, 8)) {
        Serial.printf("LoRaWan OTAA - set application EUI is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.appkey.set(node_app_key, 16)) {
        Serial.printf("LoRaWan OTAA - set application key is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.deui.set(node_device_eui, 8)) {
        Serial.printf("LoRaWan OTAA - set device EUI is incorrect! \r\n");
        return;
    }
  
    if (!api.lorawan.band.set(OTAA_BAND)) {
        Serial.printf("LoRaWan OTAA - set band is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) {
        Serial.printf("LoRaWan OTAA - set device class is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.njm.set(RAK_LORA_OTAA))	// Set the network join mode to OTAA
    {
        Serial.printf("LoRaWan OTAA - set network join mode is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.join())	// Join to Gateway
    {
        Serial.printf("LoRaWan OTAA - join fail! \r\n");
        return;
    }
  
    /** Wait for Join success */
    while (api.lorawan.njs.get() == 0) {
        Serial.print("Wait for LoRaWAN join...");
        api.lorawan.join();
        delay(10000);
    }
  
    if (!api.lorawan.adr.set(true)) {
        Serial.printf("LoRaWan OTAA - set adaptive data rate is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.rety.set(1)) {
        Serial.printf("LoRaWan OTAA - set retry times is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.cfm.set(1)) {
        Serial.printf("LoRaWan OTAA - set confirm mode is incorrect! \r\n");
        return;
    }
  
    /** Check LoRaWan Status*/
    Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get()? "ON" : "OFF");	// Check Duty Cycle status
    Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get()? "CONFIRMED" : "UNCONFIRMED");	// Check Confirm status
    uint8_t assigned_dev_addr[4] = { 0 };
    api.lorawan.daddr.get(assigned_dev_addr, 4);
    Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);	// Check Device Address
    Serial.printf("Uplink period is %ums\r\n", OTAA_PERIOD);
    Serial.println("");
    api.lorawan.registerRecvCallback(recvCallback);
    api.lorawan.registerJoinCallback(joinCallback);
    api.lorawan.registerSendCallback(sendCallback);

    if(bme.begin()) 
    {
      bme.setTemperatureOversampling(BME680_OS_8X);
      bme.setHumidityOversampling(BME680_OS_2X);
      bme.setPressureOversampling(BME680_OS_4X);
      state_bme680 = 1;
    }

    if (mma.begin())
    {
      mma.setRange(MMA8451_RANGE_2_G);
      state_accel = 1;
    }
}

/** @brief Handles the uplink routine */
void uplink_routine()
{
    uint8_t indexTemp;
    uint8_t indexHumidity;
    uint8_t indexPressure;

    /** ---------------------- BME680 ---------------------- */
    // If the sensor is detected
    if(state_bme680)
    {
      // Read necessary data
      if (bme.performReading()) 
      {
        // [0 ; 50] °C with precision of 0.2
        // Temp = Index_temp * 0.2	
        indexTemp = (uint8_t)(arrondir(bme.temperature, 0.2) / 0.2);

        // [0 ; 100] % with precision of 0.4
        // Hum = Index_hum * 0.4	
        indexHumidity = (uint8_t)(arrondir(bme.humidity, 0.4) / 0.4);

        // [990 ; 1020] hPa with precision of 0.15
        // P = (Index_P * 0.15) + 990	
        indexPressure = (uint8_t) ((arrondir(bme.pressure / 100.0, 0.15) - 990) / 0.15);
      }
    }
    else
    {
      Serial.println("Error: BME680 sensor not detected.");
    }
    /** ---------------------- END BME680 ---------------------- */

    /** ---------------------- ACCEL ---------------------- */
    uint8_t indexAx;
    uint8_t indexAy;
    uint8_t indexAz;

    if(state_accel)
    {
      mma.read();

      /* Get a new sensor event */ 
      sensors_event_t event; 
      mma.getEvent(&event);

      // [-20 ; 20] m/s^-2 with precision of 0.16
      // A = (Index_A * 0.16) - 20	
      indexAx = (uint8_t)((arrondir(event.acceleration.x, 0.16) + 20) / 0.16);
      indexAy = (uint8_t)((arrondir(event.acceleration.y, 0.16) + 20) / 0.16);
      indexAz = (uint8_t)((arrondir(event.acceleration.z, 0.16) + 20) / 0.16);  
    }
    /** ---------------------- END ACCEL ---------------------- */

    /** Create the payload */
    uint8_t data_len = 0;

    collected_data[data_len++] = (state_bme680 << 1) + state_accel;

    if(state_bme680)
    {
      collected_data[data_len++] = indexTemp;
      collected_data[data_len++] = indexHumidity;
      collected_data[data_len++] = indexPressure;
    }

    if(state_accel)
    {
      collected_data[data_len++] = indexAx;
      collected_data[data_len++] = indexAy;
      collected_data[data_len++] = indexAz;
    }
  
    Serial.println("Data Packet:");
    for (int i = 0; i < data_len; i++) {
        Serial.printf("0x%02X ", collected_data[i]);
    }
    Serial.println("");
  
    /** Send the data package */
    if (api.lorawan.send(data_len, (uint8_t *) & collected_data, 2, true, 1)) 
    {
        Serial.println("Sending is requested");
    } else 
    {
        Serial.println("Sending failed");
    }
}

/** @brief Main loop */
void loop()
{
    static uint64_t last = 0;
    static uint64_t elapsed;
  
    if ((elapsed = millis() - last) > OTAA_PERIOD) {
        uplink_routine();
        last = millis();
    }
    api.system.sleep.all(OTAA_PERIOD);
    api.system.reboot();
}