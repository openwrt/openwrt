# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .43
LINUX_VERSION-4.1 = .34
LINUX_VERSION-4.4 = .38

LINUX_KERNEL_MD5SUM-3.18.43 = b1faeb4a2e1e70ffe061bdbb3452840a
LINUX_KERNEL_MD5SUM-4.1.34 = fba99f0f4765ebf01033e69518740a3c
LINUX_KERNEL_MD5SUM-4.4.38 = 77ed9e650b65788a1f9f371ba87c2e50b5ba7624bc834ae38b036f7f592816a6

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
