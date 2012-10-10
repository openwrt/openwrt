/*
 * iwinfo - Wireless Information Library - Shared utility routines
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
 */

#include "iwinfo/utils.h"


static int ioctl_socket = -1;

static int iwinfo_ioctl_socket(void)
{
	/* Prepare socket */
	if( ioctl_socket == -1 )
	{
		ioctl_socket = socket(AF_INET, SOCK_DGRAM, 0);
		fcntl(ioctl_socket, F_SETFD, fcntl(ioctl_socket, F_GETFD) | FD_CLOEXEC);
	}

	return ioctl_socket;
}

int iwinfo_ioctl(int cmd, void *ifr)
{
	int s = iwinfo_ioctl_socket();
	return ioctl(s, cmd, ifr);
}

int iwinfo_dbm2mw(int in)
{
	double res = 1.0;
	int ip = in / 10;
	int fp = in % 10;
	int k;

	for(k = 0; k < ip; k++) res *= 10;
	for(k = 0; k < fp; k++) res *= LOG10_MAGIC;

	return (int)res;
}

int iwinfo_mw2dbm(int in)
{
	double fin = (double) in;
	int res = 0;

	while(fin > 10.0)
	{
		res += 10;
		fin /= 10.0;
	}

	while(fin > 1.000001)
	{
		res += 1;
		fin /= LOG10_MAGIC;
	}

	return (int)res;
}

int iwinfo_ifup(const char *ifname)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

	if( iwinfo_ioctl(SIOCGIFFLAGS, &ifr) )
		return 0;

	ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);

	return !iwinfo_ioctl(SIOCSIFFLAGS, &ifr);
}

int iwinfo_ifdown(const char *ifname)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

	if( iwinfo_ioctl(SIOCGIFFLAGS, &ifr) )
		return 0;

	ifr.ifr_flags &= ~(IFF_UP | IFF_RUNNING);

	return !iwinfo_ioctl(SIOCSIFFLAGS, &ifr);
}

int iwinfo_ifmac(const char *ifname)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

	if( iwinfo_ioctl(SIOCGIFHWADDR, &ifr) )
		return 0;

	ifr.ifr_hwaddr.sa_data[1]++;
	ifr.ifr_hwaddr.sa_data[2]++;

	return !iwinfo_ioctl(SIOCSIFHWADDR, &ifr);
}

void iwinfo_close(void)
{
	if( ioctl_socket > -1 )
		close(ioctl_socket);
}

struct iwinfo_hardware_entry * iwinfo_hardware(struct iwinfo_hardware_id *id)
{
	const struct iwinfo_hardware_entry *e;

	for (e = IWINFO_HARDWARE_ENTRIES; e->vendor_name; e++)
	{
		if ((e->vendor_id != 0xffff) && (e->vendor_id != id->vendor_id))
			continue;

		if ((e->device_id != 0xffff) && (e->device_id != id->device_id))
			continue;

		if ((e->subsystem_vendor_id != 0xffff) &&
			(e->subsystem_vendor_id != id->subsystem_vendor_id))
			continue;

		if ((e->subsystem_device_id != 0xffff) &&
			(e->subsystem_device_id != id->subsystem_device_id))
			continue;

		return (struct iwinfo_hardware_entry *)e;
	}

	return NULL;
}

int iwinfo_hardware_id_from_mtd(struct iwinfo_hardware_id *id)
{
	FILE *mtd;
	uint16_t *bc;

	int fd, len, off;
	char buf[128];

	if (!(mtd = fopen("/proc/mtd", "r")))
		return -1;

	while (fgets(buf, sizeof(buf), mtd) > 0)
	{
		if (fscanf(mtd, "mtd%d: %*x %x %127s", &off, &len, buf) < 3 ||
		    (strcmp(buf, "\"boardconfig\"") && strcmp(buf, "\"EEPROM\"") &&
		     strcmp(buf, "\"factory\"")))
		{
			off = -1;
			continue;
		}

		break;
	}

	fclose(mtd);

	if (off < 0)
		return -1;

	snprintf(buf, sizeof(buf), "/dev/mtdblock%d", off);

	if ((fd = open(buf, O_RDONLY)) < 0)
		return -1;

	bc = mmap(NULL, len, PROT_READ, MAP_PRIVATE|MAP_LOCKED, fd, 0);

	if ((void *)bc != MAP_FAILED)
	{
		id->vendor_id = 0;
		id->device_id = 0;

		for (off = len / 2 - 0x800; off >= 0; off -= 0x800)
		{
			/* AR531X board data magic */
			if ((bc[off] == 0x3533) && (bc[off + 1] == 0x3131))
			{
				id->vendor_id = bc[off + 0x7d];
				id->device_id = bc[off + 0x7c];
				id->subsystem_vendor_id = bc[off + 0x84];
				id->subsystem_device_id = bc[off + 0x83];
				break;
			}

			/* AR5416 EEPROM magic */
			else if ((bc[off] == 0xA55A) || (bc[off] == 0x5AA5))
			{
				id->vendor_id = bc[off + 0x0D];
				id->device_id = bc[off + 0x0E];
				id->subsystem_vendor_id = bc[off + 0x13];
				id->subsystem_device_id = bc[off + 0x14];
				break;
			}

			/* Rt3xxx SoC */
			else if ((bc[off] == 0x3352) || (bc[off] == 0x5233) ||
			         (bc[off] == 0x3350) || (bc[off] == 0x5033) ||
			         (bc[off] == 0x3050) || (bc[off] == 0x5030) ||
			         (bc[off] == 0x3052) || (bc[off] == 0x5230))
			{
				/* vendor: RaLink */
				id->vendor_id = 0x1814;
				id->subsystem_vendor_id = 0x1814;

				/* device */
				if (bc[off] & 0xf0 == 0x30)
					id->device_id = (bc[off] >> 8) | (bc[off] & 0x00ff) << 8;
				else
					id->device_id = bc[off];

				/* subsystem from EEPROM_NIC_CONF0_RF_TYPE */
				id->subsystem_device_id = (bc[off + 0x1a] & 0x0f00) >> 8;
			}
		}

		munmap(bc, len);
	}

	close(fd);

	return (id->vendor_id && id->device_id) ? 0 : -1;
}

void iwinfo_parse_rsn(struct iwinfo_crypto_entry *c, uint8_t *data, uint8_t len,
					  uint8_t defcipher, uint8_t defauth)
{
	uint16_t i, count;

	static unsigned char ms_oui[3]        = { 0x00, 0x50, 0xf2 };
	static unsigned char ieee80211_oui[3] = { 0x00, 0x0f, 0xac };

	data += 2;
	len -= 2;

	if (!memcmp(data, ms_oui, 3))
		c->wpa_version += 1;
	else if (!memcmp(data, ieee80211_oui, 3))
		c->wpa_version += 2;

	if (len < 4)
	{
		c->group_ciphers |= defcipher;
		c->pair_ciphers  |= defcipher;
		c->auth_suites   |= defauth;
		return;
	}

	if (!memcmp(data, ms_oui, 3) || !memcmp(data, ieee80211_oui, 3))
	{
		switch (data[3])
		{
			case 1: c->group_ciphers |= IWINFO_CIPHER_WEP40;  break;
			case 2: c->group_ciphers |= IWINFO_CIPHER_TKIP;   break;
			case 4: c->group_ciphers |= IWINFO_CIPHER_CCMP;   break;
			case 5: c->group_ciphers |= IWINFO_CIPHER_WEP104; break;
			case 6:  /* AES-128-CMAC */ break;
			default: /* proprietary */  break;
		}
	}

	data += 4;
	len -= 4;

	if (len < 2)
	{
		c->pair_ciphers |= defcipher;
		c->auth_suites  |= defauth;
		return;
	}

	count = data[0] | (data[1] << 8);
	if (2 + (count * 4) > len)
		return;

	for (i = 0; i < count; i++)
	{
		if (!memcmp(data + 2 + (i * 4), ms_oui, 3) ||
			!memcmp(data + 2 + (i * 4), ieee80211_oui, 3))
		{
			switch (data[2 + (i * 4) + 3])
			{
				case 1: c->pair_ciphers |= IWINFO_CIPHER_WEP40;  break;
				case 2: c->pair_ciphers |= IWINFO_CIPHER_TKIP;   break;
				case 4: c->pair_ciphers |= IWINFO_CIPHER_CCMP;   break;
				case 5: c->pair_ciphers |= IWINFO_CIPHER_WEP104; break;
				case 6:  /* AES-128-CMAC */ break;
				default: /* proprietary */  break;
			}
		}
	}

	data += 2 + (count * 4);
	len -= 2 + (count * 4);

	if (len < 2)
	{
		c->auth_suites |= defauth;
		return;
	}

	count = data[0] | (data[1] << 8);
	if (2 + (count * 4) > len)
		return;

	for (i = 0; i < count; i++)
	{
		if (!memcmp(data + 2 + (i * 4), ms_oui, 3) ||
			!memcmp(data + 2 + (i * 4), ieee80211_oui, 3))
		{
			switch (data[2 + (i * 4) + 3])
			{
				case 1: c->auth_suites |= IWINFO_KMGMT_8021x; break;
				case 2: c->auth_suites |= IWINFO_KMGMT_PSK;   break;
				case 3:  /* FT/IEEE 802.1X */                 break;
				case 4:  /* FT/PSK */                         break;
				case 5:  /* IEEE 802.1X/SHA-256 */            break;
				case 6:  /* PSK/SHA-256 */                    break;
				default: /* proprietary */                    break;
			}
		}
	}

	data += 2 + (count * 4);
	len -= 2 + (count * 4);
}
