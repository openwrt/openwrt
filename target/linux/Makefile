include $(TOPDIR)/rules.mk

define kernel_template

$(1)/$(2)-clean:
	$(MAKE) -C $(1) BOARD="$(2)" clean

$(1)/$(2)-prepare:
	$(MAKE) -C $(1) BOARD="$(2)" prepare

$(1)/$(2)-compile:
	$(MAKE) -C $(1) BOARD="$(2)" compile

$(1)/$(2)-install:
	$(MAKE) -C $(1) BOARD="$(2)" install

ifeq ($(BR2_LINUX_$(3)),y)
clean: $(1)/$(2)-clean
prepare: $(1)/$(2)-prepare
compile: $(1)/$(2)-compile
install: $(1)/$(2)-install
endif

.PHONY: $(1)/$(2)-clean $(1)/$(2)-prepare $(1)/$(2)-compile $(1)/$(2)-install
endef

prepare:
compile:
install:
clean:
	rm -rf $(BUILD_DIR)/kernel

$(eval $(call kernel_template,linux-2.4,brcm,2_4_BRCM))
