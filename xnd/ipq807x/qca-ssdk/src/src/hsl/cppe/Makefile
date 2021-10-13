LOC_DIR=src/hsl/cppe
LIB=HSL

include $(PRJ_PATH)/make/config.mk

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST+=cppe_portctrl.c
  SRC_LIST+=cppe_loopback.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST+=cppe_qos.c
endif

ifeq (, $(findstring CPPE, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
