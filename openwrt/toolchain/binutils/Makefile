include $(TOPDIR)/rules.mk

#############################################################
#
# build binutils for use on the host system
#
#############################################################
BINUTILS_VERSION:=$(strip $(subst ",, $(BR2_BINUTILS_VERSION)))
#"

BINUTILS_SITE:=http://www.fr.kernel.org/pub/linux/devel/binutils \
	       http://www.fi.kernel.org/pub/linux/devel/binutils \
	       http://ftp.kernel.org/pub/linux/devel/binutils \
	       http://www.de.kernel.org/pub/linux/devel/binutils

ifeq ($(BINUTILS_VERSION),2.15)
BINUTILS_SITE:=http://ftp.gnu.org/gnu/binutils/ \
	ftp://gatekeeper.dec.com/pub/GNU/ \
        ftp://ftp.uu.net/archive/systems/gnu/ \
        ftp://ftp.eu.uu.net/pub/gnu/ \
        ftp://ftp.funet.fi/pub/gnu/prep/ \
        ftp://ftp.leo.org/pub/comp/os/unix/gnu/ 
endif
ifeq ($(BINUTILS_VERSION),2.14)
BINUTILS_SITE:=http://ftp.gnu.org/gnu/binutils/ \
	ftp://gatekeeper.dec.com/pub/GNU/ \
        ftp://ftp.uu.net/archive/systems/gnu/ \
        ftp://ftp.eu.uu.net/pub/gnu/ \
        ftp://ftp.funet.fi/pub/gnu/prep/ \
        ftp://ftp.leo.org/pub/comp/os/unix/gnu/ 
endif
ifeq ($(BINUTILS_VERSION),2.13)
BINUTILS_SITE:=http://ftp.gnu.org/gnu/binutils/ \
	ftp://gatekeeper.dec.com/pub/GNU/ \
        ftp://ftp.uu.net/archive/systems/gnu/ \
        ftp://ftp.eu.uu.net/pub/gnu/ \
        ftp://ftp.funet.fi/pub/gnu/prep/ \
        ftp://ftp.leo.org/pub/comp/os/unix/gnu/ 
endif

BINUTILS_SOURCE:=binutils-$(BINUTILS_VERSION).tar.bz2
BINUTILS_DIR:=$(TOOL_BUILD_DIR)/binutils-$(BINUTILS_VERSION)
BINUTILS_CAT:=bzcat

BINUTILS_DIR1:=$(TOOL_BUILD_DIR)/binutils-$(BINUTILS_VERSION)-build

$(DL_DIR)/$(BINUTILS_SOURCE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(BINUTILS_SOURCE) x $(BINUTILS_SITE)

$(BINUTILS_DIR)/.unpacked: $(DL_DIR)/$(BINUTILS_SOURCE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(BINUTILS_CAT) $(DL_DIR)/$(BINUTILS_SOURCE) | tar -C $(TOOL_BUILD_DIR) $(TAR_OPTIONS) -
	touch $(BINUTILS_DIR)/.unpacked

$(BINUTILS_DIR)/.patched: $(BINUTILS_DIR)/.unpacked
	# Apply appropriate binutils patches.
	$(SCRIPT_DIR)/patch-kernel.sh $(BINUTILS_DIR) ./all \*.patch
	$(SCRIPT_DIR)/patch-kernel.sh $(BINUTILS_DIR) ./$(BINUTILS_VERSION) \*.patch
	touch $(BINUTILS_DIR)/.patched

$(BINUTILS_DIR1)/.configured: $(BINUTILS_DIR)/.patched
	mkdir -p $(BINUTILS_DIR1)
	(cd $(BINUTILS_DIR1); \
		$(BINUTILS_DIR)/configure \
		--prefix=$(STAGING_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--disable-werror \
		$(DISABLE_NLS) \
		$(MULTILIB) \
		$(SOFT_FLOAT_CONFIG_OPTION) );
	touch $(BINUTILS_DIR1)/.configured

$(BINUTILS_DIR1)/binutils/objdump: $(BINUTILS_DIR1)/.configured
	$(MAKE) -C $(BINUTILS_DIR1) all

# Make install will put gettext data in staging_dir/share/locale.
# Unfortunatey, it isn't configureable.
$(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)-ld: $(BINUTILS_DIR1)/binutils/objdump
	$(MAKE) -C $(BINUTILS_DIR1) install


#############################################################
#
# build binutils for use on the target system
#
#############################################################
BINUTILS_DIR2:=$(BUILD_DIR)/binutils-$(BINUTILS_VERSION)-target
$(BINUTILS_DIR2)/.configured: $(BINUTILS_DIR)/.patched
	mkdir -p $(BINUTILS_DIR2)
	(cd $(BINUTILS_DIR2); \
		PATH=$(TARGET_PATH) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CFLAGS_FOR_BUILD="-O2 -g" \
		$(BINUTILS_DIR)/configure \
		--prefix=/usr \
		--exec-prefix=/usr \
		--build=$(GNU_HOST_NAME) \
		--host=$(REAL_GNU_TARGET_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		$(DISABLE_NLS) \
		$(MULTILIB) \
		$(SOFT_FLOAT_CONFIG_OPTION) );
	touch $(BINUTILS_DIR2)/.configured

$(BINUTILS_DIR2)/binutils/objdump: $(BINUTILS_DIR2)/.configured
	PATH=$(TARGET_PATH) \
	$(MAKE) -C $(BINUTILS_DIR2) all

source: $(DL_DIR)/$(BINUTILS_SOURCE)
prepare: $(BINUTILS_DIR)/.patched
compile: $(BINUTILS_DIR1)/binutils/objdump
install: $(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)-ld
clean:
	rm -f $(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)*
	rm -rf $(BINUTILS_DIR) $(BINUTILS_DIR1)


