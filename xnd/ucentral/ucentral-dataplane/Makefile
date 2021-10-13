include $(TOPDIR)/rules.mk

PKG_NAME:=ucentral-dataplane
PKG_RELEASE:=1

PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define Package/ucentral-dataplane
  SECTION:=ucentral
  CATEGORY:=uCentral
  TITLE:=uCentral xBPF loader
  DEPENDS:=+tc-full +kmod-ifb @!TARGET_ipq807x
endef

define Package/ucentral-dataplane/description
	Allow loading cBPF and eBPF programs
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/ucentral-dataplane/install
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,ucentral-dataplane))
