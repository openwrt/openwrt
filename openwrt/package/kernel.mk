ifneq ($(DUMP),1)
include $(BUILD_DIR)/kernel.mk

KERNEL:=unknown
ifneq (,$(findstring 2.4.,$(LINUX_VERSION)))
KERNEL:=2.4
LINUX_KMOD_SUFFIX=o
endif
ifneq (,$(findstring 2.6.,$(LINUX_VERSION)))
KERNEL:=2.6
LINUX_KMOD_SUFFIX=ko
endif

LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/mipseb/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	-e 's/armeb/arm/' \
)

KERNEL_BUILD_DIR:=$(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)
ifeq ($(LINUX_NAME),)
LINUX_NAME:=linux-$(LINUX_VERSION)
endif
LINUX_DIR := $(KERNEL_BUILD_DIR)/$(LINUX_NAME)

KMOD_BUILD_DIR := $(KERNEL_BUILD_DIR)/linux-modules
MODULES_DIR := $(KERNEL_BUILD_DIR)/modules/$(MODULES_SUBDIR)
TARGET_MODULES_DIR := $(LINUX_TARGET_DIR)/$(MODULES_SUBDIR)
endif
