MODULE_CFLAG :=
LOCAL_CFLAGS :=

ifeq (TRUE, $(SWCONFIG))
  MODULE_CFLAG += -DIN_SWCONFIG
endif

ifeq (TRUE, $(IN_ACL))
  MODULE_CFLAG += -DIN_ACL
endif

ifeq (TRUE, $(IN_FDB))
  MODULE_CFLAG += -DIN_FDB
endif

ifeq (TRUE, $(IN_FDB_MINI))
  MODULE_CFLAG += -DIN_FDB_MINI
endif

ifeq (TRUE, $(IN_IGMP))
  MODULE_CFLAG += -DIN_IGMP
endif

ifeq (TRUE, $(IN_LEAKY))
  MODULE_CFLAG += -DIN_LEAKY
endif

ifeq (TRUE, $(IN_LED))
  MODULE_CFLAG += -DIN_LED
endif

ifeq (TRUE, $(IN_MIB))
  MODULE_CFLAG += -DIN_MIB
endif

ifeq (TRUE, $(IN_MIRROR))
  MODULE_CFLAG += -DIN_MIRROR
endif

ifeq (TRUE, $(IN_MISC))
  MODULE_CFLAG += -DIN_MISC
endif

ifeq (TRUE, $(IN_MISC_MINI))
  MODULE_CFLAG += -DIN_MISC_MINI
endif

ifeq (TRUE, $(IN_PORTCONTROL))
  MODULE_CFLAG += -DIN_PORTCONTROL
endif

ifeq (TRUE, $(IN_PORTCONTROL_MINI))
  MODULE_CFLAG += -DIN_PORTCONTROL_MINI
endif

ifeq (TRUE, $(IN_PORTVLAN))
  MODULE_CFLAG += -DIN_PORTVLAN
endif

ifeq (TRUE, $(IN_PORTVLAN_MINI))
  MODULE_CFLAG += -DIN_PORTVLAN_MINI
endif

ifeq (TRUE, $(IN_QOS))
  MODULE_CFLAG += -DIN_QOS
endif

ifeq (TRUE, $(IN_QOS_MINI))
  MODULE_CFLAG += -DIN_QOS_MINI
endif

ifeq (TRUE, $(IN_RATE))
  MODULE_CFLAG += -DIN_RATE
endif

ifeq (TRUE, $(IN_STP))
  MODULE_CFLAG += -DIN_STP
endif

ifeq (TRUE, $(IN_VLAN))
  MODULE_CFLAG += -DIN_VLAN
endif

ifeq (TRUE, $(IN_VLAN_MINI))
  MODULE_CFLAG += -DIN_VLAN_MINI
endif

ifeq (TRUE, $(IN_REDUCED_ACL))
  MODULE_CFLAG += -DIN_REDUCED_ACL
endif

ifeq (TRUE, $(IN_COSMAP))
  MODULE_CFLAG += -DIN_COSMAP
endif

ifeq (TRUE, $(IN_COSMAP_MINI))
  MODULE_CFLAG += -DIN_COSMAP_MINI
endif

ifeq (TRUE, $(IN_IP))
  MODULE_CFLAG += -DIN_IP
endif

ifeq (TRUE, $(IN_IP_MINI))
  MODULE_CFLAG += -DIN_IP_MINI
endif

ifeq (TRUE, $(IN_NAT))
  MODULE_CFLAG += -DIN_NAT
endif

ifeq (TRUE, $(IN_FLOW))
  MODULE_CFLAG += -DIN_FLOW
endif

ifeq (TRUE, $(IN_FLOW_MINI))
  MODULE_CFLAG += -DIN_FLOW_MINI
endif

ifeq (TRUE, $(IN_SFE))
  MODULE_CFLAG += -DIN_SFE
endif

ifeq (TRUE, $(IN_TRUNK))
  MODULE_CFLAG += -DIN_TRUNK
endif

ifeq (TRUE, $(IN_SEC))
  MODULE_CFLAG += -DIN_SEC
endif

ifeq (TRUE, $(IN_QM))
  MODULE_CFLAG += -DIN_QM
endif

ifeq (TRUE, $(IN_QM_MINI))
  MODULE_CFLAG += -DIN_QM_MINI
endif

ifeq (TRUE, $(IN_NAT_HELPER))
  MODULE_CFLAG += -DIN_NAT_HELPER
endif

ifeq (TRUE, $(IN_INTERFACECONTROL))
  MODULE_CFLAG += -DIN_INTERFACECONTROL
endif

ifeq (TRUE, $(IN_CTRLPKT))
  MODULE_CFLAG += -DIN_CTRLPKT
endif

ifeq (TRUE, $(IN_SERVCODE))
  MODULE_CFLAG += -DIN_SERVCODE
endif

ifeq (TRUE, $(IN_RSS_HASH))
  MODULE_CFLAG += -DIN_RSS_HASH
endif

ifeq (TRUE, $(IN_MACBLOCK))
  MODULE_CFLAG += -DIN_MACBLOCK
endif

ifeq (TRUE, $(IN_RFS))
  MODULE_CFLAG += -DIN_RFS
endif

ifeq (TRUE, $(IN_MALIBU_PHY))
  MODULE_CFLAG += -DIN_MALIBU_PHY
endif
ifeq (TRUE, $(IN_AQUANTIA_PHY))
  MODULE_CFLAG += -DIN_AQUANTIA_PHY
endif

ifeq (TRUE, $(IN_QCA803X_PHY))
  MODULE_CFLAG += -DIN_QCA803X_PHY
endif

ifeq (TRUE, $(IN_QCA808X_PHY))
  MODULE_CFLAG += -DIN_QCA808X_PHY
endif
ifeq (TRUE, $(IN_SFP_PHY))
  MODULE_CFLAG += -DIN_SFP_PHY
endif

ifeq (TRUE, $(IN_PHY_I2C_MODE))
  MODULE_CFLAG += -DIN_PHY_I2C_MODE
endif

ifeq (TRUE, $(IN_VSI))
  MODULE_CFLAG += -DIN_VSI
endif

ifeq (TRUE, $(IN_VSI_MINI))
  MODULE_CFLAG += -DIN_VSI_MINI
endif

ifeq (TRUE, $(IN_PPPOE))
  MODULE_CFLAG += -DIN_PPPOE
endif

ifeq (TRUE, $(IN_BM))
  MODULE_CFLAG += -DIN_BM
endif

ifeq (TRUE, $(IN_BM_MINI))
  MODULE_CFLAG += -DIN_BM_MINI
endif

ifeq (TRUE, $(IN_SHAPER))
  MODULE_CFLAG += -DIN_SHAPER
endif

ifeq (TRUE, $(IN_SHAPER_MINI))
  MODULE_CFLAG += -DIN_SHAPER_MINI
endif

ifeq (TRUE, $(IN_POLICER))
  MODULE_CFLAG += -DIN_POLICER
endif

ifeq (TRUE, $(IN_POLICER_MINI))
  MODULE_CFLAG += -DIN_POLICER_MINI
endif

ifeq (TRUE, $(IN_UNIPHY))
  MODULE_CFLAG += -DIN_UNIPHY
endif

ifeq (TRUE, $(IN_UNIPHY_MINI))
  MODULE_CFLAG += -DIN_UNIPHY_MINI
endif

ifeq (TRUE, $(RUMI_EMULATION))
  MODULE_CFLAG += -DRUMI_EMULATION
endif

ifeq (TRUE, $(IN_PTP))
  MODULE_CFLAG += -DIN_PTP
endif

ifneq (TRUE, $(FAL))
  MODULE_CFLAG += -DHSL_STANDALONG
endif

ifeq (TRUE, $(UK_IF))
  MODULE_CFLAG += -DUK_IF
endif

#ifdef UK_NL_PROT
  MODULE_CFLAG += -DUK_NL_PROT=$(UK_NL_PROT)
#endif

#ifdef UK_MINOR_DEV
  MODULE_CFLAG += -DUK_MINOR_DEV=$(UK_MINOR_DEV)
#endif

ifeq (TRUE, $(API_LOCK))
  MODULE_CFLAG += -DAPI_LOCK
endif

ifeq (TRUE, $(REG_ACCESS_SPEEDUP))
  MODULE_CFLAG += -DREG_ACCESS_SPEEDUP
endif

ifeq (TRUE, $(DEBUG_ON))
  MODULE_CFLAG += -g
endif

MODULE_CFLAG += $(OPT_FLAG) -Wall -DVERSION=\"$(VERSION)\" -DBUILD_DATE=\"$(BUILD_DATE)\" -DOS=\"$(OS)\" -D"KBUILD_STR(s)=\#s" -D"KBUILD_MODNAME=KBUILD_STR(qca-ssdk)"

MODULE_INC += -I$(PRJ_PATH)/include \
                   -I$(PRJ_PATH)/include/common \
                   -I$(PRJ_PATH)/include/api \
                   -I$(PRJ_PATH)/include/fal \
                   -I$(PRJ_PATH)/include/ref \
                   -I$(PRJ_PATH)/include/adpt \
                   -I$(PRJ_PATH)/include/hsl \
                   -I$(PRJ_PATH)/include/hsl/phy \
                   -I$(PRJ_PATH)/include/sal/os \
		   -I$(PRJ_PATH)/include/sal/os/linux \
                   -I$(PRJ_PATH)/include/sal/sd \
                   -I$(PRJ_PATH)/include/sal/sd/linux/hydra_howl \
                   -I$(PRJ_PATH)/include/sal/sd/linux/uk_interface \
                   -I$(PRJ_PATH)/include/init

ifneq (,$(findstring ATHENA, $(SUPPORT_CHIP)))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/athena
  MODULE_CFLAG += -DATHENA
endif

ifneq (,$(findstring GARUDA, $(SUPPORT_CHIP)))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/garuda
  MODULE_CFLAG += -DGARUDA
endif

ifneq (,$(findstring SHIVA, $(SUPPORT_CHIP)))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/shiva
  MODULE_CFLAG += -DSHIVA
endif

ifneq (,$(findstring HORUS, $(SUPPORT_CHIP)))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/horus
  MODULE_CFLAG += -DHORUS
endif

ifneq (,$(filter ISIS, $(SUPPORT_CHIP)))
     MODULE_INC   += -I$(PRJ_PATH)/include/hsl/isis
     MODULE_CFLAG += -DISIS
endif

ifneq (,$(findstring ISISC, $(SUPPORT_CHIP)))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/isisc
  MODULE_CFLAG += -DISISC
endif

ifneq (,$(findstring DESS, $(SUPPORT_CHIP)))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/dess
  MODULE_CFLAG += -DDESS
endif

ifneq (,$(findstring HPPE, $(SUPPORT_CHIP)))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/hppe
  MODULE_INC   += -I$(PRJ_PATH)/include/adpt/hppe
  MODULE_CFLAG += -DHPPE
endif

ifneq (,$(filter MP, $(SUPPORT_CHIP)))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/hppe
  MODULE_INC   += -I$(PRJ_PATH)/include/adpt/mp
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/mp
  MODULE_CFLAG += -DMP
endif

ifneq (,$(findstring CPPE, $(SUPPORT_CHIP)))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/cppe
  MODULE_INC   += -I$(PRJ_PATH)/include/adpt/cppe
  MODULE_CFLAG += -DCPPE
endif


ifneq (,$(findstring SCOMPHY, $(SUPPORT_CHIP)))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/scomphy
  MODULE_CFLAG += -DSCOMPHY
endif

ifeq (TRUE, $(IN_SFP))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/sfp
  MODULE_INC   += -I$(PRJ_PATH)/include/adpt/sfp
  MODULE_CFLAG += -DIN_SFP
endif

# check for GCC version
ifeq (4, $(GCC_VER))
  MODULE_CFLAG += -DGCCV4
endif

ifeq (TRUE, $(IN_PTP))
ifeq ($(CONFIG_PTP_1588_CLOCK), y)
	MODULE_CFLAG += -DIN_LINUX_STD_PTP
endif
endif

ifeq (KSLIB, $(MODULE_TYPE))

  MODULE_INC += -I$(PRJ_PATH)/include/shell_lib
  ifndef TARGET_NAME
	TARGET_NAME=arm-openwrt-linux-$(TARGET_SUFFIX)
  endif

  ifeq ($(CONFIG_KASAN_INLINE),y)
        CALL_THRESHOLD=10000
  else
        CALL_THRESHOLD=0
  endif
  ifneq ($(CONFIG_KASAN_SHADOW_OFFSET),)
        SHADOW_OFFSET=$(CONFIG_KASAN_SHADOW_OFFSET)
  else
        SHADOW_OFFSET=0xdfffff9000000000
  endif
  KASAN_OPTION=-fsanitize=kernel-address -fasan-shadow-offset=$(SHADOW_OFFSET) \
               --param asan-stack=1 --param asan-globals=1 \
               --param asan-instrumentation-with-call-threshold=$(CALL_THRESHOLD)

  ifeq ($(CONFIG_KASAN_SW_TAGS), y)
      KASAN_SHADOW_SCALE_SHIFT := 4
  else
      KASAN_SHADOW_SCALE_SHIFT := 3
  endif

  ifeq (5_4, $(OS_VER))
      ifeq ($(ARCH), arm64)
          KASAN_OPTION += -DKASAN_SHADOW_SCALE_SHIFT=$(KASAN_SHADOW_SCALE_SHIFT)
       endif
  endif
  ifeq ($(CONFIG_KASAN),y)
      MODULE_CFLAG += $(KASAN_OPTION)
  endif

  ifeq (3_18, $(OS_VER))
		MODULE_CFLAG += -DKVER34
		MODULE_CFLAG += -DKVER32
	    MODULE_CFLAG += -DLNX26_22
	    MODULE_INC += -I$(SYS_PATH) \
                  -I$(TOOL_PATH)/../lib/gcc/$(TARGET_NAME)/$(GCC_VERSION)/include/ \
	          -I$(SYS_PATH)/include \
              -I$(SYS_PATH)/source/include \
              -I$(SYS_PATH)/source/arch/arm/mach-msm/include \
              -I$(SYS_PATH)/arch/arm/mach-msm/include \
              -I$(SYS_PATH)/source/arch/arm/include \
              -I$(SYS_PATH)/arch/arm/include \
              -I$(SYS_PATH)/source/arch/arm/include/asm \
              -I$(SYS_PATH)/arch/arm/include/generated \
              -I$(SYS_PATH)/include/generated/uapi \
              -I$(SYS_PATH)/include/uapi \
              -I$(SYS_PATH)/arch/arm/include/uapi \
              -I$(SYS_PATH)/source/arch/arm/include/asm/mach \
	          -include $(SYS_PATH)/include/linux/kconfig.h

  endif

  ifeq ($(OS_VER),$(filter 4_4 5_4, $(OS_VER)))
                MODULE_CFLAG += -DKVER34
                MODULE_CFLAG += -DKVER32
            MODULE_CFLAG += -DLNX26_22
	    ifeq ($(ARCH), arm64)
            MODULE_INC += -I$(SYS_PATH) \
                  -I$(TOOL_PATH)/../lib/gcc/$(TARGET_NAME)/$(GCC_VERSION)/include/ \
                  -I$(SYS_PATH)/include \
              -I$(SYS_PATH)/source \
              -I$(SYS_PATH)/source/include \
	      -I$(SYS_PATH)/source/arch/arm64/mach-msm/include \
              -I$(SYS_PATH)/arch/arm64/mach-msm/include \
              -I$(SYS_PATH)/source/arch/arm64/include \
              -I$(SYS_PATH)/arch/arm64/include \
              -I$(SYS_PATH)/source/arch/arm64/include/asm \
              -I$(SYS_PATH)/arch/arm64/include/generated \
              -I$(SYS_PATH)/include/generated/uapi \
              -I$(SYS_PATH)/include/uapi \
              -I$(SYS_PATH)/arch/arm64/include/uapi \
              -I$(SYS_PATH)/source/arch/arm64/include/asm/mach

	      ifneq ($(wildcard $(SYS_PATH)/include/linux/kconfig.h),)
	          MODULE_INC += -include $(SYS_PATH)/include/linux/kconfig.h
	      else
	          MODULE_INC += -include $(KERNEL_SRC)/include/linux/kconfig.h
	      endif

	    else ifeq ($(ARCH), arm)
	    MODULE_INC += -I$(SYS_PATH) \
              -I$(TOOL_PATH)/../lib/gcc/$(TARGET_NAME)/$(GCC_VERSION)/include/ \
	      -I$(TOOL_PATH)/../lib/gcc/$(TARGET_NAME)/7.5.0/include/ \
              -I$(TOOL_PATH)/../../lib/armv7a-vfp-neon-rdk-linux-gnueabi/gcc/arm-rdk-linux-gnueabi/4.8.4/include/ \
	      -I$(TOOL_PATH)/../../lib/arm-rdk-linux-musleabi/gcc/arm-rdk-linux-musleabi/6.4.0/include/ \
              -I$(SYS_PATH)/include \
              -I$(SYS_PATH)/source \
              -I$(SYS_PATH)/source/include \
              -I$(SYS_PATH)/source/arch/arm/mach-msm/include \
              -I$(SYS_PATH)/arch/arm/mach-msm/include \
              -I$(SYS_PATH)/source/arch/arm/include \
              -I$(SYS_PATH)/arch/arm/include \
              -I$(SYS_PATH)/source/arch/arm/include/asm \
              -I$(SYS_PATH)/arch/arm/include/generated \
	      -I$(SYS_PATH)/arch/arm/include/generated/uapi \
	      -I$(SYS_PATH)/source/include/uapi \
              -I$(SYS_PATH)/include/generated/uapi \
              -I$(SYS_PATH)/include/uapi \
              -I$(SYS_PATH)/arch/arm/include/uapi \
              -I$(SYS_PATH)/source/arch/arm/include/asm/mach

	      ifneq ($(wildcard $(SYS_PATH)/include/linux/kconfig.h),)
	          MODULE_INC += -include $(SYS_PATH)/include/linux/kconfig.h
	      else
	          MODULE_INC += -include $(KERNEL_SRC)/include/linux/kconfig.h
	      endif

            else
            MODULE_INC += -I$(SYS_PATH) \
              -I$(TOOL_PATH)/../lib/gcc/$(TARGET_NAME)/$(GCC_VERSION)/include/ \
              -I$(SYS_PATH)/include \
              -I$(SYS_PATH)/source \
	      -I$(SYS_PATH)/source/include \
              -I$(SYS_PATH)/source/arch/mips/mach-msm/include \
	      -I$(SYS_PATH)/arch/mips/mach-msm/include \
	      -I$(SYS_PATH)/source/arch/mips/include \
              -I$(SYS_PATH)/arch/mips/include \
	      -I$(SYS_PATH)/source/arch/mips/include/asm \
	      -I$(SYS_PATH)/arch/mips/include/generated \
              -I$(SYS_PATH)/include/generated/uapi \
	      -I$(SYS_PATH)/include/uapi \
	      -I$(SYS_PATH)/arch/mips/include/uapi \
	      -I$(SYS_PATH)/source/arch/mips/include/asm/mach \
	      -include $(SYS_PATH)/include/linux/kconfig.h \
              -I$(SYS_PATH)/arch/mips/include/asm/mach-ar7240 \
              -I$(SYS_PATH)/arch/mips/include/asm/mach-generic \
              -I$(SYS_PATH)/arch/mips/include/asm/mach-ar7 \
              -I$(SYS_PATH)/usr/include

	    #CPU_CFLAG    = -G 0 -mno-abicalls -fno-pic -pipe -mabi=32 -march=mips32r2
            ifndef CPU_CFLAG
            CPU_CFLAG    = -Wstrict-prototypes -fomit-frame-pointer -G 0 -mno-abicalls -fno-strict-aliasing \
			-O2 -fno-pic -pipe -mabi=32 -march=mips32r2 -DMODULE -mlong-calls -DEXPORT_SYMTAB
            endif
            endif

  endif

  ifeq ($(OS_VER),$(filter 4_9 4_1, $(OS_VER)))
                MODULE_CFLAG += -DKVER34
                MODULE_CFLAG += -DKVER32
            MODULE_CFLAG += -DLNX26_22
	    ifeq ($(ARCH), arm64)
	    KCONF_FILE = $(SYS_PATH)/source/include/linux/kconfig.h
            MODULE_INC += -I$(SYS_PATH) \
                  -I$(TOOL_PATH)/../lib/gcc/$(TARGET_NAME)/$(GCC_VERSION)/include/ \
                  -I$(SYS_PATH)/include \
              -I$(SYS_PATH)/source/include \
	      -I$(SYS_PATH)/source/arch/arm64/mach-msm/include \
              -I$(SYS_PATH)/arch/arm64/mach-msm/include \
              -I$(SYS_PATH)/source/arch/arm64/include \
              -I$(SYS_PATH)/arch/arm64/include \
              -I$(SYS_PATH)/source/arch/arm64/include/asm \
              -I$(SYS_PATH)/arch/arm64/include/generated \
              -I$(SYS_PATH)/include/generated/uapi \
              -I$(SYS_PATH)/include/uapi \
              -I$(SYS_PATH)/arch/arm64/include/uapi \
	      -I$(SYS_PATH)/source/include/uapi \
              -I$(SYS_PATH)/source/arch/arm64/include/asm/mach \
	      -include $(KCONF_FILE)
	    else ifeq ($(ARCH), arm)
	    MODULE_INC += -I$(SYS_PATH) \
              -I$(TOOL_PATH)/../lib/gcc/$(TARGET_NAME)/$(GCC_VERSION)/include/ \
              -I$(TOOL_PATH)/../../lib/armv7a-vfp-neon-rdk-linux-gnueabi/gcc/arm-rdk-linux-gnueabi/4.8.4/include/ \
              -I$(SYS_PATH)/include \
              -I$(SYS_PATH)/source/include \
              -I$(SYS_PATH)/source/arch/arm/mach-msm/include \
              -I$(SYS_PATH)/arch/arm/mach-msm/include \
              -I$(SYS_PATH)/source/arch/arm/include \
              -I$(SYS_PATH)/arch/arm/include \
              -I$(SYS_PATH)/source/arch/arm/include/asm \
              -I$(SYS_PATH)/arch/arm/include/generated \
              -I$(SYS_PATH)/include/generated/uapi \
              -I$(SYS_PATH)/include/uapi \
              -I$(SYS_PATH)/arch/arm/include/uapi \
              -I$(SYS_PATH)/source/arch/arm/include/asm/mach \
	          -include $(SYS_PATH)/include/linux/kconfig.h
	    endif
  endif

  ifeq (3_14, $(OS_VER))
		MODULE_CFLAG += -DKVER34
		MODULE_CFLAG += -DKVER32
	    MODULE_CFLAG += -DLNX26_22
	    MODULE_INC += -I$(SYS_PATH) \
                  -I$(TOOL_PATH)/../lib/gcc/$(TARGET_NAME)/$(GCC_VERSION)/include/ \
		  -I$(TOOL_PATH)/../../lib/arm-poky-linux-gnueabi/gcc/arm-poky-linux-gnueabi/5.3.0/include/ \
		  -I$(TOOL_PATH)/../../lib/armv7a-vfp-neon-rdk-linux-gnueabi/gcc/arm-rdk-linux-gnueabi/4.8.4/include/ \
	          -I$(SYS_PATH)/include \
              -I$(SYS_PATH)/source/ \
              -I$(SYS_PATH)/source/include \
              -I$(SYS_PATH)/source/arch/arm/mach-msm/include \
              -I$(SYS_PATH)/arch/arm/mach-msm/include \
              -I$(SYS_PATH)/source/arch/arm/include \
              -I$(SYS_PATH)/arch/arm/include \
              -I$(SYS_PATH)/source/arch/arm/include/asm \
              -I$(SYS_PATH)/arch/arm/include/generated \
              -I$(SYS_PATH)/include/generated/uapi \
              -I$(SYS_PATH)/include/uapi \
              -I$(SYS_PATH)/source/include/uapi \
              -I$(SYS_PATH)/source/include/generated \
              -I$(SYS_PATH)/include/genearted \
              -I$(SYS_PATH)/arch/arm/include/uapi \
              -I$(SYS_PATH)/source/arch/arm/include/uapi \
              -I$(EXT_PATH) \
              -I$(SYS_PATH)/source/arch/arm/include/asm/mach
	ifneq ($(wildcard $(SYS_PATH)/include/linux/kconfig.h),)
		MODULE_INC += \
			-include $(SYS_PATH)/include/linux/kconfig.h
	else
		MODULE_INC += \
			-include $(SYS_PATH)/source/include/linux/kconfig.h
	endif

  endif

  ifeq (3_4, $(OS_VER))
		MODULE_CFLAG += -DKVER34
		MODULE_CFLAG += -DKVER32
	    MODULE_CFLAG += -DLNX26_22
	    MODULE_CFLAG += -Werror
	    MODULE_INC += -I$(SYS_PATH) \
	          -I$(SYS_PATH)/include \
              -I$(SYS_PATH)/source/include \
              -I$(SYS_PATH)/source/arch/arm/mach-msm/include \
              -I$(SYS_PATH)/source/arch/arm/include \
              -I$(SYS_PATH)/source/arch/arm/include/asm \
              -I$(SYS_PATH)/arch/arm/include/generated \
              -I$(SYS_PATH)/source/arch/arm/include/asm/mach \
	      -I$(SYS_PATH)/usr/include

  endif

  ifeq (3_2, $(OS_VER))
	MODULE_CFLAG += -DKVER32
	MODULE_CFLAG += -DLNX26_22
	ifeq (mips, $(CPU))
	  MODULE_INC += -I$(SYS_PATH) \
            -I$(SYS_PATH)/include \
            -I$(SYS_PATH)/arch/mips/include \
	    -I$(SYS_PATH)/arch/mips/include/asm/mach-ar7240 \
	    -I$(SYS_PATH)/arch/mips/include/asm/mach-generic \
		-I$(SYS_PATH)/arch/mips/include/asm/mach-ar7 \
	    -I$(SYS_PATH)/usr/include \
	    -I${KERN_SRC_PATH} \
            -I${KERN_SRC_PATH}/include \
            -I$(KERN_SRC_PATH)/arch/mips/include \
            -I$(KERN_SRC_PATH)/arch/mips/include/asm/mach-ar7240 \
            -I$(KERN_SRC_PATH)/arch/mips/include/asm/mach-generic \
            -I$(KERN_SRC_PATH)/arch/mips/include/asm/mach-ar7 \
            -I$(KERN_SRC_PATH)/usr/include

	    #CPU_CFLAG    = -G 0 -mno-abicalls -fno-pic -pipe -mabi=32 -march=mips32r2
          ifndef CPU_CFLAG
	    CPU_CFLAG    = -Wstrict-prototypes -fomit-frame-pointer -G 0 -mno-abicalls -fno-strict-aliasing \
                     -O2 -fno-pic -pipe -mabi=32 -march=mips32r2 -DMODULE -mlong-calls -DEXPORT_SYMTAB
          endif
    else
	    MODULE_INC += -I$(SYS_PATH) \
              -I$(SYS_PATH)/include \
              -I$(SYS_PATH)/arch/arm/include \
              -I$(SYS_PATH)/arch/arm/include/asm \
              -I$(SYS_PATH)/arch/arm/mach-fv16xx/include \
	      -I$(SYS_PATH)/arch/arm/include/generated \
	      -I$(SYS_PATH)/include/generated \
	      -I$(SYS_PATH)/usr/include
        endif


  endif

  ifeq (2_6, $(OS_VER))
        MODULE_CFLAG += -DKVER26
        MODULE_CFLAG += -DLNX26_22
        ifeq (mips, $(CPU))
          MODULE_INC += -I$(SYS_PATH) \
            -I$(SYS_PATH)/include \
            -I$(SYS_PATH)/arch/mips/include \
            -I$(SYS_PATH)/arch/mips/include/asm/mach-ar7240 \
            -I$(SYS_PATH)/arch/mips/include/asm/mach-generic \
            -I$(SYS_PATH)/usr/include

            #CPU_CFLAG    = -G 0 -mno-abicalls -fno-pic -pipe -mabi=32 -march=mips32r2
          ifndef CPU_CFLAG
            CPU_CFLAG    = -Wstrict-prototypes -fomit-frame-pointer -G 0 -mno-abicalls -fno-strict-aliasing \
                     -O2 -fno-pic -pipe -mabi=32 -march=mips32r2 -DMODULE -mlong-calls -DEXPORT_SYMTAB
          endif
        else
	    MODULE_INC += -I$(SYS_PATH) \
              -I$(SYS_PATH)/include \
              -I$(SYS_PATH)/arch/arm/include \
              -I$(SYS_PATH)/arch/arm/include/asm \
              -I$(SYS_PATH)/arch/arm/mach-fv16xx/include \
	      -I$(SYS_PATH)/arch/arm/include/generated \
	      -I$(SYS_PATH)/include/generated \
	      -I$(SYS_PATH)/usr/include
        endif


  endif

  MODULE_CFLAG += -D__KERNEL__ -DKERNEL_MODULE $(CPU_CFLAG)


endif

ifeq (SHELL, $(MODULE_TYPE))
  MODULE_INC += -I$(PRJ_PATH)/include/shell

    ifeq (2_6, $(OS_VER))
  	  MODULE_CFLAG += -DKVER26
    else
   	  MODULE_CFLAG += -DKVER24
    endif

    ifeq (TRUE, $(KERNEL_MODE))
      MODULE_CFLAG += -static
    else
      MODULE_CFLAG += -static -DUSER_MODE
    endif
endif

ifneq (TRUE, $(KERNEL_MODE))
  ifneq (SHELL, $(MODULE_TYPE))
    MODULE_CFLAG +=  -DUSER_MODE
  endif
endif

LOCAL_CFLAGS += $(MODULE_INC) $(MODULE_CFLAG) $(EXTRA_CFLAGS)
