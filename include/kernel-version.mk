# Use the default kernel version if the Makefile doesn't override it

ifeq ($(LINUX_VERSION),)
  ifeq ($(KERNEL),2.4)
    LINUX_VERSION:=2.4.34
    LINUX_RELEASE:=1
    LINUX_KERNEL_MD5SUM:=f59665540a7f3351ea416a0dad104b55
  else
    LINUX_VERSION:=2.6.19.2
    LINUX_RELEASE:=1
    LINUX_KERNEL_MD5SUM:=ca0ce8f288e8ae93ac243b568f906bf8
  endif
endif
KERNEL:=2.$(word 2,$(subst ., ,$(strip $(LINUX_VERSION))))

