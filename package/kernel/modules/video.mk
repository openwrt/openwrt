#
# Copyright (C) 2009 David Cooper <dave@kupesoft.com>
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

VIDEO_MENU:=Video Support

define KernelPackage/video-core
  SUBMENU:=$(VIDEO_MENU)
  TITLE=Video4Linux support
  DEPENDS:=@PCI_SUPPORT||USB_SUPPORT +!TARGET_etrax:kmod-i2c-core
  KCONFIG:= \
	CONFIG_MEDIA_SUPPORT=m \
	CONFIG_VIDEO_DEV \
	CONFIG_VIDEO_V4L1=y \
	CONFIG_VIDEO_ALLOW_V4L1=y \
	CONFIG_VIDEO_CAPTURE_DRIVERS=y \
	CONFIG_V4L_USB_DRIVERS=y
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.38)),1)
  FILES:= \
	$(LINUX_DIR)/drivers/media/video/v4l2-common.ko \
	$(LINUX_DIR)/drivers/media/video/videodev.ko
  AUTOLOAD:=$(call AutoLoad,60, videodev v4l2-common)
else
  FILES:= \
	$(LINUX_DIR)/drivers/media/video/v4l2-common.ko \
	$(LINUX_DIR)/drivers/media/video/v4l1-compat.ko \
	$(LINUX_DIR)/drivers/media/video/videodev.ko
  AUTOLOAD:=$(call AutoLoad,60, v4l1-compat videodev v4l2-common)
endif
endef

define KernelPackage/video-core/description
 Kernel modules for Video4Linux support
endef

$(eval $(call KernelPackage,video-core))


define AddDepends/video
  SUBMENU:=$(VIDEO_MENU)
  DEPENDS+=kmod-video-core $(1)
endef


define KernelPackage/video-cpia2
  TITLE:=CPIA2 video driver
  DEPENDS:=@USB_SUPPORT +kmod-usb-core
  KCONFIG:=CONFIG_VIDEO_CPIA2
  FILES:=$(LINUX_DIR)/drivers/media/video/cpia2/cpia2.ko
  AUTOLOAD:=$(call AutoLoad,70,cpia2)
  $(call AddDepends/video)
endef

define KernelPackage/video-cpia2/description
 Kernel modules for supporting CPIA2 USB based cameras.
endef

$(eval $(call KernelPackage,video-cpia2))


define KernelPackage/video-konica
  TITLE:=Konica USB webcam support
  DEPENDS:=@USB_SUPPORT +kmod-usb-core
  KCONFIG:=CONFIG_USB_KONICAWC
  FILES:=$(LINUX_DIR)/drivers/media/video/usbvideo/konicawc.ko
  AUTOLOAD:=$(call AutoLoad,70,konicawc)
  $(call AddDepends/video)
endef

define KernelPackage/video-konica/description
 Kernel support for webcams based on a Konica chipset. This is known to 
 work with the Intel YC76 webcam.
endef

$(eval $(call KernelPackage,video-konica))


define KernelPackage/video-ov511
  TITLE:=OV511 USB webcam support
  DEPENDS:=@USB_SUPPORT +kmod-usb-core
  KCONFIG:=CONFIG_USB_OV511
  FILES:=$(LINUX_DIR)/drivers/media/video/ov511.ko
  AUTOLOAD:=$(call AutoLoad,70,ov511)
  $(call AddDepends/video)
endef


define KernelPackage/video-ov511/description
 Kernel modules for supporting OmniVision OV511 USB webcams.
endef

$(eval $(call KernelPackage,video-ov511))


define KernelPackage/video-ovcamchip
  TITLE:=OV6xxx/OV7xxx Camera Chip support
  DEPENDS:=@USB_SUPPORT +kmod-usb-core
  KCONFIG:=CONFIG_VIDEO_OVCAMCHIP
  FILES:=$(LINUX_DIR)/drivers/media/video/ovcamchip/ovcamchip.ko
  AUTOLOAD:=$(call AutoLoad,70,ovcamchip)
  $(call AddDepends/video)
endef


define KernelPackage/video-ovcamchip/description
 Kernel modules for supporting OmniVision OV6xxx and OV7xxx series of 
 camera chips.
endef

$(eval $(call KernelPackage,video-ovcamchip))


define KernelPackage/video-sn9c102
  TITLE:=SN9C102 Camera Chip support
  DEPENDS:=@USB_SUPPORT +kmod-usb-core
  KCONFIG:=CONFIG_USB_SN9C102
  FILES:=$(LINUX_DIR)/drivers/media/video/sn9c102/sn9c102.ko
  AUTOLOAD:=$(call AutoLoad,70,gspca_sn9c20x)
  $(call AddDepends/video)
endef


define KernelPackage/video-sn9c102/description
 Kernel modules for supporting SN9C102
 camera chips.
endef

$(eval $(call KernelPackage,video-sn9c102))


define KernelPackage/video-pwc
  TITLE:=Philips USB webcam support
  DEPENDS:=@USB_SUPPORT +kmod-usb-core
  KCONFIG:= \
	CONFIG_USB_PWC \
	CONFIG_USB_PWC_DEBUG=n
  FILES:=$(LINUX_DIR)/drivers/media/video/pwc/pwc.ko
  AUTOLOAD:=$(call AutoLoad,70,pwc)
  $(call AddDepends/video)
endef


define KernelPackage/video-pwc/description
 Kernel modules for supporting Philips USB based cameras.
endef

$(eval $(call KernelPackage,video-pwc))

define KernelPackage/video-uvc
  TITLE:=USB Video Class (UVC) support
  DEPENDS:=@USB_SUPPORT +kmod-usb-core
  KCONFIG:= CONFIG_USB_VIDEO_CLASS
  FILES:=$(LINUX_DIR)/drivers/media/video/uvc/uvcvideo.ko
  AUTOLOAD:=$(call AutoLoad,90,uvcvideo)
  $(call AddDepends/video)
  $(call AddDepends/input)
endef


define KernelPackage/video-uvc/description
 Kernel modules for supporting USB Video Class (UVC) devices.
endef

$(eval $(call KernelPackage,video-uvc))


define KernelPackage/video-gspca-core
  MENU:=1
  TITLE:=GSPCA webcam core support framework
  DEPENDS:=@USB_SUPPORT +kmod-usb-core
  KCONFIG:=CONFIG_USB_GSPCA
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_main.ko
  AUTOLOAD:=$(call AutoLoad,70,gspca_main)
  $(call AddDepends/video)
endef

define KernelPackage/video-gspca-core/description
 Kernel modules for supporting GSPCA based webcam devices. Note this is just
 the core of the driver, please select a submodule that supports your webcam.
endef

$(eval $(call KernelPackage,video-gspca-core))


define AddDepends/video-gspca
  SUBMENU:=$(VIDEO_MENU)
  DEPENDS+=kmod-video-gspca-core $(1)
endef


define KernelPackage/video-gspca-conex
  TITLE:=conex webcam support
  KCONFIG:=CONFIG_USB_GSPCA_CONEX
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_conex.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_conex)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-conex/description
 The Conexant Camera Driver (conex) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-conex))


define KernelPackage/video-gspca-etoms
  TITLE:=etoms webcam support
  KCONFIG:=CONFIG_USB_GSPCA_ETOMS
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_etoms.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_etoms)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-etoms/description
 The Etoms USB Camera Driver (etoms) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-etoms))


define KernelPackage/video-gspca-finepix
  TITLE:=finepix webcam support
  KCONFIG:=CONFIG_USB_GSPCA_FINEPIX
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_finepix.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_finepix)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-finepix/description
 The Fujifilm FinePix USB V4L2 driver (finepix) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-finepix))


define KernelPackage/video-gspca-mars
  TITLE:=mars webcam support
  KCONFIG:=CONFIG_USB_GSPCA_MARS
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_mars.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_mars)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-mars/description
 The Mars USB Camera Driver (mars) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-mars))


define KernelPackage/video-gspca-mr97310a
  TITLE:=mr97310a webcam support
  KCONFIG:=CONFIG_USB_GSPCA_MR97310A
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_mr97310a.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_mr97310a)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-mr97310a/description
 The Mars-Semi MR97310A USB Camera Driver (mr97310a) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-mr97310a))


define KernelPackage/video-gspca-ov519
  TITLE:=ov519 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_OV519
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_ov519.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_ov519)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-ov519/description
 The OV519 USB Camera Driver (ov519) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-ov519))


define KernelPackage/video-gspca-ov534
  TITLE:=ov534 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_OV534
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_ov534.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_ov534)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-ov534/description
 The OV534 USB Camera Driver (ov534) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-ov534))


define KernelPackage/video-gspca-pac207
  TITLE:=pac207 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_PAC207
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_pac207.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_pac207)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-pac207/description
 The Pixart PAC207 USB Camera Driver (pac207) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-pac207))


define KernelPackage/video-gspca-pac7311
  TITLE:=pac7311 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_PAC7311
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_pac7311.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_pac7311)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-pac7311/description
 The Pixart PAC7311 USB Camera Driver (pac7311) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-pac7311))


define KernelPackage/video-gspca-sn9c20x
  TITLE:=sn9c20x webcam support
  KCONFIG:=CONFIG_USB_GSPCA_SN9C20X
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_sn9c20x.ko
  AUTOLOAD:=$(call AutoLoad,75,sn9c20x)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-sn9c20x/description
 The SN9C20X USB Camera Driver (sn9c20x) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-sn9c20x))


define KernelPackage/video-gspca-sonixb
  TITLE:=sonixb webcam support
  KCONFIG:=CONFIG_USB_GSPCA_SONIXB
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_sonixb.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_sonixb)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-sonixb/description
 The SONIX Bayer USB Camera Driver (sonixb) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-sonixb))


define KernelPackage/video-gspca-sonixj
  TITLE:=sonixj webcam support
  KCONFIG:=CONFIG_USB_GSPCA_SONIXJ
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_sonixj.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_sonixj)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-sonixj/description
 The SONIX JPEG USB Camera Driver (sonixj) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-sonixj))


define KernelPackage/video-gspca-spca500
  TITLE:=spca500 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_SPCA500
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_spca500.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_spca500)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-spca500/description
 The SPCA500 USB Camera Driver (spca500) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-spca500))


define KernelPackage/video-gspca-spca501
  TITLE:=spca501 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_SPCA501
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_spca501.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_spca501)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-spca501/description
 The SPCA501 USB Camera Driver (spca501) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-spca501))


define KernelPackage/video-gspca-spca505
  TITLE:=spca505 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_SPCA505
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_spca505.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_spca505)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-spca505/description
 The SPCA505 USB Camera Driver (spca505) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-spca505))


define KernelPackage/video-gspca-spca506
  TITLE:=spca506 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_SPCA506
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_spca506.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_spca506)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-spca506/description
 The SPCA506 USB Camera Driver (spca506) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-spca506))


define KernelPackage/video-gspca-spca508
  TITLE:=spca508 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_SPCA508
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_spca508.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_spca508)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-spca508/description
 The SPCA508 USB Camera Driver (spca508) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-spca508))


define KernelPackage/video-gspca-spca561
  TITLE:=spca561 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_SPCA561
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_spca561.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_spca561)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-spca561/description
 The SPCA561 USB Camera Driver (spca561) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-spca561))


define KernelPackage/video-gspca-sq905
  TITLE:=sq905 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_SQ905
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_sq905.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_sq905)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-sq905/description
 The SQ Technologies SQ905 based USB Camera Driver (sq905) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-sq905))


define KernelPackage/video-gspca-sq905c
  TITLE:=sq905c webcam support
  KCONFIG:=CONFIG_USB_GSPCA_SQ905C
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_sq905c.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_sq905c)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-sq905c/description
 The SQ Technologies SQ905C based USB Camera Driver (sq905c) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-sq905c))


define KernelPackage/video-gspca-stk014
  TITLE:=stk014 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_STK014
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_stk014.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_stk014)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-stk014/description
 The Syntek DV4000 (STK014) USB Camera Driver (stk014) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-stk014))


define KernelPackage/video-gspca-sunplus
  TITLE:=sunplus webcam support
  KCONFIG:=CONFIG_USB_GSPCA_SUNPLUS
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_sunplus.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_sunplus)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-sunplus/description
 The SUNPLUS USB Camera Driver (sunplus) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-sunplus))


define KernelPackage/video-gspca-t613
  TITLE:=t613 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_T613
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_t613.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_t613)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-t613/description
 The T613 (JPEG Compliance) USB Camera Driver (t613) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-t613))


define KernelPackage/video-gspca-tv8532
  TITLE:=tv8532 webcam support
  KCONFIG:=CONFIG_USB_GSPCA_TV8532
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_tv8532.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_tv8532)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-tv8532/description
 The TV8532 USB Camera Driver (tv8532) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-tv8532))


define KernelPackage/video-gspca-vc032x
  TITLE:=vc032x webcam support
  KCONFIG:=CONFIG_USB_GSPCA_VC032X
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_vc032x.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_vc032x)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-vc032x/description
 The VC032X USB Camera Driver (vc032x) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-vc032x))


define KernelPackage/video-gspca-zc3xx
  TITLE:=zc3xx webcam support
  KCONFIG:=CONFIG_USB_GSPCA_ZC3XX
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_zc3xx.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_zc3xx)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-zc3xx/description
 The ZC3XX USB Camera Driver (zc3xx) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-zc3xx))


define KernelPackage/video-gspca-m5602
  TITLE:=m5602 webcam support
  KCONFIG:=CONFIG_USB_M5602
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/m5602/gspca_m5602.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_m5602)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-m5602/description
 The ALi USB m5602 Camera Driver (m5602) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-m5602))


define KernelPackage/video-gspca-stv06xx
  TITLE:=stv06xx webcam support
  KCONFIG:=CONFIG_USB_STV06XX
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/stv06xx/gspca_stv06xx.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_stv06xx)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-stv06xx/description
 The STV06XX USB Camera Driver (stv06xx) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-stv06xx))


define KernelPackage/video-gspca-gl860
  TITLE:=gl860 webcam support
  DEPENDS:=@LINUX_2_6_32
  KCONFIG:=CONFIG_USB_GL860
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gl860/gspca_gl860.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_gl860)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-gl800/description
 The GL860 USB Camera Driver (gl860) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-gl860))


define KernelPackage/video-gspca-jeilinj
  TITLE:=jeilinj webcam support
  DEPENDS:=@LINUX_2_6_32
  KCONFIG:=CONFIG_USB_GSPCA_JEILINJ
  FILES:=$(LINUX_DIR)/drivers/media/video/gspca/gspca_jeilinj.ko
  AUTOLOAD:=$(call AutoLoad,75,gspca_jeilinj)
  $(call AddDepends/video-gspca)
endef

define KernelPackage/video-gspca-jeilinj/description
 The JEILINJ USB Camera Driver (jeilinj) kernel module.
endef

$(eval $(call KernelPackage,video-gspca-jeilinj))
