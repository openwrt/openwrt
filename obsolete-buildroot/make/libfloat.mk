#############################################################
#
# libfloat
#
#############################################################
LIBFLOAT_SOURCE:=libfloat_990616.orig.tar.gz
LIBFLOAT_PATCH:=libfloat_990616-3.diff.gz
LIBFLOAT_SITE:=http://ftp.debian.org/debian/pool/main/libf/libfloat
LIBFLOAT_CAT:=zcat
LIBFLOAT_DIR:=$(BUILD_DIR)/libfloat

LIBFLOAT_TARGET=
ifeq ($(strip $(SOFT_FLOAT)),true)
ifeq ("$(strip $(ARCH))","arm")
LIBFLOAT_TARGET+=$(STAGING_DIR)/lib/libfloat.so
endif
endif

$(DL_DIR)/$(LIBFLOAT_SOURCE):
	 $(WGET) -P $(DL_DIR) $(LIBFLOAT_SITE)/$(LIBFLOAT_SOURCE)

$(DL_DIR)/$(LIBFLOAT_PATCH):
	 $(WGET) -P $(DL_DIR) $(LIBFLOAT_SITE)/$(LIBFLOAT_PATCH)

libfloat-source: $(DL_DIR)/$(LIBFLOAT_SOURCE) $(DL_DIR)/$(LIBFLOAT_PATCH)

$(LIBFLOAT_DIR)/.unpacked: $(DL_DIR)/$(LIBFLOAT_SOURCE) $(DL_DIR)/$(LIBFLOAT_PATCH)
	$(LIBFLOAT_CAT) $(DL_DIR)/$(LIBFLOAT_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	# Remove the binary files distributed with the the package.
	make -C $(LIBFLOAT_DIR) clean
	$(SOURCE_DIR)/patch-kernel.sh $(LIBFLOAT_DIR) $(DL_DIR) $(LIBFLOAT_PATCH)
	$(SOURCE_DIR)/patch-kernel.sh $(LIBFLOAT_DIR) $(SOURCE_DIR) libfloat.patch
	touch $(LIBFLOAT_DIR)/.unpacked

$(LIBFLOAT_DIR)/libfloat.so.1: $(LIBFLOAT_DIR)/.unpacked $(TARGET_CC)
	$(MAKE) CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld -C $(LIBFLOAT_DIR)

$(STAGING_DIR)/lib/libfloat.so: $(LIBFLOAT_DIR)/libfloat.so.1
	cp -a $(LIBFLOAT_DIR)/libfloat.a $(STAGING_DIR)/lib/libfloat.a
	cp -a $(LIBFLOAT_DIR)/libfloat.so.1 $(STAGING_DIR)/lib/libfloat.so.1
	(cd $(STAGING_DIR)/lib ; ln -sf libfloat.so.1 libfloat.so)
	cp -a $(LIBFLOAT_DIR)/libfloat.a $(TARGET_DIR)/usr/lib/libfloat.a
	cp -a $(LIBFLOAT_DIR)/libfloat.so.1 $(TARGET_DIR)/lib/libfloat.so.1
	$(STRIP) $(TARGET_DIR)/lib/libfloat.so.1 > /dev/null 2>&1
	(cd $(TARGET_DIR)/lib ; ln -sf libfloat.so.1 libfloat.so)
	(cd $(TARGET_DIR)/usr/lib ; ln -sf /lib/libfloat.so libfloat.so)

libfloat: $(STAGING_DIR)/lib/libfloat.so

libfloat-clean:
	-$(MAKE) -C $(LIBFLOAT_DIR) clean

libfloat-dirclean:
	rm -rf $(LIBFLOAT_DIR)

