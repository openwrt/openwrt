LOC_DIR=src/adpt/mp
LIB=ADPT

include $(PRJ_PATH)/make/config.mk

SRC_LIST=adpt_mp_interrupt.c

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += adpt_mp_mib.c
endif

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += adpt_mp_portctrl.c
endif

ifeq (TRUE, $(IN_UNIPHY))
  SRC_LIST += adpt_mp_uniphy.c
endif

ifeq (TRUE, $(IN_LED))
  SRC_LIST += adpt_mp_led.c
endif

ifeq (, $(filter MP, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
