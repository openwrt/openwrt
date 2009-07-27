# Use the default kernel version if the Makefile doesn't override it

ifeq ($(KERNEL),2.4)
  LINUX_VERSION?=2.4.37.2
else
  LINUX_VERSION?=2.6.21.7
endif
LINUX_RELEASE?=1

ifeq ($(LINUX_VERSION),2.4.37.2)
  LINUX_KERNEL_MD5SUM:=8a390c782991a2bfe7d4f2fc93dab059
endif
ifeq ($(LINUX_VERSION),2.6.21.7)
  LINUX_KERNEL_MD5SUM:=bc15fad1487336d5dcb0945cd039d8ed
endif
ifeq ($(LINUX_VERSION),2.6.25.20)
  LINUX_KERNEL_MD5SUM:=0da698edccf03e2235abc2830a495114
endif
ifeq ($(LINUX_VERSION),2.6.27.28)
  LINUX_KERNEL_MD5SUM:=f05cd17ba8ee19924c9c473219fbaa9b
endif
ifeq ($(LINUX_VERSION),2.6.28.10)
  LINUX_KERNEL_MD5SUM:=c4efb2c494d749cb5de274f8ae41c3fa
endif
ifeq ($(LINUX_VERSION),2.6.30.3)
  LINUX_KERNEL_MD5SUM:=5499e1fd246215d65fc7068cfb040406
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER=$(call merge_version,$(wordlist 1,3,$(call split_version,$(KERNEL_BASE))))

