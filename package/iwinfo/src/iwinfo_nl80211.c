/*
 * iwinfo - Wireless Information Library - NL80211 Backend
 *
 *   Copyright (C) 2010 Jo-Philipp Wich <xm@subsignal.org>
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
 *
 * Parts of this code are derived from the Linux iw utility.
 */

#include "iwinfo/nl80211.h"
#include "iwinfo/wext.h"

#define min(x, y) ((x) < (y)) ? (x) : (y)

static struct nl80211_state *nls = NULL;

static int nl80211_init(void)
{
	int err, fd;

	if (!nls)
	{
		nls = malloc(sizeof(struct nl80211_state));
		if (!nls) {
			err = -ENOMEM;
			goto err;
		}

		nls->nl_sock = nl_socket_alloc();
		if (!nls->nl_sock) {
			err = -ENOMEM;
			goto err;
		}

		if( genl_connect(nls->nl_sock)) {
			err = -ENOLINK;
			goto err;
		}

		fd = nl_socket_get_fd(nls->nl_sock);
		if (fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC) < 0)
		{
			err = -EINVAL;
			goto err;
		}

		if( genl_ctrl_alloc_cache(nls->nl_sock, &nls->nl_cache)) {
			err = -ENOMEM;
			goto err;
		}

		nls->nl80211 = genl_ctrl_search_by_name(nls->nl_cache, "nl80211");
		if (!nls->nl80211)
		{
			err = -ENOENT;
			goto err;
		}
	}

	return 0;


err:
	nl80211_close();
	return err;
}

static int nl80211_msg_error(struct sockaddr_nl *nla,
	struct nlmsgerr *err, void *arg)
{
	int *ret = arg;
	*ret = err->error;
	return NL_STOP;
}

static int nl80211_msg_finish(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
	*ret = 0;
	return NL_SKIP;
}

static int nl80211_msg_ack(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
	*ret = 0;
	return NL_STOP;
}

static int nl80211_msg_response(struct nl_msg *msg, void *arg)
{
	return NL_SKIP;
}

static void nl80211_free(struct nl80211_msg_conveyor *cv)
{
	if (cv)
	{
		if (cv->cb)
			nl_cb_put(cv->cb);

		if (cv->msg)
			nlmsg_free(cv->msg);

		cv->cb  = NULL;
		cv->msg = NULL;
	}
}

static struct nl80211_msg_conveyor * nl80211_msg(const char *ifname, int cmd, int flags)
{
	static struct nl80211_msg_conveyor cv;

	int ifidx = -1, phyidx = -1;
	struct nl_msg *req = NULL;
	struct nl_cb *cb = NULL;

	if (nl80211_init() < 0)
		goto err;

	if (!strncmp(ifname, "phy", 3))
		phyidx = atoi(&ifname[3]);
	else if (!strncmp(ifname, "radio", 5))
		phyidx = atoi(&ifname[5]);
	else if (!strncmp(ifname, "mon.", 4))
		ifidx = if_nametoindex(&ifname[4]);
	else
		ifidx = if_nametoindex(ifname);

	if ((ifidx < 0) && (phyidx < 0))
		return NULL;

	req = nlmsg_alloc();
	if (!req)
		goto err;

	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb)
		goto err;

	genlmsg_put(req, 0, 0, genl_family_get_id(nls->nl80211), 0,
		flags, cmd, 0);

	if (ifidx > -1)
		NLA_PUT_U32(req, NL80211_ATTR_IFINDEX, ifidx);

	if (phyidx > -1)
		NLA_PUT_U32(req, NL80211_ATTR_WIPHY, phyidx);

	cv.msg = req;
	cv.cb  = cb;

	return &cv;

err:
nla_put_failure:
	if (cb)
		nl_cb_put(cb);

	if (req)
		nlmsg_free(req);

	return NULL;
}

static struct nl80211_msg_conveyor * nl80211_send(
	struct nl80211_msg_conveyor *cv,
	int (*cb_func)(struct nl_msg *, void *), void *cb_arg
) {
	static struct nl80211_msg_conveyor rcv;
	int err = 1;

	if (cb_func)
		nl_cb_set(cv->cb, NL_CB_VALID, NL_CB_CUSTOM, cb_func, cb_arg);
	else
		nl_cb_set(cv->cb, NL_CB_VALID, NL_CB_CUSTOM, nl80211_msg_response, &rcv);

	if (nl_send_auto_complete(nls->nl_sock, cv->msg) < 0)
		goto err;

	nl_cb_err(cv->cb,               NL_CB_CUSTOM, nl80211_msg_error,  &err);
	nl_cb_set(cv->cb, NL_CB_FINISH, NL_CB_CUSTOM, nl80211_msg_finish, &err);
	nl_cb_set(cv->cb, NL_CB_ACK,    NL_CB_CUSTOM, nl80211_msg_ack,    &err);

	while (err > 0)
		nl_recvmsgs(nls->nl_sock, cv->cb);

	return &rcv;

err:
	nl_cb_put(cv->cb);
	nlmsg_free(cv->msg);

	return NULL;
}

static struct nlattr ** nl80211_parse(struct nl_msg *msg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	static struct nlattr *attr[NL80211_ATTR_MAX + 1];

	nla_parse(attr, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		genlmsg_attrlen(gnlh, 0), NULL);

	return attr;
}

static int nl80211_freq2channel(int freq)
{
    if (freq == 2484)
        return 14;

    if (freq < 2484)
        return (freq - 2407) / 5;

    return (freq / 5) - 1000;
}

static char * nl80211_getval(const char *ifname, const char *buf, const char *key)
{
	int i, len;
	char lkey[64] = { 0 };
	const char *ln = buf;
	static char lval[256] = { 0 };

	int matched_if = ifname ? 0 : 1;


	for( i = 0, len = strlen(buf); i < len; i++ )
	{
		if (!lkey[0] && (buf[i] == ' ' || buf[i] == '\t'))
		{
			ln++;
		}
		else if (!lkey[0] && (buf[i] == '='))
		{
			if ((&buf[i] - ln) > 0)
				memcpy(lkey, ln, min(sizeof(lkey) - 1, &buf[i] - ln));
		}
		else if (buf[i] == '\n')
		{
			if (lkey[0])
			{
				memcpy(lval, ln + strlen(lkey) + 1,
					min(sizeof(lval) - 1, &buf[i] - ln - strlen(lkey) - 1));

				if ((ifname != NULL) &&
				    (!strcmp(lkey, "interface") || !strcmp(lkey, "bss")) )
				{
					matched_if = !strcmp(lval, ifname);
				}
				else if (matched_if && !strcmp(lkey, key))
				{
					return lval;
				}
			}

			ln = &buf[i+1];
			memset(lkey, 0, sizeof(lkey));
			memset(lval, 0, sizeof(lval));
		}
	}

	return NULL;
}

static int nl80211_ifname2phy_cb(struct nl_msg *msg, void *arg)
{
	char *buf = arg;
	struct nlattr **attr = nl80211_parse(msg);

	if (attr[NL80211_ATTR_WIPHY_NAME])
		sprintf(buf, "%s", nla_data(attr[NL80211_ATTR_WIPHY_NAME]));
	else
		buf[0] = 0;

	return NL_SKIP;
}

static char * nl80211_ifname2phy(const char *ifname)
{
	static char phy[32] = { 0 };
	struct nl80211_msg_conveyor *req;

	memset(phy, 0, sizeof(phy));

	req = nl80211_msg(ifname, NL80211_CMD_GET_WIPHY, 0);
	if (req)
	{
		nl80211_send(req, nl80211_ifname2phy_cb, phy);
		nl80211_free(req);
	}

	return phy[0] ? phy : NULL;
}

static char * nl80211_hostapd_info(const char *ifname)
{
	char *phy;
	char path[32] = { 0 };
	static char buf[4096] = { 0 };
	FILE *conf;

	if ((phy = nl80211_ifname2phy(ifname)) != NULL)
	{
		snprintf(path, sizeof(path), "/var/run/hostapd-%s.conf", phy);

		if ((conf = fopen(path, "r")) != NULL)
		{
			fread(buf, sizeof(buf) - 1, 1, conf);
			fclose(conf);

			return buf;
		}
	}

	return NULL;
}

static inline int nl80211_wpactl_recv(int sock, char *buf, int blen)
{
	fd_set rfds;
	struct timeval tv = { 2, 0 };

	FD_ZERO(&rfds);
	FD_SET(sock, &rfds);

	memset(buf, 0, blen);


	if (select(sock + 1, &rfds, NULL, NULL, &tv) < 0)
		return -1;

	if (!FD_ISSET(sock, &rfds))
		return -1;

	return recv(sock, buf, blen, 0);
}

static char * nl80211_wpactl_info(const char *ifname, const char *cmd,
								   const char *event)
{
	int numtry = 0;
	int sock = -1;
	char *rv = NULL;
	size_t remote_length, local_length;
	static char buffer[10240] = { 0 };

	struct sockaddr_un local = { 0 };
	struct sockaddr_un remote = { 0 };


	sock = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0)
		return NULL;

	remote.sun_family = AF_UNIX;
	remote_length = sizeof(remote.sun_family) + sprintf(remote.sun_path,
		"/var/run/wpa_supplicant-%s/%s", ifname, ifname);

	if (fcntl(sock, F_SETFD, fcntl(sock, F_GETFD) | FD_CLOEXEC) < 0)
		goto out;

	if (connect(sock, (struct sockaddr *) &remote, remote_length))
		goto out;

	local.sun_family = AF_UNIX;
	local_length = sizeof(local.sun_family) + sprintf(local.sun_path,
		"/var/run/iwinfo-%s-%d", ifname, getpid());

	if (bind(sock, (struct sockaddr *) &local, local_length))
		goto out;


	send(sock, "ATTACH", 6, 0);

	if (nl80211_wpactl_recv(sock, buffer, sizeof(buffer)) <= 0)
		goto out;


	send(sock, cmd, strlen(cmd), 0);

	while( numtry++ < 5 )
	{
		if (nl80211_wpactl_recv(sock, buffer, sizeof(buffer)) <= 0)
		{
			if (event)
				continue;

			break;
		}

		if ((!event && buffer[0] != '<') || strstr(buffer, event))
			break;
	}

	rv = buffer;

out:
	close(sock);

	if (local.sun_family)
		unlink(local.sun_path);

	return rv;
}

static inline int nl80211_readint(const char *path)
{
	int fd;
	int rv = -1;
	char buffer[16];

	if ((fd = open(path, O_RDONLY)) > -1)
	{
		if (read(fd, buffer, sizeof(buffer)) > 0)
			rv = atoi(buffer);

		close(fd);
	}

	return rv;
}

static char * nl80211_phy2ifname(const char *ifname)
{
	int fd, ifidx = -1, cifidx = -1, phyidx = -1;
	char buffer[64];
	static char nif[IFNAMSIZ] = { 0 };

	DIR *d;
	struct dirent *e;

	if (!ifname)
		return NULL;
	else if (!strncmp(ifname, "phy", 3))
		phyidx = atoi(&ifname[3]);
	else if (!strncmp(ifname, "radio", 5))
		phyidx = atoi(&ifname[5]);

	memset(nif, 0, sizeof(nif));

	if (phyidx > -1)
	{
		if ((d = opendir("/sys/class/net")) != NULL)
		{
			while( (e = readdir(d)) != NULL )
			{
				snprintf(buffer, sizeof(buffer),
					"/sys/class/net/%s/phy80211/index", e->d_name);

				if (nl80211_readint(buffer) == phyidx)
				{
					snprintf(buffer, sizeof(buffer),
						"/sys/class/net/%s/ifindex", e->d_name);

					if( (cifidx = nl80211_readint(buffer)) >= 0 &&
					    ((ifidx < 0) || (cifidx < ifidx)) )
					{
						ifidx = cifidx;
						strncpy(nif, e->d_name, sizeof(nif));
					}
				}
			}

			closedir(d);
		}
	}

	return nif[0] ? nif : NULL;
}

static char * nl80211_ifadd(const char *ifname)
{
	int phyidx;
	char *rv = NULL;
	static char nif[IFNAMSIZ] = { 0 };
	struct nl80211_msg_conveyor *req, *res;

	req = nl80211_msg(ifname, NL80211_CMD_NEW_INTERFACE, 0);
	if (req)
	{
		snprintf(nif, sizeof(nif), "tmp.%s", ifname);

		NLA_PUT_STRING(req->msg, NL80211_ATTR_IFNAME, nif);
		NLA_PUT_U32(req->msg, NL80211_ATTR_IFTYPE, NL80211_IFTYPE_STATION);

		nl80211_send(req, NULL, NULL);

		rv = nif;

	nla_put_failure:
		nl80211_free(req);
	}

	return rv;
}

static void nl80211_ifdel(const char *ifname)
{
	struct nl80211_msg_conveyor *req;

	req = nl80211_msg(ifname, NL80211_CMD_DEL_INTERFACE, 0);
	if (req)
	{
		NLA_PUT_STRING(req->msg, NL80211_ATTR_IFNAME, ifname);

		nl80211_send(req, NULL, NULL);

	nla_put_failure:
		nl80211_free(req);
	}
}

static void nl80211_hostapd_hup(const char *ifname)
{
	int fd, pid = 0;
	char buf[32];
	char *phy = nl80211_ifname2phy(ifname);

	if (phy)
	{
		snprintf(buf, sizeof(buf), "/var/run/wifi-%s.pid", phy);
		if ((fd = open(buf, O_RDONLY)) > 0)
		{
			if (read(fd, buf, sizeof(buf)) > 0)
				pid = atoi(buf);

			close(fd);
		}

		if (pid > 0)
			kill(pid, 1);
	}
}


int nl80211_probe(const char *ifname)
{
	return !!nl80211_ifname2phy(ifname);
}

void nl80211_close(void)
{
	if (nls)
	{
		if (nls->nl_sock)
			nl_socket_free(nls->nl_sock);

		if (nls->nl_cache)
			nl_cache_free(nls->nl_cache);

		free(nls);
		nls = NULL;
	}
}

int nl80211_get_mode(const char *ifname, char *buf)
{
	return wext_get_mode(ifname, buf);
}

int nl80211_get_ssid(const char *ifname, char *buf)
{
	char *ssid;

	if (!wext_get_ssid(ifname, buf))
	{
		return 0;
	}
	else if( (ssid = nl80211_hostapd_info(ifname)) &&
	         (ssid = nl80211_getval(ifname, ssid, "ssid")) )
	{
		memcpy(buf, ssid, strlen(ssid));
		return 0;
	}

	return -1;
}

int nl80211_get_bssid(const char *ifname, char *buf)
{
	char *bssid;
	unsigned char mac[6];

	if (!wext_get_bssid(ifname, buf))
	{
		return 0;
	}
	else if((bssid = nl80211_hostapd_info(ifname)) &&
	         (bssid = nl80211_getval(ifname, bssid, "bssid")))
	{
		mac[0] = strtol(&bssid[0],  NULL, 16);
		mac[1] = strtol(&bssid[3],  NULL, 16);
		mac[2] = strtol(&bssid[6],  NULL, 16);
		mac[3] = strtol(&bssid[9],  NULL, 16);
		mac[4] = strtol(&bssid[12], NULL, 16);
		mac[5] = strtol(&bssid[15], NULL, 16);

		sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
			mac[0], mac[1], mac[2],	mac[3], mac[4], mac[5]);

		return 0;
	}

	return -1;
}

int nl80211_get_channel(const char *ifname, int *buf)
{
	char *first;

	if (!wext_get_channel(ifname, buf))
		return 0;

	else if ((first = nl80211_phy2ifname(nl80211_ifname2phy(ifname))) != NULL)
		return wext_get_channel(first, buf);

	return -1;
}

int nl80211_get_frequency(const char *ifname, int *buf)
{
	char *first;

	if (!wext_get_frequency(ifname, buf))
		return 0;

	else if ((first = nl80211_phy2ifname(nl80211_ifname2phy(ifname))) != NULL)
		return wext_get_frequency(first, buf);

	return -1;
}

int nl80211_get_txpower(const char *ifname, int *buf)
{
	return wext_get_txpower(ifname, buf);
}


static int nl80211_fill_signal_cb(struct nl_msg *msg, void *arg)
{
	int8_t dbm;
	int16_t mbit;
	struct nl80211_rssi_rate *rr = arg;
	struct nlattr **attr = nl80211_parse(msg);
	struct nlattr *sinfo[NL80211_STA_INFO_MAX + 1];
	struct nlattr *rinfo[NL80211_RATE_INFO_MAX + 1];

	static struct nla_policy stats_policy[NL80211_STA_INFO_MAX + 1] = {
		[NL80211_STA_INFO_INACTIVE_TIME] = { .type = NLA_U32    },
		[NL80211_STA_INFO_RX_BYTES]      = { .type = NLA_U32    },
		[NL80211_STA_INFO_TX_BYTES]      = { .type = NLA_U32    },
		[NL80211_STA_INFO_RX_PACKETS]    = { .type = NLA_U32    },
		[NL80211_STA_INFO_TX_PACKETS]    = { .type = NLA_U32    },
		[NL80211_STA_INFO_SIGNAL]        = { .type = NLA_U8     },
		[NL80211_STA_INFO_TX_BITRATE]    = { .type = NLA_NESTED },
		[NL80211_STA_INFO_LLID]          = { .type = NLA_U16    },
		[NL80211_STA_INFO_PLID]          = { .type = NLA_U16    },
		[NL80211_STA_INFO_PLINK_STATE]   = { .type = NLA_U8     },
	};

	static struct nla_policy rate_policy[NL80211_RATE_INFO_MAX + 1] = {
		[NL80211_RATE_INFO_BITRATE]      = { .type = NLA_U16  },
		[NL80211_RATE_INFO_MCS]          = { .type = NLA_U8   },
		[NL80211_RATE_INFO_40_MHZ_WIDTH] = { .type = NLA_FLAG },
		[NL80211_RATE_INFO_SHORT_GI]     = { .type = NLA_FLAG },
	};

	if (attr[NL80211_ATTR_STA_INFO])
	{
		if( !nla_parse_nested(sinfo, NL80211_STA_INFO_MAX,
				attr[NL80211_ATTR_STA_INFO], stats_policy) )
		{
			if (sinfo[NL80211_STA_INFO_SIGNAL])
			{
				dbm = nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL]);
				rr->rssi = rr->rssi ? (int8_t)((rr->rssi + dbm) / 2) : dbm;
			}

			if (sinfo[NL80211_STA_INFO_TX_BITRATE])
			{
				if( !nla_parse_nested(rinfo, NL80211_RATE_INFO_MAX,
						sinfo[NL80211_STA_INFO_TX_BITRATE], rate_policy) )
				{
					if (rinfo[NL80211_RATE_INFO_BITRATE])
					{
						mbit = nla_get_u16(rinfo[NL80211_RATE_INFO_BITRATE]);
						rr->rate = rr->rate
							? (int16_t)((rr->rate + mbit) / 2) : mbit;
					}
				}
			}
		}
	}

	return NL_SKIP;
}

static void nl80211_fill_signal(const char *ifname, struct nl80211_rssi_rate *r)
{
	DIR *d;
	struct dirent *de;
	struct nl80211_msg_conveyor *req;

	r->rssi = 0;
	r->rate = 0;

	if ((d = opendir("/sys/class/net")) != NULL)
	{
		while ((de = readdir(d)) != NULL)
		{
			if (!strncmp(de->d_name, ifname, strlen(ifname)) &&
				(!de->d_name[strlen(ifname)] ||
				 !strncmp(&de->d_name[strlen(ifname)], ".sta", 4)))
			{
				req = nl80211_msg(de->d_name, NL80211_CMD_GET_STATION,
								  NLM_F_DUMP);

				if (req)
				{
					nl80211_send(req, nl80211_fill_signal_cb, r);
					nl80211_free(req);
				}
			}
		}

		closedir(d);
	}
}

int nl80211_get_bitrate(const char *ifname, int *buf)
{
	struct nl80211_rssi_rate rr;

	if (!wext_get_bitrate(ifname, buf))
		return 0;

	nl80211_fill_signal(ifname, &rr);

	if (rr.rate)
	{
		*buf = (rr.rate * 100);
		return 0;
	}

	return -1;
}

int nl80211_get_signal(const char *ifname, int *buf)
{
	struct nl80211_rssi_rate rr;

	if (!wext_get_signal(ifname, buf))
		return 0;

	nl80211_fill_signal(ifname, &rr);

	if (rr.rssi)
	{
		*buf = rr.rssi;
		return 0;
	}

	return -1;
}

static int nl80211_get_noise_cb(struct nl_msg *msg, void *arg)
{
	int8_t *noise = arg;
	struct nlattr **tb = nl80211_parse(msg);
	struct nlattr *si[NL80211_SURVEY_INFO_MAX + 1];

	static struct nla_policy sp[NL80211_SURVEY_INFO_MAX + 1] = {
		[NL80211_SURVEY_INFO_FREQUENCY] = { .type = NLA_U32 },
		[NL80211_SURVEY_INFO_NOISE]     = { .type = NLA_U8  },
	};

	if (!tb[NL80211_ATTR_SURVEY_INFO])
		return NL_SKIP;

	if (nla_parse_nested(si, NL80211_SURVEY_INFO_MAX,
						 tb[NL80211_ATTR_SURVEY_INFO], sp))
		return NL_SKIP;

	if (!si[NL80211_SURVEY_INFO_NOISE])
		return NL_SKIP;

	if (!*noise || si[NL80211_SURVEY_INFO_IN_USE])
		*noise = (int8_t)nla_get_u8(si[NL80211_SURVEY_INFO_NOISE]);

	return NL_SKIP;
}


int nl80211_get_noise(const char *ifname, int *buf)
{
	int8_t noise;
	struct nl80211_msg_conveyor *req;

	req = nl80211_msg(ifname, NL80211_CMD_GET_SURVEY, NLM_F_DUMP);
	if (req)
	{
		noise = 0;

		nl80211_send(req, nl80211_get_noise_cb, &noise);
		nl80211_free(req);

		if (noise)
		{
			*buf = noise;
			return 0;
		}
	}

	return -1;
}

int nl80211_get_quality(const char *ifname, int *buf)
{
	int signal;

	if (wext_get_quality(ifname, buf))
	{
		*buf = 0;

		if (!nl80211_get_signal(ifname, &signal))
		{
			/* A positive signal level is usually just a quality
			 * value, pass through as-is */
			if (signal >= 0)
			{
				*buf = signal;
			}

			/* The cfg80211 wext compat layer assumes a signal range
			 * of -110 dBm to -40 dBm, the quality value is derived
			 * by adding 110 to the signal level */
			else
			{
				if (signal < -110)
					signal = -110;
				else if (signal > -40)
					signal = -40;

				*buf = (signal + 110);
			}
		}
	}

	return 0;
}

int nl80211_get_quality_max(const char *ifname, int *buf)
{
	if (wext_get_quality_max(ifname, buf))
		/* The cfg80211 wext compat layer assumes a maximum
		 * quality of 70 */
		*buf = 70;

	return 0;
}

int nl80211_get_encryption(const char *ifname, char *buf)
{
	int i;
	char k[9];
	char *val, *res;
	struct iwinfo_crypto_entry *c = (struct iwinfo_crypto_entry *)buf;

	/* WPA supplicant */
	if( (res = nl80211_wpactl_info(ifname, "STATUS", NULL)) &&
	    (val = nl80211_getval(NULL, res, "pairwise_cipher")) )
	{
		/* WEP */
		if (strstr(val, "WEP"))
		{
			if (strstr(val, "WEP-40"))
				c->pair_ciphers |= IWINFO_CIPHER_WEP40;

			else if (strstr(val, "WEP-104"))
				c->pair_ciphers |= IWINFO_CIPHER_WEP104;

			c->enabled       = 1;
			c->group_ciphers = c->pair_ciphers;

			c->auth_suites |= IWINFO_KMGMT_NONE;
			c->auth_algs   |= IWINFO_AUTH_OPEN; /* XXX: assumption */
		}

		/* WPA */
		else
		{
			if (strstr(val, "TKIP"))
				c->pair_ciphers |= IWINFO_CIPHER_TKIP;

			else if (strstr(val, "CCMP"))
				c->pair_ciphers |= IWINFO_CIPHER_CCMP;

			else if (strstr(val, "NONE"))
				c->pair_ciphers |= IWINFO_CIPHER_NONE;

			else if (strstr(val, "WEP-40"))
				c->pair_ciphers |= IWINFO_CIPHER_WEP40;

			else if (strstr(val, "WEP-104"))
				c->pair_ciphers |= IWINFO_CIPHER_WEP104;


			if ((val = nl80211_getval(NULL, res, "group_cipher")))
			{
				if (strstr(val, "TKIP"))
					c->group_ciphers |= IWINFO_CIPHER_TKIP;

				else if (strstr(val, "CCMP"))
					c->group_ciphers |= IWINFO_CIPHER_CCMP;

				else if (strstr(val, "NONE"))
					c->group_ciphers |= IWINFO_CIPHER_NONE;

				else if (strstr(val, "WEP-40"))
					c->group_ciphers |= IWINFO_CIPHER_WEP40;

				else if (strstr(val, "WEP-104"))
					c->group_ciphers |= IWINFO_CIPHER_WEP104;
			}


			if ((val = nl80211_getval(NULL, res, "key_mgmt")))
			{
				if (strstr(val, "WPA2"))
					c->wpa_version = 2;

				else if (strstr(val, "WPA"))
					c->wpa_version = 1;


				if (strstr(val, "PSK"))
					c->auth_suites |= IWINFO_KMGMT_PSK;

				else if (strstr(val, "EAP") || strstr(val, "802.1X"))
					c->auth_suites |= IWINFO_KMGMT_8021x;

				else if (strstr(val, "NONE"))
					c->auth_suites |= IWINFO_KMGMT_NONE;
			}

			c->enabled = (c->wpa_version && c->auth_suites) ? 1 : 0;
		}

		return 0;
	}

	/* Hostapd */
	else if ((res = nl80211_hostapd_info(ifname)))
	{
		if ((val = nl80211_getval(ifname, res, "wpa")) != NULL)
			c->wpa_version = atoi(val);

		val = nl80211_getval(ifname, res, "wpa_key_mgmt");

		if (!val || strstr(val, "PSK"))
			c->auth_suites |= IWINFO_KMGMT_PSK;

		if (val && strstr(val, "EAP"))
			c->auth_suites |= IWINFO_KMGMT_8021x;

		if (val && strstr(val, "NONE"))
			c->auth_suites |= IWINFO_KMGMT_NONE;

		if ((val = nl80211_getval(ifname, res, "wpa_pairwise")) != NULL)
		{
			if (strstr(val, "TKIP"))
				c->pair_ciphers |= IWINFO_CIPHER_TKIP;

			if (strstr(val, "CCMP"))
				c->pair_ciphers |= IWINFO_CIPHER_CCMP;

			if (strstr(val, "NONE"))
				c->pair_ciphers |= IWINFO_CIPHER_NONE;
		}

		if ((val = nl80211_getval(ifname, res, "auth_algs")) != NULL)
		{
			switch(atoi(val)) {
				case 1:
					c->auth_algs |= IWINFO_AUTH_OPEN;
					break;

				case 2:
					c->auth_algs |= IWINFO_AUTH_SHARED;
					break;

				case 3:
					c->auth_algs |= IWINFO_AUTH_OPEN;
					c->auth_algs |= IWINFO_AUTH_SHARED;
					break;

				default:
					break;
			}

			for( i = 0; i < 4; i++ )
			{
				snprintf(k, sizeof(k), "wep_key%d", i);

				if ((val = nl80211_getval(ifname, res, k)))
				{
					if ((strlen(val) == 5) || (strlen(val) == 10))
						c->pair_ciphers |= IWINFO_CIPHER_WEP40;

					else if ((strlen(val) == 13) || (strlen(val) == 26))
						c->pair_ciphers |= IWINFO_CIPHER_WEP104;
				}
			}
		}

		c->group_ciphers = c->pair_ciphers;
		c->enabled = (c->wpa_version || c->pair_ciphers) ? 1 : 0;

		return 0;
	}

	return -1;
}


static int nl80211_get_assoclist_cb(struct nl_msg *msg, void *arg)
{
	struct nl80211_array_buf *arr = arg;
	struct iwinfo_assoclist_entry *e = arr->buf;
	struct nlattr **attr = nl80211_parse(msg);
	struct nlattr *sinfo[NL80211_STA_INFO_MAX + 1];

	static struct nla_policy stats_policy[NL80211_STA_INFO_MAX + 1] = {
		[NL80211_STA_INFO_INACTIVE_TIME] = { .type = NLA_U32    },
		[NL80211_STA_INFO_RX_BYTES]      = { .type = NLA_U32    },
		[NL80211_STA_INFO_TX_BYTES]      = { .type = NLA_U32    },
		[NL80211_STA_INFO_RX_PACKETS]    = { .type = NLA_U32    },
		[NL80211_STA_INFO_TX_PACKETS]    = { .type = NLA_U32    },
		[NL80211_STA_INFO_SIGNAL]        = { .type = NLA_U8     },
		[NL80211_STA_INFO_TX_BITRATE]    = { .type = NLA_NESTED },
		[NL80211_STA_INFO_LLID]          = { .type = NLA_U16    },
		[NL80211_STA_INFO_PLID]          = { .type = NLA_U16    },
		[NL80211_STA_INFO_PLINK_STATE]   = { .type = NLA_U8     },
	};

	/* advance to end of array */
	e += arr->count;

	if (attr[NL80211_ATTR_MAC])
		memcpy(e->mac, nla_data(attr[NL80211_ATTR_MAC]), 6);

	if (attr[NL80211_ATTR_STA_INFO])
	{
		if (!nla_parse_nested(sinfo, NL80211_STA_INFO_MAX,
				attr[NL80211_ATTR_STA_INFO], stats_policy))
		{
			if (sinfo[NL80211_STA_INFO_SIGNAL])
				e->signal = nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL]);
		}
	}

	e->noise = 0; /* filled in by caller */
	arr->count++;

	return NL_SKIP;
}

int nl80211_get_assoclist(const char *ifname, char *buf, int *len)
{
	DIR *d;
	int i, noise = 0;
	struct dirent *de;
	struct nl80211_msg_conveyor *req;
	struct nl80211_array_buf arr = { .buf = buf, .count = 0 };
	struct iwinfo_assoclist_entry *e;

	if ((d = opendir("/sys/class/net")) != NULL)
	{
		while ((de = readdir(d)) != NULL)
		{
			if (!strncmp(de->d_name, ifname, strlen(ifname)) &&
				(!de->d_name[strlen(ifname)] ||
				 !strncmp(&de->d_name[strlen(ifname)], ".sta", 4)))
			{
				req = nl80211_msg(de->d_name, NL80211_CMD_GET_STATION,
								  NLM_F_DUMP);

				if (req)
				{
					nl80211_send(req, nl80211_get_assoclist_cb, &arr);
					nl80211_free(req);
				}

				break;
			}
		}

		closedir(d);

		if (!nl80211_get_noise(ifname, &noise))
			for (i = 0, e = arr.buf; i < arr.count; i++, e++)
				e->noise = noise;

		*len = (arr.count * sizeof(struct iwinfo_assoclist_entry));
		return 0;
	}

	return -1;
}

static int nl80211_get_txpwrlist_cb(struct nl_msg *msg, void *arg)
{
	int *dbm_max = arg;
	int ch_cur, ch_cmp, bands_remain, freqs_remain;

	struct nlattr **attr = nl80211_parse(msg);
	struct nlattr *bands[NL80211_BAND_ATTR_MAX + 1];
	struct nlattr *freqs[NL80211_FREQUENCY_ATTR_MAX + 1];
	struct nlattr *band, *freq;

	static struct nla_policy freq_policy[NL80211_FREQUENCY_ATTR_MAX + 1] = {
		[NL80211_FREQUENCY_ATTR_FREQ]         = { .type = NLA_U32  },
		[NL80211_FREQUENCY_ATTR_DISABLED]     = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_PASSIVE_SCAN] = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_NO_IBSS]      = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_RADAR]        = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_MAX_TX_POWER] = { .type = NLA_U32  },
	};

	ch_cur = *dbm_max; /* value int* is initialized with channel by caller */
	*dbm_max = -1;

	nla_for_each_nested(band, attr[NL80211_ATTR_WIPHY_BANDS], bands_remain)
	{
		nla_parse(bands, NL80211_BAND_ATTR_MAX, nla_data(band),
			  nla_len(band), NULL);

		nla_for_each_nested(freq,
			bands[NL80211_BAND_ATTR_FREQS], freqs_remain)
		{
			nla_parse(freqs, NL80211_FREQUENCY_ATTR_MAX,
				nla_data(freq), nla_len(freq), freq_policy);

			ch_cmp = nl80211_freq2channel(
				nla_get_u32(freqs[NL80211_FREQUENCY_ATTR_FREQ]));

			if( (!ch_cur || (ch_cmp == ch_cur)) &&
				freqs[NL80211_FREQUENCY_ATTR_MAX_TX_POWER] )
			{
				*dbm_max = (int)(0.01 * nla_get_u32(
					freqs[NL80211_FREQUENCY_ATTR_MAX_TX_POWER]));

				break;
			}
		}
	}

	return NL_SKIP;
}

int nl80211_get_txpwrlist(const char *ifname, char *buf, int *len)
{
	int ch_cur;
	int dbm_max = -1, dbm_cur, dbm_cnt;
	struct nl80211_msg_conveyor *req;
	struct iwinfo_txpwrlist_entry entry;

	if (nl80211_get_channel(ifname, &ch_cur))
		ch_cur = 0;

	req = nl80211_msg(ifname, NL80211_CMD_GET_WIPHY, 0);
	if (req)
	{
		/* initialize the value pointer with channel for callback */
		dbm_max = ch_cur;

		nl80211_send(req, nl80211_get_txpwrlist_cb, &dbm_max);
		nl80211_free(req);
	}

	if (dbm_max > -1)
	{
		for (dbm_cur = 0, dbm_cnt = 0;
		     dbm_cur < dbm_max;
		     dbm_cur++, dbm_cnt++)
		{
			entry.dbm = dbm_cur;
			entry.mw  = iwinfo_dbm2mw(dbm_cur);

			memcpy(&buf[dbm_cnt * sizeof(entry)], &entry, sizeof(entry));
		}

		entry.dbm = dbm_max;
		entry.mw  = iwinfo_dbm2mw(dbm_max);

		memcpy(&buf[dbm_cnt * sizeof(entry)], &entry, sizeof(entry));
		dbm_cnt++;

		*len = dbm_cnt * sizeof(entry);
		return 0;
	}

	return -1;
}

static void nl80211_get_scancrypto(const char *spec,
	struct iwinfo_crypto_entry *c)
{
	if (strstr(spec, "WPA") || strstr(spec, "WEP"))
	{
		c->enabled = 1;

		if (strstr(spec, "WPA2-") && strstr(spec, "WPA-"))
			c->wpa_version = 3;

		else if (strstr(spec, "WPA2"))
			c->wpa_version = 2;

		else if (strstr(spec, "WPA"))
			c->wpa_version = 1;

		else if (strstr(spec, "WEP"))
			c->auth_algs = IWINFO_AUTH_OPEN | IWINFO_AUTH_SHARED;


		if (strstr(spec, "PSK"))
			c->auth_suites |= IWINFO_KMGMT_PSK;

		if (strstr(spec, "802.1X") || strstr(spec, "EAP"))
			c->auth_suites |= IWINFO_KMGMT_8021x;

		if (strstr(spec, "WPA-NONE"))
			c->auth_suites |= IWINFO_KMGMT_NONE;


		if (strstr(spec, "TKIP"))
			c->pair_ciphers |= IWINFO_CIPHER_TKIP;

		if (strstr(spec, "CCMP"))
			c->pair_ciphers |= IWINFO_CIPHER_CCMP;

		if (strstr(spec, "WEP-40"))
			c->pair_ciphers |= IWINFO_CIPHER_WEP40;

		if (strstr(spec, "WEP-104"))
			c->pair_ciphers |= IWINFO_CIPHER_WEP104;

		c->group_ciphers = c->pair_ciphers;
	}
	else
	{
		c->enabled = 0;
	}
}

int nl80211_get_scanlist(const char *ifname, char *buf, int *len)
{
	int freq, rssi, qmax, count;
	char *res;
	char ssid[128] = { 0 };
	char bssid[18] = { 0 };
	char cipher[256] = { 0 };

	/* Got a radioX pseudo interface, find some interface on it or create one */
	if (!strncmp(ifname, "radio", 5))
	{
		/* Reuse existing interface */
		if ((res = nl80211_phy2ifname(ifname)) != NULL)
		{
			return nl80211_get_scanlist(res, buf, len);
		}

		/* Need to spawn a temporary iface for scanning */
		else if ((res = nl80211_ifadd(ifname)) != NULL)
		{
			count = nl80211_get_scanlist(res, buf, len);
			nl80211_ifdel(res);
			return count;
		}
	}

	struct iwinfo_scanlist_entry *e = (struct iwinfo_scanlist_entry *)buf;

	/* WPA supplicant */
	if ((res = nl80211_wpactl_info(ifname, "SCAN", "CTRL-EVENT-SCAN-RESULTS")))
	{
		if ((res = nl80211_wpactl_info(ifname, "SCAN_RESULTS", NULL)))
		{
			nl80211_get_quality_max(ifname, &qmax);

			/* skip header line */
			while( *res++ != '\n' );

			count = 0;

			while( sscanf(res, "%17s %d %d %255s%*[ \t]%127[^\n]\n",
			              bssid, &freq, &rssi, cipher, ssid) > 0 )
			{
				/* BSSID */
				e->mac[0] = strtol(&bssid[0],  NULL, 16);
				e->mac[1] = strtol(&bssid[3],  NULL, 16);
				e->mac[2] = strtol(&bssid[6],  NULL, 16);
				e->mac[3] = strtol(&bssid[9],  NULL, 16);
				e->mac[4] = strtol(&bssid[12], NULL, 16);
				e->mac[5] = strtol(&bssid[15], NULL, 16);

				/* SSID */
				memcpy(e->ssid, ssid,
					min(strlen(ssid), sizeof(e->ssid) - 1));

				/* Mode (assume master) */
				sprintf((char *)e->mode, "Master");

				/* Channel */
				e->channel = nl80211_freq2channel(freq);

				/* Signal */
				e->signal = rssi;

				/* Quality */
				if (rssi < 0)
				{
					/* The cfg80211 wext compat layer assumes a signal range
					 * of -110 dBm to -40 dBm, the quality value is derived
					 * by adding 110 to the signal level */
					if (rssi < -110)
						rssi = -110;
					else if (rssi > -40)
						rssi = -40;

					e->quality = (rssi + 110);
				}
				else
				{
					e->quality = rssi;
				}

				/* Max. Quality */
				e->quality_max = qmax;

				/* Crypto */
				nl80211_get_scancrypto(cipher, &e->crypto);

				/* advance to next line */
				while( *res && *res++ != '\n' );

				count++;
				e++;

				memset(ssid, 0, sizeof(ssid));
				memset(bssid, 0, sizeof(bssid));
				memset(cipher, 0, sizeof(cipher));
			}

			*len = count * sizeof(struct iwinfo_scanlist_entry);
			return 0;
		}
	}

	/* AP scan */
	else
	{
		/* Got a temp interface, don't create yet another one */
		if (!strncmp(ifname, "tmp.", 4))
		{
			if (!iwinfo_ifup(ifname))
				return -1;

			wext_get_scanlist(ifname, buf, len);
			iwinfo_ifdown(ifname);
			return 0;
		}

		/* Spawn a new scan interface */
		else
		{
			if (!(res = nl80211_ifadd(ifname)))
				goto out;

			if (!iwinfo_ifmac(res))
				goto out;

			/* if we can take the new interface up, the driver supports an
			 * additional interface and there's no need to tear down the ap */
			if (iwinfo_ifup(res))
			{
				wext_get_scanlist(res, buf, len);
				iwinfo_ifdown(res);
			}

			/* driver cannot create secondary interface, take down ap
			 * during scan */
			else if (iwinfo_ifdown(ifname) && iwinfo_ifup(res))
			{
				wext_get_scanlist(res, buf, len);
				iwinfo_ifdown(res);
				iwinfo_ifup(ifname);
				nl80211_hostapd_hup(ifname);
			}

		out:
			nl80211_ifdel(res);
			return 0;
		}
	}

	return -1;
}

static int nl80211_get_freqlist_cb(struct nl_msg *msg, void *arg)
{
	int bands_remain, freqs_remain;

	struct nl80211_array_buf *arr = arg;
	struct iwinfo_freqlist_entry *e = arr->buf;

	struct nlattr **attr = nl80211_parse(msg);
	struct nlattr *bands[NL80211_BAND_ATTR_MAX + 1];
	struct nlattr *freqs[NL80211_FREQUENCY_ATTR_MAX + 1];
	struct nlattr *band, *freq;

	static struct nla_policy freq_policy[NL80211_FREQUENCY_ATTR_MAX + 1] = {
		[NL80211_FREQUENCY_ATTR_FREQ]         = { .type = NLA_U32  },
		[NL80211_FREQUENCY_ATTR_DISABLED]     = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_PASSIVE_SCAN] = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_NO_IBSS]      = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_RADAR]        = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_MAX_TX_POWER] = { .type = NLA_U32  },
	};

	nla_for_each_nested(band, attr[NL80211_ATTR_WIPHY_BANDS], bands_remain)
	{
		nla_parse(bands, NL80211_BAND_ATTR_MAX, nla_data(band),
			nla_len(band), NULL);

		nla_for_each_nested(freq,
			bands[NL80211_BAND_ATTR_FREQS], freqs_remain)
		{
			nla_parse(freqs, NL80211_FREQUENCY_ATTR_MAX,
				nla_data(freq), nla_len(freq), NULL);

			if( !freqs[NL80211_FREQUENCY_ATTR_FREQ] ||
				freqs[NL80211_FREQUENCY_ATTR_DISABLED] )
				continue;

			e->mhz = nla_get_u32(freqs[NL80211_FREQUENCY_ATTR_FREQ]);
			e->channel = nl80211_freq2channel(e->mhz);

			e->restricted = (
				freqs[NL80211_FREQUENCY_ATTR_PASSIVE_SCAN] ||
				freqs[NL80211_FREQUENCY_ATTR_NO_IBSS]      ||
				freqs[NL80211_FREQUENCY_ATTR_RADAR]
			) ? 1 : 0;

			e++;
			arr->count++;
		}
	}

	return NL_SKIP;
}

int nl80211_get_freqlist(const char *ifname, char *buf, int *len)
{
	struct nl80211_msg_conveyor *req;
	struct nl80211_array_buf arr = { .buf = buf, .count = 0 };

	req = nl80211_msg(ifname, NL80211_CMD_GET_WIPHY, 0);
	if (req)
	{
		nl80211_send(req, nl80211_get_freqlist_cb, &arr);
		nl80211_free(req);
	}

	if (arr.count > 0)
	{
		*len = arr.count * sizeof(struct iwinfo_freqlist_entry);
		return 0;
	}

	return -1;
}

static int nl80211_get_country_cb(struct nl_msg *msg, void *arg)
{
	char *buf = arg;
	struct nlattr **attr = nl80211_parse(msg);

	if (attr[NL80211_ATTR_REG_ALPHA2])
		memcpy(buf, nla_data(attr[NL80211_ATTR_REG_ALPHA2]), 2);
	else
		buf[0] = 0;

	return NL_SKIP;
}

int nl80211_get_country(const char *ifname, char *buf)
{
	int rv = -1;
	struct nl80211_msg_conveyor *req;

	req = nl80211_msg(ifname, NL80211_CMD_GET_REG, 0);
	if (req)
	{
		nl80211_send(req, nl80211_get_country_cb, buf);
		nl80211_free(req);

		if (buf[0])
			rv = 0;
	}

	return rv;
}

int nl80211_get_countrylist(const char *ifname, char *buf, int *len)
{
	int i, count;
	struct iwinfo_country_entry *e = (struct iwinfo_country_entry *)buf;
	const struct iwinfo_iso3166_label *l;

	for( l = IWINFO_ISO3166_NAMES, count = 0; l->iso3166; l++, e++, count++ )
	{
		e->iso3166 = l->iso3166;
		e->ccode[0] = (l->iso3166 / 256);
		e->ccode[1] = (l->iso3166 % 256);
	}

	*len = (count * sizeof(struct iwinfo_country_entry));
	return 0;
}

static int nl80211_get_hwmodelist_cb(struct nl_msg *msg, void *arg)
{
	int *modes = arg;
	int bands_remain, freqs_remain;
	uint16_t caps = 0;
	struct nlattr **attr = nl80211_parse(msg);
	struct nlattr *bands[NL80211_BAND_ATTR_MAX + 1];
	struct nlattr *freqs[NL80211_FREQUENCY_ATTR_MAX + 1];
	struct nlattr *band, *freq;

	*modes = 0;

	if (attr[NL80211_ATTR_WIPHY_BANDS])
	{
		nla_for_each_nested(band, attr[NL80211_ATTR_WIPHY_BANDS], bands_remain)
		{
			nla_parse(bands, NL80211_BAND_ATTR_MAX, nla_data(band),
				nla_len(band), NULL);

			if (bands[NL80211_BAND_ATTR_HT_CAPA])
				caps = nla_get_u16(bands[NL80211_BAND_ATTR_HT_CAPA]);

			/* Treat any nonzero capability as 11n */
			if (caps > 0)
				*modes |= IWINFO_80211_N;

			nla_for_each_nested(freq,
				bands[NL80211_BAND_ATTR_FREQS], freqs_remain)
			{
				nla_parse(freqs, NL80211_FREQUENCY_ATTR_MAX,
					nla_data(freq), nla_len(freq), NULL);

				if (!freqs[NL80211_FREQUENCY_ATTR_FREQ])
					continue;

				if (nla_get_u32(freqs[NL80211_FREQUENCY_ATTR_FREQ]) < 2485)
				{
					*modes |= IWINFO_80211_B;
					*modes |= IWINFO_80211_G;
				}
				else
				{
					*modes |= IWINFO_80211_A;
				}
			}
		}
	}

	return NL_SKIP;
}

int nl80211_get_hwmodelist(const char *ifname, int *buf)
{
	struct nl80211_msg_conveyor *req;

	req = nl80211_msg(ifname, NL80211_CMD_GET_WIPHY, 0);
	if (req)
	{
		nl80211_send(req, nl80211_get_hwmodelist_cb, buf);
		nl80211_free(req);
	}

	return *buf ? 0 : -1;
}

int nl80211_get_mbssid_support(const char *ifname, int *buf)
{
	/* Test whether we can create another interface */
	char *nif = nl80211_ifadd(ifname);

	if (nif)
	{
		*buf = (iwinfo_ifmac(nif) && iwinfo_ifup(nif));

		iwinfo_ifdown(nif);
		nl80211_ifdel(nif);

		return 0;
	}

	return -1;
}

int nl80211_get_hardware_id(const char *ifname, char *buf)
{
	return wext_get_hardware_id(ifname, buf);
}

int nl80211_get_hardware_name(const char *ifname, char *buf)
{
	struct iwinfo_hardware_id id;
	struct iwinfo_hardware_entry *hw;

	if (nl80211_get_hardware_id(ifname, (char *)&id))
		return -1;

	hw = iwinfo_hardware(&id);

	if (hw)
		sprintf(buf, "%s %s", hw->vendor_name, hw->device_name);
	else
		sprintf(buf, "Generic MAC80211");

	return 0;
}
