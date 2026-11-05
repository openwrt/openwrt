// SPDX-License-Identifier: GPL-2.0
/*
 * The MIB and OMCI provisioning frames built by the OLT, kept in
 * /usr/lib/gpon/gpon.json.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gpond.h"
#include "omci.h"
#include "omci_pm.h"

#ifndef DAT_DIR
#define DAT_DIR		"/usr/lib/gpon"
#endif
#define JSON_PATH	DAT_DIR "/gpon.json"
#define JSON_TMP	DAT_DIR "/.gpon.json.tmp"
#define JSON_VERSION	1
#define MAX_FRAMES	512

static struct raw_frame frames[MAX_FRAMES];
static unsigned int n_frames;
static bool replaying;
static bool dirty;

static void frames_clear(void)
{
	unsigned int i;

	for (i = 0; i < n_frames; i++)
		free(frames[i].data);
	memset(frames, 0, sizeof(frames));
	n_frames = 0;
}

void mibstore_record_frame(const uint8_t *msg, size_t len)
{
	uint8_t mt;
	uint8_t *p;

	if (replaying || len < 8)
		return;
	mt = msg[2] & 0x1f;
	if (mt == 15)				/* MT_MIB_RESET */
		frames_clear();
	if (n_frames >= MAX_FRAMES)
		return;
	p = malloc(len);
	if (!p)
		return;
	memcpy(p, msg, len);
	frames[n_frames].data = p;
	frames[n_frames].len = len;
	n_frames++;
}

static bool stored_attr(const struct omci_inst *in,
			const struct omci_attr_def *a)
{
	if (!in->val[a->idx] || !in->len[a->idx])
		return false;
	if (in->by_olt)
		return a->access & (OMCI_ACC_W | OMCI_ACC_SBC);

	return in->olt_mask & omci_mask(a->idx);
}

static uint8_t *from_hex(const char *s, size_t slen, unsigned int *len)
{
	uint8_t *v;

	if (!slen || slen % 2 || slen / 2 > 0xffff)
		return NULL;
	v = malloc(slen / 2);
	if (!v)
		return NULL;
	if (hex_decode(s, v, slen / 2)) {
		free(v);
		return NULL;
	}
	*len = slen / 2;

	return v;
}

/* ---- write ---- */

static int write_json_inst(FILE *f, const struct omci_inst *in, bool first)
{
	const struct omci_me_def *d = in->def;
	bool first_attr = true;
	unsigned int i, j;

	fprintf(f, "%s\n    { \"class\": %u, \"name\": \"%s\", "
		"\"instance\": %u, \"olt\": %u, \"attributes\": {",
		first ? "" : ",", d->class_id, d->name, in->id,
		in->by_olt ? 1 : 0);
	for (i = 0; i < d->nattr; i++) {
		const struct omci_attr_def *a = &d->attr[i];

		if (!stored_attr(in, a))
			continue;
		fprintf(f, "%s \"%u\": \"", first_attr ? "" : ",", a->idx);
		for (j = 0; j < in->len[a->idx]; j++)
			fprintf(f, "%02x", in->val[a->idx][j]);
		fputc('"', f);
		first_attr = false;
	}
	fprintf(f, " } }");

	return ferror(f) ? -EIO : 0;
}

static int write_json_file(unsigned int *n)
{
	struct omci_inst *in;
	bool first = true;
	int fd, d, ret = 0;
	unsigned int i, j;
	FILE *f;

	if (mkdir(DAT_DIR, 0755) && errno != EEXIST)
		return -errno;
	fd = open(JSON_TMP, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd < 0)
		return -errno;
	f = fdopen(fd, "w");
	if (!f) {
		ret = -errno;
		close(fd);
		unlink(JSON_TMP);
		return ret;
	}

	*n = 0;
	in = mib_find(2, 0);
	fprintf(f, "{\n  \"version\": %d,\n  \"mds\": %u,\n  \"frames\": [",
		JSON_VERSION, in ? (unsigned)mib_get_uint(in, 1) : 0);
	for (i = 0; i < n_frames; i++) {
		fprintf(f, "%s\n    \"", i ? "," : "");
		for (j = 0; j < frames[i].len; j++)
			fprintf(f, "%02x", frames[i].data[j]);
		fputc('"', f);
	}
	fprintf(f, "%s],\n  \"instances\": [", n_frames ? "\n  " : "");
	list_for_each_entry(in, &omci_mib, list) {
		if (!in->by_olt && !in->olt_mask)
			continue;
		if (write_json_inst(f, in, first)) {
			ret = -EIO;
			break;
		}
		first = false;
		(*n)++;
	}
	fprintf(f, "%s]\n}\n", first ? "" : "\n  ");
	if (ret || ferror(f) || fflush(f) || fsync(fileno(f)))
		ret = ret ? ret : -EIO;
	if (fclose(f) && !ret)
		ret = -EIO;
	if (!ret && rename(JSON_TMP, JSON_PATH))
		ret = -errno;
	if (ret) {
		unlink(JSON_TMP);
		return ret;
	}

	d = open(DAT_DIR, O_RDONLY);
	if (d >= 0) {
		fsync(d);
		close(d);
	}

	return 0;
}

int mibstore_submit(void)
{
	unsigned int n = 0;
	int ret = write_json_file(&n);

	if (ret) {
		ERR("OMCI: cannot write %s: %s", JSON_PATH, strerror(-ret));
		return ret;
	}
	dirty = false;
	INFO("OMCI: MIB stored in %s (%u instances, %u frames)", JSON_PATH, n,
	     n_frames);

	return 0;
}

int mibstore_erase(void)
{
	dirty = false;
	frames_clear();
	if (unlink(JSON_PATH) && errno != ENOENT) {
		ERR("OMCI: cannot remove %s: %s", JSON_PATH, strerror(errno));
		return -errno;
	}

	return 0;
}

void mibstore_changed(void)
{
	dirty = true;
}

bool mibstore_dirty(void)
{
	return dirty;
}

/* ---- read ---- */

static bool json_uint(const char *s, const char *key, unsigned int *out)
{
	const char *p = strstr(s, key);

	if (!p)
		return false;
	p += strlen(key);
	while (*p == ' ' || *p == '\t' || *p == ':')
		p++;

	return sscanf(p, "%u", out) == 1;
}

static void load_json_attrs(struct omci_inst *in, const char *s)
{
	const char *p = strstr(s, "\"attributes\"");

	if (!p)
		return;
	p = strchr(p, '{');
	if (!p)
		return;
	p++;
	while (*p && *p != '}') {
		const struct omci_attr_def *a;
		const char *k1, *k2, *v1, *v2;
		unsigned int idx, len = 0;
		uint8_t *v;

		k1 = strchr(p, '"');
		if (!k1 || k1 > strchr(p, '}'))
			break;
		k2 = strchr(k1 + 1, '"');
		if (!k2)
			break;
		idx = strtoul(k1 + 1, NULL, 10);
		v1 = strchr(k2 + 1, '"');
		if (!v1)
			break;
		v2 = strchr(v1 + 1, '"');
		if (!v2)
			break;
		p = v2 + 1;

		a = idx >= 1 && idx <= OMCI_MAX_ATTR ?
		    omci_attr(in->def, idx) : NULL;
		if (!a)
			continue;
		v = from_hex(v1 + 1, v2 - (v1 + 1), &len);
		if (!v)
			continue;
		if (a->format != OMCI_FMT_TABLE && len != a->size) {
			free(v);
			continue;
		}
		if (!mib_set_raw(in, idx, v, len) && !in->by_olt)
			in->olt_mask |= omci_mask(idx);
		free(v);
	}
}

static bool load_json_inst(const char *line)
{
	unsigned int cls = 0, id = 0, olt = 0;
	const struct omci_me_def *d;
	struct omci_inst *in;
	bool created = false;

	if (!json_uint(line, "\"class\"", &cls) ||
	    !json_uint(line, "\"instance\"", &id) ||
	    !json_uint(line, "\"olt\"", &olt))
		return false;
	if (cls > 0xffff || id > 0xffff)
		return false;
	d = omci_me_find(cls);
	if (!d || !(d->flags & OMCI_ME_SUPPORTED))
		return false;
	in = mib_find(cls, id);
	if (!in) {
		in = mib_create(cls, id, olt != 0);
		created = true;
	}
	if (!in)
		return false;

	load_json_attrs(in, line);
	if (cls == 45 && !mib_find(46, id))
		mib_create(46, id, false);
	else if (cls == 47 && !mib_find(50, id))
		mib_create(50, id, false);
	if (created) {
		in->restored = true;
		omci_pm_inst_create(in);
	}
	if (in->def->arc)
		omci_alarm_arc_set(in);

	return true;
}

void mibstore_load(void)
{
	unsigned int n = 0, ver = 0, mds = 0;
	bool in_frames = false;
	struct omci_inst *in;
	char *line = NULL;
	size_t cap = 0;
	ssize_t l;
	FILE *f;

	f = fopen(JSON_PATH, "r");
	if (!f) {
		INFO("OMCI: no %s, the MIB is the ONU default", JSON_PATH);
		return;
	}

	frames_clear();
	while ((l = getline(&line, &cap, f)) > 0) {
		if (json_uint(line, "\"version\"", &ver) && ver != JSON_VERSION) {
			WARN("OMCI: %s unsupported version %u", JSON_PATH, ver);
			break;
		}
		if (json_uint(line, "\"mds\"", &mds)) {
			in = mib_find(2, 0);
			if (in)
				mib_set_uint(in, 1, mds);
		}
		if (strstr(line, "\"frames\""))
			in_frames = !strchr(line, ']');
		else if (in_frames) {
			const char *q1 = strchr(line, '"');
			const char *q2 = q1 ? strchr(q1 + 1, '"') : NULL;
			unsigned int flen = 0;
			uint8_t *raw;

			if (strchr(line, ']'))
				in_frames = false;
			if (q1 && q2 && q2 > q1 + 1) {
				raw = from_hex(q1 + 1, q2 - (q1 + 1), &flen);
				if (raw) {
					mibstore_record_frame(raw, flen);
					free(raw);
				}
			}
			continue;
		}
		if (strstr(line, "\"class\""))
			n += load_json_inst(line);
	}

	INFO("OMCI: MIB read from %s (%u instances, %u frames)", JSON_PATH, n,
	     n_frames);
	free(line);
	fclose(f);
}

int mibstore_replay(void)
{
	unsigned int i;

	if (!n_frames)
		return -ENOENT;
	replaying = true;
	for (i = 0; i < n_frames; i++) {
		uint8_t *buf = malloc(frames[i].len);

		if (!buf)
			continue;
		memcpy(buf, frames[i].data, frames[i].len);
		buf[2] &= ~0x40;		/* clear AR: no TX reply */
		omci_rx(buf, frames[i].len);
		free(buf);
	}
	replaying = false;
	INFO("OMCI: replayed %u recorded frames", n_frames);

	return 0;
}
