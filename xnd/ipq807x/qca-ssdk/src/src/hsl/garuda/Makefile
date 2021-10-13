LOC_DIR=src/hsl/garuda
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=garuda_reg_access.c garuda_init.c

ifeq (TRUE, $(IN_ACL))
  SRC_LIST += garuda_acl.c
endif

ifeq (TRUE, $(IN_FDB))
  SRC_LIST += garuda_fdb.c
endif

ifeq (TRUE, $(IN_IGMP))
  SRC_LIST += garuda_igmp.c
endif

ifeq (TRUE, $(IN_LEAKY))
  SRC_LIST += garuda_leaky.c
endif

ifeq (TRUE, $(IN_LED))
  SRC_LIST += garuda_led.c
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += garuda_mib.c
endif

ifeq (TRUE, $(IN_MIRROR))
  SRC_LIST += garuda_mirror.c
endif

ifeq (TRUE, $(IN_MISC))
  SRC_LIST += garuda_misc.c
endif

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += garuda_port_ctrl.c
endif

ifeq (TRUE, $(IN_PORTVLAN))
  SRC_LIST += garuda_portvlan.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST += garuda_qos.c
endif

ifeq (TRUE, $(IN_RATE))
  SRC_LIST += garuda_rate.c
endif

ifeq (TRUE, $(IN_STP))
  SRC_LIST += garuda_stp.c
endif

ifeq (TRUE, $(IN_VLAN))
  SRC_LIST += garuda_vlan.c
endif

ifeq (TRUE, $(IN_REDUCED_ACL))
  SRC_LIST += garuda_reduced_acl.c
endif

ifeq (linux, $(OS))
	ifeq (KSLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      SRC_LIST=garuda_reg_access.c garuda_init.c
	  endif
	endif
endif

ifeq (, $(findstring GARUDA, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj