#############################################################
#
# boa
#
#############################################################

BOA_VERSION=0.94.14rc4

# Don't alter below this line unless you (think) you know
# what you are doing! Danger, Danger!

BOA_SOURCE=boa-$(BOA_VERSION).tar.gz
BOA_SITE=http://www.boa.org/
BOA_DIR=$(BUILD_DIR)/${shell basename $(BOA_SOURCE) .tar.gz}
BOA_WORKDIR=$(BUILD_DIR)/boa_workdir

$(DL_DIR)/$(BOA_SOURCE):
	$(WGET) -P $(DL_DIR) $(BOA_SITE)/$(BOA_SOURCE)

$(BOA_DIR)/.unpacked:	$(DL_DIR)/$(BOA_SOURCE)
	gzip -d -c $(DL_DIR)/$(BOA_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(BOA_DIR)/.unpacked

$(BOA_WORKDIR)/Makefile: $(BOA_DIR)/.unpacked
	rm -f $(BOA_WORKDIR)/Makefile
	mkdir -p $(BOA_WORKDIR)
	(cd $(BOA_WORKDIR) && CONFIG_SITE=$(SOURCE_DIR)/boa-config.site-$(ARCH) \
		CC=$(TARGET_CC) $(BOA_DIR)/configure)
	touch $(BOA_WORKDIR)/.depend
        
$(BOA_WORKDIR)/boa $(BOA_WORKDIR)/boa_indexer:	$(BOA_WORKDIR)/Makefile
	rm -f $@
	$(MAKE) VPATH=$(BOA_DIR)/src/ -C $(BOA_WORKDIR)

$(BOA_WORKDIR)/.installed: $(BOA_WORKDIR)/boa $(BOA_WORKDIR)/boa_indexer
	mkdir -p $(TARGET_DIR)/usr/sbin
	cp -f $(BOA_WORKDIR)/src/boa $(TARGET_DIR)/usr/sbin/boa
	mkdir -p $(TARGET_DIR)/usr/lib/boa
	cp -f $(BOA_WORKDIR)/src/boa_indexer $(TARGET_DIR)/usr/lib/boa/boa_indexer
	mkdir -p $(TARGET_DIR)/etc/boa
	cp -f $(SOURCE_DIR)/boa.conf $(TARGET_DIR)/etc/boa
	cp -f $(SOURCE_DIR)/mime.types $(TARGET_DIR)/etc/mime.types
	strip --strip-all $(TARGET_DIR)/usr/sbin/boa $(TARGET_DIR)/usr/lib/boa/boa_indexer
	touch $(BOA_WORKDIR)/.installed

boa:	uclibc $(BOA_WORKDIR)/.installed

boa-source: $(DL_DIR)/$(BOA_SOURCE)

boa-clean:
	@if [ -d $(BOA_WORKDIR)/Makefile ] ; then \
		$(MAKE) -C $(BOA_WORKDIR) clean ; \
	fi;

boa-dirclean:
	rm -rf $(BOA_DIR) $(BOA_WORKDIR)

