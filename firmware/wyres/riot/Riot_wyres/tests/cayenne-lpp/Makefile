# name of your application
APPLICATION = test-driver-sx127x-WYRES

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= ../../../../RIOT-OS/RIOT
#RIOTBASE ?= ${HOME}/github/RIOT-OS/RIOT

# Path to my boards
EXTERNAL_BOARD_DIRS ?= ../../boards

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

USEPKG += semtech-loramac
USEMODULE += $(LORA_DRIVER)

USEPKG += cayenne-lpp

USEMODULE += ztimer
USEMODULE += ztimer_sec 
USEMODULE += ztimer_msec
USEMODULE += printf_float

USEMODULE += auto_init_loramac
USEMODULE += fmt

FEATURES_OPTIONAL += periph_eeprom

include $(RIOTBASE)/Makefile.include
