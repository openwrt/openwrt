LOC_DIR=src/hsl/dess
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=dess_reg_access.c dess_init.c

ifeq (TRUE, $(IN_ACL))
  SRC_LIST += dess_acl.c dess_acl_parse.c dess_multicast_acl.c
endif

ifeq (TRUE, $(IN_FDB))
  SRC_LIST += dess_fdb.c
endif

ifeq (TRUE, $(IN_IGMP))
  SRC_LIST += dess_igmp.c
endif

ifeq (TRUE, $(IN_LEAKY))
  SRC_LIST += dess_leaky.c
endif

ifeq (TRUE, $(IN_LED))
  SRC_LIST += dess_led.c
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += dess_mib.c
endif

ifeq (TRUE, $(IN_MIRROR))
  SRC_LIST += dess_mirror.c
endif

ifeq (TRUE, $(IN_MISC))
  SRC_LIST += dess_misc.c
endif

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += dess_port_ctrl.c
endif

ifeq (TRUE, $(IN_PORTVLAN))
  SRC_LIST += dess_portvlan.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST += dess_qos.c
endif

ifeq (TRUE, $(IN_RATE))
  SRC_LIST += dess_rate.c
endif

ifeq (TRUE, $(IN_STP))
  SRC_LIST += dess_stp.c
endif

ifeq (TRUE, $(IN_VLAN))
  SRC_LIST += dess_vlan.c
endif

ifeq (TRUE, $(IN_COSMAP))
  SRC_LIST += dess_cosmap.c
endif

ifeq (TRUE, $(IN_IP))
  SRC_LIST += dess_ip.c
endif

ifeq (TRUE, $(IN_NAT))
  SRC_LIST += dess_nat.c
endif

ifeq (TRUE, $(IN_NAT_HELPER))
  SRC_LIST += nat_helper_dt.c
  SRC_LIST += nat_helper_hsl.c
  SRC_LIST += nat_ipt_helper.c
  SRC_LIST += napt_helper.c
  SRC_LIST += host_helper.c
  SRC_LIST += nat_helper.c
  SRC_LIST += napt_acl.c
endif

ifeq (TRUE, $(IN_TRUNK))
  SRC_LIST += dess_trunk.c
endif

ifeq (TRUE, $(IN_SEC))
  SRC_LIST += dess_sec.c
endif

ifeq (TRUE, $(IN_INTERFACECONTROL))
  SRC_LIST += dess_interface_ctrl.c dess_psgmii.c
endif

ifeq (linux, $(OS))
	ifeq (KSLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      SRC_LIST=dess_reg_access.c dess_init.c
	  endif
	endif
endif

ifeq (, $(findstring DESS, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
