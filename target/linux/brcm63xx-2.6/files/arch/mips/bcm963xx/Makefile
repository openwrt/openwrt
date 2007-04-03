#
# Makefile for generic Broadcom MIPS boards
#
# Copyright (C) 2004 Broadcom Corporation
#
obj-y           := irq.o prom.o setup.o time.o ser_init.o bcm63xx_led.o  board.o  boardparms.o int-handler.o

SRCBASE         := $(TOPDIR)
EXTRA_CFLAGS    += -I$(SRCBASE)/include
#EXTRA_CFLAGS    += -I$(INC_ADSLDRV_PATH) -DDBG
EXTRA_CFLAGS    += -I$(INC_ADSLDRV_PATH) 


ifeq "$(ADSL)" "ANNEX_B"
EXTRA_CFLAGS += -DADSL_ANNEXB
endif
ifeq "$(ADSL)" "SADSL"
EXTRA_CFLAGS += -DADSL_SADSL
endif
ifeq "$(ADSL)" "ANNEX_C"
EXTRA_CFLAGS += -DADSL_ANNEXC
endif

