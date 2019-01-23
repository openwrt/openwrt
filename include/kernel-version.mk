# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .132
LINUX_VERSION-4.9 = .152
LINUX_VERSION-4.14 = .95
LINUX_VERSION-4.19 = .16

LINUX_KERNEL_HASH-3.18.132 = c187bd0322372bd34c862cbb06a1996a63524ccb401466362b57ede45901a879
LINUX_KERNEL_HASH-4.9.152 = 90e47b85c09af47eefafe851685ee731538f640b0650a6a9cfa0234436708e39
LINUX_KERNEL_HASH-4.14.95 = ce6729e3fca312520e3cb4f27993852dbb019d94c59c0b35cedab571f9cb58e4
LINUX_KERNEL_HASH-4.19.16 = d8a088381fe3e7e5484c060dabcdda4b053ef7114f91cfd56db003a89bb11bdf

remove_uri_prefix=$(subst git://,,$(subst http://,,$(subst https://,,$(1))))
sanitize_uri=$(call qstrip,$(subst @,_,$(subst :,_,$(subst .,_,$(subst -,_,$(subst /,_,$(1)))))))

ifneq ($(call qstrip,$(CONFIG_KERNEL_GIT_CLONE_URI)),)
  LINUX_VERSION:=$(call sanitize_uri,$(call remove_uri_prefix,$(CONFIG_KERNEL_GIT_CLONE_URI)))
  ifeq ($(call qstrip,$(CONFIG_KERNEL_GIT_REF)),)
    CONFIG_KERNEL_GIT_REF:=HEAD
  endif
  LINUX_VERSION:=$(LINUX_VERSION)-$(call sanitize_uri,$(CONFIG_KERNEL_GIT_REF))
else
ifdef KERNEL_PATCHVER
  LINUX_VERSION:=$(KERNEL_PATCHVER)$(strip $(LINUX_VERSION-$(KERNEL_PATCHVER)))
endif
endif

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER ?= $(KERNEL)

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_HASH:=$(LINUX_KERNEL_HASH-$(strip $(LINUX_VERSION)))
LINUX_KERNEL_HASH?=x
