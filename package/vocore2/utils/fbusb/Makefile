include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=fbusb
PKG_VERSION:=v4
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/fbusb
  TITLE:=USB framebuffer
  FILES:=$(PKG_BUILD_DIR)/fbusb.ko
  SECTION:=vocore2
  CATEGORY:=VoCore2
  AUTOLOAD:=$(call AutoLoad,01,fbusb)
  DEPENDS:=+kmod-usb-core +kmod-usb-hid
endef

define KernelPackage/fbusb/description
  USB framebuffer, /dev/fb0
endef

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)"

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) modules
endef

$(eval $(call KernelPackage,fbusb))
