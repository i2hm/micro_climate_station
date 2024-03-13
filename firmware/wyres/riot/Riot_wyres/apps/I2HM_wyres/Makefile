# name of your application
APPLICATION = I2HM_CLIMATE_STATION

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= ../../../../RIOT-OS/RIOT
#RIOTBASE ?= ${HOME}/github/RIOT-OS/RIOT

# Path to my boards
EXTERNAL_BOARD_DIRS ?= ../../boards

# External modules
EXTERNAL_MODULE_DIRS += ../../modules
USEMODULE += loramac_utils
USEMODULE += sen15901
# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Fix the problem of the size of the debugging files
CFLAGS_DBG = 

# If no BOARD is found in the environment, use this default:
BOARD ?= wyres-base
LORA_DRIVER ?= sx1272
LORA_REGION ?= EU868

USEMODULE += auto_init_loramac
USEPKG += semtech-loramac
USEMODULE += semtech_loramac_rx
USEMODULE += $(LORA_DRIVER)

# By default (for all except LLCC68)
ifndef LORAMAC_JOIN_MIN_DATARATE
ifeq ($(DRIVER),llcc68) 
LORAMAC_JOIN_MIN_DATARATE ?= 1
else
LORAMAC_JOIN_MIN_DATARATE ?= 0
endif
endif

USEPKG += cayenne-lpp

USEMODULE += ztimer
USEMODULE += ztimer_sec 
USEMODULE += ztimer_msec
USEMODULE += printf_float
USEMODULE += xtimer
# Altimeter : ST [LPS22HB](https://github.com/RIOT-OS/RIOT/tree/master/tests/drivers/lpsxxx) via I2C
USEMODULE += lps22hb

#Accelero
USEMODULE += lis2dh12

# include and auto-initialize all available sensors
USEMODULE += saul_default

USEMODULE += fmt

USEMODULE += analog_util

FEATURES_REQUIRED += periph_adc
FEATURES_OPTIONAL += periph_eeprom

#power management 
FEATURES_OPTIONAL += periph_rtc
FEATURES_OPTIONAL += periph_gpio_irq

CFLAGS += -DLORAMAC_JOIN_MIN_DATARATE=$(LORAMAC_JOIN_MIN_DATARATE)

# Ajout de l'option station méteo (pluviomètre, anémomètre, girouette)
#CFLAGS += -DSEN15901_FUNCTIONS_ENABLED
# Ajout de l'option bm680 ( Température, humidité, pression)
#CFLAGS += -DBME680_FUNCTIONS_ENABLED
# include BM680 driver I2C
USEMODULE += bme680_i2c
ifeq ($(ENABLE_FP),1)
  USEMODULE += bme680_fp
endif

include $(RIOTBASE)/Makefile.include
