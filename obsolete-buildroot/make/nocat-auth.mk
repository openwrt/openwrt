#############################################################
#
# nocatauth.c
#
# There exists a real need in the embedded community for a 
# captive portal with authentication. Till now I've been hording 
# this sweet little creation to myself.
#
# NoCatSplash is a Open Public Network Gateway Daemon. It performs as a 
# [captive/open/active] portal. When run on a gateway/router on a 
# network, all web requests are redirected until the client clicks I
# Accept to an AUP. The gateway daemon then changes the firewall rules
# on the gateway to pass traffic for that client (based on IP address
# and MAC address).
#
# NoCatSplash is the successor to NoCatAuth, which was written in Perl. 
# NoCatSplash is written in multi-threaded ANSI C in order to be smaller
# and work better on embedded style devices.
#
# NoCatSplash-Auth is the successor to them all which is NoCatSplash 
# with the features found on NoCatAuth but written in ANSI C in order to
# be smaller and work better on embedded style devices.
#
#############################################################
NOCATSPLASHAUTH_SOURCE_URL:=
NOCATSPLASHAUTH_SOURCE=NoCatSplash-auth.tgz
NOCATSPLASHAUTH_BUILD_DIR=$(BUILD_DIR)/NoCatSplash-auth

$(DL_DIR)/$(NOCATSPLASHAUTH_SOURCE):
	 $(WGET) -P $(DL_DIR) $(NOCATSPLASHAUTH_SOURCE_URL)/$(NOCATSPLASHAUTH_SOURCE) 

$(NOCATSPLASHAUTH_BUILD_DIR)/.unpacked: $(DL_DIR)/$(NOCATSPLASHAUTH_SOURCE)
	zcat $(DL_DIR)/$(NOCATSPLASHAUTH_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(NOCATSPLASHAUTH_BUILD_DIR)/.unpacked

$(NOCATSPLASHAUTH_BUILD_DIR)/.configured: $(NOCATSPLASHAUTH_BUILD_DIR)/.unpacked
	# Allow patches.  Needed for openwrt for instance.
	#$(SOURCE_DIR)/patch-kernel.sh $(NOCATSPLASHAUTH_BUILD_DIR) $(SOURCE_DIR) nocatsplash-*.patch
	(cd $(NOCATSPLASHAUTH_BUILD_DIR); \
		./configure \
			--prefix=/usr \
			--disable-glibtest \
			--with-glib-prefix=$(TARGET_DIR)/usr \
			--with-mode=passive \
	)
	touch  $(NOCATSPLASHAUTH_BUILD_DIR)/.configured

$(NOCATSPLASHAUTH_BUILD_DIR)/splashd: $(NOCATSPLASHAUTH_BUILD_DIR)/.configured
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) -C $(NOCATSPLASHAUTH_BUILD_DIR) \
		CC=$(TARGET_CC) COPT_FLAGS="$(TARGET_CFLAGS)"

$(TARGET_DIR)/sbin/splashd: $(NOCATSPLASHAUTH_BUILD_DIR)/splashd
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) -C $(NOCATSPLASHAUTH_BUILD_DIR) \
		PREFIX=/usr \
		CC=$(TARGET_CC) COPT_FLAGS="$(TARGET_CFLAGS)" \
		DESTDIR=$(TARGET_DIR) install
	$(STRIP) $(TARGET_DIR)/usr/bin/nocat*
	rm -rf $(TARGET_DIR)/usr/man

nocatauth: $(TARGET_DIR)/sbin/splashd

nocatauth-source: $(DL_DIR)/$(NOCATSPLASHAUTH_SOURCE)

nocatauth-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(NOCATSPLASHAUTH_BUILD_DIR) uninstall
	-$(MAKE) -C $(NOCATSPLASHAUTH_BUILD_DIR) clean

nocatauth-dirclean:
	rm -rf $(NOCATSPLASHAUTH_BUILD_DIR)

