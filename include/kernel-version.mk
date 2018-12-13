# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .129
LINUX_VERSION-4.9 = .145
LINUX_VERSION-4.14 = .87

LINUX_KERNEL_HASH-3.18.129 = 8d420b58593a74109175be781da2320a341baec9aaa186da31ad508d3f377e72
LINUX_KERNEL_HASH-4.9.145 = 6901ca37e8c305a2f26c598952338b4dc2481ca5a9d0bf71e2b71730a5b5bc5e
LINUX_KERNEL_HASH-4.14.87 = 36744d7d657dab23e455a8096e4ad2dc6a25f25fdce062d8a8bdf4499112e125

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
