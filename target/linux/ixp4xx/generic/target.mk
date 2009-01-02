BOARDNAME:=Generic

DEFAULT_PACKAGES+= hostapd-mini

define Target/Description
	Build firmware images for ixp4xx based boards that boot from internal flash
	(e.g : Linksys NSLU2, ...)
endef

