#############################################################
#
# jffsbbc
#
#############################################################


JFFSBBC_SOURCE:=bbc-0.54.3.tgz
JFFSBBC_DIR:=$(BUILD_DIR)/bbc-0.54.3
JFFSBBC_SITE:=http://www.inf.u-szeged.hu/jffs2
JFFSBBC_CAT:=zcat

LINUX_DIR:=$(BUILD_DIR)/WRT54GS/release/src/linux/linux

$(DL_DIR)/$(JFFSBBC_SOURCE):
	 $(WGET) -P $(DL_DIR) $(JFFSBBC_SITE)/$(JFFSBBC_SOURCE)

jffsbbc-source: $(DL_DIR)/$(JFFSBBC_SOURCE)

$(JFFSBBC_DIR)/.unpacked: $(DL_DIR)/$(JFFSBBC_SOURCE)
	$(JFFSBBC_CAT) $(DL_DIR)/$(JFFSBBC_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(JFFSBBC_DIR)/.unpacked

$(LINUX_DIR)/.bbc-patched: $(LINUX_DIR)/.patched $(JFFSBBC_DIR)/.unpacked
	-(cd $(JFFSBBC_DIR)/src; ./install.jffs2 linux-2.4.25.hpatch $(LINUX_DIR))
	touch $(LINUX_DIR)/.bbc-patched

jffsbbc: $(LINUX_DIR)/.bbc-patched

jffsbbc-clean:

jffsbbc-dirclean:
	rm -rf $(JFFSBBC_DIR)
