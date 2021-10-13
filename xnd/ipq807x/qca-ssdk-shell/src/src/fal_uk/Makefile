LOC_DIR=src/fal_uk
LIB=UK_IF

include $(PRJ_PATH)/make/config.mk

SRC_LIST=fal_init.c fal_uk_if.c fal_reg_access.c

ifeq (TRUE, $(IN_ACL))
  SRC_LIST += fal_acl.c
endif

ifeq (TRUE, $(IN_FDB))
  SRC_LIST += fal_fdb.c
endif

ifeq (TRUE, $(IN_IGMP))
  SRC_LIST += fal_igmp.c
endif

ifeq (TRUE, $(IN_LEAKY))
  SRC_LIST += fal_leaky.c
endif

ifeq (TRUE, $(IN_LED))
  SRC_LIST += fal_led.c
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += fal_mib.c
endif

ifeq (TRUE, $(IN_MIRROR))
  SRC_LIST += fal_mirror.c
endif

ifeq (TRUE, $(IN_MISC))
  SRC_LIST += fal_misc.c
endif

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += fal_port_ctrl.c
endif

ifeq (TRUE, $(IN_PORTVLAN))
  SRC_LIST += fal_portvlan.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST += fal_qos.c
endif

ifeq (TRUE, $(IN_RATE))
  SRC_LIST += fal_rate.c
endif

ifeq (TRUE, $(IN_STP))
  SRC_LIST += fal_stp.c
endif

ifeq (TRUE, $(IN_VLAN))
  SRC_LIST += fal_vlan.c
endif

ifeq (TRUE, $(IN_COSMAP))
  SRC_LIST += fal_cosmap.c
endif

ifeq (TRUE, $(IN_IP))
  SRC_LIST += fal_ip.c
endif

ifeq (TRUE, $(IN_NAT))
  SRC_LIST += fal_nat.c
endif

ifeq (TRUE, $(IN_SEC))
  SRC_LIST += fal_sec.c
endif

ifeq (TRUE, $(IN_TRUNK))
  SRC_LIST += fal_trunk.c
endif

ifeq (TRUE, $(IN_INTERFACECONTROL))
  SRC_LIST += fal_interface_ctrl.c
endif

ifeq (TRUE, $(IN_CTRLPKT))
  SRC_LIST += fal_ctrlpkt.c
endif

ifeq (TRUE, $(IN_SERVCODE))
  SRC_LIST += fal_servcode.c
endif

ifeq (TRUE, $(IN_RSS_HASH))
  SRC_LIST += fal_rss_hash.c
endif

ifeq (TRUE, $(IN_VSI))
  SRC_LIST += fal_vsi.c
endif

ifeq (TRUE, $(IN_QM))
  SRC_LIST += fal_qm.c
endif


ifeq (TRUE, $(IN_FLOW))
  SRC_LIST += fal_flow.c
endif

ifeq (TRUE, $(IN_PPPOE))
  SRC_LIST += fal_pppoe.c
endif

ifeq (TRUE, $(IN_BM))
  SRC_LIST += fal_bm.c
endif

ifeq (TRUE, $(IN_SHAPER))
  SRC_LIST += fal_shaper.c
endif

ifeq (TRUE, $(IN_POLICER))
  SRC_LIST += fal_policer.c
endif

ifeq (TRUE, $(IN_PTP))
  SRC_LIST += fal_ptp.c
endif

ifeq (TRUE, $(IN_SFP))
  SRC_LIST += fal_sfp.c
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
