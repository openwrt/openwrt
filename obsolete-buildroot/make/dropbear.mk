######################################################
#
#  An example makefile to fetch a package from sources
#  then fetch the ipkg updates required to the base package
#  extract the archives into the build tree
#  and then build the source
#
######################################################


#  For this example we'll use a fairly simple package that compiles easily
#  and has sources available for download at sourceforge
DROPBEAR=dropbear-0.44
DROPBEAR_TARGET=.built
DROPBEAR_DIR=$(BUILD_DIR)/$(DROPBEAR)
DROPBEAR_IPK=$(BUILD_DIR)/$(DROPBEAR)_mipsel.ipk
DROPBEAR_IPK_DIR=$(BUILD_DIR)/$(DROPBEAR)-ipk

DROPBEAR_SITE=http://matt.ucc.asn.au/dropbear/
DROPBEAR_SOURCE=$(DROPBEAR).tar.bz2


# We need to download sources if we dont have them
$(DL_DIR)/$(DROPBEAR_SOURCE) :
	$(WGET) -P $(DL_DIR) $(DROPBEAR_SITE)/$(DROPBEAR_SOURCE)

# if we have the sources, they do no good unless they are unpacked
$(DROPBEAR_DIR)/.unpacked:	$(DL_DIR)/$(DROPBEAR_SOURCE)
	bzcat $(DL_DIR)/$(DROPBEAR_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	cat $(SOURCE_DIR)/openwrt/ipkg/dropbear/dropbear.patch | patch -p1 -d $(DROPBEAR_DIR)
	touch $(DROPBEAR_DIR)/.unpacked
	
# if we have the sources unpacked, we need to configure them
$(DROPBEAR_DIR)/.configured:	$(DROPBEAR_DIR)/.unpacked
	(cd $(DROPBEAR_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		LD=$(TARGET_CROSS)gcc \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--disable-zlib \
	);
	touch $(DROPBEAR_DIR)/.configured
	
	
# now that we have it all in place, just build it
$(DROPBEAR_DIR)/$(DROPBEAR_TARGET):	$(DROPBEAR_DIR)/.configured
	mkdir -p $(DROPBEAR_IPK_DIR)/usr/bin
	cd $(DROPBEAR_DIR) && make PROGRAMS="dbclient dropbear" MULTI=1 && rm -f ssh && ln -s dbclient ssh
	$(STAGING_DIR)/bin/sstrip $(DROPBEAR_DIR)/dropbearmulti
	cd $(DROPBEAR_DIR) && cp -dpf dropbearmulti ssh dropbear dbclient $(DROPBEAR_IPK_DIR)/usr/bin
	
	cd $(DROPBEAR_DIR) && make SCPPROGRESS=1 scp
	$(STAGING_DIR)/bin/sstrip $(DROPBEAR_DIR)/scp
	cd $(DROPBEAR_DIR) && cp -dpf scp $(DROPBEAR_IPK_DIR)/usr/bin
	
	cd $(DROPBEAR_DIR) && make clean && make dropbearkey 
	$(STAGING_DIR)/bin/sstrip $(DROPBEAR_DIR)/dropbearkey
	mkdir -p $(DROPBEAR_IPK_DIR)/tmp
	cd $(DROPBEAR_DIR) && cp -dpf dropbearkey $(DROPBEAR_IPK_DIR)/tmp/dropbearkey
	touch $(DROPBEAR_DIR)/$(DROPBEAR_TARGET)
	
$(DROPBEAR_IPK): uclibc $(DROPBEAR_DIR)/$(DROPBEAR_TARGET)
	mkdir -p $(DROPBEAR_IPK_DIR)/CONTROL
	cp $(SOURCE_DIR)/openwrt/ipkg/dropbear/CONTROL/conffiles $(DROPBEAR_IPK_DIR)/CONTROL
	cp $(SOURCE_DIR)/openwrt/ipkg/dropbear/CONTROL/control $(DROPBEAR_IPK_DIR)/CONTROL
	cp $(SOURCE_DIR)/openwrt/ipkg/dropbear/CONTROL/postinst $(DROPBEAR_IPK_DIR)/CONTROL
	mkdir -p $(DROPBEAR_IPK_DIR)/etc/init.d
	cp $(SOURCE_DIR)/openwrt/ipkg/dropbear/S51dropbear $(DROPBEAR_IPK_DIR)/etc/init.d

	cd $(BUILD_DIR); $(IPKG_BUILD) $(DROPBEAR_IPK_DIR)

dropbear-ipk: $(DROPBEAR_IPK)
