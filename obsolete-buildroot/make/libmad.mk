#############################################################
#
# libmad
#
#############################################################

LIBMAD_VERSION=0.15.0b

# Don't alter below this line unless you (think) you know
# what you are doing! Danger, Danger!

LIBMAD_SOURCE=libmad-$(LIBMAD_VERSION).tar.gz
LIBMAD_SITE=http://easynews.dl.sourceforge.net/sourceforge/mad/
LIBMAD_DIR=$(BUILD_DIR)/${shell basename $(LIBMAD_SOURCE) .tar.gz}
LIBMAD_WORKDIR=$(BUILD_DIR)/libmad-$(LIBMAD_VERSION)

$(DL_DIR)/$(LIBMAD_SOURCE):
	$(WGET) -P $(DL_DIR) $(LIBMAD_SITE)/$(LIBMAD_SOURCE)

$(LIBMAD_DIR)/.unpacked:	$(DL_DIR)/$(LIBMAD_SOURCE)
	gzip -d -c $(DL_DIR)/$(LIBMAD_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(LIBMAD_DIR)/.unpacked

$(LIBMAD_DIR)/.configured: $(LIBMAD_DIR)/.unpacked
	(cd $(LIBMAD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--sysconfdir=/etc \
		$(DISABLE_NLS) \
	);
	touch  $(LIBMAD_DIR)/.configured

$(LIBMAD_WORKDIR)/libmad:	$(LIBMAD_DIR)/.configured
	rm -f $@
	$(MAKE) CC=$(TARGET_CC) -C $(LIBMAD_WORKDIR)

$(LIBMAD_WORKDIR)/.installed: 	$(LIBMAD_WORKDIR)/libmad
	$(MAKE) prefix=$(TARGET_DIR)/usr -C $(LIBMAD_WORKDIR) install
	touch $(LIBMAD_WORKDIR)/.installed

libmad:	uclibc $(LIBMAD_WORKDIR)/.installed

libmad-source: $(DL_DIR)/$(LIBMAD_SOURCE)

libmad-clean:
	@if [ -d $(LIBMAD_WORKDIR)/Makefile ] ; then \
		$(MAKE) -C $(LIBMAD_WORKDIR) clean ; \
	fi;

libmad-dirclean:
	rm -rf $(LIBMAD_DIR) $(LIBMAD_WORKDIR)

