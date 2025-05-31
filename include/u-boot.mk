include $(INCLUDE_DIR)/prereq.mk

PKG_NAME ?= u-boot

ifndef PKG_SOURCE_PROTO
PKG_SOURCE = $(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL = \
	https://mirror.cyberbits.eu/u-boot \
	https://ftp.denx.de/pub/u-boot \
	ftp://ftp.denx.de/pub/u-boot
endif

PKG_BUILD_DIR = $(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

PKG_TARGETS := bin
PKG_FLAGS:=nonshared

PKG_LICENSE:=GPL-2.0 GPL-2.0+
PKG_LICENSE_FILES:=Licenses/README

PKG_BUILD_PARALLEL ?= 1

ifdef UBOOT_USE_BINMAN
  $(eval $(call TestHostCommand,python3-pyelftools, \
    Please install the Python3 elftools module, \
    $(STAGING_DIR_HOST)/bin/python3 -c 'import elftools'))
endif

ifdef UBOOT_USE_INTREE_DTC
  $(eval $(call TestHostCommand,python3-dev, \
    Please install the python3-dev package, \
    python3.11-config --includes 2>&1 | grep 'python3', \
    python3.10-config --includes 2>&1 | grep 'python3', \
    python3.9-config --includes 2>&1 | grep 'python3', \
    python3.8-config --includes 2>&1 | grep 'python3', \
    python3.7-config --includes 2>&1 | grep 'python3', \
    python3-config --includes 2>&1 | grep -E 'python3\.([7-9]|[0-9][0-9])\.?'))

  $(eval $(call TestHostCommand,python3-setuptools, \
    Please install the Python3 setuptools module, \
    $(STAGING_DIR_HOST)/bin/python3 -c 'import setuptools'))

  $(eval $(call TestHostCommand,swig, \
    Please install the swig package, \
    swig -version))
endif

export GCC_HONOUR_COPTS=s

define Package/u-boot/install/default
	$(CP) $(patsubst %,$(PKG_BUILD_DIR)/%,$(UBOOT_IMAGE)) $(1)/
endef

Package/u-boot/install = $(Package/u-boot/install/default)

define U-Boot/Init
  BUILD_TARGET:=
  BUILD_SUBTARGET:=
  BUILD_DEVICES:=
  NAME:=
  DEPENDS:=
  HIDDEN:=
  DEFAULT:=
  VARIANT:=$(1)
  UBOOT_CONFIG:=$(1)
  UBOOT_IMAGE:=u-boot.bin
endef

TARGET_DEP = TARGET_$(BUILD_TARGET)$(if $(BUILD_SUBTARGET),_$(BUILD_SUBTARGET))

UBOOT_MAKE_FLAGS = \
	PATH=$(STAGING_DIR_HOST)/bin:$(PATH) \
	HOSTCC="$(HOSTCC)" \
	HOSTCFLAGS="$(HOST_CFLAGS) $(HOST_CPPFLAGS) -std=gnu11" \
	HOSTLDFLAGS="$(HOST_LDFLAGS)" \
	LOCALVERSION="-OpenWrt-$(REVISION)" \
	STAGING_PREFIX="$(STAGING_DIR_HOST)" \
	PKG_CONFIG_PATH="$(STAGING_DIR_HOST)/lib/pkgconfig" \
	PKG_CONFIG_LIBDIR="$(STAGING_DIR_HOST)/lib/pkgconfig" \
	PKG_CONFIG_EXTRAARGS="--static" \
	$(if $(KBUILD_CFLAGS),KCFLAGS="$(KBUILD_CFLAGS)") \
	$(if $(findstring c,$(OPENWRT_VERBOSE)),V=1,V='')

define Build/U-Boot/Target
  $(eval $(call U-Boot/Init,$(1)))
  $(eval $(call U-Boot/Default,$(1)))
  $(eval $(call U-Boot/$(1),$(1)))

 define Package/u-boot-$(1)
    SECTION:=boot
    CATEGORY:=Boot Loaders
    TITLE:=U-Boot for $(NAME)
    VARIANT:=$(VARIANT)
    DEPENDS:=@!IN_SDK $(DEPENDS)
    HIDDEN:=$(HIDDEN)
    ifneq ($(BUILD_TARGET),)
      DEPENDS += @$(TARGET_DEP)
      ifneq ($(BUILD_DEVICES),)
        DEFAULT := y if ($(TARGET_DEP)_Default \
		$(patsubst %,|| $(TARGET_DEP)_DEVICE_%,$(BUILD_DEVICES)) \
		$(patsubst %,|| $(patsubst TARGET_%,TARGET_DEVICE_%,$(TARGET_DEP))_DEVICE_%,$(BUILD_DEVICES)))
      endif
    endif
    $(if $(DEFAULT),DEFAULT:=$(DEFAULT))
    URL:=http://www.denx.de/wiki/U-Boot
  endef

  define Package/u-boot-$(1)/install
	$$(Package/u-boot/install)
  endef
endef

define Build/Configure/U-Boot
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE=$(TARGET_CROSS) \
		$(UBOOT_CONFIGURE_VARS) \
		$(firstword $(UBOOT_CONFIG))_config \
		$(wordlist 2,$(words $(UBOOT_CONFIG)),$(UBOOT_CONFIG:%=%.config))
	$(if $(strip $(UBOOT_CUSTOMIZE_CONFIG)),
		$(PKG_BUILD_DIR)/scripts/config --file $(PKG_BUILD_DIR)/.config $(UBOOT_CUSTOMIZE_CONFIG)
		+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) CROSS_COMPILE=$(TARGET_CROSS) $(UBOOT_CONFIGURE_VARS) oldconfig)
endef

ifndef UBOOT_USE_INTREE_DTC
  DTC=$(wildcard $(LINUX_DIR)/scripts/dtc/dtc)
endif

define Build/Compile/U-Boot
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE=$(TARGET_CROSS) \
		$(if $(DTC),DTC="$(DTC)") \
		$(UBOOT_MAKE_FLAGS)
endef

define BuildPackage/U-Boot/Defaults
  Build/Configure/Default = $$$$(Build/Configure/U-Boot)
  Build/Compile/Default = $$$$(Build/Compile/U-Boot)
endef

define BuildPackage/U-Boot
  $(eval $(call BuildPackage/U-Boot/Defaults))
  $(foreach type,$(if $(DUMP),$(UBOOT_TARGETS),$(BUILD_VARIANT)), \
    $(eval $(call Build/U-Boot/Target,$(type)))
  )
  $(eval $(call Build/DefaultTargets))
  $(foreach type,$(if $(DUMP),$(UBOOT_TARGETS),$(BUILD_VARIANT)), \
    $(call BuildPackage,u-boot-$(type))
  )
endef
