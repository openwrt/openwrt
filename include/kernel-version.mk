# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

ifeq ($(LINUX_VERSION),2.6.25.20)
  LINUX_KERNEL_MD5SUM:=0da698edccf03e2235abc2830a495114
endif
ifeq ($(LINUX_VERSION),2.6.30.10)
  LINUX_KERNEL_MD5SUM:=eb6be465f914275967a5602cb33662f5
endif
ifeq ($(LINUX_VERSION),2.6.31.14)
  LINUX_KERNEL_MD5SUM:=3e7feb224197d8e174a90dd3759979fd
endif
ifeq ($(LINUX_VERSION),2.6.32.20)
  LINUX_KERNEL_MD5SUM:=c254831e3f59b9fef14e61871e5f4738
endif
ifeq ($(LINUX_VERSION),2.6.33.7)
  LINUX_KERNEL_MD5SUM:=2cea51deeaa0620a07d005ec3b148f06
endif
ifeq ($(LINUX_VERSION),2.6.34.5)
  LINUX_KERNEL_MD5SUM:=d280b596b32f7497bbe9dd54669c99d6
endif
ifeq ($(LINUX_VERSION),2.6.35.3)
  LINUX_KERNEL_MD5SUM:=5ebff49fd65a7bad234ce103b0b3ede0
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER=$(call merge_version,$(wordlist 1,3,$(call split_version,$(KERNEL_BASE))))

