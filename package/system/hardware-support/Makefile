# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org

include $(TOPDIR)/rules.mk

PKG_NAME:=hardware-support
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0-only
PKG_MAINTAINER:=Daniel Golle <daniel@makrotopia.org>

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/hardware-support/Default
  SECTION:=hardware
  CATEGORY:=Hardware support
  DEPENDS:=+procd
  DEFAULT:=m if ALL||ALL_KMODS
  PKGARCH:=all
endef

# 1: destination, 2: fragment file name
define InstallFragment
	$(INSTALL_DIR) $(1)/etc/hotplug.json.d
	$(INSTALL_CONF) ./files/$(2) $(1)/etc/hotplug.json.d/
endef

define Package/audio-support
  $(call Package/hardware-support/Default)
  TITLE:=Audio device support
  USERID:=:audio=29 :audio-capture=32 :midi=55
endef

define Package/audio-support/description
 Feature gate for audio devices. Allocates the audio, audio-capture and
 midi groups and installs the hotplug rules which put the ALSA character
 devices into them. Kernel modules exposing audio devices and userspace
 using those groups depend on this package.
endef

define Package/audio-support/install
	$(call InstallFragment,$(1),20-audio.json)
endef

define Package/video-support
  $(call Package/hardware-support/Default)
  TITLE:=Video device support
  USERID:=:video=40 :video-capture=36
endef

define Package/video-support/description
 Feature gate for display and video capture devices. Allocates the video
 and video-capture groups and installs the hotplug rules which put the DRM
 and V4L2 character devices into them. Kernel modules exposing such devices
 and userspace using those groups depend on this package.
endef

define Package/video-support/install
	$(call InstallFragment,$(1),30-video.json)
endef

define Package/accel-support
  $(call Package/hardware-support/Default)
  TITLE:=Render and compute device support
  USERID:=:render=43
endef

define Package/accel-support/description
 Feature gate for GPU render and compute devices. Allocates the render
 group and installs the hotplug rules which put the DRM render, accel and
 kfd character devices into it.
endef

define Package/accel-support/install
	$(call InstallFragment,$(1),35-accel.json)
endef

define Package/input-support
  $(call Package/hardware-support/Default)
  TITLE:=Input device support
  USERID:=:input=41
endef

define Package/input-support/description
 Feature gate for input devices. Allocates the input group and installs
 the hotplug rules which put the evdev character devices into it. Kernel
 modules exposing input devices and userspace using the group depend on
 this package.
endef

define Package/input-support/install
	$(call InstallFragment,$(1),40-input.json)
endef

define Package/dvb-support
  $(call Package/hardware-support/Default)
  TITLE:=DVB device support
  USERID:=:dvb=49
endef

define Package/dvb-support/description
 Feature gate for DVB devices. Allocates the dvb group and installs the
 hotplug rules which put the DVB adapter character devices into it.
endef

define Package/dvb-support/install
	$(call InstallFragment,$(1),45-dvb.json)
endef

define Package/kvm-support
  $(call Package/hardware-support/Default)
  TITLE:=KVM device support
  USERID:=:kvm=34
endef

define Package/kvm-support/description
 Feature gate for hardware virtualisation. Allocates the kvm group and
 installs the hotplug rule which puts /dev/kvm into it.
endef

define Package/kvm-support/install
	$(call InstallFragment,$(1),50-kvm.json)
endef

define Package/printer-support
  $(call Package/hardware-support/Default)
  TITLE:=Printer device support
  USERID:=:lp=7
endef

define Package/printer-support/description
 Feature gate for printers. Allocates the lp group and installs the
 hotplug rules which put the USB and parallel port printer character
 devices into it.
endef

define Package/printer-support/install
	$(call InstallFragment,$(1),55-printer.json)
endef

define Package/scanner-support
  $(call Package/hardware-support/Default)
  TITLE:=Scanner device support
  USERID:=:scanner=47
endef

define Package/scanner-support/description
 Feature gate for scanners. Allocates the scanner group, so that the id
 is the same on every image and one package owns it. Ships no hotplug
 rule: a USB scanner is matched by vendor and product id against a
 database of supported devices, which only the driver package has, so
 the rule granting access to a particular scanner stays there.
endef

define Package/scanner-support/install
	$(INSTALL_DIR) $(1)
endef

define Package/hidraw-support
  $(call Package/hardware-support/Default)
  TITLE:=Raw HID device support
  USERID:=:hidraw=48
endef

define Package/hidraw-support/description
 Feature gate for raw HID devices. Allocates the hidraw group and
 installs the hotplug rules which put the hidraw character devices into
 it. These are the HID devices a driver hands to userspace whole, such
 as a UPS, a relay board or a programmable keypad, as opposed to the
 evdev nodes covered by input-support.
endef

define Package/hidraw-support/install
	$(call InstallFragment,$(1),60-hidraw.json)
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

$(eval $(call BuildPackage,audio-support))
$(eval $(call BuildPackage,video-support))
$(eval $(call BuildPackage,accel-support))
$(eval $(call BuildPackage,input-support))
$(eval $(call BuildPackage,dvb-support))
$(eval $(call BuildPackage,kvm-support))
$(eval $(call BuildPackage,printer-support))
$(eval $(call BuildPackage,scanner-support))
$(eval $(call BuildPackage,hidraw-support))
