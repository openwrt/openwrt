include $(TOPDIR)/rules.mk

PKG_NAME:=trace-cmd
PKG_VERSION:=v2.6
PKG_RELEASE=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=\
		https://kernel.googlesource.com/pub/scm/linux/kernel/git/rostedt/trace-cmd \
		https://git.kernel.org/pub/scm/linux/kernel/git/rostedt/trace-cmd.git
PKG_SOURCE_VERSION:=9be5d74805830a291615f2f34a27c903f6a37b1e
PKG_MIRROR_HASH:=735b69f61a8c627037dcc01361cdb8415e5ab0ec892fbd731236c444003b0c71
PKG_INSTALL:=1
PKG_USE_MIPS16:=0
PKG_LICENSE:=GPL-2.0

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
