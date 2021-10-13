LOC_DIR=src/adpt/hppe
LIB=ADPT

include $(PRJ_PATH)/make/config.mk

SRC_LIST=

ifeq (TRUE, $(IN_FDB))
  SRC_LIST += adpt_hppe_fdb.c
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += adpt_hppe_mib.c
endif

ifeq (TRUE, $(IN_STP))
  SRC_LIST += adpt_hppe_stp.c
endif

ifeq (TRUE, $(IN_VSI))
  SRC_LIST += adpt_hppe_vsi.c
endif

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += adpt_hppe_portctrl.c
endif

ifeq (TRUE, $(IN_MIRROR))
  SRC_LIST += adpt_hppe_mirror.c
endif

ifeq (TRUE, $(IN_TRUNK))
  SRC_LIST += adpt_hppe_trunk.c
endif

ifeq (TRUE, $(IN_IP))
  SRC_LIST += adpt_hppe_ip.c
endif

ifeq (TRUE, $(IN_FLOW))
  SRC_LIST += adpt_hppe_flow.c
endif

ifeq (TRUE, $(IN_QM))
  SRC_LIST += adpt_hppe_qm.c
endif

ifeq (TRUE, $(IN_PPPOE))
  SRC_LIST += adpt_hppe_pppoe.c
endif

ifeq (TRUE, $(IN_PORTVLAN))
  SRC_LIST += adpt_hppe_portvlan.c
endif

ifeq (TRUE, $(IN_CTRLPKT))
  SRC_LIST += adpt_hppe_ctrlpkt.c
endif

ifeq (TRUE, $(IN_SERVCODE))
  SRC_LIST += adpt_hppe_servcode.c
endif

ifeq (TRUE, $(IN_RSS_HASH))
  SRC_LIST += adpt_hppe_rss_hash.c
endif

ifeq (TRUE, $(IN_SEC))
  SRC_LIST += adpt_hppe_sec.c
endif

ifeq (TRUE, $(IN_ACL))
  SRC_LIST += adpt_hppe_acl.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST += adpt_hppe_qos.c
endif

ifeq (TRUE, $(IN_BM))
  SRC_LIST += adpt_hppe_bm.c
endif

ifeq (TRUE, $(IN_SHAPER))
  SRC_LIST += adpt_hppe_shaper.c
endif

ifeq (TRUE, $(IN_POLICER))
  SRC_LIST += adpt_hppe_policer.c
endif

ifeq (TRUE, $(IN_MISC))
  SRC_LIST += adpt_hppe_misc.c
endif

ifeq (TRUE, $(IN_UNIPHY))
  SRC_LIST += adpt_hppe_uniphy.c
endif

ifeq (TRUE, $(IN_PTP))
  SRC_LIST += adpt_hppe_ptp.c
endif

ifeq (, $(findstring HPPE, $(SUPPORT_CHIP)))
  SRC_LIST=
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
