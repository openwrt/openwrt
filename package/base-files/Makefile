#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=base-files
PKG_RELEASE:=13

PKG_FILE_DEPEND:=$(PLATFORM_DIR)/ $(GENERIC_PLATFORM_DIR)/base-files/

include $(INCLUDE_DIR)/package.mk

ifneq ($(DUMP),1)
  TARGET:=-$(BOARD)
  ifneq ($(wildcard $(PLATFORM_DIR)/base-files-$(PROFILE) $(PLATFORM_SUBDIR)/base-files-$(PROFILE)),)
    TARGET:=$(TARGET)-$(PROFILE)
  endif
  LIBGCC_VERSION:=$(GCC_VERSION)
  ifeq ($(ARCH),x86_64)
    LIB_SUFFIX:=64
  endif
else
  UCLIBC_VERSION:=<UCLIBC_VERSION>
  LIBGCC_VERSION:=<LIBGCC_VERSION>
endif

CONFIG_PACKAGE_base-files$(TARGET):=$(CONFIG_PACKAGE_base-files)

define Package/base-files$(TARGET)
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=Base filesystem for OpenWrt
  URL:=http://openwrt.org/
  VERSION:=$(PKG_RELEASE)-$(REVISION)
  $(call Config,network.lan.proto,string,static,LAN Protocol)
  $(call Config,network.lan.ipaddr,ip,192.168.1.1,LAN IP Address)
  $(call Config,network.lan.netmask,netmask,255.255.255.0,LAN Network Mask)
  $(call Config,network.lan.gateway,ip,,LAN Gateway)
  $(call Config,network.lan.dns,ip,,LAN DNS server)
endef

define Package/base-files$(TARGET)/conffiles
/etc/banner
/etc/hosts
/etc/inittab
/etc/group
/etc/passwd
/etc/profile
/etc/shells
/etc/sysctl.conf
$(call $(TARGET)/conffiles)
endef

define Package/base-files$(TARGET)/description
 This package contains a base filesystem and system scripts for OpenWrt.
endef

define Package/gcc/Default
  SECTION:=libs
  CATEGORY:=Base system
  DEPENDS:=@!NATIVE_TOOLCHAIN
  URL:=http://gcc.gnu.org/
  VERSION:=$(LIBGCC_VERSION)-$(PKG_RELEASE)
endef

define Package/libgcc
$(call Package/gcc/Default)
  TITLE:=GCC support library
  DEPENDS:=@!TARGET_avr32 @!NATIVE_TOOLCHAIN
endef

define Package/libssp
$(call Package/gcc/Default)
  DEPENDS:=@SSP_SUPPORT
  TITLE:=GCC support library
endef

define Package/libstdcpp
$(call Package/gcc/Default)
  NAME:=libstdc++
  TITLE:=GNU Standard C++ Library v3
  DEPENDS:=@!NATIVE_TOOLCHAIN&&@INSTALL_LIBSTDCPP
endef

define Package/uclibc/Default
  SECTION:=libs
  CATEGORY:=Base system
  DEPENDS:=@!NATIVE_TOOLCHAIN
  URL:=http://uclibc.org/
  VERSION:=$(UCLIBC_VERSION)$(UCLIBC_PATCHVER)-$(PKG_RELEASE)
endef

define Package/libpthread
$(call Package/uclibc/Default)
  TITLE:=POSIX thread library
endef

define Package/uclibc
$(call Package/uclibc/Default)
  TITLE:=C library embedded systems
endef

define Package/ldd
$(call Package/uclibc/Default)
  TITLE:=LDD trace utility
endef

define Package/ldconfig
$(call Package/uclibc/Default)
  TITLE:=Shared library path configuration
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default
endef

define Build/Compile
	$(call Build/Compile/Default)
endef

define Package/base-files$(TARGET)/install
	$(CP) ./files/* $(1)/
	if [ -d $(GENERIC_PLATFORM_DIR)/base-files/. ]; then \
		$(CP) $(GENERIC_PLATFORM_DIR)/base-files/* $(1)/; \
	fi
	if [ -d $(PLATFORM_DIR)/base-files/. ]; then \
		$(CP) $(PLATFORM_DIR)/base-files/* $(1)/; \
	fi
	if [ -d $(PLATFORM_DIR)/base-files-$(PROFILE)/. ]; then \
		$(CP) $(PLATFORM_DIR)/base-files-$(PROFILE)/* $(1)/; \
	fi
	$(if $(filter-out $(PLATFORM_DIR),$(PLATFORM_SUBDIR)), \
		if [ -d $(PLATFORM_SUBDIR)/base-files/. ]; then \
			$(CP) $(PLATFORM_SUBDIR)/base-files/* $(1)/; \
		fi; \
		if [ -d $(PLATFORM_SUBDIR)/base-files-$(PROFILE)/. ]; then \
			$(CP) $(PLATFORM_SUBDIR)/base-files-$(PROFILE)/* $(1)/; \
		fi \
	)
# Form valid /etc/shadow entries, by copying the user:pw pairs
# from /etc/passwd, and adding on pw change data
	$(if $(CONFIG_BUSYBOX_CONFIG_FEATURE_SHADOWPASSWDS), \
		cut -d ':' -f 1-2 $(1)/etc/passwd > $(1)/etc/shadow; \
		$(SED) 's/$$$$/:13848:0:99999:7:::/' $(1)/etc/shadow \
	)
	$(SED) 's,$$$$R,$(REVISION),g' $(1)/etc/banner
	mkdir -p $(1)/dev
	mkdir -p $(1)/etc/crontabs
	mkdir -p $(1)/jffs
	mkdir -p $(1)/lib/firmware
	mkdir -p $(1)/mnt
	mkdir -p $(1)/proc
	mkdir -p $(1)/tmp
	mkdir -p $(1)/usr/lib
	mkdir -p $(1)/usr/bin
	mkdir -p $(1)/sys
	mkdir -p $(1)/www
	mkdir -p $(1)/root
	ln -sf /proc/mounts $(1)/etc/mtab
	rm -f $(1)/var
	ln -sf /tmp $(1)/var
	mkdir -p $(1)/etc
	ln -sf /tmp/resolv.conf /tmp/fstab /tmp/TZ $(1)/etc/
	$(call Package/base-files/install-target,$(1))
	for conffile in $(1)/etc/config/*; do \
		if [ -f "$$$$conffile" ]; then \
			grep "$$$$conffile" $(1)/CONTROL/conffiles || \
				echo "$$$${conffile##$(1)}" >> $(1)/CONTROL/conffiles; \
		fi \
	done
endef

define Package/libgcc/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(TOOLCHAIN_DIR)/lib$(LIB_SUFFIX)/libgcc_s.so.* $(1)/lib/
endef

ifeq ($(word 1,$(subst ., ,$(LIBGCC_VERSION))),4)
  define Package/libssp/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(TOOLCHAIN_DIR)/lib$(LIB_SUFFIX)/libssp.so.* $(1)/lib/
  endef
endif

define Package/libstdcpp/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(TOOLCHAIN_DIR)/lib$(LIB_SUFFIX)/libstdc++.so.* $(1)/lib/
endef

define Package/libpthread/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(TOOLCHAIN_DIR)/lib/libpthread.so.* $(1)/lib/
	$(CP) $(TOOLCHAIN_DIR)/lib/libpthread-$(UCLIBC_VERSION).so $(1)/lib/
endef

define Package/uclibc/install
	$(INSTALL_DIR) $(1)/lib
	for file in ld$(LIB_SUFFIX)-uClibc libc libcrypt libdl libm libnsl libresolv librt libuClibc libutil; do \
		$(CP) $(TOOLCHAIN_DIR)/lib/$$$$file.so.* $(1)/lib/; \
		$(CP) $(TOOLCHAIN_DIR)/lib/$$$$file-$(UCLIBC_VERSION).so $(1)/lib/; \
	done
endef

define Package/ldd/install
	$(INSTALL_DIR) $(1)/bin/
	$(CP) $(TOOLCHAIN_DIR)/target-utils/ldd $(1)/bin/
endef

define Package/ldconfig/install
	$(INSTALL_DIR) $(1)/bin/
	$(CP) $(TOOLCHAIN_DIR)/target-utils/ldconfig $(1)/bin/
endef

ifneq ($(DUMP),1)
  -include $(PLATFORM_DIR)/base-files.mk
endif

$(eval $(call BuildPackage,base-files$(TARGET)))
$(eval $(call BuildPackage,libgcc))
$(eval $(call BuildPackage,libssp))
$(eval $(call BuildPackage,libstdcpp))
$(eval $(call BuildPackage,libpthread))
$(eval $(call BuildPackage,uclibc))
$(eval $(call BuildPackage,ldd))
$(eval $(call BuildPackage,ldconfig))
