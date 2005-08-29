/*
This file is part of Wi-viz (http://wiviz.natetrue.com).

Wi-viz is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License v2 as published by
the Free Software Foundation.

Wi-viz is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wi-viz; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
//wl_access.h - functions for accessing broadcom crap

#define WL_DEVICE "eth1"

typedef unsigned int uint32;
typedef unsigned char uchar;
typedef int bool;

typedef struct ether_addr {
	uchar addr[6];
} ether_addr_t;

typedef struct wlc_ssid {
	uint32		SSID_len;
	uchar		SSID[32];
} wlc_ssid_t;
/* For ioctls that take a list of MAC addresses */
typedef struct maclist {
	uint count;			/* number of MAC addresses */
	struct ether_addr ea[1];	/* variable length array of MAC addresses */
} maclist_t;
/* Linux network driver ioctl encoding */
typedef struct wl_ioctl {
	uint cmd;	/* common ioctl definition */
	void *buf;	/* pointer to user buffer */
	uint len;	/* length of user buffer */
	bool set;	/* get or set request (optional) */
	uint used;	/* bytes read or written (optional) */
	uint needed;	/* bytes needed (optional) */
} wl_ioctl_t;
/* channel encoding */
typedef struct channel_info {
	int hw_channel;
	int target_channel;
	int scan_channel;
} channel_info_t;
/* RSSI info for sta */
typedef struct sta_rssi {
  int RSSI;
  char mac[6];
  u_short zero_ex_forty_one;
  } sta_rssi_t;
/* check this magic number */
#define WLC_IOCTL_MAGIC		0x14e46c77

#define WLC_GET_MAGIC				0
#define WLC_GET_BSSID				23
#define WLC_SET_BSSID				24
#define WLC_GET_SSID				25
#define WLC_SET_SSID				26
#define WLC_GET_CHANNEL				29
#define WLC_SET_CHANNEL				30
#define WLC_GET_MONITOR				107     /* discovered by nbd */
#define WLC_SET_MONITOR				108     /* discovered by nbd */
#define WLC_GET_AP				117
#define WLC_SET_AP				118
#define WLC_GET_RSSI				127
#define WLC_GET_ASSOCLIST			159


int wl_ioctl(char *name, int cmd, void *buf, int len);
