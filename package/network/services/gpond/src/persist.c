// SPDX-License-Identifier: GPL-2.0
/*
 * UCI writes for /etc/config/gpon and the gpond-owned sections of
 * /etc/config/network.
 */
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <uci.h>

#include "gpond.h"

static struct uci_context *open_pkg(const char *pkg, struct uci_package **p)
{
	struct uci_context *ctx = uci_alloc_context();
	char path[256];

	if (!ctx)
		return NULL;

	if (uci_load(ctx, pkg, p)) {
		FILE *f;

		snprintf(path, sizeof(path), "%s/%s", UCI_CONFDIR, pkg);
		f = fopen(path, "a");
		if (f)
			fclose(f);
		if (uci_load(ctx, pkg, p)) {
			uci_free_context(ctx);
			return NULL;
		}
	}

	return ctx;
}

/* the package with the changes staged in /tmp/.uci as well */
struct uci_context *persist_open(const char *pkg, struct uci_package **p)
{
	return open_pkg(pkg, p);
}

/*
 * Did gpond write this section: its "comment" starts with "gpond" (every
 * section it writes says so, with the ME or the device it comes from).  The
 * only mark there is, so what the admin wrote is never taken for it.
 */
bool persist_ours(struct uci_context *ctx, struct uci_section *s)
{
	const char *comment = uci_lookup_option_string(ctx, s, "comment");

	if (!comment)
		return false;
	return !strncmp(comment, "gpond", 5) &&
	       (!comment[5] || comment[5] == ' ');
}

/* @opt NULL: create the named section of type @val; @val NULL: delete */
int persist_set(struct uci_context *ctx, const char *pkg, const char *sect,
		const char *opt, const char *val)
{
	struct uci_ptr ptr = {
		.package = pkg,
		.section = sect,
		.option = opt,
		.value = val,
	};

	if (uci_lookup_ptr(ctx, &ptr, NULL, false))
		return -EINVAL;

	if (!val) {
		if (!(ptr.flags & UCI_LOOKUP_COMPLETE))
			return 0;
		return uci_delete(ctx, &ptr) ? -EIO : 0;
	}
	if (!opt && ptr.s)
		return 0;			/* section already there */
	if (opt && ptr.o && ptr.o->type == UCI_TYPE_STRING &&
	    !strcmp(ptr.o->v.string, val))
		return 0;			/* unchanged */

	ptr.value = val;

	return uci_set(ctx, &ptr) ? -EIO : 0;
}

int persist_comment(struct uci_context *ctx, const char *pkg, const char *sect,
		    const char *fmt, ...)
{
	char buf[96];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	return persist_set(ctx, pkg, sect, "comment", buf);
}

int persist_add_list(struct uci_context *ctx, const char *pkg,
		     const char *sect, const char *opt, const char *val)
{
	struct uci_ptr ptr = {
		.package = pkg,
		.section = sect,
		.option = opt,
		.value = val,
	};

	if (uci_lookup_ptr(ctx, &ptr, NULL, false))
		return -EINVAL;
	ptr.value = val;

	return uci_add_list(ctx, &ptr) ? -EIO : 0;
}

int persist_del_list(struct uci_context *ctx, const char *pkg,
		     const char *sect, const char *opt, const char *val)
{
	struct uci_ptr ptr = {
		.package = pkg,
		.section = sect,
		.option = opt,
		.value = val,
	};

	if (uci_lookup_ptr(ctx, &ptr, NULL, false))
		return -EINVAL;
	if (!(ptr.flags & UCI_LOOKUP_COMPLETE))
		return 0;
	ptr.value = val;

	return uci_del_list(ctx, &ptr) ? -EIO : 0;
}

/*
 * The package as text, taken before and after a writer ran: when nothing
 * differs the sections are already what the model says, so nothing has to be
 * committed and no service has to be reloaded.  A commit would also rewrite
 * the file (and drop the admin's comments) for no reason.
 */
char *persist_snapshot(struct uci_context *ctx, struct uci_package *p)
{
	char *buf = NULL;
	size_t len = 0;
	FILE *f;

	f = open_memstream(&buf, &len);
	if (!f)
		return NULL;
	if (uci_export(ctx, f, p, false)) {
		fclose(f);
		free(buf);
		return NULL;
	}
	fclose(f);

	return buf;
}

int persist_close(struct uci_context *ctx, struct uci_package *p, bool commit)
{
	int ret = 0;

	if (commit && uci_commit(ctx, &p, false))
		ret = -EIO;

	if (p)
		uci_unload(ctx, p);
	uci_free_context(ctx);

	return ret;
}
