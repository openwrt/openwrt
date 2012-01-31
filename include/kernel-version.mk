# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

ifeq ($(LINUX_VERSION),2.6.30.10)
  LINUX_KERNEL_MD5SUM:=eb6be465f914275967a5602cb33662f5
endif
ifeq ($(LINUX_VERSION),2.6.31.14)
  LINUX_KERNEL_MD5SUM:=3e7feb224197d8e174a90dd3759979fd
endif
ifeq ($(LINUX_VERSION),2.6.32.33)
  LINUX_KERNEL_MD5SUM:=2b4e5ed210534d9b4f5a563089dfcc80
endif
ifeq ($(LINUX_VERSION),2.6.34.8)
  LINUX_KERNEL_MD5SUM:=6dedac89df1af57b08981fcc6ad387db
endif
ifeq ($(LINUX_VERSION),2.6.35.11)
  LINUX_KERNEL_MD5SUM:=4c9ee33801f5ad0f4d5e615fac66d535
endif
ifeq ($(LINUX_VERSION),2.6.36.4)
  LINUX_KERNEL_MD5SUM:=c05dd941d0e249695e9f72568888e1bf
endif
ifeq ($(LINUX_VERSION),2.6.37.6)
  LINUX_KERNEL_MD5SUM:=05970afdce8ec4323a10dcd42bc4fb0c
endif
ifeq ($(LINUX_VERSION),2.6.38.8)
  LINUX_KERNEL_MD5SUM:=d27b85795c6bc56b5a38d7d31bf1d724
endif
ifeq ($(LINUX_VERSION),2.6.39.4)
  LINUX_KERNEL_MD5SUM:=a17c748c2070168f1e784e9605ca043d
endif
ifeq ($(LINUX_VERSION),3.0.18)
  LINUX_KERNEL_MD5SUM:=67252770d7009eabe8bac7c26e074f9d
endif
ifeq ($(LINUX_VERSION),3.1.10)
  LINUX_KERNEL_MD5SUM:=3649bdaa50ffd9114cc16486ec54d83a
endif
ifeq ($(LINUX_VERSION),3.2.2)
  LINUX_KERNEL_MD5SUM:=c0303125c8eda431b10084eece7564e3
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
ifeq ($(firstword $(call split_version,$(KERNEL_BASE))),2)
  KERNEL_PATCHVER=$(call merge_version,$(wordlist 1,3,$(call split_version,$(KERNEL_BASE))))
else
  KERNEL_PATCHVER=$(KERNEL)
endif

