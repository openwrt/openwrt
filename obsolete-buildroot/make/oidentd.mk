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
OIDENTD=oidentd-2.0.7
OIDENTD_TARGET=oidentd
OIDENTD_DIR=$(BUILD_DIR)/oidentd-2.0.7
OIDENTD_IPKTARGET=oidentd-2.0.7_mipsel.ipk
OIDENTD_SITE=http://easynews.dl.sourceforge.net/sourceforge/ojnk
OIDENTD_SOURCE=oidentd-2.0.7.tar.gz
OIDENTDIPK_SITE=http://openwrt.rozeware.bc.ca/ipkg-dev
OIDENTDIPK_SRC=oidentd-2.0.7-pkg.tgz




# define a target for the master makefile
oidentd:	$(OIDENTD_DIR)/$(OIDENTD_TARGET)

oidentd-ipk:	$(BUILD_DIR)/$(OIDENTD_IPKTARGET) 

# We need to download sources if we dont have them
$(DL_DIR)/$(OIDENTD_SOURCE) :
	$(WGET) -P $(DL_DIR) $(OIDENTD_SITE)/$(OIDENTD_SOURCE)

# As well as the upstream package sources, we need the updates
# for ipkg packaging
$(DL_DIR)/$(OIDENTDIPK_SRC)  :  
	$(WGET) -P $(DL_DIR) $(OIDENTDIPK_SITE)/$(OIDENTDIPK_SRC)

# if we have the sources, they do no good unless they are unpacked
$(OIDENTD_DIR)/.unpacked:	$(DL_DIR)/$(OIDENTD_SOURCE)
	tar -C $(BUILD_DIR) -zxf $(DL_DIR)/$(OIDENTD_SOURCE)
	touch $(OIDENTD_DIR)/.unpacked
	
# if we have the sources unpacked, we need to configure them
$(OIDENTD_DIR)/.configured:	$(OIDENTD_DIR)/.unpacked
	(cd $(OIDENTD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
	);
	touch $(OIDENTD_DIR)/.configured
	
	
# with the upstream sources unpacked, they still dont do much good without
# the ipkg control and rule files
$(OIDENTD_DIR)/ipkg/rules  :	$(DL_DIR)/$(OIDENTDIPK_SRC) $(OIDENTD_DIR)/.unpacked
	tar -C $(OIDENTD_DIR) -zxf $(DL_DIR)/$(OIDENTDIPK_SRC)
	
# now that we have it all in place, just build it
$(OIDENTD_DIR)/$(OIDENTD_TARGET):	$(OIDENTD_DIR)/.configured
	cd $(OIDENTD_DIR); make
	

$(BUILD_DIR)/$(OIDENTD_IPKTARGET):	$(OIDENTD_DIR)/$(OIDENTD_TARGET) $(OIDENTD_DIR)/ipkg/rules
	(cd $(OIDENTD_DIR); ipkg-buildpackage)
	
	
		
