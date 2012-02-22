/*
 * iwinfo - Wireless Information Library - Madwifi Backend
 *
 *   Copyright (C) 2009-2010 Jo-Philipp Wich <xm@subsignal.org>
 *
 * The iwinfo library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * The iwinfo library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with the iwinfo library. If not, see http://www.gnu.org/licenses/.
 *
 * The signal handling code is derived from the official madwifi tools,
 * wlanconfig.c in particular. The encryption property handling was
 * inspired by the hostapd madwifi driver.
 */

#include "iwinfo/madwifi.h"
#include "iwinfo/wext.h"


/*
 * Madwifi ISO 3166 to Country/Region Code mapping.
 */

static struct ISO3166_to_CCode
{
	u_int16_t iso3166;
	u_int16_t ccode;
} CountryCodes[] = {
	{ 0x3030 /* 00 */,   0 }, /* World */
	{ 0x4145 /* AE */, 784 }, /* U.A.E. */
	{ 0x414C /* AL */,   8 }, /* Albania */
	{ 0x414D /* AM */,  51 }, /* Armenia */
	{ 0x4152 /* AR */,  32 }, /* Argentina */
	{ 0x4154 /* AT */,  40 }, /* Austria */
	{ 0x4155 /* AU */,  36 }, /* Australia */
	{ 0x415A /* AZ */,  31 }, /* Azerbaijan */
	{ 0x4245 /* BE */,  56 }, /* Belgium */
	{ 0x4247 /* BG */, 100 }, /* Bulgaria */
	{ 0x4248 /* BH */,  48 }, /* Bahrain */
	{ 0x424E /* BN */,  96 }, /* Brunei Darussalam */
	{ 0x424F /* BO */,  68 }, /* Bolivia */
	{ 0x4252 /* BR */,  76 }, /* Brazil */
	{ 0x4259 /* BY */, 112 }, /* Belarus */
	{ 0x425A /* BZ */,  84 }, /* Belize */
	{ 0x4341 /* CA */, 124 }, /* Canada */
	{ 0x4348 /* CH */, 756 }, /* Switzerland */
	{ 0x434C /* CL */, 152 }, /* Chile */
	{ 0x434E /* CN */, 156 }, /* People's Republic of China */
	{ 0x434F /* CO */, 170 }, /* Colombia */
	{ 0x4352 /* CR */, 188 }, /* Costa Rica */
	{ 0x4359 /* CY */, 196 }, /* Cyprus */
	{ 0x435A /* CZ */, 203 }, /* Czech Republic */
	{ 0x4445 /* DE */, 276 }, /* Germany */
	{ 0x444B /* DK */, 208 }, /* Denmark */
	{ 0x444F /* DO */, 214 }, /* Dominican Republic */
	{ 0x445A /* DZ */,  12 }, /* Algeria */
	{ 0x4543 /* EC */, 218 }, /* Ecuador */
	{ 0x4545 /* EE */, 233 }, /* Estonia */
	{ 0x4547 /* EG */, 818 }, /* Egypt */
	{ 0x4553 /* ES */, 724 }, /* Spain */
	{ 0x4649 /* FI */, 246 }, /* Finland */
	{ 0x464F /* FO */, 234 }, /* Faeroe Islands */
	{ 0x4652 /* FR */, 250 }, /* France */
	{ 0x4652 /* FR */, 255 }, /* France2 */
	{ 0x4742 /* GB */, 826 }, /* United Kingdom */
	{ 0x4745 /* GE */, 268 }, /* Georgia */
	{ 0x4752 /* GR */, 300 }, /* Greece */
	{ 0x4754 /* GT */, 320 }, /* Guatemala */
	{ 0x484B /* HK */, 344 }, /* Hong Kong S.A.R., P.R.C. */
	{ 0x484E /* HN */, 340 }, /* Honduras */
	{ 0x4852 /* HR */, 191 }, /* Croatia */
	{ 0x4855 /* HU */, 348 }, /* Hungary */
	{ 0x4944 /* ID */, 360 }, /* Indonesia */
	{ 0x4945 /* IE */, 372 }, /* Ireland */
	{ 0x494C /* IL */, 376 }, /* Israel */
	{ 0x494E /* IN */, 356 }, /* India */
	{ 0x4951 /* IQ */, 368 }, /* Iraq */
	{ 0x4952 /* IR */, 364 }, /* Iran */
	{ 0x4953 /* IS */, 352 }, /* Iceland */
	{ 0x4954 /* IT */, 380 }, /* Italy */
	{ 0x4A4D /* JM */, 388 }, /* Jamaica */
	{ 0x4A4F /* JO */, 400 }, /* Jordan */
	{ 0x4A50 /* JP */, 392 }, /* Japan */
	{ 0x4A50 /* JP */, 393 }, /* Japan (JP1) */
	{ 0x4A50 /* JP */, 394 }, /* Japan (JP0) */
	{ 0x4A50 /* JP */, 395 }, /* Japan (JP1-1) */
	{ 0x4A50 /* JP */, 396 }, /* Japan (JE1) */
	{ 0x4A50 /* JP */, 397 }, /* Japan (JE2) */
	{ 0x4A50 /* JP */, 399 }, /* Japan (JP6) */
	{ 0x4A50 /* JP */, 900 }, /* Japan */
	{ 0x4A50 /* JP */, 901 }, /* Japan */
	{ 0x4A50 /* JP */, 902 }, /* Japan */
	{ 0x4A50 /* JP */, 903 }, /* Japan */
	{ 0x4A50 /* JP */, 904 }, /* Japan */
	{ 0x4A50 /* JP */, 905 }, /* Japan */
	{ 0x4A50 /* JP */, 906 }, /* Japan */
	{ 0x4A50 /* JP */, 907 }, /* Japan */
	{ 0x4A50 /* JP */, 908 }, /* Japan */
	{ 0x4A50 /* JP */, 909 }, /* Japan */
	{ 0x4A50 /* JP */, 910 }, /* Japan */
	{ 0x4A50 /* JP */, 911 }, /* Japan */
	{ 0x4A50 /* JP */, 912 }, /* Japan */
	{ 0x4A50 /* JP */, 913 }, /* Japan */
	{ 0x4A50 /* JP */, 914 }, /* Japan */
	{ 0x4A50 /* JP */, 915 }, /* Japan */
	{ 0x4A50 /* JP */, 916 }, /* Japan */
	{ 0x4A50 /* JP */, 917 }, /* Japan */
	{ 0x4A50 /* JP */, 918 }, /* Japan */
	{ 0x4A50 /* JP */, 919 }, /* Japan */
	{ 0x4A50 /* JP */, 920 }, /* Japan */
	{ 0x4A50 /* JP */, 921 }, /* Japan */
	{ 0x4A50 /* JP */, 922 }, /* Japan */
	{ 0x4A50 /* JP */, 923 }, /* Japan */
	{ 0x4A50 /* JP */, 924 }, /* Japan */
	{ 0x4A50 /* JP */, 925 }, /* Japan */
	{ 0x4A50 /* JP */, 926 }, /* Japan */
	{ 0x4A50 /* JP */, 927 }, /* Japan */
	{ 0x4A50 /* JP */, 928 }, /* Japan */
	{ 0x4A50 /* JP */, 929 }, /* Japan */
	{ 0x4A50 /* JP */, 930 }, /* Japan */
	{ 0x4A50 /* JP */, 931 }, /* Japan */
	{ 0x4A50 /* JP */, 932 }, /* Japan */
	{ 0x4A50 /* JP */, 933 }, /* Japan */
	{ 0x4A50 /* JP */, 934 }, /* Japan */
	{ 0x4A50 /* JP */, 935 }, /* Japan */
	{ 0x4A50 /* JP */, 936 }, /* Japan */
	{ 0x4A50 /* JP */, 937 }, /* Japan */
	{ 0x4A50 /* JP */, 938 }, /* Japan */
	{ 0x4A50 /* JP */, 939 }, /* Japan */
	{ 0x4A50 /* JP */, 940 }, /* Japan */
	{ 0x4A50 /* JP */, 941 }, /* Japan */
	{ 0x4B45 /* KE */, 404 }, /* Kenya */
	{ 0x4B50 /* KP */, 408 }, /* North Korea */
	{ 0x4B52 /* KR */, 410 }, /* South Korea */
	{ 0x4B52 /* KR */, 411 }, /* South Korea */
	{ 0x4B57 /* KW */, 414 }, /* Kuwait */
	{ 0x4B5A /* KZ */, 398 }, /* Kazakhstan */
	{ 0x4C42 /* LB */, 422 }, /* Lebanon */
	{ 0x4C49 /* LI */, 438 }, /* Liechtenstein */
	{ 0x4C54 /* LT */, 440 }, /* Lithuania */
	{ 0x4C55 /* LU */, 442 }, /* Luxembourg */
	{ 0x4C56 /* LV */, 428 }, /* Latvia */
	{ 0x4C59 /* LY */, 434 }, /* Libya */
	{ 0x4D41 /* MA */, 504 }, /* Morocco */
	{ 0x4D43 /* MC */, 492 }, /* Principality of Monaco */
	{ 0x4D4B /* MK */, 807 }, /* the Former Yugoslav Republic of Macedonia */
	{ 0x4D4F /* MO */, 446 }, /* Macau */
	{ 0x4D58 /* MX */, 484 }, /* Mexico */
	{ 0x4D59 /* MY */, 458 }, /* Malaysia */
	{ 0x4E49 /* NI */, 558 }, /* Nicaragua */
	{ 0x4E4C /* NL */, 528 }, /* Netherlands */
	{ 0x4E4F /* NO */, 578 }, /* Norway */
	{ 0x4E5A /* NZ */, 554 }, /* New Zealand */
	{ 0x4F4D /* OM */, 512 }, /* Oman */
	{ 0x5041 /* PA */, 591 }, /* Panama */
	{ 0x5045 /* PE */, 604 }, /* Peru */
	{ 0x5048 /* PH */, 608 }, /* Republic of the Philippines */
	{ 0x504B /* PK */, 586 }, /* Islamic Republic of Pakistan */
	{ 0x504C /* PL */, 616 }, /* Poland */
	{ 0x5052 /* PR */, 630 }, /* Puerto Rico */
	{ 0x5054 /* PT */, 620 }, /* Portugal */
	{ 0x5059 /* PY */, 600 }, /* Paraguay */
	{ 0x5141 /* QA */, 634 }, /* Qatar */
	{ 0x524F /* RO */, 642 }, /* Romania */
	{ 0x5255 /* RU */, 643 }, /* Russia */
	{ 0x5341 /* SA */, 682 }, /* Saudi Arabia */
	{ 0x5345 /* SE */, 752 }, /* Sweden */
	{ 0x5347 /* SG */, 702 }, /* Singapore */
	{ 0x5349 /* SI */, 705 }, /* Slovenia */
	{ 0x534B /* SK */, 703 }, /* Slovak Republic */
	{ 0x5356 /* SV */, 222 }, /* El Salvador */
	{ 0x5359 /* SY */, 760 }, /* Syria */
	{ 0x5448 /* TH */, 764 }, /* Thailand */
	{ 0x544E /* TN */, 788 }, /* Tunisia */
	{ 0x5452 /* TR */, 792 }, /* Turkey */
	{ 0x5454 /* TT */, 780 }, /* Trinidad y Tobago */
	{ 0x5457 /* TW */, 158 }, /* Taiwan */
	{ 0x5541 /* UA */, 804 }, /* Ukraine */
	{ 0x554B /* UK */, 826 }, /* United Kingdom */
	{ 0x5553 /* US */, 840 }, /* United States */
	{ 0x5553 /* US */, 842 }, /* United States (Public Safety)*/
	{ 0x5559 /* UY */, 858 }, /* Uruguay */
	{ 0x555A /* UZ */, 860 }, /* Uzbekistan */
	{ 0x5645 /* VE */, 862 }, /* Venezuela */
	{ 0x564E /* VN */, 704 }, /* Viet Nam */
	{ 0x5945 /* YE */, 887 }, /* Yemen */
	{ 0x5A41 /* ZA */, 710 }, /* South Africa */
	{ 0x5A57 /* ZW */, 716 }, /* Zimbabwe */
};


static int madwifi_wrq(struct iwreq *wrq, const char *ifname, int cmd, void *data, size_t len)
{
	strncpy(wrq->ifr_name, ifname, IFNAMSIZ);

	if( data != NULL )
	{
		if( len < IFNAMSIZ )
		{
			memcpy(wrq->u.name, data, len);
		}
		else
		{
			wrq->u.data.pointer = data;
			wrq->u.data.length = len;
		}
	}

	return iwinfo_ioctl(cmd, wrq);
}

static int get80211priv(const char *ifname, int op, void *data, size_t len)
{
	struct iwreq iwr;

	if( madwifi_wrq(&iwr, ifname, op, data, len) < 0 )
		return -1;

	return iwr.u.data.length;
}

static char * madwifi_isvap(const char *ifname, const char *wifiname)
{
	int fd, ln;
	char path[32];
	char *ret = NULL;
	static char name[IFNAMSIZ];

	if( strlen(ifname) <= 9 )
	{
		sprintf(path, "/proc/sys/net/%s/%%parent", ifname);

		if( (fd = open(path, O_RDONLY)) > -1 )
		{
			if( wifiname != NULL )
			{
				if( read(fd, name, strlen(wifiname)) == strlen(wifiname) )
					ret = strncmp(name, wifiname, strlen(wifiname))
						? NULL : name;
			}
			else if( (ln = read(fd, name, IFNAMSIZ)) >= 4 )
			{
				name[ln-1] = 0;
				ret = name;
			}

			(void) close(fd);
		}
	}

	return ret;
}

static int madwifi_iswifi(const char *ifname)
{
	int ret;
	char path[32];
	struct stat s;

	ret = 0;

	if( strlen(ifname) <= 7 )
	{
		sprintf(path, "/proc/sys/dev/%s/diversity", ifname);

		if( ! stat(path, &s) )
			ret = (s.st_mode & S_IFREG);
	}

	return ret;
}

static char * madwifi_ifadd(const char *ifname)
{
	char *wifidev = NULL;
	struct ifreq ifr = { 0 };
	struct ieee80211_clone_params cp = { 0 };
	static char nif[IFNAMSIZ] = { 0 };

	if( !(wifidev = madwifi_isvap(ifname, NULL)) && madwifi_iswifi(ifname) )
		wifidev = (char *)ifname;

	if( wifidev )
	{
		snprintf(nif, sizeof(nif), "tmp.%s", ifname);

		strncpy(cp.icp_name, nif, IFNAMSIZ);
		cp.icp_opmode = IEEE80211_M_STA;
		cp.icp_flags  = IEEE80211_CLONE_BSSID;

		strncpy(ifr.ifr_name, wifidev, IFNAMSIZ);
		ifr.ifr_data  = (void *)&cp;

		if( !iwinfo_ioctl(SIOC80211IFCREATE, &ifr) )
		{
			return nif;
		}
		else
		{
			cp.icp_opmode = IEEE80211_M_MONITOR;

			if( !iwinfo_ioctl(SIOC80211IFCREATE, &ifr) )
				return nif;
		}
	}

	return NULL;
}

static void madwifi_ifdel(const char *ifname)
{
	struct ifreq ifr = { 0 };

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	iwinfo_ioctl(SIOC80211IFDESTROY, &ifr);
}


int madwifi_probe(const char *ifname)
{
	return ( !!madwifi_isvap(ifname, NULL) || madwifi_iswifi(ifname) );
}

void madwifi_close(void)
{
	/* Nop */
}

int madwifi_get_mode(const char *ifname, char *buf)
{
	return wext_get_mode(ifname, buf);
}

int madwifi_get_ssid(const char *ifname, char *buf)
{
	return wext_get_ssid(ifname, buf);
}

int madwifi_get_bssid(const char *ifname, char *buf)
{
	return wext_get_bssid(ifname, buf);
}

int madwifi_get_channel(const char *ifname, int *buf)
{
	int i;
	uint16_t freq;
	struct iwreq wrq;
	struct ieee80211req_chaninfo chans;

	if( madwifi_wrq(&wrq, ifname, SIOCGIWFREQ, NULL, 0) >= 0 )
	{
		/* Madwifi returns a Hz frequency, get it's freq list to find channel index */
		freq = (uint16_t)(wrq.u.freq.m / 100000);

		if( get80211priv(ifname, IEEE80211_IOCTL_GETCHANINFO, &chans, sizeof(chans)) >= 0 )
		{
			*buf = 0;

			for( i = 0; i < chans.ic_nchans; i++ )
			{
				if( freq == chans.ic_chans[i].ic_freq )
				{
					*buf = chans.ic_chans[i].ic_ieee;
					break;
				}
			}

			return 0;
		}
	}

	return -1;
}

int madwifi_get_frequency(const char *ifname, int *buf)
{
	struct iwreq wrq;

	if( madwifi_wrq(&wrq, ifname, SIOCGIWFREQ, NULL, 0) >= 0 )
	{
		*buf = (uint16_t)(wrq.u.freq.m / 100000);
		return 0;
	}

	return -1;
}

int madwifi_get_txpower(const char *ifname, int *buf)
{
	return wext_get_txpower(ifname, buf);
}

int madwifi_get_bitrate(const char *ifname, int *buf)
{
	unsigned int mode, len, rate, rate_count;
	uint8_t tmp[24*1024];
	uint8_t *cp;
	struct iwreq wrq;
	struct ieee80211req_sta_info *si;

	if( madwifi_wrq(&wrq, ifname, SIOCGIWMODE, NULL, 0) >= 0 )
	{
		mode = wrq.u.mode;

		/* Calculate bitrate average from associated stations in ad-hoc mode */
		if( mode == 1 )
		{
			rate = rate_count = 0;

			if( (len = get80211priv(ifname, IEEE80211_IOCTL_STA_INFO, tmp, 24*1024)) > 0 )
			{
				cp = tmp;

				do {
					si = (struct ieee80211req_sta_info *) cp;

					if( si->isi_rssi > 0 )
					{
						rate_count++;
						rate += ((si->isi_rates[si->isi_txrate] & IEEE80211_RATE_VAL) / 2);
					}

					cp   += si->isi_len;
					len  -= si->isi_len;
				} while (len >= sizeof(struct ieee80211req_sta_info));
			}

			*buf = (rate == 0 || rate_count == 0) ? 0 : (rate / rate_count) * 1000;
			return 0;
		}

		/* Return whatever wext tells us ... */
		return wext_get_bitrate(ifname, buf);
	}

	return -1;
}

int madwifi_get_signal(const char *ifname, int *buf)
{
	unsigned int mode, len, rssi, rssi_count;
	uint8_t tmp[24*1024];
	uint8_t *cp;
	struct iwreq wrq;
	struct ieee80211req_sta_info *si;

	if( madwifi_wrq(&wrq, ifname, SIOCGIWMODE, NULL, 0) >= 0 )
	{
		mode = wrq.u.mode;

		/* Calculate signal average from associated stations in ap or ad-hoc mode */
		if( mode == 1 )
		{
			rssi = rssi_count = 0;

			if( (len = get80211priv(ifname, IEEE80211_IOCTL_STA_INFO, tmp, 24*1024)) > 0 )
			{
				cp = tmp;

				do {
					si = (struct ieee80211req_sta_info *) cp;

					if( si->isi_rssi > 0 )
					{
						rssi_count++;
						rssi -= (si->isi_rssi - 95);
					}

					cp   += si->isi_len;
					len  -= si->isi_len;
				} while (len >= sizeof(struct ieee80211req_sta_info));
			}

			*buf = (rssi == 0 || rssi_count == 0) ? 1 : -(rssi / rssi_count);
			return 0;
		}

		/* Return whatever wext tells us ... */
		return wext_get_signal(ifname, buf);
	}

	return -1;
}

int madwifi_get_noise(const char *ifname, int *buf)
{
	return wext_get_noise(ifname, buf);
}

int madwifi_get_quality(const char *ifname, int *buf)
{
	unsigned int mode, len, quality, quality_count;
	uint8_t tmp[24*1024];
	uint8_t *cp;
	struct iwreq wrq;
	struct ieee80211req_sta_info *si;

	if( madwifi_wrq(&wrq, ifname, SIOCGIWMODE, NULL, 0) >= 0 )
	{
		mode = wrq.u.mode;

		/* Calculate signal average from associated stations in ad-hoc mode */
		if( mode == 1 )
		{
			quality = quality_count = 0;

			if( (len = get80211priv(ifname, IEEE80211_IOCTL_STA_INFO, tmp, 24*1024)) > 0 )
			{
				cp = tmp;

				do {
					si = (struct ieee80211req_sta_info *) cp;

					if( si->isi_rssi > 0 )
					{
						quality_count++;
						quality += si->isi_rssi;
					}

					cp   += si->isi_len;
					len  -= si->isi_len;
				} while (len >= sizeof(struct ieee80211req_sta_info));
			}

			*buf = (quality == 0 || quality_count == 0) ? 0 : (quality / quality_count);
			return 0;
		}

		/* Return whatever wext tells us ... */
		return wext_get_quality(ifname, buf);
	}

	return -1;
}

int madwifi_get_quality_max(const char *ifname, int *buf)
{
	return wext_get_quality_max(ifname, buf);
}

int madwifi_get_encryption(const char *ifname, char *buf)
{
	int ciphers = 0, key_len = 0;
	char keybuf[IW_ENCODING_TOKEN_MAX];
	struct iwinfo_crypto_entry *c = (struct iwinfo_crypto_entry *)buf;
	struct iwreq wrq;
	struct ieee80211req_key wk;

	memset(&wrq, 0, sizeof(wrq));

	/* Obtain key info */
	if( madwifi_wrq(&wrq, ifname, SIOCGIWENCODE, keybuf, sizeof(keybuf)) < 0 )
		return -1;

#if 0
	/* Have any encryption? */
	if( (wrq.u.data.flags & IW_ENCODE_DISABLED) || (wrq.u.data.length == 0) )
		return 0;
#endif

	/* Save key len */
	key_len = wrq.u.data.length;

	/* Get wpa protocol version */
	wrq.u.mode = IEEE80211_PARAM_WPA;
	if( madwifi_wrq(&wrq, ifname, IEEE80211_IOCTL_GETPARAM, NULL, 0) >= 0 )
		c->wpa_version = wrq.u.mode;

	/* Get authentication suites */
	wrq.u.mode = IEEE80211_PARAM_AUTHMODE;
	if( madwifi_wrq(&wrq, ifname, IEEE80211_IOCTL_GETPARAM, NULL, 0) >= 0 )
	{
		switch(wrq.u.mode) {
			case IEEE80211_AUTH_8021X:
				c->auth_suites |= IWINFO_KMGMT_8021x;
				break;

			case IEEE80211_AUTH_WPA:
				c->auth_suites |= IWINFO_KMGMT_PSK;
				break;

			case IEEE80211_AUTH_OPEN:
				c->auth_algs |= IWINFO_AUTH_OPEN;
				break;

			case IEEE80211_AUTH_SHARED:
				c->auth_algs |= IWINFO_AUTH_SHARED;
				break;

			default:
				c->auth_suites |= IWINFO_KMGMT_NONE;
				break;
		}
	}

	memset(&wk, 0, sizeof(wk));
	memset(wk.ik_macaddr, 0xff, IEEE80211_ADDR_LEN);

	/* Get key information */
	if( get80211priv(ifname, IEEE80211_IOCTL_GETKEY, &wk, sizeof(wk)) >= 0 )
	{
		/* Type 0 == WEP */
		if( (wk.ik_type == 0) && (c->auth_algs == 0) )
			c->auth_algs = (IWINFO_AUTH_OPEN | IWINFO_AUTH_SHARED);
	}

	/* Get used pairwise ciphers */
	wrq.u.mode = IEEE80211_PARAM_UCASTCIPHERS;
	if( madwifi_wrq(&wrq, ifname, IEEE80211_IOCTL_GETPARAM, NULL, 0) >= 0 )
	{
		ciphers = wrq.u.mode;

		if( c->wpa_version && ciphers & (1 << IEEE80211_CIPHER_TKIP) )
			c->pair_ciphers |= IWINFO_CIPHER_TKIP;

		if( c->wpa_version && ciphers & (1 << IEEE80211_CIPHER_AES_CCM) )
			c->pair_ciphers |= IWINFO_CIPHER_CCMP;

		if( c->wpa_version && ciphers & (1 << IEEE80211_CIPHER_AES_OCB) )
			c->pair_ciphers |= IWINFO_CIPHER_AESOCB;

		if( c->wpa_version && ciphers & (1 << IEEE80211_CIPHER_CKIP) )
			c->pair_ciphers |= IWINFO_CIPHER_CKIP;

		if( !c->pair_ciphers && ciphers & (1 << IEEE80211_CIPHER_WEP) )
		{
			switch(key_len) {
				case 13:
					c->pair_ciphers |= IWINFO_CIPHER_WEP104;
					break;

				case 5:
					c->pair_ciphers |= IWINFO_CIPHER_WEP40;
					break;

				case 0:
					break;

				default:
					c->pair_ciphers = IWINFO_CIPHER_WEP40 |
						IWINFO_CIPHER_WEP104;
					break;
			}
		}

		if( ciphers & (1 << IEEE80211_CIPHER_NONE) )
			c->pair_ciphers |= IWINFO_CIPHER_NONE;
	}

	/* Get used group cipher */
	wrq.u.mode = IEEE80211_PARAM_MCASTCIPHER;
	if( madwifi_wrq(&wrq, ifname, IEEE80211_IOCTL_GETPARAM, NULL, 0) >= 0 )
	{
		ciphers = c->wpa_version ? wrq.u.mode : IEEE80211_CIPHER_WEP;

		switch(ciphers) {
			case IEEE80211_CIPHER_TKIP:
				c->group_ciphers |= IWINFO_CIPHER_TKIP;
				break;

			case IEEE80211_CIPHER_AES_CCM:
				c->group_ciphers |= IWINFO_CIPHER_CCMP;
				break;

			case IEEE80211_CIPHER_AES_OCB:
				c->group_ciphers |= IWINFO_CIPHER_AESOCB;
				break;

			case IEEE80211_CIPHER_CKIP:
				c->group_ciphers |= IWINFO_CIPHER_CKIP;
				break;

			case IEEE80211_CIPHER_WEP:
				switch(key_len) {
					case 13:
						c->group_ciphers |= IWINFO_CIPHER_WEP104;
						break;

					case 5:
						c->group_ciphers |= IWINFO_CIPHER_WEP40;
						break;

					default:
						break;
				}
				break;

			case IEEE80211_CIPHER_NONE:
				c->group_ciphers |= IWINFO_CIPHER_NONE;
				break;

			default:
				break;
		}
	}

	c->enabled = (c->wpa_version || (c->auth_algs && c->pair_ciphers)) ? 1 : 0;

	return 0;
}

int madwifi_get_assoclist(const char *ifname, char *buf, int *len)
{
	int bl, tl, noise;
	uint8_t *cp;
	uint8_t tmp[24*1024];
	struct ieee80211req_sta_info *si;
	struct iwinfo_assoclist_entry entry;

	if( (tl = get80211priv(ifname, IEEE80211_IOCTL_STA_INFO, tmp, 24*1024)) > 0 )
	{
		cp = tmp;
		bl = 0;

		if( madwifi_get_noise(ifname, &noise) )
			noise = 0;

		do {
			si = (struct ieee80211req_sta_info *) cp;

			memset(&entry, 0, sizeof(entry));

			entry.signal = (si->isi_rssi - 95);
			entry.noise  = noise;
			memcpy(entry.mac, &si->isi_macaddr, 6);

			entry.inactive = si->isi_inact * 1000;

			entry.tx_packets = (si->isi_txseqs[0] & IEEE80211_SEQ_SEQ_MASK)
				>> IEEE80211_SEQ_SEQ_SHIFT;

			entry.rx_packets = (si->isi_rxseqs[0] & IEEE80211_SEQ_SEQ_MASK)
				>> IEEE80211_SEQ_SEQ_SHIFT;

			entry.tx_rate.rate =
				(si->isi_rates[si->isi_txrate] & IEEE80211_RATE_VAL) * 500;

			/* XXX: this is just a guess */
			entry.rx_rate.rate = entry.tx_rate.rate;

			entry.rx_rate.mcs = -1;
			entry.tx_rate.mcs = -1;

			memcpy(&buf[bl], &entry, sizeof(struct iwinfo_assoclist_entry));

			bl += sizeof(struct iwinfo_assoclist_entry);
			cp += si->isi_len;
			tl -= si->isi_len;
		} while (tl >= sizeof(struct ieee80211req_sta_info));

		*len = bl;
		return 0;
	}

	return -1;
}

int madwifi_get_txpwrlist(const char *ifname, char *buf, int *len)
{
	int rc = -1;
	char *res;

	/* A wifiX device? */
	if( madwifi_iswifi(ifname) )
	{
		if( (res = madwifi_ifadd(ifname)) != NULL )
		{
			rc = wext_get_txpwrlist(res, buf, len);
			madwifi_ifdel(res);
		}
	}

	/* Its an athX ... */
	else if( !!madwifi_isvap(ifname, NULL) )
	{
		rc = wext_get_txpwrlist(ifname, buf, len);
	}

	return rc;
}

int madwifi_get_scanlist(const char *ifname, char *buf, int *len)
{
	int ret;
	char *res;
	DIR *proc;
	struct dirent *e;

	ret = -1;

	/* We got a wifiX device passed, try to lookup a vap on it */
	if( madwifi_iswifi(ifname) )
	{
		if( (proc = opendir("/proc/sys/net/")) != NULL )
		{
			while( (e = readdir(proc)) != NULL )
			{
				if( !!madwifi_isvap(e->d_name, ifname) )
				{
					if( iwinfo_ifup(e->d_name) )
					{
						ret = wext_get_scanlist(e->d_name, buf, len);
						break;
					}
				}
			}

			closedir(proc);
		}

		/* Still nothing found, try to create a vap */
		if( ret == -1 )
		{
			if( (res = madwifi_ifadd(ifname)) != NULL )
			{
				if( iwinfo_ifup(res) )
				{
					wext_get_scanlist(res, buf, len);
					sleep(1);

					wext_get_scanlist(res, buf, len);
					sleep(1);

					ret = wext_get_scanlist(res, buf, len);
				}

				iwinfo_ifdown(res);
				madwifi_ifdel(res);
			}
		}
	}

	/* Got athX device? */
	else if( !!madwifi_isvap(ifname, NULL) )
	{
		ret = wext_get_scanlist(ifname, buf, len);
	}

	return ret;
}

int madwifi_get_freqlist(const char *ifname, char *buf, int *len)
{
	int i, bl;
	int rc = -1;
	char *res;
	struct ieee80211req_chaninfo chans;
	struct iwinfo_freqlist_entry entry;

	/* A wifiX device? */
	if( madwifi_iswifi(ifname) )
	{
		if( (res = madwifi_ifadd(ifname)) != NULL )
		{
			rc = get80211priv(res, IEEE80211_IOCTL_GETCHANINFO,
							  &chans, sizeof(chans));

			madwifi_ifdel(res);
		}
	}

	/* Its an athX ... */
	else if( !!madwifi_isvap(ifname, NULL) )
	{
		rc = get80211priv(ifname, IEEE80211_IOCTL_GETCHANINFO,
						  &chans, sizeof(chans));
	}


	/* Got chaninfo? */
	if( rc >= 0 )
	{
		bl = 0;

		for( i = 0; i < chans.ic_nchans; i++ )
		{
			entry.mhz        = chans.ic_chans[i].ic_freq;
			entry.channel    = chans.ic_chans[i].ic_ieee;
			entry.restricted = 0;

			memcpy(&buf[bl], &entry, sizeof(struct iwinfo_freqlist_entry));
			bl += sizeof(struct iwinfo_freqlist_entry);
		}

		*len = bl;
		return 0;
	}

	return -1;
}

int madwifi_get_country(const char *ifname, char *buf)
{
	int i, fd, ccode = -1;
	char buffer[34];
	char *wifi = madwifi_iswifi(ifname)
		? (char *)ifname : madwifi_isvap(ifname, NULL);

	struct ISO3166_to_CCode *e;

	if( wifi )
	{
		snprintf(buffer, sizeof(buffer), "/proc/sys/dev/%s/countrycode", wifi);

		if( (fd = open(buffer, O_RDONLY)) > -1 )
		{
			memset(buffer, 0, sizeof(buffer));

			if( read(fd, buffer, sizeof(buffer)-1) > 0 )
				ccode = atoi(buffer);

			close(fd);
		}
	}

	for( i = 0; i < (sizeof(CountryCodes)/sizeof(CountryCodes[0])); i++ )
	{
		e = &CountryCodes[i];

		if( e->ccode == ccode )
		{
			sprintf(buf, "%c%c", e->iso3166 / 256, e->iso3166 % 256);
			return 0;
		}
	}

	return -1;
}

int madwifi_get_countrylist(const char *ifname, char *buf, int *len)
{
	int i, count;
	struct ISO3166_to_CCode *e, *p = NULL;
	struct iwinfo_country_entry *c = (struct iwinfo_country_entry *)buf;

	count = 0;

	for( int i = 0; i < (sizeof(CountryCodes)/sizeof(CountryCodes[0])); i++ )
	{
		e = &CountryCodes[i];

		if( !p || (e->iso3166 != p->iso3166) )
		{
			c->iso3166 = e->iso3166;
			snprintf(c->ccode, sizeof(c->ccode), "%i", e->ccode);

			c++;
			count++;
		}

		p = e;
	}

	*len = (count * sizeof(struct iwinfo_country_entry));
	return 0;
}

int madwifi_get_hwmodelist(const char *ifname, int *buf)
{
	char chans[IWINFO_BUFSIZE] = { 0 };
	struct iwinfo_freqlist_entry *e = NULL;
	int len = 0;

	if( !madwifi_get_freqlist(ifname, chans, &len) )
	{
		for( e = (struct iwinfo_freqlist_entry *)chans; e->channel; e++ )
		{
			if( e->channel <= 14 )
			{
				*buf |= IWINFO_80211_B;
				*buf |= IWINFO_80211_G;
			}
			else
			{
				*buf |= IWINFO_80211_A;
			}
		}

		return 0;
	}

	return -1;
}

int madwifi_get_mbssid_support(const char *ifname, int *buf)
{
	/* Test whether we can create another interface */
	char *nif = madwifi_ifadd(ifname);

	if( nif )
	{
		*buf = iwinfo_ifup(nif);

		iwinfo_ifdown(nif);
		madwifi_ifdel(nif);

		return 0;
	}

	return -1;
}

int madwifi_get_hardware_id(const char *ifname, char *buf)
{
	char vendor[64];
	char device[64];
	struct iwinfo_hardware_id *ids;
	struct iwinfo_hardware_entry *e;

	if (wext_get_hardware_id(ifname, buf))
		return iwinfo_hardware_id_from_mtd((struct iwinfo_hardware_id *)buf);

	return 0;
}

static const struct iwinfo_hardware_entry *
madwifi_get_hardware_entry(const char *ifname)
{
	struct iwinfo_hardware_id id;

	if (madwifi_get_hardware_id(ifname, (char *)&id))
		return NULL;

	return iwinfo_hardware(&id);
}

int madwifi_get_hardware_name(const char *ifname, char *buf)
{
	const struct iwinfo_hardware_entry *hw;

	if (!(hw = madwifi_get_hardware_entry(ifname)))
		sprintf(buf, "Generic Atheros");
	else
		sprintf(buf, "%s %s", hw->vendor_name, hw->device_name);

	return 0;
}

int madwifi_get_txpower_offset(const char *ifname, int *buf)
{
	const struct iwinfo_hardware_entry *hw;

	if (!(hw = madwifi_get_hardware_entry(ifname)))
		return -1;

	*buf = hw->txpower_offset;
	return 0;
}

int madwifi_get_frequency_offset(const char *ifname, int *buf)
{
	const struct iwinfo_hardware_entry *hw;

	if (!(hw = madwifi_get_hardware_entry(ifname)))
		return -1;

	*buf = hw->frequency_offset;
	return 0;
}
