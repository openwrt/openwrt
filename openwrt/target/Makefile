include $(TOPDIR)/rules.mk
include $(TOPDIR)/rules.mk

# Default target skeleton stuff, may be overridden
TARGET_SKEL_DIR=default/target_skeleton

all: install

define IMAGE_template
$(BIN_DIR)/openwrt-wrt54g-$(1).bin: $(BIN_DIR)/openwrt-generic-$(patsubst jffs2,jffs2-4MB,$(1)).trx
	PATH=$(TARGET_PATH) addpattern -2 -i $$< -o $$@ -g
	$(SED) "1s,^W54S,W54G," $$@
	
$(BIN_DIR)/openwrt-wrt54gs-$(1).bin: $(BIN_DIR)/openwrt-generic-$(patsubst jffs2,jffs2-8MB,$(1)).trx
	PATH=$(TARGET_PATH) addpattern -2 -i $$< -o $$@ -g
	
ifeq ($(BR2_TARGET_WRT54G),y)
IMAGE_TARGETS += $(BIN_DIR)/openwrt-wrt54g-$(1).bin
endif
ifeq ($(BR2_TARGET_WRT54GS),y)
IMAGE_TARGETS += $(BIN_DIR)/openwrt-wrt54gs-$(1).bin
endif
endef

TARGET_DIRS:=
TARGET_FS:=
IMAGE_TARGETS:=
ifeq ($(strip $(BR2_TARGET_ROOTFS_JFFS2)),y)
TARGET_DIRS += jffs2
TARGET_FS += jffs2
endif
ifeq ($(strip $(BR2_TARGET_ROOTFS_SQUASHFS_LZMA)),y)
TARGET_DIRS += squashfs-lzma
TARGET_FS += squashfs
endif
$(foreach fs,$(TARGET_FS),$(eval $(call IMAGE_template,$(fs))))

INSTALL_TARGET_DIRS:=$(patsubst %,%-install,$(TARGET_DIRS))
$(INSTALL_TARGET_DIRS): image_clean $(TARGET_DIR)/etc/sysconf $(BIN_DIR)

prepare: $(patsubst %,%-prepare,$(TARGET_DIRS))
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
	-find $(TARGET_DIR) -type d -name CVS | xargs rm -rf
	-find $(TARGET_DIR) -type d -name .svn | xargs rm -rf

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

compile: $(patsubst %,%-compile,$(TARGET_DIRS))
install: utils-install $(patsubst %,%-install,$(TARGET_DIRS)) $(IMAGE_TARGETS)
clean: $(patsubst %,%-clean,$(TARGET_DIRS)) image_clean

image_clean:
	rm -f $(BIN_DIR)/openwrt-*

$(TARGET_DIR)/etc/sysconf:
	mkdir -p $(TARGET_DIR)/etc
	-grep \^BR2_SYSCONF $(TOPDIR)/.config > $(TARGET_DIR)/etc/sysconf

%-clean:
	$(MAKE) -C $(patsubst %-clean,%,$@) clean
%-prepare:
	$(MAKE) -C $(patsubst %-prepare,%,$@) prepare
%-compile: %-prepare
	$(MAKE) -C $(patsubst %-compile,%,$@) compile
%-install: %-compile
	$(MAKE) -C $(patsubst %-install,%,$@) install



# Default target skeleton stuff, may be overridden
TARGET_SKEL_DIR=default/target_skeleton

all: install

define IMAGE_template
$(BIN_DIR)/openwrt-wrt54g-$(1).bin: $(BIN_DIR)/openwrt-generic-$(patsubst jffs2,jffs2-4MB,$(1)).trx
	PATH=$(TARGET_PATH) addpattern -2 -i $$< -o $$@ -g
	$(SED) "1s,^W54S,W54G," $$@
	
$(BIN_DIR)/openwrt-wrt54gs-$(1).bin: $(BIN_DIR)/openwrt-generic-$(patsubst jffs2,jffs2-8MB,$(1)).trx
	PATH=$(TARGET_PATH) addpattern -2 -i $$< -o $$@ -g
	
ifeq ($(BR2_TARGET_WRT54G),y)
IMAGE_TARGETS += $(BIN_DIR)/openwrt-wrt54g-$(1).bin
endif
ifeq ($(BR2_TARGET_WRT54GS),y)
IMAGE_TARGETS += $(BIN_DIR)/openwrt-wrt54gs-$(1).bin
endif
endef

TARGET_DIRS:=
TARGET_FS:=
IMAGE_TARGETS:=
ifeq ($(strip $(BR2_TARGET_ROOTFS_JFFS2)),y)
TARGET_DIRS += jffs2
TARGET_FS += jffs2
endif
ifeq ($(strip $(BR2_TARGET_ROOTFS_SQUASHFS_LZMA)),y)
TARGET_DIRS += squashfs-lzma
TARGET_FS += squashfs
endif
$(foreach fs,$(TARGET_FS),$(eval $(call IMAGE_template,$(fs))))

INSTALL_TARGET_DIRS:=$(patsubst %,%-install,$(TARGET_DIRS))
$(INSTALL_TARGET_DIRS): image_clean $(TARGET_DIR)/etc/sysconf $(BIN_DIR)

prepare: $(patsubst %,%-prepare,$(TARGET_DIRS))
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

compile: $(patsubst %,%-compile,$(TARGET_DIRS))
install: utils-install $(patsubst %,%-install,$(TARGET_DIRS)) $(IMAGE_TARGETS)
clean: $(patsubst %,%-clean,$(TARGET_DIRS)) image_clean

image_clean:
	rm -f $(BIN_DIR)/openwrt-*

$(TARGET_DIR)/etc/sysconf:
	mkdir -p $(TARGET_DIR)/etc
	-grep \^BR2_SYSCONF $(TOPDIR)/.config > $(TARGET_DIR)/etc/sysconf

%-clean:
	$(MAKE) -C $(patsubst %-clean,%,$@) clean
%-prepare:
	$(MAKE) -C $(patsubst %-prepare,%,$@) prepare
%-compile: %-prepare
	$(MAKE) -C $(patsubst %-compile,%,$@) compile
%-install: %-compile
	$(MAKE) -C $(patsubst %-install,%,$@) install


