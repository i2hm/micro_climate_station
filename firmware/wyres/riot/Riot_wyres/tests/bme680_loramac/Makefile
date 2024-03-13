# name of your application
APPLICATION = test_BME680_loramac

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= ../../../../RIOT-OS/RIOT
#RIOTBASE ?= ${HOME}/github/RIOT-OS/RIOT

# Path to my boards
EXTERNAL_BOARD_DIRS ?= ../../boards

# External modules
EXTERNAL_MODULE_DIRS += ../../modules

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
DRIVER ?= bme680_i2c
USEMODULE += loramac_utils
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
USEMODULE += $(DRIVER)

ifeq ($(ENABLE_FP),1)
  USEMODULE += bme680_fp
endif


USEMODULE += fmt

USEMODULE += analog_util

FEATURES_REQUIRED += periph_adc
FEATURES_OPTIONAL += periph_eeprom

CFLAGS += -DLORAMAC_JOIN_MIN_DATARATE=$(LORAMAC_JOIN_MIN_DATARATE)

include $(RIOTBASE)/Makefile.include
