include $(TOPDIR)/rules.mk

PKG_NAME:=gnulib-l10n
PKG_CPE_ID:=cpe:/a:gnu:$(PKG_NAME)
PKG_VERSION:=20241231

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/gnulib
PKG_HASH:=155752ba6796aa294cde87c2bf0e771a5891eeeacb131345be0e080a3ec0ceea

PKG_GPG_KEYS_DIR:=$(CURDIR)/keys
PKG_SOURCE_SIG:=$(PKG_SOURCE).sig
PKG_VALIDPGPKEYS:=9001B85AF9E1B83DF1BDA942F5BE8B267C6A406D

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/gnulib-l10n
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=GNU Gnulib localization translation files
  URL:=https://www.gnu.org/software/gnulib/
endef

define Package/gnulib-l10n/description
 Localizations (translations) of messages for GNU gnulib code.
endef

HOST_CONFIGURE_ARGS += \
	--localedir=$(HOST_BUILD_PREFIX)/share/locale

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

define Package/gnulib-l10n/install
	$(call Build/Install/Default,install SUBDIRS='$(PKG_SUBDIRS)')
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,gnulib-l10n))
