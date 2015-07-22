# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .19
LINUX_VERSION-4.0 = .8
LINUX_VERSION-4.1 = .3

LINUX_KERNEL_MD5SUM-3.18.19 = a143f9421bd8413752609d0d771c2955
LINUX_KERNEL_MD5SUM-4.0.8 = a99375dd855de00635e9ac8e62b0f8e6
LINUX_KERNEL_MD5SUM-4.1.3 = 96c2c77b1c54ba01cfd8fc2d13fbf828

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
