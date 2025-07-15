#
# Copyright (C) 2016 Yousong Zhou <yszhou4tech@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
define KernelPackage/irqbypass
  SUBMENU:=Virtualization
  TITLE:=IRQ offload/bypass manager
  KCONFIG:=CONFIG_IRQ_BYPASS_MANAGER
  HIDDEN:=1
  FILES:= $(LINUX_DIR)/virt/lib/irqbypass.ko
  AUTOLOAD:=$(call AutoProbe,irqbypass.ko)
endef
$(eval $(call KernelPackage,irqbypass))


define KernelPackage/kvm-x86
  SUBMENU:=Virtualization
  TITLE:=Kernel-based Virtual Machine (KVM) support
  DEPENDS:=@TARGET_x86_generic||TARGET_x86_64 +kmod-irqbypass
  KCONFIG:=\
	  CONFIG_KVM \
	  CONFIG_KVM_MMU_AUDIT=n \
	  CONFIG_KVM_SMM=y \
	  CONFIG_VIRTUALIZATION=y
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
  DEPENDS:=+kmod-kvm-x86
  KCONFIG:=CONFIG_KVM_INTEL
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
  DEPENDS:=+kmod-kvm-x86
  KCONFIG:=CONFIG_KVM_AMD
  FILES:= $(LINUX_DIR)/arch/$(LINUX_KARCH)/kvm/kvm-amd.ko
  AUTOLOAD:=$(call AutoProbe,kvm-amd.ko)
endef

define KernelPackage/kvm-amd/description
  Provides support for KVM on AMD processors equipped with the AMD-V
  (SVM) extensions.
endef

$(eval $(call KernelPackage,kvm-amd))


define KernelPackage/vfio
  SUBMENU:=Virtualization
  TITLE:=VFIO Non-Privileged userspace driver framework
  DEPENDS:=@TARGET_x86_64||TARGET_armsr_armv8||TARGET_mediatek_filogic
  KCONFIG:= \
	CONFIG_VFIO \
	CONFIG_VFIO_NOIOMMU=n \
	CONFIG_VFIO_MDEV=n
  FILES:= \
	$(LINUX_DIR)/drivers/vfio/vfio.ko \
	$(LINUX_DIR)/drivers/vfio/vfio_iommu_type1.ko
  AUTOLOAD:=$(call AutoProbe,vfio vfio_iommu_type1)
endef

define KernelPackage/vfio/description
  VFIO provides a framework for secure userspace device drivers.
endef

$(eval $(call KernelPackage,vfio))


define KernelPackage/vfio-pci
  SUBMENU:=Virtualization
  TITLE:=Generic VFIO support for any PCI device
  DEPENDS:=@TARGET_x86_64||TARGET_armsr_armv8 @PCI_SUPPORT +kmod-vfio +kmod-irqbypass
  KCONFIG:= \
	CONFIG_VFIO_PCI \
	CONFIG_VFIO_PCI_IGD=n
  FILES:= \
	$(LINUX_DIR)/drivers/vfio/pci/vfio-pci-core.ko \
	$(LINUX_DIR)/drivers/vfio/pci/vfio-pci.ko
  AUTOLOAD:=$(call AutoProbe,vfio-pci)
endef

define KernelPackage/vfio-pci/description
  Support for the generic PCI VFIO bus driver which can connect any PCI
  device to the VFIO framework.
endef

$(eval $(call KernelPackage,vfio-pci))


define KernelPackage/vhost
  SUBMENU:=Virtualization
  TITLE:=Host kernel accelerator for virtio (base)
  KCONFIG:=CONFIG_VHOST
  FILES:=$(LINUX_DIR)/drivers/vhost/vhost.ko \
    $(LINUX_DIR)/drivers/vhost/vhost_iotlb.ko
  AUTOLOAD:=$(call AutoProbe,vhost vhost_iotlb)
endef

$(eval $(call KernelPackage,vhost))


define KernelPackage/vhost-net
  SUBMENU:=Virtualization
  TITLE:=Host kernel accelerator for virtio-net
  DEPENDS:=+kmod-tun +kmod-vhost
  KCONFIG:=CONFIG_VHOST_NET
  FILES:=$(LINUX_DIR)/drivers/vhost/vhost_net.ko
  AUTOLOAD:=$(call AutoProbe,vhost_net)
endef

$(eval $(call KernelPackage,vhost-net))
