#ifndef _LINUX_ATMRT2684_H
#define _LINUX_ATMRT2684_H

#include <linux/atm.h>
#include <linux/if.h>		/* For IFNAMSIZ */

#define RT2684_ENCAPS_NULL	(0)	/* VC-mux */
#define RT2684_ENCAPS_LLC	(1)
#define RT2684_ENCAPS_AUTODETECT (2)	/* Unsuported */

/*
 * This is for the ATM_NEWBACKENDIF call - these are like socket families:
 * the first element of the structure is the backend number and the rest
 * is per-backend specific
 */
struct atm_newif_rt2684 {
	atm_backend_t	backend_num;	/* ATM_BACKEND_RT2684 */
	char		ifname[IFNAMSIZ];
};

/*
 * This structure is used to specify a rt2684 interface - either by a
 * positive integer (returned by ATM_NEWBACKENDIF) or the interfaces name
 */
#define RT2684_FIND_BYNOTHING	(0)
#define RT2684_FIND_BYNUM	(1)
#define RT2684_FIND_BYIFNAME	(2)
struct rt2684_if_spec {
	int method;			/* RT2684_FIND_* */
	union {
		char		ifname[IFNAMSIZ];
		int		devnum;
	} spec;
};

/*
 * This is for the ATM_SETBACKEND call - these are like socket families:
 * the first element of the structure is the backend number and the rest
 * is per-backend specific
 */
struct atm_backend_rt2684 {
	atm_backend_t	backend_num;	/* ATM_BACKEND_RT2684 */
	struct rt2684_if_spec ifspec;
	unsigned char	encaps;		/* RT2684_ENCAPS_* */
};


#endif /* _LINUX_ATMRT2684_H */
