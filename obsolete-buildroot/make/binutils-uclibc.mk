#############################################################
#
# build binutils for use on the host system
#
#############################################################
BINUTILS_SITE:=http://ftp.kernel.org/pub/linux/devel/binutils
BINUTILS_SOURCE:=binutils-2.14.90.0.7.tar.bz2
BINUTILS_DIR:=$(TOOL_BUILD_DIR)/binutils-2.14.90.0.7
BINUTILS_CAT:=bzcat

BINUTILS_DIR1:=$(TOOL_BUILD_DIR)/binutils-build

$(DL_DIR)/$(BINUTILS_SOURCE):
	$(WGET) -P $(DL_DIR) $(BINUTILS_SITE)/$(BINUTILS_SOURCE)

$(BINUTILS_DIR)/.unpacked: $(DL_DIR)/$(BINUTILS_SOURCE)
	mkdir -p $(TOOL_BUILD_DIR)
	mkdir -p $(DL_DIR)
	$(BINUTILS_CAT) $(DL_DIR)/$(BINUTILS_SOURCE) | tar -C $(TOOL_BUILD_DIR) -xvf -
	touch $(BINUTILS_DIR)/.unpacked

$(BINUTILS_DIR)/.patched: $(BINUTILS_DIR)/.unpacked
	# Apply any files named binutils-*.patch from the source directory to binutils
	$(SOURCE_DIR)/patch-kernel.sh $(BINUTILS_DIR) $(SOURCE_DIR) binutils-uclibc*.patch
	touch $(BINUTILS_DIR)/.patched

$(BINUTILS_DIR1)/.configured: $(BINUTILS_DIR)/.patched
	mkdir -p $(BINUTILS_DIR1)
	(cd $(BINUTILS_DIR1); \
		$(BINUTILS_DIR)/configure \
		--prefix=$(STAGING_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		$(DISABLE_NLS) \
		$(MULTILIB) \
		$(SOFT_FLOAT_CONFIG_OPTION) );
	touch $(BINUTILS_DIR1)/.configured

$(BINUTILS_DIR1)/binutils/objdump: $(BINUTILS_DIR1)/.configured
	$(MAKE) $(JLEVEL) -C $(BINUTILS_DIR1) all

# Make install will put gettext data in staging_dir/share/locale.
# Unfortunatey, it isn't configureable.
$(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/bin/ld: $(BINUTILS_DIR1)/binutils/objdump 
	$(MAKE) $(JLEVEL) -C $(BINUTILS_DIR1) install

binutils-dependancies:
	@if [ ! -x /usr/bin/bison ] ; then \
		echo -e "\n\nYou must install 'bison' on your build machine\n"; \
		exit 1; \
	fi;
	@if [ ! -x /usr/bin/flex ] ; then \
		echo -e "\n\nYou must install 'flex' on your build machine\n"; \
		exit 1; \
	fi;
	@if [ ! -x /usr/bin/msgfmt ] ; then \
		echo -e "\n\nYou must install 'gettext' on your build machine\n"; \
		exit 1; \
	fi;

binutils: binutils-dependancies $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/bin/ld

binutils-source: $(DL_DIR)/$(BINUTILS_SOURCE)

binutils-clean:
	rm -f $(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)*
	-$(MAKE) -C $(BINUTILS_DIR1) clean

binutils-dirclean:
	rm -rf $(BINUTILS_DIR1)



#############################################################
#
# build binutils for use on the target system
#
#############################################################
BINUTILS_DIR2:=$(BUILD_DIR)/binutils-target
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
	$(MAKE) $(JLEVEL) -C $(BINUTILS_DIR2) all

$(TARGET_DIR)/usr/bin/ld: $(BINUTILS_DIR2)/binutils/objdump 
	PATH=$(TARGET_PATH) \
	$(MAKE) $(JLEVEL) DESTDIR=$(TARGET_DIR) \
		tooldir=/usr build_tooldir=/usr \
		-C $(BINUTILS_DIR2) install
	#rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
	#	$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc
	-$(STRIP) $(TARGET_DIR)/usr/$(REAL_GNU_TARGET_NAME)/bin/* > /dev/null 2>&1
	-$(STRIP) $(TARGET_DIR)/usr/bin/* > /dev/null 2>&1 

binutils_target: $(GCC_DEPENDANCY) $(TARGET_DIR)/usr/bin/ld

binutils_target-clean:
	rm -f $(TARGET_DIR)/bin/$(REAL_GNU_TARGET_NAME)*
	-$(MAKE) -C $(BINUTILS_DIR2) clean

binutils_target-dirclean:
	rm -rf $(BINUTILS_DIR2)

