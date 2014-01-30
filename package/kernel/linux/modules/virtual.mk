#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

VIRTUAL_MENU:=Virtualization Support

define KernelPackage/virtio-balloon
  SUBMENU:=$(VIRTUAL_MENU)
  TITLE:=VirtIO balloon driver
  DEPENDS:=@TARGET_x86_kvm_guest
  KCONFIG:=CONFIG_VIRTIO_BALLOON
  FILES:=$(LINUX_DIR)/drivers/virtio/virtio_balloon.ko
  AUTOLOAD:=$(call AutoLoad,06,virtio-balloon)
endef

define KernelPackage/virtio-balloon/description
 Kernel module for VirtIO memory ballooning support
endef

$(eval $(call KernelPackage,virtio-balloon))


define KernelPackage/virtio-net
  SUBMENU:=$(VIRTUAL_MENU)
  TITLE:=VirtIO network driver
  DEPENDS:=@TARGET_x86_kvm_guest
  KCONFIG:=CONFIG_VIRTIO_NET
  FILES:=$(LINUX_DIR)/drivers/net/virtio_net.ko
  AUTOLOAD:=$(call AutoLoad,50,virtio_net)
endef

define KernelPackage/virtio-net/description
 Kernel module for the VirtIO paravirtualized network device
endef

$(eval $(call KernelPackage,virtio-net))


define KernelPackage/virtio-random
  SUBMENU:=$(VIRTUAL_MENU)
  TITLE:=VirtIO Random Number Generator support
  DEPENDS:=@TARGET_x86_kvm_guest
  KCONFIG:=CONFIG_HW_RANDOM_VIRTIO
  FILES:=$(LINUX_DIR)/drivers/char/hw_random/virtio-rng.ko
  AUTOLOAD:=$(call AutoLoad,09,virtio-rng)
endef

define KernelPackage/virtio-random/description
 Kernel module for the VirtIO Random Number Generator
endef

$(eval $(call KernelPackage,virtio-random))


define KernelPackage/xen-privcmd
  SUBMENU:=$(VIRTUAL_MENU)
  TITLE:=Xen private commands
  DEPENDS:=@TARGET_x86_xen_domu
  KCONFIG:=CONFIG_XEN_PRIVCMD
  FILES:=$(LINUX_DIR)/drivers/xen/xen-privcmd.ko
  AUTOLOAD:=$(call AutoLoad,04,xen-privcmd)
endef

define KernelPackage/xen-privcmd/description
 Kernel module for Xen private commands
endef

$(eval $(call KernelPackage,xen-privcmd))


define KernelPackage/xen-fs
  SUBMENU:=$(VIRTUAL_MENU)
  TITLE:=Xen filesystem
  DEPENDS:=@TARGET_x86_xen_domu +kmod-xen-privcmd
  KCONFIG:= \
  	CONFIG_XENFS \
  	CONFIG_XEN_COMPAT_XENFS=y
  FILES:=$(LINUX_DIR)/drivers/xen/xenfs/xenfs.ko
  AUTOLOAD:=$(call AutoLoad,05,xenfs)
endef

define KernelPackage/xen-fs/description
 Kernel module for the Xen filesystem
endef

$(eval $(call KernelPackage,xen-fs))


define KernelPackage/xen-evtchn
  SUBMENU:=$(VIRTUAL_MENU)
  TITLE:=Xen event channels
  DEPENDS:=@TARGET_x86_xen_domu
  KCONFIG:=CONFIG_XEN_DEV_EVTCHN
  FILES:=$(LINUX_DIR)/drivers/xen/xen-evtchn.ko
  AUTOLOAD:=$(call AutoLoad,06,xen-evtchn)
endef

define KernelPackage/xen-evtchn/description
 Kernel module for the /dev/xen/evtchn device
endef

$(eval $(call KernelPackage,xen-evtchn))

define KernelPackage/xen-fbdev
  SUBMENU:=$(VIRTUAL_MENU)
  TITLE:=Xen virtual frame buffer
  DEPENDS:=@TARGET_x86_xen_domu +kmod-fb
  KCONFIG:= \
  	CONFIG_XEN_FBDEV_FRONTEND \
  	CONFIG_FB_DEFERRED_IO=y \
  	CONFIG_FB_SYS_COPYAREA \
  	CONFIG_FB_SYS_FILLRECT \
  	CONFIG_FB_SYS_FOPS \
  	CONFIG_FB_SYS_IMAGEBLIT \
  	CONFIG_FIRMWARE_EDID=n
  FILES:= \
  	$(LINUX_DIR)/drivers/video/xen-fbfront.ko \
  	$(LINUX_DIR)/drivers/video/syscopyarea.ko \
  	$(LINUX_DIR)/drivers/video/sysfillrect.ko \
  	$(LINUX_DIR)/drivers/video/fb_sys_fops.ko \
  	$(LINUX_DIR)/drivers/video/sysimgblt.ko
  AUTOLOAD:=$(call AutoLoad,07, \
  	fb \
  	syscopyarea \
  	sysfillrect \
  	fb_sys_fops \
  	sysimgblt \
  	xen-fbfront \
  )
endef

define KernelPackage/xen-fbdev/description
 Kernel module for the Xen virtual frame buffer
endef

$(eval $(call KernelPackage,xen-fbdev))


define KernelPackage/xen-kbddev
  SUBMENU:=$(VIRTUAL_MENU)
  TITLE:=Xen virtual keyboard and mouse
  DEPENDS:=@TARGET_x86_xen_domu
  KCONFIG:=CONFIG_XEN_KBDDEV_FRONTEND
  FILES:=$(LINUX_DIR)/drivers/input/xen-kbdfront.ko
  AUTOLOAD:=$(call AutoLoad,08,xen-kbdfront)
endef

define KernelPackage/xen-kbddev/description
 Kernel module for the Xen virtual keyboard and mouse
endef

$(eval $(call KernelPackage,xen-kbddev))


define KernelPackage/xen-netdev
  SUBMENU:=$(VIRTUAL_MENU)
  TITLE:=Xen network device frontend
  DEPENDS:=@TARGET_x86_xen_domu
  KCONFIG:=CONFIG_XEN_NETDEV_FRONTEND
  FILES:=$(LINUX_DIR)/drivers/net/xen-netfront.ko
  AUTOLOAD:=$(call AutoLoad,09,xen-netfront)
endef

define KernelPackage/xen-netdev/description
 Kernel module for the Xen network device frontend
endef

$(eval $(call KernelPackage,xen-netdev))


define KernelPackage/xen-pcidev
  SUBMENU:=$(VIRTUAL_MENU)
  TITLE:=Xen PCI device frontend
  DEPENDS:=@TARGET_x86_xen_domu
  KCONFIG:=CONFIG_XEN_PCIDEV_FRONTEND
  FILES:=$(LINUX_DIR)/drivers/pci/xen-pcifront.ko
  AUTOLOAD:=$(call AutoLoad,10,xen-pcifront)
endef

define KernelPackage/xen-pcidev/description
 Kernel module for the Xen network device frontend
endef

$(eval $(call KernelPackage,xen-pcidev))
