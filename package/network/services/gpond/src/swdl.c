// SPDX-License-Identifier: GPL-2.0
/*
 * OMCI software download (G.988 A.2.25-A.2.27, baseline messages).
 *
 * Off by default: updates are enabled only when /etc/config/gpon gives
 * the version of the running image (onu.sw_version, or onu.sw_version<slot>
 * for the slot it runs from); otherwise Start software download is answered
 * "not supported".  Once enabled, the image is written to onu.sw_dir
 * (onu.sw_store, on by default) and on success the slot is reported valid,
 * with the version of onu.sw_version<slot>.  Nothing is ever flashed.  The
 * slot state is written as main.config_mode says.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "gpond.h"
#include "omci.h"

static struct swdl_state dl = { .fd = -1 };

/* ITU-T I.363.5 (AAL5) CRC-32: MSB first, preset and final inversion */
static uint32_t crc32_aal5(uint32_t crc, const uint8_t *p, size_t len)
{
	int i;

	while (len--) {
		crc ^= (uint32_t)*p++ << 24;
		for (i = 0; i < 8; i++)
			crc = crc & 0x80000000 ? crc << 1 ^ 0x04c11db7 : crc << 1;
	}

	return crc;
}

static void dl_close(bool keep)
{
	if (dl.fd >= 0)
		close(dl.fd);
	dl.fd = -1;
	if (!keep && dl.path[0])
		unlink(dl.path);
	dl.active = false;
}

static void slot_valid(uint16_t inst, bool valid)
{
	struct omci_inst *in = mib_find(7, inst);

	if (in)
		mib_set_uint(in, 4, valid);
	cfg.sw_valid[inst] = valid;
}

uint8_t swdl_start(uint16_t inst, const uint8_t *c, uint8_t *rc)
{
	unsigned int window = c[0] + 1;
	int active = cfg.sw_active == 1;

	if (inst > 1 || !mib_find(7, inst))
		return RES_UNKNOWN_INST;
	if (!cfg.sw_ver_slot[active][0]) {
		INFO("swdl: image %d has no version (onu.sw_version), updates not supported",
		     active);
		return RES_NOT_SUPPORTED;
	}
	if (inst == (uint16_t)active)
		return RES_PARAM_ERR;		/* never the running image */

	dl_close(false);
	memset(dl.path, 0, sizeof(dl.path));
	memset(dl.ver, 0, sizeof(dl.ver));
	dl.inst = inst;
	dl.size = rd_be32(c + 1);
	dl.done = 0;
	dl.fill = 0;
	dl.window = window > MAX_WINDOW ? MAX_WINDOW : window;
	dl.crc = 0xffffffff;
	dl.active = true;

	if (cfg.sw_store) {
		snprintf(dl.path, sizeof(dl.path), "%s/gpon-image%u.bin",
			 cfg.sw_dir, inst);
		dl.fd = open(dl.path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
		if (dl.fd < 0) {
			ERR("swdl: %s: %s", dl.path, strerror(errno));
			dl.active = false;
			return RES_PROC_ERR;
		}
	}

	slot_valid(inst, false);
	INFO("swdl: image %u, %u bytes, window %u%s%s", inst, dl.size,
	     dl.window, dl.fd >= 0 ? " -> " : "", dl.fd >= 0 ? dl.path : "");

	rc[1] = dl.window - 1;
	rc[2] = 1;			/* one instance */
	rc[3] = inst >> 8;
	rc[4] = inst & 0xff;
	rc[5] = RES_OK;

	return RES_OK;
}

uint8_t swdl_section(uint16_t inst, const uint8_t *c, bool ack, uint8_t *rc)
{
	unsigned int sec = c[0];
	uint32_t off = sec * SECTION_LEN, len;

	rc[1] = sec;
	if (!dl.active || inst != dl.inst || sec >= dl.window)
		return RES_PARAM_ERR;

	if (dl.done + off < dl.size) {
		len = dl.size - dl.done - off;
		if (len > SECTION_LEN)
			len = SECTION_LEN;
		memcpy(dl.buf + off, c + 1, len);
		if (off + len > dl.fill)
			dl.fill = off + len;
	}

	if (!ack)
		return RES_OK;

	/* window complete: account for it */
	if (!dl.done && dl.fill >= 4) {
		unsigned int i, n = 0;

		for (i = 0; i < dl.fill && i < 14; i++) {
			if (dl.buf[i] < 0x21 || dl.buf[i] > 0x7e)
				break;
			dl.ver[n++] = (char)dl.buf[i];
		}
		if (n < 4)
			dl.ver[0] = 0;
		else
			dl.ver[n] = 0;
	}
	dl.crc = crc32_aal5(dl.crc, dl.buf, dl.fill);
	if (dl.fd >= 0 && write(dl.fd, dl.buf, dl.fill) != (ssize_t)dl.fill) {
		ERR("swdl: write: %s", strerror(errno));
		dl_close(false);
		return RES_PROC_ERR;
	}
	dl.done += dl.fill;
	dl.fill = 0;

	return RES_OK;
}

uint8_t swdl_end(uint16_t inst, const uint8_t *c, uint8_t *rc)
{
	uint32_t crc = rd_be32(c), size = rd_be32(c + 4);
	uint8_t res = RES_OK;

	rc[1] = 1;
	rc[2] = inst >> 8;
	rc[3] = inst & 0xff;

	if (!dl.active || inst != dl.inst) {
		rc[4] = RES_PARAM_ERR;
		return RES_PARAM_ERR;
	}

	if (size != dl.size || dl.done != dl.size ||
	    ~dl.crc != crc) {
		ERR("swdl: image %u rejected: %u/%u bytes, crc %08x/%08x",
		    inst, dl.done, size, ~dl.crc, crc);
		res = RES_PROC_ERR;
		dl_close(false);
	} else {
		struct omci_inst *in = mib_find(7, inst);
		const char *ver;

		dl_close(true);
		slot_valid(inst, true);
		if (dl.ver[0])
			snprintf(cfg.sw_ver_slot[inst],
				 sizeof(cfg.sw_ver_slot[inst]), "%s", dl.ver);
		ver = cfg.sw_ver_slot[inst];
		if (in)
			mib_set_str(in, 1, ver);
		INFO("swdl: image %u complete (%u bytes, version '%s')",
		     inst, size, ver);
		if (config_write(&cfg))
			ERR("swdl: cannot record the image state");
	}

	rc[4] = res;

	return res;
}
