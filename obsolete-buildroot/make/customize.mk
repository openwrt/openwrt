#############################################################
#
# Any custom stuff you feel like doing....
#
#############################################################
CUST_DIR:=$(SOURCE_DIR)/customize
ROOT_DIR:=$(BUILD_DIR)/root

customize:
	cp -af $(CUST_DIR)/* $(ROOT_DIR)/
