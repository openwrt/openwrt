# Use the default kernel version if the Makefile doesn't override it

ifeq ($(KERNEL),2.4)
  LINUX_VERSION?=2.4.35.4
else
  LINUX_VERSION?=2.6.21.5
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
ifeq ($(LINUX_VERSION),2.6.22.18)
  LINUX_KERNEL_MD5SUM:=bd375d5885bf114c74a2216d52d86e34
endif
ifeq ($(LINUX_VERSION),2.6.23.16)
  LINUX_KERNEL_MD5SUM:=2637a7f1d4450bc5e27422f307fc6529
endif
ifeq ($(LINUX_VERSION),2.6.24.5)
  LINUX_KERNEL_MD5SUM:=26500f8f92895bd33e391088b5edd4ad
endif
ifeq ($(LINUX_VERSION),2.6.25.1)
  LINUX_KERNEL_MD5SUM:=0d26fcafa00dc5cf27d4bf01301409a0
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

KERNEL?=2.$(word 2,$(subst ., ,$(strip $(LINUX_VERSION))))
KERNEL_PATCHVER=$(shell echo '$(LINUX_VERSION)' | cut -d. -f1,2,3 | cut -d- -f1)

