# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .39
LINUX_VERSION-4.1 = .30
LINUX_VERSION-4.4 = .14

LINUX_KERNEL_MD5SUM-3.18.39 = 8302ace4cf92f6671f004afbcc80967a
LINUX_KERNEL_MD5SUM-4.1.30 = 431e53c413a1deee0634359b7c245c11
LINUX_KERNEL_MD5SUM-4.4.14 = 59e99c3bf5d495f1f95d26257962ca3e

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
