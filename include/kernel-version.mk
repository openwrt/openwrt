# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .44
LINUX_VERSION-4.1 = .35
LINUX_VERSION-4.4 = .39

LINUX_KERNEL_MD5SUM-3.18.44 = 7c52da2f54381a84960118b5bf23381a
LINUX_KERNEL_MD5SUM-4.1.35 = 14ca049674ee83a651be1dc69f5b8d2c
LINUX_KERNEL_MD5SUM-4.4.39 = 864bc186b02bf62974676efa282eda775fe12e5729a3416cfe4b17f0c59111a1

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
