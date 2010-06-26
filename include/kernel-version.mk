# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

ifeq ($(LINUX_VERSION),2.6.25.20)
  LINUX_KERNEL_MD5SUM:=0da698edccf03e2235abc2830a495114
endif
ifeq ($(LINUX_VERSION),2.6.30.10)
  LINUX_KERNEL_MD5SUM:=eb6be465f914275967a5602cb33662f5
endif
ifeq ($(LINUX_VERSION),2.6.31.12)
  LINUX_KERNEL_MD5SUM:=517be354b81b780e2f4b2ad614d030de
endif
ifeq ($(LINUX_VERSION),2.6.32.14)
  LINUX_KERNEL_MD5SUM:=b13dc3ce727039597163cb4bce4cbdbb
endif
ifeq ($(LINUX_VERSION),2.6.33.4)
  LINUX_KERNEL_MD5SUM:=f75d21e5c60f18adf0e99d61c243f964
endif
ifeq ($(LINUX_VERSION),2.6.34)
  LINUX_KERNEL_MD5SUM:=10eebcb0178fb4540e2165bfd7efc7ad
endif
ifeq ($(LINUX_VERSION),2.6.35-rc2)
  LINUX_KERNEL_MD5SUM:=6b6b76e689e11b70b2e53f9482006929
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER=$(call merge_version,$(wordlist 1,3,$(call split_version,$(KERNEL_BASE))))

