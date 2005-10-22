include $(TOPDIR)/rules.mk

PKG_OS:=$(shell uname -s)
PKG_CPU:=$(shell uname -m)

IB_NAME:=OpenWrt-ImageBuilder-$(PKG_OS)-$(PKG_CPU)
IB_DIR:=$(BUILD_DIR)/$(IB_NAME)

define kernel_template

$(1)/$(2)-clean:
	$(MAKE) -C linux-$(1) KERNEL="$(1)" BOARD="$(2)" clean

$(1)/$(2)-prepare:
	$(MAKE) -C linux-$(1) KERNEL="$(1)" BOARD="$(2)" prepare

$(1)/$(2)-compile:
	$(MAKE) -C linux-$(1) KERNEL="$(1)" BOARD="$(2)" compile

$(1)/$(2)-rebuild:
	$(MAKE) -C linux-$(1) KERNEL="$(1)" BOARD="$(2)" rebuild

$(1)/$(2)-install:
	$(MAKE) -C linux-$(1) KERNEL="$(1)" BOARD="$(2)" install

$(1)/$(2)-image: $(1)/$(2)-install
	$(MAKE) -C image KERNEL="$(1)" BOARD="$(2)" install

$(1)/$(2)-install-ib:
	$(MAKE) -C image KERNEL="$(1)" BOARD="$(2)" IB_DIR="$(IB_DIR)" install-ib
	echo '$$$$(eval $$$$(call image_template,$(2),$(1)))' >> $(IB_DIR)/kernel.mk


ifeq ($(BR2_LINUX_$(3)),y)
clean: $(1)/$(2)-clean
prepare: $(1)/$(2)-prepare
compile: $(1)/$(2)-compile
rebuild: $(1)/$(2)-rebuild
$(BIN_DIR)/$(IB_NAME).tar.bz2 install: $(1)/$(2)-image
install-ib: $(1)/$(2)-install-ib
endif

.PHONY: $(1)/$(2)-clean $(1)/$(2)-prepare $(1)/$(2)-compile $(1)/$(2)-rebuild $(1)/$(2)-install $(1)/$(2)-image $(1)/$(2)-install-ib
endef

.PHONY: clean prepare compile rebuild install imagebuilder install-ib

install-ib:

$(BIN_DIR)/$(IB_NAME).tar.bz2:	
	rm -rf $(IB_DIR)
	mkdir -p $(IB_DIR)/packages
	mkdir -p $(IB_DIR)/scripts
	cp -fpR ./imagebuilder/* $(IB_DIR)/
	cp -fpR $(TOPDIR)/bin/packages/*.ipk $(IB_DIR)/packages/
	cp $(TOPDIR)/scripts/ipkg $(IB_DIR)/scripts/
	$(MAKE) install-ib
	cp $(TOPDIR)/rules.mk $(IB_DIR)
	cp -fpR ./image $(IB_DIR)/
	cp $(TOPDIR)/.config $(IB_DIR)/
	find $(IB_DIR) -name .svn | xargs rm -rf
	find $(IB_DIR) -name CVS | xargs rm -rf
	(cd $(BUILD_DIR); tar c $(IB_NAME) | bzip2 -c > $(BIN_DIR)/$(IB_NAME).tar.bz2)

ifeq ($(BR2_PACKAGE_IMAGEBUILDER),y)
install: $(BIN_DIR)/$(IB_NAME).tar.bz2
endif

prepare:
	@$(TRACE) target/linux/image/prepare
	$(MAKE) -C image prepare
compile:
	@$(TRACE) target/linux/image/compile
	$(MAKE) -C image compile
install:
rebuild:
clean:

$(eval $(call kernel_template,2.4,brcm,2_4_BRCM))
$(eval $(call kernel_template,2.4,ar7,2_4_AR7))
$(eval $(call kernel_template,2.4,soekris,2_4_SOEKRIS))
$(eval $(call kernel_template,2.6,brcm,2_6_BRCM))
