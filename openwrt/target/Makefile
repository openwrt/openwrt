include $(TOPDIR)/rules.mk

# Default target skeleton stuff, may be overridden
TARGET_SKEL_DIR=default/target_skeleton

all: install

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)
	if [ -f "$(TARGET_SKELETON)" ] ; then \
	zcat $(TARGET_SKELETON) | tar -C $(BUILD_DIR) -xf -; \
	fi;
	if [ -d "$(TARGET_SKEL_DIR)" ] ; then \
		cp -a $(TARGET_SKEL_DIR)/* $(TARGET_DIR)/; \
	fi;
	mkdir -p $(TARGET_DIR)/jffs
	mkdir -p $(TARGET_DIR)/dev
	mkdir -p $(TARGET_DIR)/proc
	mkdir -p $(TARGET_DIR)/tmp
	mkdir -p $(TARGET_DIR)/lib
	mkdir -p $(TARGET_DIR)/usr/lib
	mkdir -p $(TARGET_DIR)/usr/bin
	ln -sf /tmp/resolv.conf $(TARGET_DIR)/etc/resolv.conf
	rm -rf $(TARGET_DIR)/var
	ln -sf /tmp $(TARGET_DIR)/var
	-find $(TARGET_DIR) -type d -name CVS | xargs rm -rf
	-find $(TARGET_DIR) -type d -name .svn | xargs rm -rf

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET_DIR)/etc/sysconf:
	mkdir -p $(TARGET_DIR)/etc
	-grep \^BR2_SYSCONF $(TOPDIR)/.config > $(TARGET_DIR)/etc/sysconf

linux-compile: utils-install lzma-install
linux-install: $(TARGET_DIR)/etc/sysconf $(BIN_DIR)

prepare: $(TARGET_DIR)
compile: linux-compile
install: image_clean linux-install
clean: linux-clean utils-clean lzma-clean image_clean

image_clean:
	rm -f $(BIN_DIR)/openwrt-*

%-clean:
	$(MAKE) -C $(patsubst %-clean,%,$@) clean
%-prepare:
	$(MAKE) -C $(patsubst %-prepare,%,$@) prepare
%-compile: %-prepare
	$(MAKE) -C $(patsubst %-compile,%,$@) compile
%-rebuild: 
	$(MAKE) -C $(patsubst %-rebuild,%,$@) rebuild
%-install: %-compile
	$(MAKE) -C $(patsubst %-install,%,$@) install


