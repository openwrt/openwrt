LOC_DIR=src/hsl/mp
LIB=HSL

include $(PRJ_PATH)/make/config.mk

ifeq (TRUE, $(IN_MIB))
    SRC_LIST+=mp_mib.c
endif

ifeq (TRUE, $(IN_PORTCONTROL))
	SRC_LIST+=mp_portctrl.c
endif

ifeq (TRUE, $(IN_UNIPHY))
	SRC_LIST+=mp_uniphy.c
endif

ifeq (, $(filter MP, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
