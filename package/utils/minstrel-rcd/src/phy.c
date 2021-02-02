// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2021 Felix Fietkau <nbd@nbd.name> */
#include <libubox/avl-cmp.h>
#include <glob.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <libgen.h>
#include "rcd.h"

static void phy_update(struct vlist_tree *tree, struct vlist_node *node_new,
		       struct vlist_node *node_old);

VLIST_TREE(phy_list, avl_strcmp, phy_update, true, false);

static const char *
phy_file_path(struct phy *phy, const char *file)
{
	static char path[64];

	snprintf(path, sizeof(path), "/sys/kernel/debug/ieee80211/%s/rc/%s", phy_name(phy), file);

	return path;
}

static int
phy_event_read_buf(struct phy *phy, char *buf)
{
	char *cur, *next;
	int len;

	for (cur = buf; (next = strchr(cur, '\n')); cur = next + 1) {
		*next = 0;

		rcd_client_phy_event(phy, cur);
	}

	len = strlen(cur);
	if (cur > buf)
		memmove(buf, cur, len + 1);

	return len;
}

static void
phy_event_cb(struct uloop_fd *fd, unsigned int events)
{
	struct phy *phy = container_of(fd, struct phy, event_fd);
	char buf[512];
	int len, offset = 0;

	while (1) {
		len = read(fd->fd, buf + offset, sizeof(buf) - 1 - offset);
		if (len < 0) {
			if (errno == EAGAIN)
				return;

			if (errno == EINTR)
				continue;

			vlist_delete(&phy_list, &phy->node);
			return;
		}

		if (!len)
			return;

		buf[offset + len] = 0;
		offset = phy_event_read_buf(phy, buf);
	}
}

static void
phy_init(struct phy *phy)
{
	phy->control_fd = -1;
}

static void
phy_add(struct phy *phy)
{
	int cfd, efd;

	cfd = open(phy_file_path(phy, "api_control"), O_WRONLY);
	if (cfd < 0)
		goto remove;

	efd = open(phy_file_path(phy, "api_event"), O_RDONLY);
	if (efd < 0)
		goto close_cfd;

	phy->control_fd = cfd;
	phy->event_fd.fd = efd;
	phy->event_fd.cb = phy_event_cb;
	uloop_fd_add(&phy->event_fd, ULOOP_READ);

	rcd_client_set_phy_state(NULL, phy, true);
	return;

close_cfd:
	close(cfd);
remove:
	vlist_delete(&phy_list, &phy->node);
}

static void
phy_remove(struct phy *phy)
{
	if (phy->control_fd < 0)
		goto out;

	rcd_client_set_phy_state(NULL, phy, false);
	uloop_fd_delete(&phy->event_fd);
	close(phy->control_fd);
	close(phy->event_fd.fd);

out:
	free(phy);
}

static void
phy_update(struct vlist_tree *tree, struct vlist_node *node_new,
	   struct vlist_node *node_old)
{
	struct phy *phy_new = node_new ? container_of(node_new, struct phy, node) : NULL;
	struct phy *phy_old = node_old ? container_of(node_old, struct phy, node) : NULL;

	if (phy_new && phy_old)
		phy_remove(phy_new);
	else if (phy_new)
		phy_add(phy_new);
	else
		phy_remove(phy_old);
}

static void phy_refresh_timer(struct uloop_timeout *t)
{
	unsigned int i;
	glob_t gl;

	glob("/sys/kernel/debug/ieee80211/phy*", 0, NULL, &gl);
	for (i = 0; i < gl.gl_pathc; i++) {
		struct phy *phy;
		char *name, *name_buf;

		name = basename(gl.gl_pathv[i]);
		phy = calloc_a(sizeof(*phy), &name_buf, strlen(name) + 1);
		phy_init(phy);
		vlist_add(&phy_list, &phy->node, strcpy(name_buf, name));
	}
	globfree(&gl);

	uloop_timeout_set(t, 1000);
}

void rcd_phy_init_client(struct client *cl)
{
	struct phy *phy;

	vlist_for_each_element(&phy_list, phy, node)
		rcd_client_set_phy_state(cl, phy, true);
}

void rcd_phy_dump(struct client *cl, struct phy *phy)
{
	char buf[128];
	FILE *f;

	f = fopen(phy_file_path(phy, "api_info"), "r");
	if (!f)
		return;

	while (fgets(buf, sizeof(buf), f) != NULL)
		client_printf(cl, "*;0;%s", buf);

	fclose(f);
}

void rcd_phy_control(struct client *cl, char *data)
{
	struct phy *phy;
	const char *err;
	char *sep;

	sep = strchr(data, ';');
	if (!sep) {
		err = "Syntax error";
		goto error;
	}

	*sep = 0;
	phy = vlist_find(&phy_list, data, phy, node);
	if (!phy) {
		err = "PHY not found";
		goto error;
	}

	data = sep + 1;
retry:
	if (write(phy->control_fd, data, strlen(data)) < 0) {
		if (errno == EINTR || errno == EAGAIN)
			goto retry;

		err = strerror(errno);
		goto error;
	}

	return;

error:
	client_printf(cl, "*;0;#error;%s\n", err);
}

void rcd_phy_init(void)
{
	static struct uloop_timeout t = {
		.cb = phy_refresh_timer
	};

	uloop_timeout_set(&t, 1);
}
