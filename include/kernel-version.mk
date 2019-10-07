# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

ifdef CONFIG_TESTING_KERNEL
  KERNEL_PATCHVER:=$(KERNEL_TESTING_PATCHVER)
endif

LINUX_VERSION-4.9 = .195
LINUX_VERSION-4.14 = .146
LINUX_VERSION-4.19 = .76

LINUX_KERNEL_HASH-4.9.195 = 54be562a2da16eecd612f641146dc099c1aa05c0fc559c971bdfde161c929d68
LINUX_KERNEL_HASH-4.14.146 = a3c54b887ea3e679382bd4c0536e6a281b071dab2258bd3ee8af75baef2023f5
LINUX_KERNEL_HASH-4.19.76 = ba4dd29902734b263a5197410363b17f30dc60bc999bd6ce90b6f5910b971e66

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
ifdef KERNEL_TESTING_PATCHVER
  LINUX_TESTING_VERSION:=$(KERNEL_TESTING_PATCHVER)$(strip $(LINUX_VERSION-$(KERNEL_TESTING_PATCHVER)))
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
