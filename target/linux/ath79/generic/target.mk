BOARDNAME:=Generic

DEFAULT_PACKAGES += wpad-basic-mbedtls kmod-phy-broadcom kmod-phy-marvell kmod-phy-realtek \
		    kmod-switch-ip17xx kmod-phy-intel-xway kmod-phy-vitesse kmod-phy-micrel

define Target/Description
	Build firmware images for generic Atheros AR71xx/AR913x/AR934x based boards.
endef
