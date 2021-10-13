LOC_DIR=src/hsl/scomphy
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=scomphy_reg_access.c scomphy_init.c

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += scomphy_port_ctrl.c
endif

ifeq (linux, $(OS))
	ifeq (KSLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      SRC_LIST=scomphy_reg_access.c scomphy_init.c
	  endif
	endif
endif

ifeq (, $(findstring SCOMPHY, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
