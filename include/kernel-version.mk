# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .43
LINUX_VERSION-4.4 = .107

LINUX_KERNEL_HASH-3.18.43 = 1236e8123a6ce537d5029232560966feed054ae31776fe8481dd7d18cdd5492c
LINUX_KERNEL_HASH-4.4.107 = d25dc23c8b05d34518ede68f03668cd344f805049a16bc4f189da90533881a17

ifdef KERNEL_PATCHVER
  LINUX_VERSION:=$(KERNEL_PATCHVER)$(strip $(LINUX_VERSION-$(KERNEL_PATCHVER)))
endif

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER ?= $(KERNEL)

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_HASH:=$(LINUX_KERNEL_HASH-$(strip $(LINUX_VERSION)))
LINUX_KERNEL_HASH?=x
