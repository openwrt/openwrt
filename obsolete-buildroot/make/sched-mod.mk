######################################################
#
#  A really simple example that doesn't actually build anything
#  It'll fetch a package from another repository for inclusion
#  into our final list
#  Using one from the openwrt home as an example
#
######################################################


SCHEDM=sched-modules

SCHEDM_TARGET=sched-modules_2.4.20_mipsel.ipk
SCHEDM_SITE=http://openwrt.rozeware.bc.ca/ipkg-dev
SCHEDM_SOURCE=openwrt-kmodules.tar.bz2
SCHEDMIPK_SRC=sched-modules-ipk.tgz


sched-modules-ipk:	$(BUILD_DIR)/$(SCHEDM_TARGET)


# fetch the ipkg rules stuff
$(DL_DIR)/$(SCHEDMIPK_SRC)  :  
	$(WGET) -P $(DL_DIR) $(SCHEDM_SITE)/$(SCHEDMIPK_SRC)
	
#make sure we have a directory to extract to
$(BUILD_DIR)/$(SCHEDM) :
	mkdir $(BUILD_DIR)/$(SCHEDM)
		
#extract the archive
$(BUILD_DIR)/$(SCHEDM)/modules/2.4.20/net/sched/cls_fw.o :	$(BUILD_DIR)/$(SCHEDM) $(SCHEDM_SOURCE)
	tar -C $(BUILD_DIR)/$(SCHEDM) -jxf $(SCHEDM_SOURCE)
	
# extract the ipkg overlay
$(BUILD_DIR)/$(SCHEDM)/ipkg/control : $(DL_DIR)/$(SCHEDMIPK_SRC) $(BUILD_DIR)/$(SCHEDM)/modules/2.4.20/net/sched/cls_fw.o
	tar -C $(BUILD_DIR)/$(SCHEDM) -zxf $(DL_DIR)/$(SCHEDMIPK_SRC)

# now that we have it all in place, just build it
$(BUILD_DIR)/$(SCHEDM_TARGET):	$(BUILD_DIR)/$(SCHEDM)/ipkg/control $(BUILD_DIR)/$(SCHEDM)/modules/2.4.20/net/sched/cls_fw.o
	cd $(BUILD_DIR)/$(SCHEDM); $(IPKG_BUILDPACKAGE)
 		



