# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .23
LINUX_VERSION-4.1 = .13
LINUX_VERSION-4.3 =
LINUX_VERSION-4.4 = -rc7

LINUX_KERNEL_MD5SUM-3.18.23 = dc6d265ab38716be3676ac294b481ad8
LINUX_KERNEL_MD5SUM-4.1.13 = af9dd5d8f71185a64a8eccface15fc00
LINUX_KERNEL_MD5SUM-4.3 = 58b35794eee3b6d52ce7be39357801e7
LINUX_KERNEL_MD5SUM-4.4-rc7 = 7f4b1d85736a180df73af45adde8937b

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
