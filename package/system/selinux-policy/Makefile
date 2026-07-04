#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=selinux-policy
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.defensec.nl/selinux-policy.git
PKG_VERSION:=2.8.4
PKG_MIRROR_HASH:=5ee24ac072414bb2a8a93cefeeb827a11123d7a61313d4389fce554ef2fb94ce
PKG_SOURCE_VERSION:=v$(PKG_VERSION)
PKG_BUILD_DEPENDS:=secilc/host policycoreutils/host

PKG_MAINTAINER:=Dominick Grift <dominick.grift@defensec.nl>
PKG_CPE_ID:=cpe:/a:defensec:selinux-policy
PKG_LICENSE:=Unlicense
PKG_LICENSE_FILES:=LICENSE

# The firstboot policy additions in files/mr6400-local.cil.in are
# specific to the TL-MR6400 v5 devices (jffs2 tmpfs-overlay firstboot).
# They are appended as an additive CIL (in ...) block at prepare time -
# no quilt patch to rebase on policy updates - and only when the build
# is configured for one of these devices (either as the single target
# profile or selected in a multi-device build). The device symbols are
# listed in PKG_PREPARED_DEPENDS so toggling them re-runs prepare.
MR6400_DEVICE_CONFIGS := \
	CONFIG_TARGET_ramips_mt76x8_DEVICE_tplink_tl-mr6400-v5 \
	CONFIG_TARGET_ramips_mt76x8_DEVICE_tplink_tl-mr6400-v5-ubootmod \
	CONFIG_TARGET_ramips_mt76x8_DEVICE_tplink_tl-mr6400-v5-ubootmod-16m \
	CONFIG_TARGET_DEVICE_ramips_mt76x8_DEVICE_tplink_tl-mr6400-v5 \
	CONFIG_TARGET_DEVICE_ramips_mt76x8_DEVICE_tplink_tl-mr6400-v5-ubootmod \
	CONFIG_TARGET_DEVICE_ramips_mt76x8_DEVICE_tplink_tl-mr6400-v5-ubootmod-16m

PKG_PREPARED_DEPENDS:=$(MR6400_DEVICE_CONFIGS)

include $(INCLUDE_DIR)/package.mk

ifneq ($(strip $(foreach sym,$(MR6400_DEVICE_CONFIGS),$($(sym)))),)
  define Build/Prepare
	$(call Build/Prepare/Default)
	$(SHELL) ./files/mr6400-local-cil.sh $(PKG_BUILD_DIR)
  endef
endif

define Package/selinux-policy
	SECTION:=system
	CATEGORY:=Base system
	TITLE:=SELinux security policy for OpenWrt
	URL:=https://git.defensec.nl/?p=selinux-policy.git;a=summary
	PKGARCH:=all
endef

define Package/selinux-policy/description
	Basic SELinux Security Policy designed specifically for
	OpenWrt and written in Common Intermediate Language.
endef

define Build/Compile
	$(call Build/Compile/Default,policy)
endef

define Package/selinux-policy/conffiles
/etc/selinux/config
endef

define Package/selinux-policy/install
	$(INSTALL_DIR) $(1)/etc/selinux/$(PKG_NAME)/contexts/files/
	$(INSTALL_DIR) $(1)/etc/selinux/$(PKG_NAME)/policy/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/policy.* $(1)/etc/selinux/$(PKG_NAME)/policy/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/customizable_types $(1)/etc/selinux/$(PKG_NAME)/contexts/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/default_contexts $(1)/etc/selinux/$(PKG_NAME)/contexts/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/default_type $(1)/etc/selinux/$(PKG_NAME)/contexts/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/failsafe_context $(1)/etc/selinux/$(PKG_NAME)/contexts/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/file_contexts $(1)/etc/selinux/$(PKG_NAME)/contexts/files/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/file_contexts.subs_dist $(1)/etc/selinux/$(PKG_NAME)/contexts/files/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/seusers $(1)/etc/selinux/$(PKG_NAME)/
	$(INSTALL_DATA) ./files/selinux-config $(1)/etc/selinux/config
endef

$(eval $(call BuildPackage,selinux-policy))
