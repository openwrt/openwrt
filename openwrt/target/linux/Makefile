include $(TOPDIR)/rules.mk

PKG_OS:=$(shell uname -s)
PKG_CPU:=$(shell uname -m)

define kernel_template
ifeq ($(CONFIG_LINUX_$(3)),y)
KERNEL:=$(1)
BOARD:=$(2)
endif
endef

$(eval $(call kernel_template,2.4,brcm,2_4_BRCM))
$(eval $(call kernel_template,2.4,ar7,2_4_AR7))
$(eval $(call kernel_template,2.4,x86,2_4_X86))
$(eval $(call kernel_template,2.6,brcm,2_6_BRCM))
$(eval $(call kernel_template,2.6,rb532,2_6_RB532))
$(eval $(call kernel_template,2.6,x86,2_6_X86))
$(eval $(call kernel_template,2.4,ar531x,2_4_AR531X))
$(eval $(call kernel_template,2.6,aruba,2_6_ARUBA))
$(eval $(call kernel_template,2.6,au1000,2_6_AU1000))
$(eval $(call kernel_template,2.6,xscale,2_6_XSCALE))
$(eval $(call kernel_template,2.6,sibyte,2_6_SIBYTE))

export BOARD
export KERNEL

source:
	$(MAKE) -C $(BOARD)-$(KERNEL) source
	$(MAKE) -C image/$(BOARD) source

prepare:
	rm -f $(TOPDIR)/.kernel.mk
	$(MAKE) -C $(BOARD)-$(KERNEL) prepare

compile:
	$(MAKE) -C $(BOARD)-$(KERNEL) compile

install:
	$(MAKE) -C $(BOARD)-$(KERNEL) install
	$(MAKE) -C image/$(BOARD) install
	
clean:
	$(MAKE) -C $(BOARD)-$(KERNEL) clean

image/%:
	$(MAKE) -C image $(patsubst image/%,%,$@)
