include $(TOPDIR)/rules.mk

define kernel_template

$(1)/$(2)-clean:
	$(MAKE) -C linux-$(1) BOARD="$(2)" clean

$(1)/$(2)-prepare:
	$(MAKE) -C linux-$(1) BOARD="$(2)" prepare

$(1)/$(2)-compile:
	$(MAKE) -C linux-$(1) BOARD="$(2)" compile

$(1)/$(2)-rebuild:
	$(MAKE) -C linux-$(1) BOARD="$(2)" rebuild

$(1)/$(2)-install:
	$(MAKE) -C linux-$(1) BOARD="$(2)" install

$(1)/$(2)-image: $(1)/$(2)-install
	$(MAKE) -C image KERNEL="$(1)" BOARD="$(2)" install

ifeq ($(BR2_LINUX_$(3)),y)
clean: $(1)/$(2)-clean
prepare: $(1)/$(2)-prepare
compile: $(1)/$(2)-compile
rebuild: $(1)/$(2)-rebuild
install: $(1)/$(2)-image
endif

.PHONY: $(1)/$(2)-clean $(1)/$(2)-prepare $(1)/$(2)-compile $(1)/$(2)-rebuild $(1)/$(2)-install
endef

prepare:
	$(MAKE) -C image prepare
compile:
	$(MAKE) -C image compile
install:
rebuild:
clean:

$(eval $(call kernel_template,2.4,brcm,2_4_BRCM))
$(eval $(call kernel_template,2.4,ar7,2_4_AR7))
