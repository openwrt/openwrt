/*
 * OLPC touchpad PS/2 mouse driver
 *
 * Copyright (c) 2006 One Laptop Per Child, inc.
 *
 * This driver is partly based on the ALPS driver.
 * Copyright (c) 2003 Peter Osterlund <petero2@telia.com>
 * Copyright (c) 2005 Vojtech Pavlik <vojtech@suse.cz>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#ifndef _OLPC_H
#define _OLPC_H

struct olpc_model_info {
	unsigned char signature[3];
	unsigned char flags;
};

struct olpc_data {
	struct input_dev *dev2;		/* Relative device */
	struct psmouse *psmouse;
	char name[32];			/* Name */
	char phys[32];			/* Phys */
	struct olpc_model_info *i; /* Info */
	int pending_mode;
	int current_mode;
	s64 late;
	struct delayed_work mode_switch;
};

#ifdef CONFIG_MOUSE_PS2_OLPC
int olpc_detect(struct psmouse *psmouse, int set_properties);
int olpc_init(struct psmouse *psmouse);
#else
inline int olpc_detect(struct psmouse *psmouse, int set_properties)
{
	return -ENOSYS;
}
inline int olpc_init(struct psmouse *psmouse)
{
	return -ENOSYS;
}
#endif

#endif
