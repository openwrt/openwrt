include $(TOPDIR)/rules.mk

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
HOST_SED_TARGET=$(shell ./sedcheck.sh)

$(DL_DIR)/$(SED_SOURCE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(SED_SOURCE) x $(SED_SITE)

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

source: 
prepare: 
compile: 
install: $(HOST_SED_TARGET)
clean: 
	rm -rf $(SED_DIR1)
	rm -f $(STAGING_DIR)/$(SED_TARGET_BINARY)

