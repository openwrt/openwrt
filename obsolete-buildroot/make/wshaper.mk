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
WSHAPER=wondershaper-1.1a
WSHAPER_TARGET=wondershaper-1.1a_mipsel.ipk
WSHAPER_SITE=http://lartc.org/wondershaper
WSHAPER_SOURCE=wondershaper-1.1a.tar.gz
WSHAPERIPK_SITE=http://openwrt.rozeware.bc.ca/ipkg-dev
WSHAPERIPK_SRC=wondershaper-1.1a-pkg.tgz



# define a target for the master makefile
wshaper-ipk:	$(BUILD_DIR)/$(WSHAPER_TARGET)

# We need to download sources if we dont have them
$(DL_DIR)/$(WSHAPER_SOURCE) :
	$(WGET) -P $(DL_DIR) $(WSHAPER_SITE)/$(WSHAPER_SOURCE)

# As well as the upstream package sources, we need the updates
# for ipkg packaging
$(DL_DIR)/$(WSHAPERIPK_SRC)  :  
	$(WGET) -P $(DL_DIR) $(WSHAPERIPK_SITE)/$(WSHAPERIPK_SRC)

# if we have the sources, they do no good unless they are unpacked
$(BUILD_DIR)/$(WSHAPER)/.unpacked:	$(DL_DIR)/$(WSHAPER_SOURCE)
	tar -C $(BUILD_DIR) -zxf $(DL_DIR)/$(WSHAPER_SOURCE)
	touch $(BUILD_DIR)/$(WSHAPER)/.unpacked
	
# with the upstream sources unpacked, they still dont do much good without
# the ipkg control and rule files
$(BUILD_DIR)/$(WSHAPER)/ipkg/control  :	$(BUILD_DIR)/$(WSHAPER)/.unpacked $(DL_DIR)/$(WSHAPERIPK_SRC) 
	tar -C $(BUILD_DIR)/$(WSHAPER) -zxf $(DL_DIR)/$(WSHAPERIPK_SRC)
	
# now that we have it all in place, just build it
$(BUILD_DIR)/$(WSHAPER_TARGET):	$(BUILD_DIR)/$(WSHAPER)/ipkg/control
	cd $(BUILD_DIR)/$(WSHAPER); $(IPKG_BUILDPACKAGE)
	
		
	
		
