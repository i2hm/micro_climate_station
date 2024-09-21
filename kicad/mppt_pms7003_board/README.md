# I2HM Microclimate Station :: Air quality and Power extension for MPPT and PMS7003

This board is an extension board for adding air quality monitoring to the [Microclimate station board](../rak3172_board/README.md).

It includes :
* a 10-pin connector for a [Plantower PMS7003 Particulate Matter sensor](https://www.plantower.com/en/products_33/76.html)
* a 4-channel I2C ADC ([ADS1115](https://www.ti.com/product/ADS1115)) for analog sensors such as PT100, PT1000 ...
* a JST connector for a LiPo battery
* a terminal block (2 pins) for [lithium-ion capacitor](https://en.wikipedia.org/wiki/Lithium-ion_capacitor)
* a JST connector for a solar panel
* a optional connector for a [Plantower PMS9003M Particulate Matter sensor](https://www.plantower.com/en/products_33/99.html)

## MPPT

> Remark: the fan of the Plantower PMS7003 requieres a 5V DC power supply and the LoRa board requieres a 3V3 DC power supply

## References

* [e-peas AEM10941 Solar Energy Harvesting](https://e-peas.com/product/aem10941)
* [Sunny Buddy Solar Charger V13 Hookup Guide](https://learn.sparkfun.com/tutorials/sunny-buddy-solar-charger-v13-hookup-guide) with [LT3652](https://cdn.sparkfun.com/datasheets/Prototyping/LT3652.pdf)
* [Firmware with RIOT-OS](https://airqualitystation.github.io/#lora-e5-mini-with-bme280-and-pms7003)
* [KiCad lib for PMS7003](https://github.com/Tinkerforge/kicad-libraries/blob/master/PMS7003.kicad_mod)

### Plantower PMS7003 pinout
| Pin | Function |
| --- | -------- |
| 1 | (5V) |
| 2 | (5V) |
| 3 | (GND) |
| 4 | (GND) |
| 5 | (Reset) |
| 6 | (N/C) |
| 7 | (RX) |
| 8 | (N/C) |
| 9 | (TX) |
| 10 | (Set) |
