#############################################################
#
# iproute2
#
#############################################################

IPROUTE2_DIR=$(BUILD_DIR)/iproute2

#IPROUTE2_SOURCE_URL=ftp://ftp.inr.ac.ru/ip-routing/
#IPROUTE2_SOURCE=iproute2-2.4.7-now-ss020116-try.tar.gz

#Use the debian source for now, as the .ru site has availability problems
IPROUTE2_SOURCE_URL=http://ftp.debian.org/debian/pool/main/i/iproute/
IPROUTE2_SOURCE=iproute_20010824.orig.tar.gz
IPROUTE2_PATCH:=iproute_20010824-8.diff.gz
IPROUTE2_PATCH_2:=iproute2-cross-ar-20010824.patch

IPROUTE2_IPKTARGET=iproute.ipk
IPROUTE2_IPKSRC:=iproute-pkg.tgz
IPROUTE2_IPKSITE:=http://openwrt.rozeware.bc.ca/ipkg-dev


$(DL_DIR)/$(IPROUTE2_SOURCE):
	 $(WGET) -P $(DL_DIR) $(IPROUTE2_SOURCE_URL)$(IPROUTE2_SOURCE)

$(DL_DIR)/$(IPROUTE2_PATCH):
	$(WGET) -P $(DL_DIR) $(IPROUTE2_SOURCE_URL)/$(IPROUTE2_PATCH)

$(DL_DIR)/$(IPROUTE2_IPKSRC):
	$(WGET) -P $(DL_DIR) $(IPROUTE2_IPKSITE)/$(IPROUTE2_IPKSRC)


iproute2-source: $(DL_DIR)/$(IPROUTE2_SOURCE) #$(DL_DIR)/$(IPROUTE2_PATCH)

$(IPROUTE2_DIR)/.unpacked: $(DL_DIR)/$(IPROUTE2_SOURCE) #$(DL_DIR)/$(IPROUTE2_PATCH)
	rm -rf $(IPROUTE2_DIR).orig $(IPROUTE2_DIR)
	zcat $(DL_DIR)/$(IPROUTE2_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	#zcat $(DL_DIR)/$(IPROUTE2_PATCH) | patch -p1 -d $(IPROUTE2_DIR)
	cat $(SOURCE_DIR)/$(IPROUTE2_PATCH_2) | patch -p1 -d $(IPROUTE2_DIR)
	# fun sed hacks.
	$(SED) "s:-O2:${TARGET_CFLAGS}:g" $(IPROUTE2_DIR)/Makefile
ifeq ($(ARCH),i386)
	@# needed on atleast i386.. however breaks on mipsel
	$(SED) 's:-I\.\./include-glibc::g' $(IPROUTE2_DIR)/Makefile
endif
	touch $(IPROUTE2_DIR)/.unpacked

$(IPROUTE2_DIR)/.configured: $(IPROUTE2_DIR)/.unpacked
	$(SED) "s,-I/usr/include/db3,," $(IPROUTE2_DIR)/Makefile
	$(SED) "s,^KERNEL_INCLUDE.*,KERNEL_INCLUDE=$(LINUX_DIR)/include," \
		$(IPROUTE2_DIR)/Makefile
	$(SED) "s,^LIBC_INCLUDE.*,LIBC_INCLUDE=$(STAGING_DIR)/include," \
		$(IPROUTE2_DIR)/Makefile
	# For now disable compiling of the misc directory because it seems to fail
	rm -rf $(IPROUTE2_DIR)/misc 
	$(SED) "s, misc,," $(IPROUTE2_DIR)/Makefile
	touch  $(IPROUTE2_DIR)/.configured

$(IPROUTE2_DIR)/tc/tc: $(IPROUTE2_DIR)/.configured
	$(MAKE) -C $(IPROUTE2_DIR) $(TARGET_CONFIGURE_OPTS) KERNEL_INCLUDE=$(LINUX_DIR)/include
	$(STRIP) $(IPROUTE2_DIR)/tc/tc

$(TARGET_DIR)/usr/sbin/tc: $(IPROUTE2_DIR)/tc/tc
	@# Make sure our $(TARGET_DIR)/usr/sbin/ exists.
	-mkdir -p $(TARGET_DIR)/usr/sbin/
	@# Copy The tc binary
	cp -af $(IPROUTE2_DIR)/tc/tc $(TARGET_DIR)/usr/sbin/ && \
		$(STRIP) $(TARGET_DIR)/usr/sbin/tc

iproute2: $(TARGET_DIR)/usr/sbin/tc

iproute2-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(IPROUTE2_DIR) uninstall
	-$(MAKE) -C $(IPROUTE2_DIR) clean

iproute2-dirclean:
	rm -rf $(IPROUTE2_DIR)
	
iproute2-ipk:	$(IPROUTE2_IPKTARGET)

$(IPROUTE2_IPKTARGET):	$(IPROUTE2_DIR)/ipkg/rules
	(cd $(IPROUTE2_DIR); $(IPKG_BUILDPACKAGE) )
	
$(IPROUTE2_DIR)/ipkg/rules:	$(IPROUTE2_DIR)/tc/tc $(DL_DIR)/$(IPROUTE2_IPKSRC)
	tar -C $(IPROUTE2_DIR) -zxf $(DL_DIR)/$(IPROUTE2_IPKSRC)
	
