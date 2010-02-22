/*
 * arch/ubicom32/mach-common/switch-core.c
 *   Ubicom32 architecture switch and /proc/switch/... implementation.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 2005 Felix Fietkau <openwrt@nbd.name>
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
 *
 * Basic doc of driver's /proc interface:
 * /proc/switch/<interface>/
 *   registers:              read-only
 *   counters:               read-only
 *   reset:                  write causes hardware reset
 *   enable:                 "0", "1"
 *   enable_vlan:            "0", "1"
 *   port/<port-number>/
 *     enabled:              "0", "1"
 *     link state:           read-only
 *     media:                "AUTO", "1000FD", "100FD", "100HD", "10FD", "10HD"
 *   vlan/<port-number>/
 *     ports: same syntax as for nvram's vlan*ports (eg. "1 2 3 4 5*")
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>
#include <linux/proc_fs.h>
#include <linux/list.h>
#include <linux/rwsem.h>
#include <linux/device.h>

#include "switch-core.h"

/*
 * Pointer to the root of our filesystem
 */
static struct proc_dir_entry *switch_root;

/*
 * Lock used to manage access to the switch list
 */
DECLARE_RWSEM(switch_list_lock);
EXPORT_SYMBOL_GPL(switch_list_lock);

/*
 * List of switches we are managing
 */
LIST_HEAD(switch_list);
EXPORT_SYMBOL_GPL(switch_list);

/*
 * List of handlers we have
 */
LIST_HEAD(switch_handler_list);
EXPORT_SYMBOL_GPL(switch_handler_list);

/*
 * Keep track of all the handlers we added
 */
struct switch_handler_entry {
	struct list_head		node;
	struct proc_dir_entry		*parent;
	struct switch_device		*dev;
	const struct switch_handler	*handler;
	int				inst;
};

/*
 * Keep track of all VLAN dirs we created
 */
struct switch_vlan_entry {
	struct list_head		node;
	struct proc_dir_entry		*pde;
	int				vlan_id;
	const struct switch_handler	*handlers;
};

/*
 * switch_parse_vlan_ports
 *	Parse the vlan properties written to <driver>/vlan/<vlan_id>/ports
 */
void switch_parse_vlan_ports(struct switch_device *switch_dev,
			     char *buf, u32_t *untag,
			     u32_t *ports, u32_t *def)
{
	u32_t tag = 0;
	*untag = 0;
	*ports = 0;
	*def = 0;


	/*
	 * Skip any leading spaces
	 */
	while (isspace(*buf)) {
		buf++;
	}

	/*
	 * Parse out the string
	 */
	while (*buf) {
		u32_t port = simple_strtoul(buf, &buf, 10);
		u32_t mask = (1 << port);

		/*
		 * Parse out any flags
		 */
		while (*buf && !isspace(*buf)) {
			switch (*buf++) {
			case 't':
				tag |= mask;
				break;
			case '*':
				*def |= mask;
				break;
			}
		}
		*ports |= mask;

		/*
		 * Skip any spaces
		 */
		while (isspace(*buf)) {
			buf++;
		}
	}

	*untag = ~tag & *ports;
}

/*
 * switch_proc_read
 *	Handle reads from the procfs, dispatches the driver specific handler
 */
static ssize_t switch_proc_read(struct file *file, char *buf, size_t count,
				loff_t *ppos)
{
	struct proc_dir_entry *pde = PDE(file->f_dentry->d_inode);
	char *page;
	int len = 0;

	page = kmalloc(SWITCH_MAX_BUFSZ, GFP_KERNEL);
	if (!page) {
		return -ENOBUFS;
	}

	if (pde->data != NULL) {
		struct switch_handler_entry *she =
			(struct switch_handler_entry *)pde->data;
		if (she->handler->read) {
			len += she->handler->read(she->dev, page + len,
						  she->inst);
		}
	}
	len += 1;

	if (*ppos < len) {
		len = min_t(int, len - *ppos, count);
		if (copy_to_user(buf, (page + *ppos), len)) {
			kfree(page);
			return -EFAULT;
		}
		*ppos += len;
	} else {
		len = 0;
	}

	kfree(page);

	return len;
}

/*
 * switch_proc_write
 *	Handle writes from the procfs, dispatches the driver specific handler
 */
static ssize_t switch_proc_write(struct file *file, const char *buf,
				 size_t count, loff_t *data)
{
	struct proc_dir_entry *pde = PDE(file->f_dentry->d_inode);
	char *page;
	int ret = -EINVAL;

	page = kmalloc(count + 1, GFP_KERNEL);
	if (page == NULL)
		return -ENOBUFS;

	if (copy_from_user(page, buf, count)) {
		kfree(page);
		return -EINVAL;
	}
	page[count] = 0;

	if (pde->data != NULL) {
		struct switch_handler_entry *she =
			(struct switch_handler_entry *)pde->data;
		if (she->handler->write) {
			ret = she->handler->write(she->dev, page, she->inst);
			if (ret >= 0) {
				ret = count;
			}
		}
	}

	kfree(page);
	return ret;
}

/*
 * File operations for the proc_fs, we must cast here since proc_fs' definitions
 * differ from file_operations definitions.
 */
static struct file_operations switch_proc_fops = {
	.read = (ssize_t (*) (struct file *, char __user *,
			      size_t, loff_t *))switch_proc_read,
	.write = (ssize_t (*) (struct file *, const char __user *,
			       size_t, loff_t *))switch_proc_write,
};

/*
 * switch_add_handler
 */
static int switch_add_handler(struct switch_device *switch_dev,
			      struct proc_dir_entry *parent,
			      const struct switch_handler *handler,
			      int inst)
{
	struct switch_handler_entry *she;
	struct proc_dir_entry *pde;
	int mode;

	she = (struct switch_handler_entry *)
		kzalloc(sizeof(struct switch_handler_entry), GFP_KERNEL);
	if (!she) {
		return -ENOMEM;
	}

	INIT_LIST_HEAD(&she->node);
	she->parent = parent;
	she->dev = switch_dev;
	she->inst = inst;
	she->handler = handler;
	list_add(&she->node, &switch_dev->handlers);

	mode = 0;
	if (handler->read != NULL) {
		mode |= S_IRUSR;
	}
	if (handler->write != NULL) {
		mode |= S_IWUSR;
	}

	pde = create_proc_entry(handler->name, mode, parent);
	if (!pde) {
		kfree(she);
		printk("Failed to create node '%s' in parent %p\n",
		       handler->name, parent);
		return -ENOMEM;
	}
	pde->data = (void *)she;
	pde->proc_fops = &switch_proc_fops;

	return 0;
}

/*
 * switch_add_handlers
 */
static int switch_add_handlers(struct switch_device *switch_dev,
			       struct proc_dir_entry *parent,
			       const struct switch_handler *handlers,
			       int inst)
{
	while (handlers->name) {
		int ret = switch_add_handler(switch_dev,
					     parent, handlers, inst);
		if (ret) {
			return ret;
		}
		handlers++;
	}

	return 0;
}

/*
 * switch_remove_vlan_dirs
 *	Removes all vlan directories
 *
 * Assumes all vlan directories are empty, should be called after
 * switch_remove_handlers
 */
static void switch_remove_vlan_dirs(struct switch_device *switch_dev)
{
	struct list_head *pos;
	struct list_head *tmp;
	struct switch_vlan_entry *sve;

	list_for_each_safe(pos, tmp, &switch_dev->vlan_dirs) {
		sve = list_entry(pos, struct switch_vlan_entry, node);
		list_del(pos);
		remove_proc_entry(sve->pde->name, switch_dev->vlan_dir);
		kfree(sve);
	}
}

/*
 * switch_remove_handlers
 *	Removes all handlers registered to the given switch_device
 */
static void switch_remove_handlers(struct switch_device *switch_dev)
{
	struct list_head *pos;
	struct list_head *tmp;
	struct switch_handler_entry *she;

	list_for_each_safe(pos, tmp, &switch_dev->handlers) {
		she = list_entry(pos, struct switch_handler_entry, node);
		list_del(pos);
		remove_proc_entry(she->handler->name, she->parent);
		kfree(she);
	}
}

/*
 * switch_unregister_proc_nodes
 *	Unregisters all proc nodes related to switch_dev
 */
void switch_unregister_proc_nodes(struct switch_device *switch_dev)
{
	switch_remove_handlers(switch_dev);

	if (switch_dev->port_dirs) {
		int i;

		for (i = 0; i < switch_dev->ports; i++) {
			if (switch_dev->port_dirs[i]) {
				remove_proc_entry(
					switch_dev->port_dirs[i]->name,
					switch_dev->port_dir);
			}
		}
	}

	if (switch_dev->port_dir) {
		remove_proc_entry("port", switch_dev->driver_dir);
		switch_dev->port_dir = NULL;
	}

	if (switch_dev->reg_dir) {
		remove_proc_entry("reg", switch_dev->reg_dir);
		switch_dev->reg_dir = NULL;
	}

	if (switch_dev->vlan_dir) {
		switch_remove_vlan_dirs(switch_dev);
		remove_proc_entry("vlan", switch_dev->driver_dir);
		switch_dev->vlan_dir = NULL;
	}

	if (switch_dev->driver_dir) {
		remove_proc_entry(switch_dev->name, switch_root);
		switch_dev->driver_dir = NULL;
	}
}

/*
 * switch_remove_vlan_dir
 *	Removes vlan dir in switch/<switch_driver>/vlan/<vlan_id>
 */
int switch_remove_vlan_dir(struct switch_device *switch_dev, int vlan_id)
{
	struct list_head *pos;
	struct switch_vlan_entry *sve = NULL;

	list_for_each(pos, &switch_dev->vlan_dirs) {
		struct switch_vlan_entry *tmp =
			list_entry(pos, struct switch_vlan_entry, node);
		if (tmp->vlan_id == vlan_id) {
			sve = tmp;
			break;
		}
	}

	if (!sve) {
		return -ENOENT;
	}

	/*
	 * Remove it from the list
	 */
	list_del(pos);

	/*
	 * Remove the handlers
	 */
	while (sve->handlers->name) {
		remove_proc_entry(sve->handlers->name, sve->pde);
		sve->handlers++;
	}

	/*
	 * Remove the proc entry for the <vlan_id> dir
	 */
	remove_proc_entry(sve->pde->name, switch_dev->vlan_dir);

	kfree(sve);

	return 0;
}

/*
 * switch_create_vlan_dir
 *	Creates vlan dir in switch/<switch_driver>/vlan/<vlan_id>
 */
int switch_create_vlan_dir(struct switch_device *switch_dev,
			   int vlan_id, const struct switch_handler *handlers)
{
	char s[14];
	struct proc_dir_entry *pde = NULL;
	struct switch_vlan_entry *sve = NULL;
	int ret;
	struct list_head *pos;

	/*
	 * Check to see if it exists already
	 */
	list_for_each(pos, &switch_dev->vlan_dirs) {
		sve = list_entry(pos, struct switch_vlan_entry, node);
		if (sve->vlan_id == vlan_id) {
			return -EEXIST;
		}
	}
	sve = NULL;

	/*
	 * Create the vlan directory if we didn't have it before
	 */
	if (!switch_dev->vlan_dir) {
		switch_dev->vlan_dir = proc_mkdir("vlan",
						  switch_dev->driver_dir);
		if (!switch_dev->vlan_dir) {
			goto fail;
		}
		if (switch_dev->vlan_handlers) {
			ret = switch_add_handlers(switch_dev,
						  switch_dev->vlan_dir,
						  switch_dev->vlan_handlers, 0);
			if (ret) {
				goto fail;
			}
		}
	}

	/*
	 * Create the vlan_id directory
	 */
	snprintf(s, 14, "%d", vlan_id);
	pde = proc_mkdir(s, switch_dev->vlan_dir);
	if (!pde) {
		goto fail;
	}

	/*
	 * Create the handlers for this vlan
	 */
	if (handlers) {
		ret = switch_add_handlers(switch_dev, pde, handlers, vlan_id);
		if (ret) {
			goto fail;
		}
	}

	/*
	 * Keep track of all the switch vlan entries created
	 */
	sve = (struct switch_vlan_entry *)
		kzalloc(sizeof(struct switch_vlan_entry), GFP_KERNEL);
	if (!sve) {
		goto fail;
	}
	INIT_LIST_HEAD(&sve->node);
	sve->handlers = handlers;
	sve->vlan_id = vlan_id;
	sve->pde = pde;
	list_add(&sve->node, &switch_dev->vlan_dirs);

	return 0;

fail:
	if (sve) {
		kfree(sve);
	}

	if (pde) {
		/*
		 * Remove any proc entries we might have created
		 */
		while (handlers->name) {
			remove_proc_entry(handlers->name, pde);
			handlers++;
		}

		remove_proc_entry(s, switch_dev->driver_dir);
	}

	return -ENOMEM;
}

/*
 * switch_register_proc_nodes
 */
int switch_register_proc_nodes(struct switch_device *switch_dev)
{
	int i;
	int n;

	switch_dev->port_dirs = kzalloc(switch_dev->ports *
					sizeof(struct proc_dir_entry *),
					GFP_KERNEL);
	if (!switch_dev->port_dirs) {
		return -ENOMEM;
	}

	/*
	 * Create a new proc entry for this switch
	 */
	switch_dev->driver_dir = proc_mkdir(switch_dev->name, switch_root);
	if (!switch_dev->driver_dir) {
		goto fail;
	}
	if (switch_dev->driver_handlers) {
		switch_add_handlers(switch_dev,
				    switch_dev->driver_dir,
				    switch_dev->driver_handlers,
				    0);
	}

	/*
	 * Create the ports
	 */
	switch_dev->port_dir = proc_mkdir("port", switch_dev->driver_dir);
	if (!switch_dev->port_dir) {
		goto fail;
	}
	for (n = 0, i = 0; i < (SWITCH_PORT_MASK_SIZE * 32); i++) {
		if (switch_dev->port_mask[i / 32] & (1 << i % 32)) {
			char s[14];

			snprintf(s, 14, "%d", i);
			switch_dev->port_dirs[n] =
				proc_mkdir(s, switch_dev->port_dir);
			if (!switch_dev->port_dirs[n]) {
				goto fail;
			}
			if (switch_dev->port_handlers) {
				switch_add_handlers(switch_dev,
						    switch_dev->port_dirs[n],
						    switch_dev->port_handlers,
						    i);
			}
			n++;
		}
	}

	/*
	 * Create the register directory for switch register access.
	 */
	if (switch_dev->reg_handlers) {
		switch_dev->reg_dir = proc_mkdir("reg", switch_dev->driver_dir);
		if (!switch_dev->reg_dir) {
			goto fail;
		}

		switch_add_handlers(switch_dev,
				    switch_dev->reg_dir,
				    switch_dev->reg_handlers,
				    0);
	}

	/*
	 * Create the vlan directory
	 */
	if (switch_dev->vlan_handlers) {
		switch_dev->vlan_dir = proc_mkdir("vlan",
						  switch_dev->driver_dir);
		if (!switch_dev->vlan_dir) {
			goto fail;
		}
		if (switch_dev->vlan_handlers) {
			switch_add_handlers(switch_dev,
					    switch_dev->vlan_dir,
					    switch_dev->vlan_handlers,
					    0);
		}
	}

	return 0;

fail:
	switch_unregister_proc_nodes(switch_dev);
	return -ENOMEM;
}

/*
 * switch_release
 */
void switch_release(struct switch_device *switch_dev)
{
	kfree(switch_dev);
}

/*
 * switch_alloc
 */
struct switch_device *switch_alloc(void)
{
	struct switch_device *switch_dev =
		kzalloc(sizeof(struct switch_device),
						GFP_KERNEL);
	INIT_LIST_HEAD(&switch_dev->node);
	INIT_LIST_HEAD(&switch_dev->vlan_dirs);
	INIT_LIST_HEAD(&switch_dev->handlers);
	return switch_dev;
}

/*
 * switch_register
 */
int switch_register(struct switch_device *switch_dev)
{
	int ret;
	int i;

	/*
	 * Make sure that the number of ports and the port mask make sense
	 */
	for (ret = 0, i = 0; i < (SWITCH_PORT_MASK_SIZE * 32); i++) {
		if (switch_dev->port_mask[i / 32] & (1 << i % 32)) {
			ret++;
		}
	}
	if (ret > switch_dev->ports) {
		return -EINVAL;
	}

	/*
	 * Create the /proc entries
	 */
	ret = switch_register_proc_nodes(switch_dev);
	if (ret) {
		return ret;
	}

	/*
	 * Add it to the list of switches
	 */
	down_write(&switch_list_lock);
	list_add_tail(&switch_dev->node, &switch_list);
	up_write(&switch_list_lock);

	printk(KERN_INFO "Registered switch device: %s\n", switch_dev->name);

	return 0;
}
EXPORT_SYMBOL_GPL(switch_register);

/*
 * switch_unregister
 *	Unregisters a previously registered switch_device object
 */
void switch_unregister(struct switch_device *switch_dev)
{
	/*
	 * remove the proc entries
	 */
	switch_unregister_proc_nodes(switch_dev);

	/*
	 * Remove it from the list of switches
	 */
	down_write(&switch_list_lock);
	list_del(&switch_dev->node);
	up_write(&switch_list_lock);

	printk(KERN_INFO "Unregistered switch device: %s\n", switch_dev->name);
}
EXPORT_SYMBOL_GPL(switch_unregister);

/*
 * switch_init
 */
static int __init switch_init(void)
{
	switch_root = proc_mkdir("switch", NULL);
	if (!switch_root) {
		printk(KERN_WARNING "Failed to make root switch node\n");
		return -ENODEV;
	}
	return 0;
}
module_init(switch_init);

/*
 * switch_exit
 */
static void __exit switch_exit(void)
{
	remove_proc_entry("switch", NULL);
}
module_exit(switch_exit);

MODULE_AUTHOR("Patrick Tjin");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Ethernet Switch Class Interface");
