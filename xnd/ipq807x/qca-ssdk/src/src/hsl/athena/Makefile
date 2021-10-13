LOC_DIR=src/hsl/athena
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=athena_reg_access.c athena_init.c

ifeq (TRUE, $(IN_FDB))
  SRC_LIST += athena_fdb.c
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += athena_mib.c
endif

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += athena_port_ctrl.c
endif

ifeq (TRUE, $(IN_PORTVLAN))
  SRC_LIST += athena_portvlan.c
endif

ifeq (TRUE, $(IN_VLAN))
  SRC_LIST += athena_vlan.c
endif

ifeq (linux, $(OS))
	ifeq (KSLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      SRC_LIST=athena_reg_access.c athena_init.c
	  endif
	endif
endif

ifeq (, $(findstring ATHENA, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj