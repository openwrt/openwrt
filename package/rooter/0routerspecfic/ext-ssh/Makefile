#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-ssh
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-ssh
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  DEPENDS:=+openssh-client +openssh-client-utils +openssh-keygen +openssh-moduli +openssh-server 
  TITLE:=Install scripts for SSH
  PKGARCH:=all
endef

define Package/ext-ssh/description
  Helper scripts to install scripts for SSH
endef


define Build/Compile
endef

define Package/ext-ssh/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,ext-ssh))
