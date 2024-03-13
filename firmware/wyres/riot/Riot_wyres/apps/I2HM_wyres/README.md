I2HM station test
================

Usage
-----

Simply run the application on native using:

    gmake flash

If you want more options like : 
- Adding meteo station (wind speed, wind direction, rain), simply remove # from line CFLAGS +=-DSEN15901_FUNCTIONS_ENABLED in makefile 
- adding bm680 sensor, simply remove # from line CFLAGS += -DBME680_FUNCTIONS_ENABLED in makefile 

Expected result
---------------

The application should display the following output:

main(): This is RIOT! (Version: 2024.01-devel-259-ga38ab)
I2HM LORAMAC CAYENNE SENSORS START
Successfully initialized ADC_LINE(0) 
I2HM LoRa join procedure 
[otaa] Starting join procedure: dr=5
[otaa] Join procedure succeeded 
[otaa] DevAddr: fc00ac60
[otaa] NwkSKey:937621c8c0530bbdaf0538d6c0457ab0
[otaa] AppSKey:40335c9c56b361123b6183785715a18
[otaa] Network: Experimental
Join procedure succeeded
Hello timer!
mesure starts in 10
Mesure starts TEST
ADC_LINE(0): raw value: 0005, percent: 00 % 


Dev: lis2dh12	Type: SENSE_ACCEL
Data:	[0]          92 mg
	    [1]         -72 mg
	    [2]        -984 mg

Dev: lis2dh12	Type: SENSE_TEMP
Data:	          -5.00 °C

Dev: lps22hb	Type: SENSE_PRESS
Data:	            996 hPa

Dev: lps22hb	Type: SENSE_TEMP
Data:	          18.40 °C

##########################
Mesure ends

==== Point #test ====
 temperature:     18.40 °C
 humidity:        0.0 RH
 pressure:        996 hPa
 luminosity:      5.0 lux
 accelerometer:        x=92.000000mg y=-72.000000mg z=-984.000000mg
 battery_voltage: 3.6 mV
LORAMAC start sending buffer 

tx mode : 0
dr : 0
Message sent with success: : 016700102684803732765LORAMAC 
LORAMAC buffer sent
LORAMAC start waiting


Explaination
-------------
This code is designed to send environmental data to a server using the LoRa protocol.

First, it joins the LoRa Network.

Then, it reads the ADC line of the light sensors and displays the data.

It utilizes the SAUL module to read three sensors:

BM680 (temperature, humidity, pressure, and gas) (if connected)
LPS22HB (Temperature, pressure) (Sensor already connected to Wyres base)
LIS2DH12 (Accelerometer) (Sensor already connected to Wyres base)
If implemented, it reads interruptions from the SEN15901 sensor (pluviometer, wind speed, wind direction) for 60 seconds.

Next, it constructs the payload using the Cayenne LPP format and sends it to the server via LoRa.

Finally, the CPU enters low power mode for 600 seconds before waking up to take measurements and send data again


TODO
-------------

- Adding the pms particule sensor to the microstation 
- More research on the power management ( Consumption on low power mode when doing nothing is 0.5mA which seems too much)
- Adding battery power value to the payload ( read and send it)
- Implement a procedure to measure and send data dependning on your strategy ( for example if humidity > 50 and temp > 25 maybe we need 6 mesures/hour, and if lower humidity < 50 and temp < 25 maybe we only need 1 mesure/hour. 
Also, depending on the time of the day
Also, LorA consumes a lot, so maybe making 6 mesures/hours then send it every hour would be an idea)
- Make your own payload to consumes less while sending ( cayenne payload uses more bytes than necessary if we know what we want to send)