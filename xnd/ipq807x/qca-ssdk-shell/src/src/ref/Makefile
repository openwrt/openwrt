LOC_DIR=src/ref
LIB=REF

include $(PRJ_PATH)/make/config.mk

SRC_LIST=

ifeq (TRUE, $(IN_VLAN))
	SRC_LIST += ref_vlan.c
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
