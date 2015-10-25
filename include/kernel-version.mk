# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .21
LINUX_VERSION-4.0 = .9
LINUX_VERSION-4.1 = .11
LINUX_VERSION-4.3 = -rc7

LINUX_KERNEL_MD5SUM-3.18.21 = e4248caaa4cef318c04657e971b37298
LINUX_KERNEL_MD5SUM-4.0.9 = 40fc5f6e2d718e539b45e6601c71985b
LINUX_KERNEL_MD5SUM-4.1.11 = 63e5981e38f935772a5309289fe76972
LINUX_KERNEL_MD5SUM-4.3-rc7 = b33f9c9c1a597bb8e0d6f84c074e733f

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
