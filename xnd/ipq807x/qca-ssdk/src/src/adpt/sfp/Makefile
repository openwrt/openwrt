LOC_DIR=src/adpt/sfp
LIB=ADPT

include $(PRJ_PATH)/make/config.mk

SRC_LIST=

ifeq (TRUE, $(IN_SFP))
	SRC_LIST += adpt_sfp.c
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
