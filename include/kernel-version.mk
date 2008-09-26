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
ifeq ($(LINUX_VERSION),2.6.25.17)
  LINUX_KERNEL_MD5SUM:=30618bff93fd4fd048e20a9a6aab8e5d
endif
ifeq ($(LINUX_VERSION),2.6.26.5)
  LINUX_KERNEL_MD5SUM:=98261b39a558cf0739703ffea7db9f43
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

KERNEL?=2.$(word 2,$(subst ., ,$(strip $(LINUX_VERSION))))
KERNEL_PATCHVER=$(shell echo '$(LINUX_VERSION)' | cut -d. -f1,2,3 | cut -d- -f1)

