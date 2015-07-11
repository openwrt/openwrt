# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .18
LINUX_VERSION-4.0 = .8
LINUX_VERSION-4.1 = .2

LINUX_KERNEL_MD5SUM-3.18.18 = 6fce868a56f2d83f80cdad5fed457110
LINUX_KERNEL_MD5SUM-4.0.8 = a99375dd855de00635e9ac8e62b0f8e6
LINUX_KERNEL_MD5SUM-4.1.2 = 26879db288cd9ffa17c9fd5db213b272

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
