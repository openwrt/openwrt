LOC_DIR=src/hsl/isis
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=isis_reg_access.c isis_init.c

ifeq (TRUE, $(IN_ACL))
  SRC_LIST += isis_acl.c isis_acl_parse.c isis_multicast_acl.c
endif

ifeq (TRUE, $(IN_FDB))
  SRC_LIST += isis_fdb.c
endif

ifeq (TRUE, $(IN_IGMP))
  SRC_LIST += isis_igmp.c
endif

ifeq (TRUE, $(IN_LEAKY))
  SRC_LIST += isis_leaky.c
endif

ifeq (TRUE, $(IN_LED))
  SRC_LIST += isis_led.c
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += isis_mib.c
endif

ifeq (TRUE, $(IN_MIRROR))
  SRC_LIST += isis_mirror.c
endif

ifeq (TRUE, $(IN_MISC))
  SRC_LIST += isis_misc.c
endif

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += isis_port_ctrl.c
endif

ifeq (TRUE, $(IN_PORTVLAN))
  SRC_LIST += isis_portvlan.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST += isis_qos.c
endif

ifeq (TRUE, $(IN_RATE))
  SRC_LIST += isis_rate.c
endif

ifeq (TRUE, $(IN_STP))
  SRC_LIST += isis_stp.c
endif

ifeq (TRUE, $(IN_VLAN))
  SRC_LIST += isis_vlan.c
endif

ifeq (TRUE, $(IN_REDUCED_ACL))
  SRC_LIST += isis_reduced_acl.c
endif

ifeq (TRUE, $(IN_COSMAP))
  SRC_LIST += isis_cosmap.c
endif

ifeq (TRUE, $(IN_IP))
  SRC_LIST += isis_ip.c
endif

ifeq (TRUE, $(IN_NAT))
  SRC_LIST += isis_nat.c
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
  SRC_LIST += isis_trunk.c
endif

ifeq (TRUE, $(IN_SEC))
  SRC_LIST += isis_sec.c
endif

ifeq (TRUE, $(IN_INTERFACECONTROL))
  SRC_LIST += isis_interface_ctrl.c
endif

ifeq (TRUE, $(IN_MACBLOCK))
  SRC_LIST += isis_mac_block.c
endif


ifeq (linux, $(OS))
	ifeq (KSLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      SRC_LIST=isis_reg_access.c isis_init.c
	  endif
	endif
endif

ifeq (, $(filter ISIS, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
