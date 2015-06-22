# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.18 = .16
LINUX_VERSION-4.0 = .5
LINUX_VERSION-4.1 =

LINUX_KERNEL_MD5SUM-3.18.16 = 3b0e66fdf47b0660521d79b881eabf04
LINUX_KERNEL_MD5SUM-4.0.5 = 214836ae60a5674c31bb7cede5f47794
LINUX_KERNEL_MD5SUM-4.1 = fe9dc0f6729f36400ea81aa41d614c37

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
