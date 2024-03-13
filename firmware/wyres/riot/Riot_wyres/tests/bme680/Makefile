# name of your application
APPLICATION = test_BME680

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


USEMODULE += $(DRIVER)
USEMODULE += ztimer
USEMODULE += ztimer_msec

ifeq ($(ENABLE_FP),1)
  USEMODULE += bme680_fp
endif

include $(RIOTBASE)/Makefile.include
