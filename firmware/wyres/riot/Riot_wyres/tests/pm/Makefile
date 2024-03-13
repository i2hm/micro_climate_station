# name of your application
APPLICATION = pm

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= ../../../../RIOT-OS/RIOT
#RIOTBASE ?=/Documents/GitHub/IoT/RIOT-OS/RIOT
# Path to my boards
EXTERNAL_BOARD_DIRS ?= ../../boards

# If no BOARD is found in the environment, use this default:
BOARD ?= wyres-base


FEATURES_OPTIONAL += periph_rtc
FEATURES_OPTIONAL += periph_gpio_irq

USEMODULE += shell_cmds_default

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
