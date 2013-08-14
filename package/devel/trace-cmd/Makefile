include $(TOPDIR)/rules.mk

PKG_NAME:=trace-cmd
PKG_VERSION:=v2.2.1
PKG_RELEASE=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://git.kernel.org/pub/scm/linux/kernel/git/rostedt/trace-cmd.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=8c10a774f1f8586cd8b0e3ce8666a376cb1d98e3
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
# PKG_MIRROR_MD5SUM:=6a08dfa2519a969d184e7bb5bb390620
PKG_INSTALL:=1
PKG_USE_MIPS16:=0

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/trace-cmd
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=Linux trace command line utility
  DEPENDS:=
endef

define Package/trace-cmd-extra
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=Extra plugins for trace-cmd
  DEPENDS:=
endef

MAKE_FLAGS += \
	NO_PYTHON=1 \
	prefix=/usr

PLUGINS_DIR := $(PKG_INSTALL_DIR)/usr/lib/trace-cmd/plugins
PLUGINS_MAIN := function hrtimer mac80211 sched_switch

TARGET_CFLAGS += --std=gnu99 -D_GNU_SOURCE

define Package/trace-cmd/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/usr/lib/trace-cmd/plugins
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/trace-cmd $(1)/usr/bin/
	$(CP) \
		$(patsubst %,$(PLUGINS_DIR)/plugin_%.so,$(PLUGINS_MAIN)) \
		$(1)/usr/lib/trace-cmd/plugins
endef

define Package/trace-cmd-extra/install
	$(INSTALL_DIR) $(1)/usr/lib/trace-cmd/plugins
	$(CP) \
		$$(patsubst %,$(PLUGINS_DIR)/plugin_%.so, \
			$$(filter-out $(PLUGINS_MAIN), \
				$$(patsubst $(PLUGINS_DIR)/plugin_%.so,%, \
					$$(wildcard $(PLUGINS_DIR)/plugin_*.so)))) \
		$(1)/usr/lib/trace-cmd/plugins
endef

$(eval $(call BuildPackage,trace-cmd))
$(eval $(call BuildPackage,trace-cmd-extra))
