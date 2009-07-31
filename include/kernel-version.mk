# Use the default kernel version if the Makefile doesn't override it

ifeq ($(KERNEL),2.4)
  LINUX_VERSION?=2.4.37.4
else
  LINUX_VERSION?=2.6.21.7
endif
LINUX_RELEASE?=1

ifeq ($(LINUX_VERSION),2.4.37.4)
  LINUX_KERNEL_MD5SUM:=
endif
ifeq ($(LINUX_VERSION),2.6.21.7)
  LINUX_KERNEL_MD5SUM:=bc15fad1487336d5dcb0945cd039d8ed
endif
ifeq ($(LINUX_VERSION),2.6.25.20)
  LINUX_KERNEL_MD5SUM:=0da698edccf03e2235abc2830a495114
endif
ifeq ($(LINUX_VERSION),2.6.27.29)
  LINUX_KERNEL_MD5SUM:=ab6991ee42603a4dbfba7956a54c89a4
endif
ifeq ($(LINUX_VERSION),2.6.28.10)
  LINUX_KERNEL_MD5SUM:=c4efb2c494d749cb5de274f8ae41c3fa
endif
ifeq ($(LINUX_VERSION),2.6.30.4)
  LINUX_KERNEL_MD5SUM:=ac05e32764368af7eff79c5e3df65efb
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER=$(call merge_version,$(wordlist 1,3,$(call split_version,$(KERNEL_BASE))))

