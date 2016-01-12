# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .23
LINUX_VERSION-4.1 = .15
LINUX_VERSION-4.3 =
LINUX_VERSION-4.4 =

LINUX_KERNEL_MD5SUM-3.18.23 = dc6d265ab38716be3676ac294b481ad8
LINUX_KERNEL_MD5SUM-4.1.15 = b227333912b161c96ff3e30f5041e1c0
LINUX_KERNEL_MD5SUM-4.3 = 58b35794eee3b6d52ce7be39357801e7
LINUX_KERNEL_MD5SUM-4.4 = 9a78fa2eb6c68ca5a40ed5af08142599

ifdef KERNEL_PATCHVER
  LINUX_VERSION:=$(KERNEL_PATCHVER)$(strip $(LINUX_VERSION-$(KERNEL_PATCHVER)))
endif

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER ?= $(KERNEL)

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM:=$(LINUX_KERNEL_MD5SUM-$(strip $(LINUX_VERSION)))
LINUX_KERNEL_MD5SUM?=x
