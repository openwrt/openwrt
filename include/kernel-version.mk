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
ifeq ($(LINUX_VERSION),2.6.22.4)
  LINUX_KERNEL_MD5SUM:=6cf83acf21e65dcea4a5170c0bbc7125
endif
ifeq ($(LINUX_VERSION),2.6.21.5)
  LINUX_KERNEL_MD5SUM:=2e9a302b5d514b231640227d6a2ab7bf
endif
ifeq ($(LINUX_VERSION),2.6.23)
  LINUX_KERNEL_MD5SUM:=2cc2fd4d521dc5d7cfce0d8a9d1b3472
endif
ifeq ($(LINUX_VERSION),2.6.23.1)
  LINUX_KERNEL_MD5SUM:=518d57e08fdacd88907166a3bfe383b7
endif
ifeq ($(LINUX_VERSION),2.6.23.14)
  LINUX_KERNEL_MD5SUM:=63a6a28ad2480edcffbc09c008b0939d
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

KERNEL?=2.$(word 2,$(subst ., ,$(strip $(LINUX_VERSION))))
KERNEL_PATCHVER=$(shell echo '$(LINUX_VERSION)' | cut -d. -f1,2,3 | cut -d- -f1)

