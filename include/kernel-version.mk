# Use the default kernel version if the Makefile doesn't override it

ifeq ($(KERNEL),2.4)
  LINUX_VERSION?=2.4.35.4
else
  LINUX_VERSION?=2.6.21.7
endif
LINUX_RELEASE?=1

ifeq ($(LINUX_VERSION),2.4.34)
  LINUX_KERNEL_MD5SUM:=f59665540a7f3351ea416a0dad104b55
endif
ifeq ($(LINUX_VERSION),2.4.35.4)
  LINUX_KERNEL_MD5SUM:=34066faff3d8c042df1c7600b08b8070
endif
ifeq ($(LINUX_VERSION),2.6.21.7)
  LINUX_KERNEL_MD5SUM:=bc15fad1487336d5dcb0945cd039d8ed
endif
ifeq ($(LINUX_VERSION),2.6.23.17)
  LINUX_KERNEL_MD5SUM:=a0300a393ac91ce9c64bf31522b45e2e
endif
ifeq ($(LINUX_VERSION),2.6.24.7)
  LINUX_KERNEL_MD5SUM:=40a73780d51525d28d36dec852c680c4
endif
ifeq ($(LINUX_VERSION),2.6.25.20)
  LINUX_KERNEL_MD5SUM:=0da698edccf03e2235abc2830a495114
endif
ifeq ($(LINUX_VERSION),2.6.26.8)
  LINUX_KERNEL_MD5SUM:=05dd0d4f8f110b4219ae6ec7a36c046d
endif
ifeq ($(LINUX_VERSION),2.6.27.21)
  LINUX_KERNEL_MD5SUM:=2912af7938fae1a3f2a9a6bcf8c0009f
endif
ifeq ($(LINUX_VERSION),2.6.28.9)
  LINUX_KERNEL_MD5SUM:=a4a870fdb8d0a6a7f218a6e25c9d4891
endif
ifeq ($(LINUX_VERSION),2.6.29.1)
  LINUX_KERNEL_MD5SUM:=4ada43caecb08fe2af71b416b6f586d8
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER=$(call merge_version,$(wordlist 1,3,$(call split_version,$(KERNEL_BASE))))

