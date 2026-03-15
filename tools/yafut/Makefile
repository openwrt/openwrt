include $(TOPDIR)/rules.mk

PKG_NAME:=yafut
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/kempniu/yafut.git
PKG_MIRROR_HASH:=f5c76edc81477d2b68a7b032487d10fa361c8eaeecfc36908fde2ae828b7e822
PKG_SOURCE_DATE:=2025-02-18
PKG_SOURCE_VERSION:=e342c93981dc255bba58c17925b5c8983d7dacf8

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_INSTALL:=1

$(eval $(call HostBuild))
