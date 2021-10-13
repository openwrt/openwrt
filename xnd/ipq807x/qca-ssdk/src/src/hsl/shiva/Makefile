LOC_DIR=src/hsl/shiva
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=shiva_reg_access.c shiva_init.c

ifeq (TRUE, $(IN_ACL))
  SRC_LIST += shiva_acl.c
endif

ifeq (TRUE, $(IN_FDB))
  SRC_LIST += shiva_fdb.c
endif

ifeq (TRUE, $(IN_IGMP))
  SRC_LIST += shiva_igmp.c
endif

ifeq (TRUE, $(IN_LEAKY))
  SRC_LIST += shiva_leaky.c
endif

ifeq (TRUE, $(IN_LED))
  SRC_LIST += shiva_led.c
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += shiva_mib.c
endif

ifeq (TRUE, $(IN_MIRROR))
  SRC_LIST += shiva_mirror.c
endif

ifeq (TRUE, $(IN_MISC))
  SRC_LIST += shiva_misc.c
endif

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += shiva_port_ctrl.c
endif

ifeq (TRUE, $(IN_PORTVLAN))
  SRC_LIST += shiva_portvlan.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST += shiva_qos.c
endif

ifeq (TRUE, $(IN_RATE))
  SRC_LIST += shiva_rate.c
endif

ifeq (TRUE, $(IN_STP))
  SRC_LIST += shiva_stp.c
endif

ifeq (TRUE, $(IN_VLAN))
  SRC_LIST += shiva_vlan.c
endif

ifeq (TRUE, $(IN_REDUCED_ACL))
  SRC_LIST += shiva_reduced_acl.c
endif

ifeq (linux, $(OS))
	ifeq (KSLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      SRC_LIST=shiva_reg_access.c shiva_init.c
	  endif
	endif
endif

ifeq (, $(findstring SHIVA, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj