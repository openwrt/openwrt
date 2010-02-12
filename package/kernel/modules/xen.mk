#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

XEN_MENU:=Xen paravirtualized guest support

define KernelPackage/xen-fs
  SUBMENU:=$(XEN_MENU)
  TITLE:=Xen filesystem
  DEPENDS:=@TARGET_x86_xen_domu
  KCONFIG:= \
  	CONFIG_XENFS \
  	CONFIG_XEN_COMPAT_XENFS=y
  FILES:=$(LINUX_DIR)/drivers/xen/xenfs/xenfs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,05,xenfs)
endef

define KernelPackage/xen-fs/description
  Kernel module for the Xen filesystem
endef

$(eval $(call KernelPackage,xen-fs))


define KernelPackage/xen-evtchn
  SUBMENU:=$(XEN_MENU)
  TITLE:=Xen event channels
  DEPENDS:=@TARGET_x86_xen_domu
  KCONFIG:=CONFIG_XEN_DEV_EVTCHN
  FILES:=$(LINUX_DIR)/drivers/xen/evtchn.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,06,evtchn)
endef

define KernelPackage/xen-evtchn/description
  Kernel module for the /dev/xen/evtchn device
endef

$(eval $(call KernelPackage,xen-evtchn))

# FIXME: the FB support should be moved to a separate kmod
define KernelPackage/xen-fbdev
  SUBMENU:=$(XEN_MENU)
  TITLE:=Xen virtual frame buffer
  DEPENDS:=@TARGET_x86_xen_domu
  KCONFIG:= \
  	CONFIG_XEN_FBDEV_FRONTEND \
  	CONFIG_FB \
  	CONFIG_FB_DEFERRED_IO=y \
  	CONFIG_FB_SYS_COPYAREA \
  	CONFIG_FB_SYS_FILLRECT \
  	CONFIG_FB_SYS_FOPS \
  	CONFIG_FB_SYS_IMAGEBLIT \
  	CONFIG_FIRMWARE_EDID=n
  FILES:= \
  	$(LINUX_DIR)/drivers/video/xen-fbfront.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/drivers/video/fb.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/drivers/video/syscopyarea.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/drivers/video/sysfillrect.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/drivers/video/fb_sys_fops.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/drivers/video/sysimgblt.$(LINUX_KMOD_SUFFIX)  	
  AUTOLOAD:=$(call AutoLoad,07, \
  	fb \
  	syscopyarea \
  	sysfillrect \
  	fb_sys_fops \
  	sysimgblt \
  	xen-fbfront \
  )
endef

define KernelPackage/xen-fs/description
  Kernel module for the Xen virtual frame buffer
endef

$(eval $(call KernelPackage,xen-fbdev))


define KernelPackage/xen-kbddev
  SUBMENU:=$(XEN_MENU)
  TITLE:=Xen virtual keyboard and mouse
  DEPENDS:=@TARGET_x86_xen_domu
  KCONFIG:=CONFIG_XEN_KBDDEV_FRONTEND
  FILES:=$(LINUX_DIR)/drivers/input/xen-kbdfront.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,08,xen-kbdfront)
endef

define KernelPackage/xen-kbddev/description
  Kernel module for the Xen virtual keyboard and mouse
endef

$(eval $(call KernelPackage,xen-kbddev))


define KernelPackage/xen-netdev
  SUBMENU:=$(XEN_MENU)
  TITLE:=Xen network device frontend
  DEPENDS:=@TARGET_x86_xen_domu
  KCONFIG:=CONFIG_XEN_NETDEV_FRONTEND
  FILES:=$(LINUX_DIR)/drivers/net/xen-netfront.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,xen-netfront)
endef

define KernelPackage/xen-netdev/description
  Kernel module for the Xen network device frontend
endef

$(eval $(call KernelPackage,xen-netdev))
