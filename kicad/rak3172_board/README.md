# I2HM Microclimate Station :: Board (RAK3172 version)

![Top](media/ih2m-rak-top.png) ![Botton](media/ih2m-rak-bottom.png)

## Features
* [RAK3172](https://store.rakwireless.com/products/wisduo-lpwan-module-rak3172?variant=43329454407878) (MCU [STM32WLE5](https://www.st.com/en/microcontrollers-microprocessors/stm32wlex.html)) LoRa module (available for 800 MHz and 900 MHz regions w/o TCXO)
* On-board Sensors:
  * [BME680](https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme680/) : Temperature, Humidity, Pressure
  * [LIS2DH12](https://www.st.com/en/mems-and-sensors/lis2dh12.html) : MEMS digital output motion sensor: ultra-low-power high-performance 3-axis "femto" accelerometer
* Grove connectors for UART (x2), I2C (x1), Analog GPIO (x2) and Digital GPIO (x1)  
* Connectivity: LoRaWAN, [LoRa Mesh](https://meshtastic.org/) (coming soon)
* Firmware : [RIOT OS](https://github.com/RIOT-OS/RIOT/tree/master/boards/nucleo-wl55jc), [STM32CubeIDE](https://docs.rakwireless.com/Product-Categories/WisDuo/RAK3172-Module/Low-Level-Development/#rak3172-on-stm32cubeide-with-stm32wl-sdk-v1-2-0)

## HOW TO INSTALL

Clone this repository.

Open the KiCad project using KiCad 7 and above.

If necessary import the symbol and footprint libraries in the project.
