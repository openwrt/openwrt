/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#ifndef __LINUX_RPMSG_DVF99_DUA_H
#define __LINUX_RPMSG_DVF99_DUA_H

struct device;
struct dvf99_dua;

#include <linux/types.h>

#define DVF99_DUA_NEW_CONN	(-3)

enum dvf99_dua_audio_mode {
	DVF99_DUA_AUDIO_IDLE,
	DVF99_DUA_AUDIO_HANDSET,
	DVF99_DUA_AUDIO_GROUP_HAND,
	DVF99_DUA_AUDIO_HEADSET,
	DVF99_DUA_AUDIO_GROUP_HEAD,
	DVF99_DUA_AUDIO_SPEAKER,
	DVF99_DUA_AUDIO_USB,
	DVF99_DUA_AUDIO_GROUP_USB,
};

struct dvf99_dua *dvf99_dua_get(struct device *css_dev);
void dvf99_dua_put(struct dvf99_dua *dua);
int dvf99_dua_unit_alloc(struct dvf99_dua *dua, const char *type,
			 unsigned int inst);
int dvf99_dua_unit_free(struct dvf99_dua *dua, int uid);
int dvf99_dua_unit_mode(struct dvf99_dua *dua, const char *type,
			unsigned int inst, const char *mode);
int dvf99_dua_unit_connect(struct dvf99_dua *dua, int uid, int conn);
int dvf99_dua_unit_disconnect(struct dvf99_dua *dua, int uid, int conn);
int dvf99_dua_audio_conn(struct dvf99_dua *dua);
int dvf99_dua_unit_set(struct dvf99_dua *dua, int uid, int elem, u32 param,
		       s32 value);
int dvf99_dua_audio_mode(struct dvf99_dua *dua, enum dvf99_dua_audio_mode mode);
int dvf99_dua_audio_volume(struct dvf99_dua *dua, unsigned int level);
int dvf99_dua_audio_mute(struct dvf99_dua *dua, bool mute);
int dvf99_dua_audio_join(struct dvf99_dua *dua, int uid);

#endif
