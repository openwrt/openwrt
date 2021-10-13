LOC_DIR=src/hsl/phy
LIB=HSL

include $(PRJ_PATH)/make/config.mk

ifeq (ATHENA, $(CHIP_TYPE))
  SRC_LIST = f2_phy.c
endif

ifeq (GARUDA, $(CHIP_TYPE))
  SRC_LIST = f1_phy.c
endif

ifeq (SHIVA, $(CHIP_TYPE))
  SRC_LIST = f2_phy.c
endif

ifeq (HORUS, $(CHIP_TYPE))
  SRC_LIST = f2_phy.c
endif

ifeq (ISIS, $(CHIP_TYPE))
  SRC_LIST = f1_phy.c
endif

ifeq (MP, $(CHIP_TYPE))
  SRC_LIST = mpge_phy.c
ifeq (TRUE, $(IN_LED))
  SRC_LIST += mpge_led.c
endif
endif

ifneq (,$(filter ISISC, $(SUPPORT_CHIP)))
  SRC_LIST += f1_phy.c
endif

ifeq (TRUE, $(IN_MALIBU_PHY))
  SRC_LIST += malibu_phy.c
endif

ifeq (ALL_CHIP, $(CHIP_TYPE))
     SRC_LIST = f1_phy.c f2_phy.c malibu_phy.c
ifneq (,$(filter MP, $(SUPPORT_CHIP)))
     SRC_LIST += mpge_phy.c
ifeq (TRUE, $(IN_LED))
     SRC_LIST += mpge_led.c
endif
endif
endif

ifeq (NONHK_CHIP, $(CHIP_TYPE))
     SRC_LIST = f1_phy.c f2_phy.c malibu_phy.c
endif

ifeq (TRUE, $(IN_AQUANTIA_PHY))
    SRC_LIST += aquantia_phy.c
endif

ifeq (TRUE, $(IN_QCA803X_PHY))
    SRC_LIST += qca803x_phy.c
endif

ifeq (TRUE, $(IN_QCA808X_PHY))
    SRC_LIST += qca808x_phy.c
    SRC_LIST += qca808x.c
ifeq (TRUE, $(IN_LED))
    SRC_LIST += qca808x_led.c
endif
ifeq (TRUE, $(IN_PTP))
    SRC_LIST += qca808x_ptp.c
    SRC_LIST += qca808x_ptp_api.c
ifeq ($(CONFIG_PTP_1588_CLOCK), y)
    SRC_LIST += qca808x_phc.c
endif
endif
endif

ifeq (TRUE, $(IN_SFP_PHY))
    SRC_LIST += sfp_phy.c
endif

SRC_LIST += hsl_phy.c

ifeq (linux, $(OS))
	ifeq (KSLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      SRC_LIST=
	  endif
	endif
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
