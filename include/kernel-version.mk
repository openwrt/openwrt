# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .29
LINUX_VERSION-4.1 = .20
LINUX_VERSION-4.3 = .4
LINUX_VERSION-4.4 = .7

LINUX_KERNEL_MD5SUM-3.18.29 = b25737a0bc98e80d12200de93f239c28
LINUX_KERNEL_MD5SUM-4.1.20 = 075c38a3a23ca5bc80437b13606df00a
LINUX_KERNEL_MD5SUM-4.3.4 = 5275d02132107c28b85f986bad576d91
LINUX_KERNEL_MD5SUM-4.4.7 = 4345597c9a10bd73c28b6ae3a854d8d7

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
