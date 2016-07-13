# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .36
LINUX_VERSION-4.1 = .27
LINUX_VERSION-4.4 = .14

LINUX_KERNEL_MD5SUM-3.18.36 = 31b1699c1285aeeb81fb25750baa50d9
LINUX_KERNEL_MD5SUM-4.1.27 = 0347b5790c85bdabce41b8b9d9d4b98e
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
