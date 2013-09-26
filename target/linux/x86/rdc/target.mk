BOARDNAME:=RDC R-321x
FEATURES:=squashfs pci usb gpio pcmcia
MAINTAINER:=Florian Fainelli <florian@openwrt.org>

DEFAULT_PACKAGES += kmod-llc kmod-stp kmod-bridge kmod-8021q \
		    wpad-mini kmod-libphy kmod-r6040 kmod-input-core \
		    kmod-input-polldev kmod-button-hotplug kmod-rdc321x-wdt

define Target/Description
        Build firmware images for RDC321x based routers
        (e.g. Airlink101 AR525W, Linksys WRT54GR, Sitecom WL-153)
endef
