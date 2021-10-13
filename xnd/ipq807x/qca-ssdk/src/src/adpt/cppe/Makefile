LOC_DIR=src/adpt/cppe
LIB=ADPT

include $(PRJ_PATH)/make/config.mk

SRC_LIST=

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += adpt_cppe_portctrl.c
endif

ifeq (TRUE, $(IN_QM))
  SRC_LIST += adpt_cppe_qm.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST += adpt_cppe_qos.c
endif

ifeq (TRUE, $(IN_UNIPHY))
  SRC_LIST += adpt_cppe_uniphy.c
endif

ifeq (TRUE, $(IN_MISC))
ifneq (TRUE, $(IN_MISC_MINI))
  SRC_LIST += adpt_cppe_misc.c
endif
endif

ifeq (TRUE, $(IN_FLOW))
ifneq (TRUE, $(IN_FLOW_MINI))
  SRC_LIST += adpt_cppe_flow.c
endif
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += adpt_cppe_mib.c
endif

ifeq (, $(findstring CPPE, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
