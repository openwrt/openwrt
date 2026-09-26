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

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

$(eval $(call BuildPackage,audio-support))
$(eval $(call BuildPackage,video-support))
$(eval $(call BuildPackage,accel-support))
