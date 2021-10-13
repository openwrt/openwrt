
include $(PRJ_PATH)/config
-include $(SYS_PATH)/include/config/auto.conf

ifndef SYS_PATH
  $(error SYS_PATH isn't defined!)
endif

ifndef TOOL_PATH
  $(error TOOL_PATH isn't defined!)
endif

#define cpu type such as PPC MIPS ARM X86
ifndef CPU
  CPU=mips
endif

#define os type such as linux netbsd vxworks
ifndef OS
  OS=linux
endif

ifndef OS_VER
  OS_VER=2_6
endif

#support chip type such as ATHENA GARUDA
ifndef CHIP_TYPE
  SUPPORT_CHIP = GARUDA
else
  ifeq (GARUDA, $(CHIP_TYPE))
    SUPPORT_CHIP = GARUDA
  endif

  ifeq (ATHENA, $(CHIP_TYPE))
     SUPPORT_CHIP = ATHENA
  endif

  ifeq (SHIVA, $(CHIP_TYPE))
     SUPPORT_CHIP = SHIVA
  endif

  ifeq (HORUS, $(CHIP_TYPE))
     SUPPORT_CHIP = HORUS
  endif

  ifeq (ISIS, $(CHIP_TYPE))
     SUPPORT_CHIP = ISIS
  endif

  ifeq (ISISC, $(CHIP_TYPE))
     SUPPORT_CHIP = ISISC
  endif

  ifeq (DESS, $(CHIP_TYPE))
     SUPPORT_CHIP = DESS
  endif

  ifeq (HPPE, $(CHIP_TYPE))
     SUPPORT_CHIP = HPPE
  endif

  ifeq (CPPE, $(CHIP_TYPE))
     SUPPORT_CHIP = HPPE CPPE
  endif

  ifeq (MP, $(CHIP_TYPE))
     SUPPORT_CHIP = SCOMPHY MP
  endif

  ifeq ($(ISISC_ENABLE), enable)
      SUPPORT_CHIP += ISISC
  endif

  ifeq (ALL_CHIP, $(CHIP_TYPE))
     ifneq (TRUE, $(FAL))
         $(error FAL must be TRUE when CHIP_TYPE is defined as ALL_CHIP!)
     endif
     SUPPORT_CHIP = ISIS ISISC SHIVA DESS HPPE CPPE SCOMPHY MP
  endif

  ifeq (NONHK_CHIP, $(CHIP_TYPE))
     ifneq (TRUE, $(FAL))
         $(error FAL must be TRUE when CHIP_TYPE is defined as ALL_CHIP!)
     endif
     SUPPORT_CHIP = ISIS ISISC SHIVA DESS
  endif

  ifndef SUPPORT_CHIP
    $(error defined CHIP_TYPE isn't supported!)
  endif
endif

#define compile tool prefix
ifndef TOOLPREFIX
  TOOLPREFIX=$(CPU)-$(OS)-uclibc-
endif

DEBUG_ON=FALSE
OPT_FLAG=
LD_FLAG=

SHELLOBJ=ssdk_sh
US_MOD=ssdk_us
KS_MOD=ssdk_ks

ifeq (TRUE, $(KERNEL_MODE))
  RUNMODE=km
else
  RUNMODE=um
endif

BLD_DIR=$(PRJ_PATH)/build/$(OS)
BIN_DIR=$(PRJ_PATH)/build/bin

VER=2.0.0
BUILD_NUMBER=$(shell cat $(PRJ_PATH)/make/.build_number)
VERSION=$(VER).$(BUILD_NUMBER)
BUILD_DATE=$(shell date -u  +%F-%T)
