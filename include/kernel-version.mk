# Use the default kernel version if the Makefile doesn't override it

ifeq ($(LINUX_VERSION),)
  ifeq ($(KERNEL),2.4)
    LINUX_VERSION:=2.4.34
    LINUX_RELEASE:=1
    LINUX_KERNEL_MD5SUM:=f59665540a7f3351ea416a0dad104b55
  else
    LINUX_VERSION:=2.6.21.4
    LINUX_RELEASE:=1
    LINUX_KERNEL_MD5SUM:=c50995d713ba2d73f148e10073342568
  endif
endif
KERNEL:=2.$(word 2,$(subst ., ,$(strip $(LINUX_VERSION))))

