#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

VIDEO_MENU:=Video Support

define KernelPackage/video-core
  SUBMENU:=$(VIDEO_MENU)
  TITLE=Video4Linux support
  DEPENDS:=@PCI_SUPPORT||USB_SUPPORT
  KCONFIG:= \
	CONFIG_VIDEO_DEV \
	CONFIG_VIDEO_V4L1=y \
	CONFIG_VIDEO_CAPTURE_DRIVERS=y \
	CONFIG_V4L_USB_DRIVERS=y 
endef

define KernelPackage/video-core/2.4
  FILES:=$(LINUX_DIR)/drivers/media/video/videodev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,videodev)
endef

ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.29)),1)
define KernelPackage/video-core/2.6
  FILES:= \
	$(LINUX_DIR)/drivers/media/video/v4l2-common.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/media/video/v4l1-compat.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/media/video/videodev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60, \
	v4l1-compat \
	videodev \
	v4l2-common \
  )
endef
else
define KernelPackage/video-core/2.6
  FILES:= \
	$(LINUX_DIR)/drivers/media/video/v4l2-common.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/media/video/v4l1-compat.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/media/video/compat_ioctl32.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/media/video/videodev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60, \
	v4l1-compat \
	videodev \
	v4l2-common \
	compat_ioctl32 \
  )
endef
endif

define KernelPackage/video-core/description
 Kernel modules for Video4Linux support
endef

$(eval $(call KernelPackage,video-core))


define KernelPackage/video-cpia2
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=CPIA2 video driver
  DEPENDS:=@LINUX_2_6 @USB_SUPPORT +kmod-usb-core +kmod-video-core
  KCONFIG:=CONFIG_VIDEO_CPIA2
  FILES:=$(LINUX_DIR)/drivers/media/video/cpia2/cpia2.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,cpia2)
endef

define KernelPackage/video-cpia2/description
 Kernel modules for supporting CPIA2 USB based cameras.
endef

$(eval $(call KernelPackage,video-cpia2))


define KernelPackage/video-konica
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=Konica USB webcam support
  DEPENDS:=@LINUX_2_6 @USB_SUPPORT +kmod-usb-video +kmod-video-core
  KCONFIG:=CONFIG_USB_KONICAWC
  FILES:=$(LINUX_DIR)/drivers/media/video/usbvideo/konicawc.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,konicawc)
endef

define KernelPackage/video-konica/description
 Kernel support for webcams based on a Konica chipset. This is known to 
 work with the Intel YC76 webcam.
endef

$(eval $(call KernelPackage,video-konica))


define KernelPackage/video-ov511
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=OV511 USB webcam support
  DEPENDS:=@LINUX_2_6 @USB_SUPPORT +kmod-usb-core +kmod-video-core
  KCONFIG:=CONFIG_VIDEO_OV511
  FILES:=$(LINUX_DIR)/drivers/media/video/ov511.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,ov511)
endef


define KernelPackage/video-ov511/description
 Kernel modules for supporting OmniVision OV511 USB webcams.
endef

$(eval $(call KernelPackage,video-ov511))


define KernelPackage/video-ovcamchip
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=OV6xxx/OV7xxx Camera Chip support
  DEPENDS:=@LINUX_2_6 @USB_SUPPORT +kmod-i2c-core +kmod-video-core
  KCONFIG:=CONFIG_VIDEO_OVCAMCHIP
  FILES:=$(LINUX_DIR)/drivers/media/video/ovcamchip/ovcamchip.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,ovcamchip)
endef


define KernelPackage/video-ovcamchip/description
 Kernel modules for supporting OmniVision OV6xxx and OV7xxx series of 
 camera chips.
endef

$(eval $(call KernelPackage,video-ovcamchip))


define KernelPackage/video-sn9c102
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=SN9C102 Camera Chip support
  DEPENDS:=@LINUX_2_6 @USB_SUPPORT +kmod-usb-core +kmod-video-core
  KCONFIG:=CONFIG_USB_SN9C102
  FILES:=$(LINUX_DIR)/drivers/media/video/sn9c102/sn9c102.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,sn9c102)
endef


define KernelPackage/video-sn9c102/description
 Kernel modules for supporting SN9C102
 camera chips.
endef

$(eval $(call KernelPackage,video-sn9c102))


define KernelPackage/video-pwc
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=Philips USB webcam support
  DEPENDS:=@LINUX_2_6 @USB_SUPPORT +kmod-usb-core +kmod-video-core
  KCONFIG:= \
	CONFIG_USB_PWC \
	CONFIG_USB_PWC_DEBUG=n
  FILES:=$(LINUX_DIR)/drivers/media/video/pwc/pwc.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,pwc)
endef


define KernelPackage/video-pwc/description
 Kernel modules for supporting Philips USB based cameras.
endef

$(eval $(call KernelPackage,video-pwc))

define KernelPackage/video-uvc
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=USB Video Class (UVC) support
  DEPENDS:=@LINUX_2_6 @!LINUX_2_6_25 @!LINUX_2_6_24 @!LINUX_2_6_23 @USB_SUPPORT +kmod-usb-core +kmod-video-core
  KCONFIG:= \
	CONFIG_USB_VIDEO_CLASS \
	CONFIG_USB_VIDEO_CLASS_INPUT_EVDEV=y
  FILES:=$(LINUX_DIR)/drivers/media/video/uvc/uvcvideo.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,uvcvideo)
endef


define KernelPackage/video-uvc/description
 Kernel modules for supporting USB Video Class (UVC) devices.
endef

$(eval $(call KernelPackage,video-uvc))
