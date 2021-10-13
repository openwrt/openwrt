LOC_DIR=src/hsl/sfp
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=

ifeq (TRUE, $(IN_SFP))
	SRC_LIST+=sfp.c sfp_access.c
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
