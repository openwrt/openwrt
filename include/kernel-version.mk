# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .134
LINUX_VERSION-4.9 = .158
LINUX_VERSION-4.14 = .99
LINUX_VERSION-4.19 = .21

LINUX_KERNEL_HASH-3.18.134 = 36bdd04cab3b6c824a4b7e32ae02503f437e0916d5a4ff04c90aa22da2749c2f
LINUX_KERNEL_HASH-4.9.158 = 8c8a69f590e6f1103c949b45ff1bfd42c705388321f75e1520be3556f81375ef
LINUX_KERNEL_HASH-4.14.99 = 8ff98caed5b20b733dedcbe99559d71a0e09e239c0c2488b3fd799c96489eb0a
LINUX_KERNEL_HASH-4.19.21 = db08ede726fe410da274e1e98f0de2aed9ff4b0e0b63a89e5928bef46998bbc1

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
