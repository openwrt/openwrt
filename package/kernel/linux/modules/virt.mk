#
# Copyright (C) 2016 Yousong Zhou <yszhou4tech@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
define KernelPackage/vhost_net
  SUBMENU:=Virtualization
  TITLE:=Host kernel accelerator for virtio net
  DEPENDS:=
  KCONFIG:=CONFIG_VHOST_NET
  FILES:= $(LINUX_DIR)/drivers/vhost/vhost_net.ko
  AUTOLOAD:=$(call AutoProbe,vhost_net.ko)
endef

define KernelPackage/vhost_net/description
  This kernel module can be loaded in host kernel to accelerate
  guest networking with virtio_net. Not to be confused with virtio_net
  module itself which needs to be loaded in guest kernel.
endef

$(eval $(call KernelPackage,vhost_net))


define KernelPackage/irqbypass
  SUBMENU:=Virtualization
  TITLE:=IRQ offload/bypass manager
  KCONFIG:=CONFIG_IRQ_BYPASS_MANAGER
  FILES:= $(LINUX_DIR)/virt/lib/irqbypass.ko
  AUTOLOAD:=$(call AutoProbe,irqbypass.ko)
endef

define KernelPackage/irqbypass/description
  Various virtualization hardware acceleration techniques allow bypassing or
  offloading interrupts received from devices around the host kernel.  Posted
  Interrupts on Intel VT-d systems can allow interrupts to be received
  directly by a virtual machine.  ARM IRQ Forwarding allows forwarded physical
  interrupts to be directly deactivated by the guest.  This manager allows
  interrupt producers and consumers to find each other to enable this sort of
  bypass.
endef

$(eval $(call KernelPackage,irqbypass))


define KernelPackage/kvm-x86
  SUBMENU:=Virtualization
  TITLE:=Kernel-based Virtual Machine (KVM) support
  DEPENDS:=@TARGET_x86 +kmod-irqbypass
  KCONFIG:=\
	  CONFIG_VIRTUALIZATION=y \
	  CONFIG_KVM \
	  CONFIG_VHOST_NET=n \
	  CONFIG_VHOST_CROSS_ENDIAN_LEGACY=n \
	  CONFIG_TASK_XACCT=n \

  FILES:= $(LINUX_DIR)/arch/$(LINUX_KARCH)/kvm/kvm.ko
  AUTOLOAD:=$(call AutoProbe,kvm.ko)
endef

define KernelPackage/kvm-x86/description
  Support hosting fully virtualized guest machines using hardware
  virtualization extensions.  You will need a fairly recent
  processor equipped with virtualization extensions. You will also
  need to select one or more of the processor modules.

  This module provides access to the hardware capabilities through
  a character device node named /dev/kvm.
endef

$(eval $(call KernelPackage,kvm-x86))


define KernelPackage/kvm-intel
  SUBMENU:=Virtualization
  TITLE:=KVM for Intel processors support
  DEPENDS:=@TARGET_x86 +kmod-kvm-x86
  KCONFIG:=CONFIG_KVM_INTEL CONFIG_KVM_AMD=n
  FILES:= $(LINUX_DIR)/arch/$(LINUX_KARCH)/kvm/kvm-intel.ko
  AUTOLOAD:=$(call AutoProbe,kvm-intel.ko)
endef

define KernelPackage/kvm-intel/description
  Provides support for KVM on Intel processors equipped with the VT
  extensions.
endef

$(eval $(call KernelPackage,kvm-intel))


define KernelPackage/kvm-amd
  SUBMENU:=Virtualization
  TITLE:=KVM for AMD processors support
  DEPENDS:=@TARGET_x86 +kmod-kvm-x86
  KCONFIG:=CONFIG_KVM_INTEL=n CONFIG_KVM_AMD
  FILES:= $(LINUX_DIR)/arch/$(LINUX_KARCH)/kvm/kvm-amd.ko
  AUTOLOAD:=$(call AutoProbe,kvm-amd.ko)
endef

define KernelPackage/kvm-amd/description
  Provides support for KVM on AMD processors equipped with the AMD-V
  (SVM) extensions.
endef

$(eval $(call KernelPackage,kvm-amd))
