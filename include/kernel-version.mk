# Use the default kernel version if the Makefile doesn't override it

ifeq ($(LINUX_VERSION),)
  ifeq ($(KERNEL),2.4)
    LINUX_VERSION:=2.4.34
  else
    LINUX_VERSION:=2.6.21.5
  endif
endif
LINUX_RELEASE?=1

ifeq ($(LINUX_VERSION),2.4.34)
  LINUX_KERNEL_MD5SUM:=f59665540a7f3351ea416a0dad104b55
endif
ifeq ($(LINUX_VERSION),2.6.22)
  LINUX_KERNEL_MD5SUM:=2e230d005c002fb3d38a3ca07c0200d0
endif
ifeq ($(LINUX_VERSION),2.6.21.5)
  LINUX_KERNEL_MD5SUM:=2e9a302b5d514b231640227d6a2ab7bf
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

KERNEL:=2.$(word 2,$(subst ., ,$(strip $(LINUX_VERSION))))

