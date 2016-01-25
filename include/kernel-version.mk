# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .26
LINUX_VERSION-4.1 = .16
LINUX_VERSION-4.3 = .4
LINUX_VERSION-4.4 =

LINUX_KERNEL_MD5SUM-3.18.26 = fb3ef8da32a25607807887b9567a6927
LINUX_KERNEL_MD5SUM-4.1.16 = e4bf22fed49b476b47acffc00c03267a
LINUX_KERNEL_MD5SUM-4.3.4 = 5275d02132107c28b85f986bad576d91
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
