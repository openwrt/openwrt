# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Package/Default
  CONFIGFILE:=
  SECTION:=opt
  CATEGORY:=Extra packages
  DEPENDS:=
  PROVIDES:=
  EXTRA_DEPENDS:=
  MAINTAINER:=OpenWrt Developers Team <openwrt-devel@openwrt.org>
  SOURCE:=$(patsubst $(TOPDIR)/%,%,${shell pwd})
  ifneq ($(PKG_VERSION),)
    ifneq ($(PKG_RELEASE),)
      VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
    else
      VERSION:=$(PKG_VERSION)
    endif
  else
    VERSION:=$(PKG_RELEASE)
  endif
  PKGARCH:=$(ARCH)
  PRIORITY:=optional
  DEFAULT:=
  MENU:=
  SUBMENU:=
  SUBMENUDEP:=
  TITLE:=
  KCONFIG:=
  BUILDONLY:=
endef

Build/Patch:=$(Build/Patch/Default)
ifneq ($(strip $(PKG_UNPACK)),)
  define Build/Prepare/Default
  	$(PKG_UNPACK)
	$(Build/Patch)
	$(if $(QUILT),touch $(PKG_BUILD_DIR)/.quilt_used)
  endef
endif

export PKG_CONFIG_PATH=$(STAGING_DIR)/usr/lib/pkgconfig:$(STAGING_DIR_HOST)/usr/lib/pkgconfig
export PKG_CONFIG_LIBDIR=$(STAGING_DIR)/usr/lib/pkgconfig

CONFIGURE_PREFIX:=/usr
CONFIGURE_ARGS = \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=$(CONFIGURE_PREFIX) \
		--exec-prefix=$(CONFIGURE_PREFIX) \
		--bindir=$(CONFIGURE_PREFIX)/bin \
		--sbindir=$(CONFIGURE_PREFIX)/sbin \
		--libexecdir=$(CONFIGURE_PREFIX)/lib \
		--sysconfdir=/etc \
		--datadir=$(CONFIGURE_PREFIX)/share \
		--localstatedir=/var \
		--mandir=$(CONFIGURE_PREFIX)/man \
		--infodir=$(CONFIGURE_PREFIX)/info \
		$(DISABLE_NLS)

CONFIGURE_VARS = \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		CXXFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS)" \
		PKG_CONFIG_PATH="$(PKG_CONFIG_PATH)" \
		PKG_CONFIG_LIBDIR="$(PKG_CONFIG_LIBDIR)"

CONFIGURE_PATH = .
CONFIGURE_CMD = ./configure

replace_script=$(FIND) $(1) -name $(2) | $(XARGS) chmod u+w; $(FIND) $(1) -name $(2) | $(XARGS) -n1 cp $(SCRIPT_DIR)/$(2);

define Build/Configure/Default
	(cd $(PKG_BUILD_DIR)/$(CONFIGURE_PATH)/$(strip $(3)); \
	if [ -x $(CONFIGURE_CMD) ]; then \
		$(call replace_script,$(PKG_BUILD_DIR)/$(3),config.guess) \
		$(call replace_script,$(PKG_BUILD_DIR)/$(3),config.sub) \
		$(CONFIGURE_VARS) \
		$(2) \
		$(CONFIGURE_CMD) \
		$(CONFIGURE_ARGS) \
		$(1); \
	fi; \
	)
endef

MAKE_VARS = \
	CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS) $(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
	CXXFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS) $(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS)"

MAKE_FLAGS = \
	$(TARGET_CONFIGURE_OPTS) \
	CROSS="$(TARGET_CROSS)" \
	ARCH="$(ARCH)"

MAKE_INSTALL_FLAGS = \
	$(MAKE_FLAGS) \
	DESTDIR="$(PKG_INSTALL_DIR)"

MAKE_PATH = .

define Build/Compile/Default
	$(MAKE_VARS) \
	$(MAKE) -C $(PKG_BUILD_DIR)/$(MAKE_PATH) \
		$(MAKE_FLAGS) \
		$(1);
endef

define Build/Install/Default
	$(MAKE_VARS) \
	$(MAKE) -C $(PKG_BUILD_DIR)/$(MAKE_PATH) \
		$(MAKE_INSTALL_FLAGS) \
		$(1) install;
endef
