LOC_DIR=src/hsl/hppe
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=hppe_init.c hppe_reg_access.c hppe_global.c

ifeq (TRUE, $(IN_FLOW))
	SRC_LIST+=hppe_flow.c
endif

ifeq (TRUE, $(IN_IP))
	SRC_LIST+=hppe_ip.c
endif

ifeq (TRUE, $(IN_QOS))
	SRC_LIST+=hppe_qos.c
endif

ifeq (TRUE, $(IN_FDB))
	SRC_LIST+=hppe_fdb.c
endif

ifeq (TRUE, $(IN_PORTVLAN))
	SRC_LIST+=hppe_portvlan.c
endif

ifeq (TRUE, $(IN_STP))
	SRC_LIST+=hppe_stp.c
endif

ifeq (TRUE, $(IN_QM))
	SRC_LIST+=hppe_qm.c
endif

ifeq (TRUE, $(IN_VSI))
	SRC_LIST+=hppe_vsi.c
endif

ifeq (TRUE, $(IN_SEC))
	SRC_LIST+=hppe_sec.c
endif

ifeq (TRUE, $(IN_BM))
	SRC_LIST+=hppe_bm.c
endif

ifeq (TRUE, $(IN_MIB))
	SRC_LIST+=hppe_mib.c hppe_xgmacmib.c
endif

ifeq (TRUE, $(IN_SERVCODE))
	SRC_LIST+=hppe_servcode.c
endif

ifeq (TRUE, $(IN_CTRLPKT))
	SRC_LIST+=hppe_ctrlpkt.c
endif

ifeq (TRUE, $(IN_RSS_HASH))
	SRC_LIST+=hppe_rss.c
endif

ifeq (TRUE, $(IN_PPPOE))
	SRC_LIST+=hppe_pppoe.c
endif

ifeq (TRUE, $(IN_PORTCONTROL))
	SRC_LIST+=hppe_portctrl.c hppe_xgportctrl.c
endif

ifeq (TRUE, $(IN_MIRROR))
	SRC_LIST+=hppe_mirror.c
endif

ifeq (TRUE, $(IN_TRUNK))
	SRC_LIST+=hppe_trunk.c
endif

ifeq (TRUE, $(IN_ACL))
	SRC_LIST+=hppe_acl.c
endif

ifeq (TRUE, $(IN_POLICER))
	SRC_LIST+=hppe_policer.c
endif

ifeq (TRUE, $(IN_SHAPER))
	SRC_LIST+=hppe_shaper.c
endif

ifeq (TRUE, $(IN_UNIPHY))
	SRC_LIST+=hppe_uniphy.c
endif

ifeq (, $(findstring HPPE, $(SUPPORT_CHIP)))
  SRC_LIST=
ifneq (, $(filter MP, $(SUPPORT_CHIP)))
  SRC_LIST=hppe_reg_access.c
ifeq (TRUE, $(IN_UNIPHY))
  SRC_LIST+=hppe_uniphy.c
endif
endif
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
