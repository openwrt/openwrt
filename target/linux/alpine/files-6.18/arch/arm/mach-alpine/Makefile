# SPDX-License-Identifier: GPL-2.0-only

ccflags-$(CONFIG_ARCH_MULTIPLATFORM) := -I$(srctree)/$(src)/include

obj-y				+= alpine_machine.o
obj-y				+= al_hal_serdes.o
obj-y				+= al_fabric.o
obj-y				+= al_init_sys_fabric.o
obj-y				+= al_hal_iofic.o
obj-y				+= al_hal_udma_main.o
obj-y				+= al_hal_udma_iofic.o
obj-y				+= al_hal_udma_config.o
obj-y				+= al_hal_exports.o
obj-$(CONFIG_SMP)		+= platsmp.o alpine_cpu_pm.o
obj-$(CONFIG_PCI)		+= pcie_of.o
obj-$(CONFIG_PCI)		+= al_hal_pcie.o
obj-$(CONFIG_CPU_IDLE)		+= cpuidle-alpine.o
obj-$(CONFIG_HOTPLUG_CPU)	+= sleep-alpine.o
ifneq ($(CONFIG_HOTPLUG_CPU), y)
obj-$(CONFIG_CPU_IDLE)		+= sleep-alpine.o
endif
