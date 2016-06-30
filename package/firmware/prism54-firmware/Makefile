#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=prism54-firmware
PKG_RELEASE:=1

# Prism54 FullMAC firmware (jbnore.free.fr seems to be rather slow, so we use daemonizer.de)
PRG_URL:=https://daemonizer.de/prism54/prism54-fw/

include $(INCLUDE_DIR)/package.mk


# PRISM54/P54 firmwares
PRISM54FW:=1.0.4.3.arm
P54USBFW:=2.13.24.0.lm87.arm
P54PCIFW:=2.13.12.0.arm
P54SPIFW:=2.13.0.0.a.13.14.arm

define Download/prism54-firmware
  FILE:=$(PRISM54FW)
  URL:=$(PRG_URL)/fw-fullmac
  MD5SUM:=8bd4310971772a486b9784c77f8a6df9
endef
$(eval $(call Download,prism54-firmware))

define Download/p54-usb-firmware
  FILE:=$(P54USBFW)
  URL:=$(PRG_URL)/fw-usb
  MD5SUM:=8e8ab005a4f8f0123bcdc51bc25b47f6
endef
$(eval $(call Download,p54-usb-firmware))

define Download/p54-pci-firmware
  FILE:=$(P54PCIFW)
  URL:=$(PRG_URL)/fw-softmac
  MD5SUM:=ff7536af2092b1c4b21315bd103ef4c4
endef
$(eval $(call Download,p54-pci-firmware))

define Download/p54-spi-firmware
  FILE:=$(P54SPIFW)
  URL:=$(PRG_URL)/stlc4560
  MD5SUM:=42661f8ecbadd88012807493f596081d
endef
$(eval $(call Download,p54-spi-firmware))


define Package/prism54-firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://daemonizer.de/prism54/prism54-fw
endef

define Package/prism54-firmware
$(Package/prism54-firmware-default)
  TITLE:=prism54 firmware
endef

define Package/p54-usb-firmware
$(Package/prism54-firmware-default)
  TITLE:=p54-usb firmware
endef

define Package/p54-pci-firmware
$(Package/prism54-firmware-default)
  TITLE:=p54-pci firmware
endef

define Package/p54-spi-firmware
$(Package/prism54-firmware-default)
  TITLE:=p54-spi firmware
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile

endef

define Package/prism54-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(PRISM54FW) $(1)/lib/firmware/isl3890
endef

define Package/p54-usb-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(P54USBFW) $(1)/lib/firmware/isl3887usb
endef

define Package/p54-pci-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(P54PCIFW) $(1)/lib/firmware/isl3886pci
endef

define Package/p54-spi-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(P54SPIFW) $(1)/lib/firmware/3826.arm
endef


$(eval $(call BuildPackage,prism54-firmware))
$(eval $(call BuildPackage,p54-usb-firmware))
$(eval $(call BuildPackage,p54-pci-firmware))
$(eval $(call BuildPackage,p54-spi-firmware))
