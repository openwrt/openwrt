# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

ifeq ($(LINUX_VERSION),2.6.30.10)
  LINUX_KERNEL_MD5SUM:=eb6be465f914275967a5602cb33662f5
endif
ifeq ($(LINUX_VERSION),2.6.31.14)
  LINUX_KERNEL_MD5SUM:=3e7feb224197d8e174a90dd3759979fd
endif
ifeq ($(LINUX_VERSION),2.6.32.27)
  LINUX_KERNEL_MD5SUM:=c8df8bed01a3b7e4ce13563e74181d71
endif
ifeq ($(LINUX_VERSION),2.6.34.7)
  LINUX_KERNEL_MD5SUM:=8964e26120e84844998a673464a980ea
endif
ifeq ($(LINUX_VERSION),2.6.35.9)
  LINUX_KERNEL_MD5SUM:=18d339e9229560e73c4249dffdc3fd90
endif
ifeq ($(LINUX_VERSION),2.6.36.4)
  LINUX_KERNEL_MD5SUM:=c05dd941d0e249695e9f72568888e1bf
endif
ifeq ($(LINUX_VERSION),2.6.37.1)
  LINUX_KERNEL_MD5SUM:=07d3b1868a67c1a7ddcf1d54444cb5d1
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER=$(call merge_version,$(wordlist 1,3,$(call split_version,$(KERNEL_BASE))))

