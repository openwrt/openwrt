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
ifeq ($(LINUX_VERSION),2.6.32.16)
  LINUX_KERNEL_MD5SUM:=d94d91ef3be4eb76765401b4fa462759
endif
ifeq ($(LINUX_VERSION),2.6.33.6)
  LINUX_KERNEL_MD5SUM:=7d8f8a4a09866a786fb59c53fba8232a
endif
ifeq ($(LINUX_VERSION),2.6.34.1)
  LINUX_KERNEL_MD5SUM:=d31d241dc2058698a45fe41359cafb45
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

