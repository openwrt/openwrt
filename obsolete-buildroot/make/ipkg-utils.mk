#############################################################
#
# ipkg-utils for use on the host system
#
#############################################################

IPKG_UTILS_VERSION:=1.7
IPKG_UTILS_SITE:=http://handhelds.org/packages/ipkg-utils/
IPKG_UTILS_SOURCE:=ipkg-utils-$(IPKG_UTILS_VERSION).tar.gz
IPKG_UTILS_DIR:=$(TOOL_BUILD_DIR)/ipkg-utils-$(IPKG_UTILS_VERSION)

IPKG_UTILS_PATCHES += ipkg-utils-1.7-ipkg_buildpackage.patch
IPKG_UTILS_PATCHES += ipkg-utils-1.7-ipkg_build_clean.patch

$(DL_DIR)/$(IPKG_UTILS_SOURCE):
	$(WGET) -P $(DL_DIR) $(IPKG_UTILS_SITE)/$(IPKG_UTILS_SOURCE)

$(IPKG_UTILS_DIR)/.unpacked: $(DL_DIR)/$(IPKG_UTILS_SOURCE)
	mkdir -p $(TOOL_BUILD_DIR)
	mkdir -p $(DL_DIR)
	zcat $(DL_DIR)/$(IPKG_UTILS_SOURCE) | tar -C $(TOOL_BUILD_DIR) -xvf -
	cd $(SOURCE_DIR); cat $(IPKG_UTILS_PATCHES) | patch -p1 -d $(IPKG_UTILS_DIR)
	touch $(IPKG_UTILS_DIR)/.unpacked

$(STAGING_DIR)/bin/ipkg-build: $(IPKG_UTILS_DIR)/.unpacked
	mkdir -p $(STAGING_DIR)/bin
	install -m0755 $(IPKG_UTILS_DIR)/ipkg-build* $(STAGING_DIR)/bin
	install -m0755 $(IPKG_UTILS_DIR)/ipkg-make-index $(STAGING_DIR)/bin
	install -m0755 $(IPKG_UTILS_DIR)/ipkg.py $(STAGING_DIR)/bin

ipkg-utils: $(STAGING_DIR)/bin/ipkg-build

ipkg-utils-clean:
	rm -f $(STAGING_DIR)/bin/ipkg*

ipkg-utils-dirclean:
	rm -rf $(IPKG_UTILS_DIR)


IPKG_BUILDPACKAGE := PATH=$(TARGET_PATH) ipkg-buildpackage -c -o root -g root
IPKG_BUILD := PATH=$(TARGET_PATH) ipkg-build -c -o root -g root
IPKG_MAKE_INDEX := PATH=$(TARGET_PATH) ipkg-make-index

