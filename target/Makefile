include $(TOPDIR)/rules.mk

all: install

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

TARGETS-y := linux utils
TARGETS-$(CONFIG_SDK) += sdk

linux-compile: utils-install
linux-install: $(BIN_DIR)
sdk-compile: linux-install

download: $(patsubst %,%-source,$(TARGETS-y))
prepare: linux-prepare
compile: linux-compile
install: image_clean $(patsubst %,%-install,$(TARGETS-y))
clean: $(patsubst %,%-clean,$(TARGETS-y)) image_clean

image_clean: FORCE
	rm -f $(BIN_DIR)/openwrt-*

%-clean: FORCE
	$(MAKE) -C $(patsubst %-clean,%,$@) clean
%-source: FORCE
	$(MAKE) -C $(patsubst %-source,%,$@) source
%-prepare: FORCE
	$(MAKE) -C $(patsubst %-prepare,%,$@) prepare
%-compile: %-prepare
	$(MAKE) -C $(patsubst %-compile,%,$@) compile
%-install: %-compile
	$(MAKE) -C $(patsubst %-install,%,$@) install

