#############################################################
#
# sed
#
#############################################################
SED_VER:=4.1.2
SED_SOURCE:=sed-$(SED_VER).tar.gz
SED_SITE:=ftp://ftp.gnu.org/gnu/sed
SED_CAT:=zcat
SED_DIR1:=$(TOOL_BUILD_DIR)/sed-$(SED_VER)
SED_DIR2:=$(BUILD_DIR)/sed-$(SED_VER)
SED_BINARY:=sed/sed
SED_TARGET_BINARY:=bin/sed
ifeq ($(strip $(BUILD_WITH_LARGEFILE)),true)
SED_CPPFLAGS=-D_FILE_OFFSET_BITS=64
endif
SED:=$(STAGING_DIR)/bin/sed -i -e
HOST_SED_TARGET=$(shell package/sed/sedcheck.sh)

$(DL_DIR)/$(SED_SOURCE):
	mkdir -p $(DL_DIR)
	$(WGET) -P $(DL_DIR) $(SED_SITE)/$(SED_SOURCE)

sed-source: $(DL_DIR)/$(SED_SOURCE)


#############################################################
#
# build sed for use on the host system
#
#############################################################
$(SED_DIR1)/.unpacked: $(DL_DIR)/$(SED_SOURCE)
	mkdir -p $(TOOL_BUILD_DIR)
	mkdir -p $(STAGING_DIR)/bin;
	$(SED_CAT) $(DL_DIR)/$(SED_SOURCE) | tar -C $(TOOL_BUILD_DIR) $(TAR_OPTIONS) -
	touch $(SED_DIR1)/.unpacked

$(SED_DIR1)/.configured: $(SED_DIR1)/.unpacked
	(cd $(SED_DIR1); rm -rf config.cache; \
		./configure \
		--prefix=$(STAGING_DIR) \
		--prefix=/usr \
	);
	touch  $(SED_DIR1)/.configured

$(SED_DIR1)/$(SED_BINARY): $(SED_DIR1)/.configured
	$(MAKE) -C $(SED_DIR1)

# This stuff is needed to work around GNU make deficiencies
build-sed-host-binary: $(SED_DIR1)/$(SED_BINARY)
	@if [ -L $(STAGING_DIR)/$(SED_TARGET_BINARY) ] ; then \
		rm -f $(STAGING_DIR)/$(SED_TARGET_BINARY); fi;
	@if [ ! -f $(STAGING_DIR)/$(SED_TARGET_BINARY) -o $(STAGING_DIR)/$(SED_TARGET_BINARY) \
	-ot $(SED_DIR1)/$(SED_BINARY) ] ; then \
	    set -x; \
	    mkdir -p $(STAGING_DIR)/bin; \
	    $(MAKE) DESTDIR=$(STAGING_DIR) -C $(SED_DIR1) install; \
	    mv $(STAGING_DIR)/usr/bin/sed $(STAGING_DIR)/bin/; \
	    rm -rf $(STAGING_DIR)/share/locale $(STAGING_DIR)/usr/info \
		    $(STAGING_DIR)/usr/man $(STAGING_DIR)/usr/share/doc; fi

use-sed-host-binary:
	@if [ -x /usr/bin/sed ]; then SED="/usr/bin/sed"; else \
	    if [ -x /bin/sed ]; then SED="/bin/sed"; fi; fi; \
	    mkdir -p $(STAGING_DIR)/bin; \
	    rm -f $(STAGING_DIR)/$(SED_TARGET_BINARY); \
	    ln -s $$SED $(STAGING_DIR)/$(SED_TARGET_BINARY)

host-sed: $(HOST_SED_TARGET)

host-sed-clean:
	$(MAKE) DESTDIR=$(STAGING_DIR) -C $(SED_DIR1) uninstall
	-$(MAKE) -C $(SED_DIR1) clean

host-sed-dirclean:
	rm -rf $(SED_DIR1)


#############################################################
#
# build sed for use on the target system
#
#############################################################
$(SED_DIR2)/.unpacked: $(DL_DIR)/$(SED_SOURCE)
	$(SED_CAT) $(DL_DIR)/$(SED_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(SED_DIR2)/.unpacked

$(SED_DIR2)/.configured: $(SED_DIR2)/.unpacked
	(cd $(SED_DIR2); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="$(SED_CFLAGS)" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
	);
	touch  $(SED_DIR2)/.configured

$(SED_DIR2)/$(SED_BINARY): $(SED_DIR2)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(SED_DIR2)

# This stuff is needed to work around GNU make deficiencies
sed-target_binary: $(SED_DIR2)/$(SED_BINARY)
	@if [ -L $(TARGET_DIR)/$(SED_TARGET_BINARY) ] ; then \
		rm -f $(TARGET_DIR)/$(SED_TARGET_BINARY); fi;

	@if [ ! -f $(SED_DIR2)/$(SED_BINARY) -o $(TARGET_DIR)/$(SED_TARGET_BINARY) \
	-ot $(SED_DIR2)/$(SED_BINARY) ] ; then \
	    set -x; \
	    $(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(SED_DIR2) install; \
	    mv $(TARGET_DIR)/usr/bin/sed $(TARGET_DIR)/bin/; \
	    rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		    $(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc; fi

sed: uclibc sed-target_binary

sed-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(SED_DIR2) uninstall
	-$(MAKE) -C $(SED_DIR2) clean

sed-dirclean:
	rm -rf $(SED_DIR2)


