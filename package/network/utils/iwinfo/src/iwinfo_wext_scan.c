/*
 * iwinfo - Wireless Information Library - Linux Wireless Extension Backend
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
 * Parts of this code are derived from the Linux wireless tools, iwlib.c,
 * iwlist.c and iwconfig.c in particular.
 */

#include "iwinfo.h"
#include "iwinfo_wext.h"


static int wext_ioctl(const char *ifname, int cmd, struct iwreq *wrq)
{
	strncpy(wrq->ifr_name, ifname, IFNAMSIZ);
	return iwinfo_ioctl(cmd, wrq);
}

static inline double wext_freq2float(const struct iw_freq *in)
{
	int		i;
	double	res = (double) in->m;
	for(i = 0; i < in->e; i++) res *= 10;
	return res;
}

static inline int wext_extract_event(struct stream_descr *stream, struct iw_event *iwe, int wev)
{
	const struct iw_ioctl_description *descr = NULL;
	int event_type = 0;
	unsigned int event_len = 1;
	char *pointer;
	unsigned cmd_index;		/* *MUST* be unsigned */

	/* Check for end of stream */
	if((stream->current + IW_EV_LCP_PK_LEN) > stream->end)
		return 0;

	/* Extract the event header (to get the event id).
	 * Note : the event may be unaligned, therefore copy... */
	memcpy((char *) iwe, stream->current, IW_EV_LCP_PK_LEN);

	/* Check invalid events */
	if(iwe->len <= IW_EV_LCP_PK_LEN)
		return -1;

	/* Get the type and length of that event */
	if(iwe->cmd <= SIOCIWLAST)
	{
		cmd_index = iwe->cmd - SIOCIWFIRST;
		if(cmd_index < standard_ioctl_num)
			descr = &(standard_ioctl_descr[cmd_index]);
	}
	else
	{
		cmd_index = iwe->cmd - IWEVFIRST;
		if(cmd_index < standard_event_num)
			descr = &(standard_event_descr[cmd_index]);
	}

	if(descr != NULL)
		event_type = descr->header_type;

	/* Unknown events -> event_type=0 => IW_EV_LCP_PK_LEN */
	event_len = event_type_size[event_type];

	/* Fixup for earlier version of WE */
	if((wev <= 18) && (event_type == IW_HEADER_TYPE_POINT))
		event_len += IW_EV_POINT_OFF;

	/* Check if we know about this event */
	if(event_len <= IW_EV_LCP_PK_LEN)
	{
		/* Skip to next event */
		stream->current += iwe->len;
		return 2;
	}

	event_len -= IW_EV_LCP_PK_LEN;

	/* Set pointer on data */
	if(stream->value != NULL)
		pointer = stream->value;			/* Next value in event */
	else
		pointer = stream->current + IW_EV_LCP_PK_LEN;	/* First value in event */

	/* Copy the rest of the event (at least, fixed part) */
	if((pointer + event_len) > stream->end)
	{
		/* Go to next event */
		stream->current += iwe->len;
		return -2;
	}

	/* Fixup for WE-19 and later : pointer no longer in the stream */
	/* Beware of alignement. Dest has local alignement, not packed */
	if( (wev > 18) && (event_type == IW_HEADER_TYPE_POINT) )
		memcpy((char *) iwe + IW_EV_LCP_LEN + IW_EV_POINT_OFF, pointer, event_len);
	else
		memcpy((char *) iwe + IW_EV_LCP_LEN, pointer, event_len);

	/* Skip event in the stream */
	pointer += event_len;

	/* Special processing for iw_point events */
	if(event_type == IW_HEADER_TYPE_POINT)
	{
		/* Check the length of the payload */
		unsigned int extra_len = iwe->len - (event_len + IW_EV_LCP_PK_LEN);
		if(extra_len > 0)
		{
			/* Set pointer on variable part (warning : non aligned) */
			iwe->u.data.pointer = pointer;

			/* Check that we have a descriptor for the command */
			if(descr == NULL)
				/* Can't check payload -> unsafe... */
				iwe->u.data.pointer = NULL;	/* Discard paylod */
			else
			{
				/* Those checks are actually pretty hard to trigger,
				* because of the checks done in the kernel... */

				unsigned int	token_len = iwe->u.data.length * descr->token_size;

				/* Ugly fixup for alignement issues.
				* If the kernel is 64 bits and userspace 32 bits,
				* we have an extra 4+4 bytes.
				* Fixing that in the kernel would break 64 bits userspace. */
				if((token_len != extra_len) && (extra_len >= 4))
				{
					uint16_t alt_dlen = *((uint16_t *) pointer);
					unsigned int alt_token_len = alt_dlen * descr->token_size;
					if((alt_token_len + 8) == extra_len)
					{
						/* Ok, let's redo everything */
						pointer -= event_len;
						pointer += 4;
						/* Dest has local alignement, not packed */
						memcpy((char *) iwe + IW_EV_LCP_LEN + IW_EV_POINT_OFF, pointer, event_len);
						pointer += event_len + 4;
						iwe->u.data.pointer = pointer;
						token_len = alt_token_len;
					}
				}

				/* Discard bogus events which advertise more tokens than
				* what they carry... */
				if(token_len > extra_len)
					iwe->u.data.pointer = NULL;	/* Discard paylod */

				/* Check that the advertised token size is not going to
				* produce buffer overflow to our caller... */
				if((iwe->u.data.length > descr->max_tokens)
				&& !(descr->flags & IW_DESCR_FLAG_NOMAX))
					iwe->u.data.pointer = NULL;	/* Discard paylod */

				/* Same for underflows... */
				if(iwe->u.data.length < descr->min_tokens)
					iwe->u.data.pointer = NULL;	/* Discard paylod */
			}
		}
		else
			/* No data */
			iwe->u.data.pointer = NULL;

		/* Go to next event */
		stream->current += iwe->len;
	}
	else
	{
		/* Ugly fixup for alignement issues.
		* If the kernel is 64 bits and userspace 32 bits,
		* we have an extra 4 bytes.
		* Fixing that in the kernel would break 64 bits userspace. */
		if((stream->value == NULL)
		&& ((((iwe->len - IW_EV_LCP_PK_LEN) % event_len) == 4)
		|| ((iwe->len == 12) && ((event_type == IW_HEADER_TYPE_UINT) ||
		(event_type == IW_HEADER_TYPE_QUAL))) ))
		{
			pointer -= event_len;
			pointer += 4;
			/* Beware of alignement. Dest has local alignement, not packed */
			memcpy((char *) iwe + IW_EV_LCP_LEN, pointer, event_len);
			pointer += event_len;
		}

		/* Is there more value in the event ? */
		if((pointer + event_len) <= (stream->current + iwe->len))
			/* Go to next value */
			stream->value = pointer;
		else
		{
			/* Go to next event */
			stream->value = NULL;
			stream->current += iwe->len;
		}
	}

	return 1;
}

static inline void wext_fill_wpa(unsigned char *iebuf, int ielen, struct iwinfo_scanlist_entry *e)
{
	static unsigned char ms_oui[3] = { 0x00, 0x50, 0xf2 };

	while (ielen >= 2 && ielen >= iebuf[1])
	{
		switch (iebuf[0])
		{
		case 48: /* RSN */
			iwinfo_parse_rsn(&e->crypto, iebuf + 2, iebuf[1],
			                 IWINFO_CIPHER_CCMP, IWINFO_KMGMT_8021x);
			break;

		case 221: /* Vendor */
			if (iebuf[1] >= 4 && !memcmp(iebuf + 2, ms_oui, 3) && iebuf[5] == 1)
				iwinfo_parse_rsn(&e->crypto, iebuf + 6, iebuf[1] - 4,
				                 IWINFO_CIPHER_TKIP, IWINFO_KMGMT_PSK);
			break;
		}

		ielen -= iebuf[1] + 2;
		iebuf += iebuf[1] + 2;
	}
}


static inline void wext_fill_entry(struct stream_descr *stream, struct iw_event *event,
	struct iw_range *iw_range, int has_range, struct iwinfo_scanlist_entry *e)
{
	int i;
	double freq;

	/* Now, let's decode the event */
	switch(event->cmd)
	{
		case SIOCGIWAP:
			memcpy(e->mac, &event->u.ap_addr.sa_data, 6);
			break;

		case SIOCGIWFREQ:
			if( event->u.freq.m >= 1000 )
			{
				freq = wext_freq2float(&(event->u.freq));

				for(i = 0; i < iw_range->num_frequency; i++)
				{
					if( wext_freq2float(&iw_range->freq[i]) == freq )
					{
						e->channel = iw_range->freq[i].i;
						break;
					}
				}
			}
			else
			{
				e->channel = event->u.freq.m;
			}

			break;

		case SIOCGIWMODE:
			switch(event->u.mode)
			{
				case 1:
					e->mode = IWINFO_OPMODE_ADHOC;
					break;

				case 2:
				case 3:
					e->mode = IWINFO_OPMODE_MASTER;
					break;

				default:
					e->mode = IWINFO_OPMODE_UNKNOWN;
					break;
			}

			break;

		case SIOCGIWESSID:
			if( event->u.essid.pointer && event->u.essid.length && event->u.essid.flags )
				memcpy(e->ssid, event->u.essid.pointer, event->u.essid.length);

			break;

		case SIOCGIWENCODE:
			e->crypto.enabled = !(event->u.data.flags & IW_ENCODE_DISABLED);
			break;

		case IWEVQUAL:
			e->signal = event->u.qual.level;
			e->quality = event->u.qual.qual;
			e->quality_max = iw_range->max_qual.qual;
			break;
#if 0
		case SIOCGIWRATE:
			if(state->val_index == 0)
			{
				lua_pushstring(L, "bitrates");
				lua_newtable(L);
			}
			//iw_print_bitrate(buffer, sizeof(buffer), event->u.bitrate.value);
			snprintf(buffer, sizeof(buffer), "%d", event->u.bitrate.value);
			lua_pushinteger(L, state->val_index + 1);
			lua_pushstring(L, buffer);
			lua_settable(L, -3);

			/* Check for termination */
			if(stream->value == NULL)
			{
				lua_settable(L, -3);
				state->val_index = 0;
			} else
				state->val_index++;
			break;
#endif
		 case IWEVGENIE:
			wext_fill_wpa(event->u.data.pointer, event->u.data.length, e);
			break;
	}
}


int wext_get_scanlist(const char *ifname, char *buf, int *len)
{
	struct iwreq wrq;
	struct iw_scan_req scanopt;        /* Options for 'set' */
	unsigned char *buffer = NULL;      /* Results */
	int buflen = IW_SCAN_MAX_DATA; /* Min for compat WE<17 */
	struct iw_range range;
	int has_range = 1;
	struct timeval tv;             /* Select timeout */
	int timeout = 15000000;     /* 15s */

	int entrylen = 0;
	struct iwinfo_scanlist_entry e;

	wrq.u.data.pointer = (caddr_t) &range;
	wrq.u.data.length  = sizeof(struct iw_range);
	wrq.u.data.flags   = 0;

	if( wext_ioctl(ifname, SIOCGIWRANGE, &wrq) >= 0 )
	{
		/* Init timeout value -> 250ms between set and first get */
		tv.tv_sec  = 0;
		tv.tv_usec = 250000;

		/* Clean up set args */
		memset(&scanopt, 0, sizeof(scanopt));

		wrq.u.data.pointer = NULL;
		wrq.u.data.flags   = 0;
		wrq.u.data.length  = 0;

		/* Initiate Scanning */
		if( wext_ioctl(ifname, SIOCSIWSCAN, &wrq) >= 0 )
		{
			timeout -= tv.tv_usec;

			/* Forever */
			while(1)
			{
				fd_set rfds;       /* File descriptors for select */
				int last_fd;    /* Last fd */
				int ret;

				/* Guess what ? We must re-generate rfds each time */
				FD_ZERO(&rfds);
				last_fd = -1;
				/* In here, add the rtnetlink fd in the list */

				/* Wait until something happens */
				ret = select(last_fd + 1, &rfds, NULL, NULL, &tv);

				/* Check if there was an error */
				if(ret < 0)
				{
					if(errno == EAGAIN || errno == EINTR)
						continue;

					return -1;
				}

				/* Check if there was a timeout */
				if(ret == 0)
				{
					unsigned char *newbuf;

		realloc:
					/* (Re)allocate the buffer - realloc(NULL, len) == malloc(len) */
					newbuf = realloc(buffer, buflen);
					if(newbuf == NULL)
					{
						if(buffer)
							free(buffer);

						return -1;
					}

					buffer = newbuf;

					/* Try to read the results */
					wrq.u.data.pointer = buffer;
					wrq.u.data.flags   = 0;
					wrq.u.data.length  = buflen;

					if( wext_ioctl(ifname, SIOCGIWSCAN, &wrq) )
					{
						/* Check if buffer was too small (WE-17 only) */
						if((errno == E2BIG) && (range.we_version_compiled > 16))
						{
							/* Some driver may return very large scan results, either
							 * because there are many cells, or because they have many
							 * large elements in cells (like IWEVCUSTOM). Most will
							 * only need the regular sized buffer. We now use a dynamic
							 * allocation of the buffer to satisfy everybody. Of course,
							 * as we don't know in advance the size of the array, we try
							 * various increasing sizes. Jean II */

							/* Check if the driver gave us any hints. */
							if(wrq.u.data.length > buflen)
								buflen = wrq.u.data.length;
							else
								buflen *= 2;

							/* Try again */
							goto realloc;
						}

						/* Check if results not available yet */
						if(errno == EAGAIN)
						{
							/* Restart timer for only 100ms*/
							tv.tv_sec = 0;
							tv.tv_usec = 100000;
							timeout -= tv.tv_usec;

							if(timeout > 0)
								continue;   /* Try again later */
						}

						/* Bad error */
						free(buffer);
						return -1;

					} else {
						/* We have the results, go to process them */
						break;
					}
				}
			}

			if( wrq.u.data.length )
			{
				struct iw_event       iwe;
				struct stream_descr   stream;
				int ret;
				int first = 1;

				memset(&stream, 0, sizeof(stream));
				stream.current = (char *)buffer;
				stream.end     = (char *)buffer + wrq.u.data.length;

				do
				{
					/* Extract an event and print it */
					ret = wext_extract_event(&stream, &iwe, range.we_version_compiled);

					if(ret >= 0)
					{
						if( (iwe.cmd == SIOCGIWAP) || (ret == 0) )
						{
							if( first )
							{
								first = 0;
							}
							else if( (entrylen + sizeof(struct iwinfo_scanlist_entry)) <= IWINFO_BUFSIZE )
							{
								/* if encryption is off, clear the crypto strunct */
								if( !e.crypto.enabled )
									memset(&e.crypto, 0, sizeof(struct iwinfo_crypto_entry));

								memcpy(&buf[entrylen], &e, sizeof(struct iwinfo_scanlist_entry));
								entrylen += sizeof(struct iwinfo_scanlist_entry);
							}
							else
							{
								/* we exceed the callers buffer size, abort here ... */
								break;
							}

							memset(&e, 0, sizeof(struct iwinfo_scanlist_entry));
						}

						wext_fill_entry(&stream, &iwe, &range, has_range, &e);
					}

				} while(ret > 0);

				free(buffer);
				*len = entrylen;
				return 0;
			}

			*len = 0;
			free(buffer);
			return 0;
		}
	}

	return -1;
}
