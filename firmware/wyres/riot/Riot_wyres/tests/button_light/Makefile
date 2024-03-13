# name of your application
APPLICATION = button_light

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= ../../../../RIOT-OS/RIOT

# Path to my boards
EXTERNAL_BOARD_DIRS ?= ../../boards

# If no BOARD is found in the environment, use this default:
BOARD ?= wyres-base

EXTERNAL_MODULE_DIRS += ../../modules

EXTERNAL_MODULE_DIRS += ../../modules
# Accelero : ST [LIS2DE12](https://github.com/RIOT-OS/RIOT/tree/master/tests/drivers/lis2dh12) via I2C 
# TODO add driver

# Altimeter : ST [LPS22HB](https://github.com/RIOT-OS/RIOT/tree/master/tests/drivers/lpsxxx) via I2C
USEMODULE += lps22hb

# include and auto-initialize all available sensors
USEMODULE += saul_default

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += xtimer
USEMODULE += analog_util

FEATURES_REQUIRED += periph_adc

include $(RIOTBASE)/Makefile.include
