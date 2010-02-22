/*
 * arch/ubicom32/mach-common/switch-core.h
 *   Private data for the switch module
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _SWITCH_CORE_H_
#define _SWITCH_CORE_H_

struct switch_handler_entry;
struct switch_vlan_entry;

#define SWITCH_PORT_MASK_SIZE	2

struct switch_device {
	struct list_head		node;

	const char			*name;
	void				*drvdata;

	u8_t				ports;

	struct proc_dir_entry		*driver_dir;
	const struct switch_handler	*driver_handlers;

	struct proc_dir_entry		*port_dir;
	struct proc_dir_entry		**port_dirs;
	const struct switch_handler	*port_handlers;

	struct proc_dir_entry		*reg_dir;
	const struct switch_handler	*reg_handlers;

	struct proc_dir_entry		*vlan_dir;
	const struct switch_handler	*vlan_handlers;
	struct list_head		vlan_dirs;

	struct list_head		handlers;

	u32_t				port_mask[SWITCH_PORT_MASK_SIZE];
};

typedef int (*switch_handler_fn)(struct switch_device *, char *buf, int nr);
struct switch_handler {
	const char		*name;

	switch_handler_fn	read;
	switch_handler_fn	write;
};

#define SWITCH_MAX_BUFSZ	4096

static inline void switch_set_drvdata(struct switch_device *switch_dev, void *drvdata)
{
	switch_dev->drvdata = drvdata;
}

static inline void *switch_get_drvdata(struct switch_device *switch_dev)
{
	return switch_dev->drvdata;
}

extern int switch_create_vlan_dir(struct switch_device *switch_dev, int vlan_id, const struct switch_handler *handlers);
extern int switch_remove_vlan_dir(struct switch_device *switch_dev, int vlan_id);
extern void switch_parse_vlan_ports(struct switch_device *switch_dev, char *buf, u32_t *untag, u32_t *ports, u32_t *def);

extern void switch_release(struct switch_device *switch_dev);
extern struct switch_device *switch_alloc(void);
extern int switch_register(struct switch_device *switch_dev);
extern void switch_unregister(struct switch_device *switch_dev);

#endif
