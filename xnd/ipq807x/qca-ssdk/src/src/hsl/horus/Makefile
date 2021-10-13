LOC_DIR=src/hsl/horus
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=horus_reg_access.c horus_init.c

ifeq (TRUE, $(IN_FDB))
  SRC_LIST += horus_fdb.c
endif

ifeq (TRUE, $(IN_IGMP))
  SRC_LIST += horus_igmp.c
endif

ifeq (TRUE, $(IN_LEAKY))
  SRC_LIST += horus_leaky.c
endif

ifeq (TRUE, $(IN_LED))
  SRC_LIST += horus_led.c
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += horus_mib.c
endif

ifeq (TRUE, $(IN_MIRROR))
  SRC_LIST += horus_mirror.c
endif

ifeq (TRUE, $(IN_MISC))
  SRC_LIST += horus_misc.c
endif

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += horus_port_ctrl.c
endif

ifeq (TRUE, $(IN_PORTVLAN))
  SRC_LIST += horus_portvlan.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST += horus_qos.c
endif

ifeq (TRUE, $(IN_RATE))
  SRC_LIST += horus_rate.c
endif

ifeq (TRUE, $(IN_STP))
  SRC_LIST += horus_stp.c
endif

ifeq (TRUE, $(IN_VLAN))
  SRC_LIST += horus_vlan.c
endif

ifeq (TRUE, $(IN_REDUCED_ACL))
  SRC_LIST += horus_reduced_acl.c
endif

ifeq (linux, $(OS))
	ifeq (KSLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      SRC_LIST=horus_reg_access.c horus_init.c
	  endif
	endif
endif

ifeq (, $(findstring HORUS, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj