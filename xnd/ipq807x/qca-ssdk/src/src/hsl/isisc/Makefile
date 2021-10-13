LOC_DIR=src/hsl/isisc
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=isisc_reg_access.c isisc_init.c

ifeq (TRUE, $(IN_ACL))
  SRC_LIST += isisc_acl.c isisc_acl_parse.c
endif

ifeq (TRUE, $(IN_FDB))
  SRC_LIST += isisc_fdb.c
endif

ifeq (TRUE, $(IN_IGMP))
  SRC_LIST += isisc_igmp.c
	ifeq (TRUE, $(IN_ACL))
		SRC_LIST += isisc_multicast_acl.c
	endif
endif

ifeq (TRUE, $(IN_LEAKY))
  SRC_LIST += isisc_leaky.c
endif

ifeq (TRUE, $(IN_LED))
  SRC_LIST += isisc_led.c
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += isisc_mib.c
endif

ifeq (TRUE, $(IN_MIRROR))
  SRC_LIST += isisc_mirror.c
endif

ifeq (TRUE, $(IN_MISC))
  SRC_LIST += isisc_misc.c
endif

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += isisc_port_ctrl.c
endif

ifeq (TRUE, $(IN_PORTVLAN))
  SRC_LIST += isisc_portvlan.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST += isisc_qos.c
endif

ifeq (TRUE, $(IN_RATE))
  SRC_LIST += isisc_rate.c
endif

ifeq (TRUE, $(IN_STP))
  SRC_LIST += isisc_stp.c
endif

ifeq (TRUE, $(IN_VLAN))
  SRC_LIST += isisc_vlan.c
endif

ifeq (TRUE, $(IN_REDUCED_ACL))
  SRC_LIST += isisc_reduced_acl.c
endif

ifeq (TRUE, $(IN_COSMAP))
  SRC_LIST += isisc_cosmap.c
endif

ifeq (TRUE, $(IN_IP))
  SRC_LIST += isisc_ip.c
endif

ifeq (TRUE, $(IN_NAT))
  SRC_LIST += isisc_nat.c
endif

ifeq (TRUE, $(IN_NAT_HELPER))
  SRC_LIST += nat_helper_dt.c
  SRC_LIST += nat_helper_hsl.c
  SRC_LIST += nat_ipt_helper.c
  SRC_LIST += napt_helper.c
  SRC_LIST += host_helper.c
  SRC_LIST += nat_helper.c
  SRC_LIST += napt_acl.c
  SRC_LIST += napt_procfs.c
endif

ifeq (TRUE, $(IN_TRUNK))
  SRC_LIST += isisc_trunk.c
endif

ifeq (TRUE, $(IN_SEC))
  SRC_LIST += isisc_sec.c
endif

ifeq (TRUE, $(IN_INTERFACECONTROL))
  SRC_LIST += isisc_interface_ctrl.c
endif

ifeq (linux, $(OS))
	ifeq (KSLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      SRC_LIST=isisc_reg_access.c isisc_init.c
	  endif
	endif
endif

ifeq (, $(findstring ISISC, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
