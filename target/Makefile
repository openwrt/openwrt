include $(TOPDIR)/rules.mk

all: install

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

linux-compile: utils-install lzma-install
linux-install: $(BIN_DIR)

prepare: linux-prepare
compile: linux-compile
install: image_clean linux-install
clean: linux-clean utils-clean lzma-clean image_clean

image_clean: FORCE
	rm -f $(BIN_DIR)/openwrt-*

%-clean: FORCE
	$(MAKE) -C $(patsubst %-clean,%,$@) clean
%-prepare: FORCE
	$(MAKE) -C $(patsubst %-prepare,%,$@) prepare
%-compile: %-prepare
	$(MAKE) -C $(patsubst %-compile,%,$@) compile
%-rebuild: FORCE
	$(MAKE) -C $(patsubst %-rebuild,%,$@) rebuild
%-install: %-compile
	$(MAKE) -C $(patsubst %-install,%,$@) install
linux-imagebuilder: FORCE
	$(MAKE) -C linux imagebuilder


