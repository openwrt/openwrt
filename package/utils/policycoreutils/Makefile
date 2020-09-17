#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=policycoreutils
PKG_VERSION:=3.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/SELinuxProject/selinux/releases/download/20200710
PKG_HASH:=c889f62ee80f8b6a369469a9b8af51f5b797975aeaa291f5c5960cc12eed1934
PKG_INSTALL:=1
HOST_BUILD_DEPENDS:=libsemanage/host
PKG_BUILD_DEPENDS:=BUSYBOX_CONFIG_PAM:libpam

PKG_MAINTAINER:=Thomas Petazzoni <thomas.petazzoni@bootlin.com>
PKG_CPE_ID:=cpe:/a:selinuxproject:policycoreutils
PKG_LICENSE:=GPL-2.0-or-later
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/policycoreutils
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:= +libsemanage +libcap-ng +BUSYBOX_CONFIG_PAM:libpam $(INTL_DEPENDS)
  TITLE:=SELinux policy utilities
  URL:=http://selinuxproject.org/page/Main_Page
  ALTERNATIVES:=\
    300:/sbin/restorecon:policycoreutils-restorecon \
    300:/sbin/setfiles:policycoreutils-setfiles \
    300:/usr/sbin/load_policy:policycoreutils-load_policy \
    300:/usr/sbin/sestatus:policycoreutils-sestatus \
    300:/usr/sbin/setsebool:policycoreutils-setsebool
endef

define Package/policycoreutils/description
	Policycoreutils is a collection of policy utilities
	(originally the "core" set of utilities needed to use
	SELinux, although it has grown a bit over time), which have
	different dependencies.  sestatus, secon, run_init, and
	newrole only use libselinux. load_policy and setfiles only
	use libselinux and libsepol. semodule and semanage use
	libsemanage (and thus bring in dependencies on libsepol and
	libselinux as well). setsebool uses libselinux to make
	non-persistent boolean changes (via the kernel interface)
	and uses libsemanage to make persistent boolean changes.
endef

include $(INCLUDE_DIR)/host-build.mk

HOST_MAKE_FLAGS += \
	PAMH=$(CONFIG_BUSYBOX_CONFIG_PAM) \
	PREFIX=$(STAGING_DIR_HOSTPKG) \
	SBINDIR=$(STAGING_DIR_HOSTPKG)/sbin \
	ETCDIR=$(STAGING_DIR_HOSTPKG)/etc

MAKE_FLAGS += \
	PAMH=$(CONFIG_BUSYBOX_CONFIG_PAM)

define Package/policycoreutils/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/restorecon_xattr $(1)/sbin/restorecon_xattr
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/restorecon $(1)/sbin/policycoreutils-restorecon
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/setfiles $(1)/sbin/policycoreutils-setfiles
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/fixfiles $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/genhomedircon $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/open_init_pty $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/run_init $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/semodule $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/load_policy $(1)/usr/sbin/policycoreutils-load_policy
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/sestatus $(1)/usr/sbin/policycoreutils-sestatus
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/setsebool $(1)/usr/sbin/policycoreutils-setsebool
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,policycoreutils))
