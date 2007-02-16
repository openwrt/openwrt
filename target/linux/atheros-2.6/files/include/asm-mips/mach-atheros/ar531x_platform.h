#ifndef __AR531X_PLATFORM_H
#define __AR531X_PLATFORM_H

/*
 * Board support data.  The driver is required to locate
 * and fill-in this information before passing a reference to
 * this structure as the HAL_BUS_TAG parameter supplied to
 * ath_hal_attach.
 */
struct ar531x_config {
	const char  *board;	/* board config data */
	const char	*radio;			/* radio config data */
	int		unit;			/* unit number [0, 1] */
	u32		tag;			/* used as devid for now */
};

struct ar531x_eth {
	int phy;
	int mac;
	u32 reset_base;
	u32 reset_mac;
	u32 reset_phy;
	u32 phy_base;
	char *board_config;
	char *macaddr;
};

#endif /* __AR531X_PLATFORM_H */
