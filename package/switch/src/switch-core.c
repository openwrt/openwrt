/*
 * switch-core.c
 *
 * Copyright (C) 2005 Felix Fietkau <openwrt@nbd.name>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *
 * Basic doc of driver's /proc interface:
 * /proc/switch/<interface>/
 *   registers:              read-only
 *   counters:               read-only
 *   reset:                  write causes hardware reset
 *   enable_vlan:            "0", "1"
 *   port/<port-number>/
 *     enabled:              "0", "1"
 *     media:                "AUTO", "1000FD", "1000HD", "100FD", "100HD", "10FD", "10HD"
 *   vlan/<port-number>/
 *     ports: same syntax as for nvram's vlan*ports (eg. "1 2 3 4 5*")
 */

#include <linux/module.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/list.h>

#include "switch-core.h"

static int drv_num = 0;
static struct proc_dir_entry *switch_root;
switch_driver drivers;

typedef struct {
	struct list_head list;
	struct proc_dir_entry *parent;
	int nr;
	void *driver;
	switch_config handler;
} switch_proc_handler;

typedef struct {
	struct proc_dir_entry *driver_dir, *port_dir, *vlan_dir;
	struct proc_dir_entry **ports, **vlans;
	switch_proc_handler data;
	int nr;
} switch_priv;

static ssize_t switch_proc_read(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t switch_proc_write(struct file *file, const char *buf, size_t count, void *data);

static struct file_operations switch_proc_fops = {
	.read = (ssize_t (*) (struct file *, char __user *, size_t, loff_t *))switch_proc_read,
	.write = (ssize_t (*) (struct file *, const char __user *, size_t, loff_t *))switch_proc_write
};

static ssize_t switch_proc_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	struct proc_dir_entry *dent = PDE(file->f_dentry->d_inode);
	char *page;
	int len = 0;

	if ((page = kmalloc(SWITCH_MAX_BUFSZ, GFP_KERNEL)) == NULL)
		return -ENOBUFS;

	if (dent->data != NULL) {
		switch_proc_handler *handler = (switch_proc_handler *) dent->data;
		if (handler->handler.read != NULL)
			len += handler->handler.read(handler->driver, page + len, handler->nr);
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


static ssize_t switch_proc_write(struct file *file, const char *buf, size_t count, void *data)
{
	struct proc_dir_entry *dent = PDE(file->f_dentry->d_inode);
	char *page;
	int ret = -EINVAL;

	if ((page = kmalloc(count + 1, GFP_KERNEL)) == NULL)
		return -ENOBUFS;

	if (copy_from_user(page, buf, count)) {
		kfree(page);
		return -EINVAL;
	}
	page[count] = 0;

	if (dent->data != NULL) {
		switch_proc_handler *handler = (switch_proc_handler *) dent->data;
		if (handler->handler.write != NULL) {
			if ((ret = handler->handler.write(handler->driver, page, handler->nr)) >= 0)
				ret = count;
		}
	}

	kfree(page);
	return ret;
}

static int handle_driver_name(void *driver, char *buf, int nr)
{
	const char *name = ((switch_driver *) driver)->name;
	return sprintf(buf, "%s\n", name);
}

static int handle_driver_version(void *driver, char *buf, int nr)
{
	const char *version = ((switch_driver *) driver)->version;
	strcpy(buf, version);
	return sprintf(buf, "%s\n", version);
}

static int handle_driver_cpuport(void *driver, char *buf, int nr)
{
	int cpuport = ((switch_driver *) driver)->cpuport;
	return sprintf(buf, "%i\n", cpuport);
}

static int handle_driver_ports(void *driver, char *buf, int nr)
{
	int ports = ((switch_driver *) driver)->ports;
	return sprintf(buf, "%i\n", ports);
}

static int handle_driver_vlans(void *driver, char *buf, int nr)
{
	int vlans = ((switch_driver *) driver)->vlans;
	return sprintf(buf, "%i\n", vlans);
}

static void add_handler(switch_driver *driver, const switch_config *handler, struct proc_dir_entry *parent, int nr)
{
	switch_priv *priv = (switch_priv *) driver->data;
	struct proc_dir_entry *p;
	int mode;

	switch_proc_handler *tmp;
	tmp = (switch_proc_handler *) kmalloc(sizeof(switch_proc_handler), GFP_KERNEL);
	if (!tmp)
		return;
	INIT_LIST_HEAD(&tmp->list);
	tmp->parent = parent;
	tmp->nr = nr;
	tmp->driver = driver;
	memcpy(&tmp->handler, handler, sizeof(switch_config));
	list_add(&tmp->list, &priv->data.list);

	mode = 0;
	if (handler->read != NULL) mode |= S_IRUSR;
	if (handler->write != NULL) mode |= S_IWUSR;

	if ((p = create_proc_entry(handler->name, mode, parent)) != NULL) {
		p->data = (void *) tmp;
		p->proc_fops = &switch_proc_fops;
	}
}

static inline void add_handlers(switch_driver *driver, const switch_config *handlers, struct proc_dir_entry *parent, int nr)
{
	int i;

	for (i = 0; handlers[i].name != NULL; i++) {
		add_handler(driver, &(handlers[i]), parent, nr);
	}
}

static void remove_handlers(switch_priv *priv)
{
	struct list_head *pos, *q;
	switch_proc_handler *tmp;

	list_for_each_safe(pos, q, &priv->data.list) {
		tmp = list_entry(pos, switch_proc_handler, list);
		list_del(pos);
		remove_proc_entry(tmp->handler.name, tmp->parent);
		kfree(tmp);
	}
}


static void do_unregister(switch_driver *driver)
{
	char buf[4];
	int i;
	switch_priv *priv = (switch_priv *) driver->data;

	remove_handlers(priv);

	for(i = 0; priv->ports[i] != NULL; i++) {
		sprintf(buf, "%d", i);
		remove_proc_entry(buf, priv->port_dir);
	}
	kfree(priv->ports);
	remove_proc_entry("port", priv->driver_dir);

	for(i = 0; priv->vlans[i] != NULL; i++) {
		sprintf(buf, "%d", i);
		remove_proc_entry(buf, priv->vlan_dir);
	}
	kfree(priv->vlans);
	remove_proc_entry("vlan", priv->driver_dir);

	remove_proc_entry(driver->interface, switch_root);

	if (priv->nr == (drv_num - 1))
		drv_num--;

	kfree(priv);
}

switch_config global_driver_handlers[] = {
	{"driver", handle_driver_name, NULL},
	{"version", handle_driver_version, NULL},
	{"cpuport", handle_driver_cpuport, NULL},
	{"ports", handle_driver_ports, NULL},
	{"vlans", handle_driver_vlans, NULL},
	{NULL, NULL, NULL}
};

static int do_register(switch_driver *driver)
{
	switch_priv *priv;
	int i;
	char buf[4];

	priv = kmalloc(sizeof(switch_priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;
	driver->data = (void *) priv;

	priv->ports = kmalloc((driver->ports + 1) * sizeof(struct proc_dir_entry *),
			      GFP_KERNEL);
	if (!priv->ports) {
		kfree(priv);
		return -ENOMEM;
	}
	priv->vlans = kmalloc((driver->vlans + 1) * sizeof(struct proc_dir_entry *),
			      GFP_KERNEL);
	if (!priv->vlans) {
		kfree(priv->ports);
		kfree(priv);
		return -ENOMEM;
	}

	INIT_LIST_HEAD(&priv->data.list);

	priv->nr = drv_num++;
	priv->driver_dir = proc_mkdir(driver->interface, switch_root);
	if (driver->driver_handlers != NULL) {
		add_handlers(driver, driver->driver_handlers, priv->driver_dir, 0);
		add_handlers(driver, global_driver_handlers, priv->driver_dir, 0);
	}

	priv->port_dir = proc_mkdir("port", priv->driver_dir);
	for (i = 0; i < driver->ports; i++) {
		sprintf(buf, "%d", i);
		priv->ports[i] = proc_mkdir(buf, priv->port_dir);
		if (driver->port_handlers != NULL)
			add_handlers(driver, driver->port_handlers, priv->ports[i], i);
	}
	priv->ports[i] = NULL;

	priv->vlan_dir = proc_mkdir("vlan", priv->driver_dir);
	for (i = 0; i < driver->vlans; i++) {
		sprintf(buf, "%d", i);
		priv->vlans[i] = proc_mkdir(buf, priv->vlan_dir);
		if (driver->vlan_handlers != NULL)
			add_handlers(driver, driver->vlan_handlers, priv->vlans[i], i);
	}
	priv->vlans[i] = NULL;


	return 0;
}

static inline int isspace(char c) {
	switch(c) {
		case ' ':
		case 0x09:
		case 0x0a:
		case 0x0d:
			return 1;
		default:
			return 0;
	}
}

#define toupper(c) (islower(c) ? ((c) ^ 0x20) : (c))
#define islower(c) (((unsigned char)((c) - 'a')) < 26)

int switch_parse_media(char *buf)
{
	char *str = buf;
	while (*buf != 0) {
		*buf = toupper(*buf);
		buf++;
	}

	if (strncmp(str, "AUTO", 4) == 0)
		return SWITCH_MEDIA_AUTO;
	else if (strncmp(str, "1000FD", 6) == 0)
		return SWITCH_MEDIA_1000 | SWITCH_MEDIA_FD;
	else if (strncmp(str, "1000HD", 6) == 0)
		return SWITCH_MEDIA_1000;
	else if (strncmp(str, "100FD", 5) == 0)
		return SWITCH_MEDIA_100 | SWITCH_MEDIA_FD;
	else if (strncmp(str, "100HD", 5) == 0)
		return SWITCH_MEDIA_100;
	else if (strncmp(str, "10FD", 4) == 0)
		return SWITCH_MEDIA_FD;
	else if (strncmp(str, "10HD", 4) == 0)
		return 0;
	else return -1;
}

int switch_print_media(char *buf, int media)
{
	int len = 0;

	if (media & SWITCH_MEDIA_AUTO)
		len = sprintf(buf, "Auto");
	else if (media == (SWITCH_MEDIA_1000 | SWITCH_MEDIA_FD))
		len = sprintf(buf, "1000FD");
	else if (media == SWITCH_MEDIA_1000)
		len = sprintf(buf, "1000HD");
	else if (media == (SWITCH_MEDIA_100 | SWITCH_MEDIA_FD))
		len = sprintf(buf, "100FD");
	else if (media == SWITCH_MEDIA_100)
		len = sprintf(buf, "100HD");
	else if (media == SWITCH_MEDIA_FD)
		len = sprintf(buf, "10FD");
	else if (media == 0)
		len = sprintf(buf, "10HD");
	else
		len = sprintf(buf, "Invalid");

	return len;
}

switch_vlan_config *switch_parse_vlan(switch_driver *driver, char *buf)
{
	switch_vlan_config *c;
	int j, u, p, s;

	c = kzalloc(sizeof(switch_vlan_config), GFP_KERNEL);
	if (!c)
		return NULL;

	while (isspace(*buf)) buf++;
	j = 0;
	while (*buf >= '0' && *buf <= '9') {
		j *= 10;
		j += *buf++ - '0';

		u = ((j == driver->cpuport) ? 0 : 1);
		p = 0;
		s = !(*buf >= '0' && *buf <= '9');

		if (s) {
			while (s && !isspace(*buf) && (*buf != 0)) {
				switch(*buf) {
					case 'u':
						u = 1;
						break;
					case 't':
						u = 0;
						break;
					case '*':
						p = 1;
						break;
				}
				buf++;
			}
			c->port |= (1 << j);
			if (u)
				c->untag |= (1 << j);
			if (p)
				c->pvid |= (1 << j);

			j = 0;
		}

		while (isspace(*buf)) buf++;
	}
	if (*buf != 0) {
		kfree(c);
		return NULL;
	}

	c->port &= (1 << driver->ports) - 1;
	c->untag &= (1 << driver->ports) - 1;
	c->pvid &= (1 << driver->ports) - 1;

	return c;
}


int switch_device_registered (char* device) {
	struct list_head *pos;

	list_for_each(pos, &drivers.list) {
		if (strcmp(list_entry(pos, switch_driver, list)->interface, device) == 0) {
			printk("There is already a switch registered on the device '%s'\n", device);
			return -EINVAL;
		}
	}

	return 0;
}


int switch_register_driver(switch_driver *driver)
{
	struct list_head *pos;
	switch_driver *new;
	int ret;

	list_for_each(pos, &drivers.list) {
		if (strcmp(list_entry(pos, switch_driver, list)->name, driver->name) == 0) {
			printk("Switch driver '%s' already exists in the kernel\n", driver->name);
			return -EINVAL;
		}
		if (strcmp(list_entry(pos, switch_driver, list)->interface, driver->interface) == 0) {
			printk("There is already a switch registered on the device '%s'\n", driver->interface);
			return -EINVAL;
		}
	}

	new = kmalloc(sizeof(switch_driver), GFP_KERNEL);
	if (!new)
		return -ENOMEM;
	memcpy(new, driver, sizeof(switch_driver));
	new->name = strdup(driver->name);
	new->interface = strdup(driver->interface);

	if ((ret = do_register(new)) < 0) {
		kfree(new->name);
		kfree(new);
		return ret;
	}
	INIT_LIST_HEAD(&new->list);
	list_add(&new->list, &drivers.list);

	return 0;
}

void switch_unregister_driver(char *name) {
	struct list_head *pos, *q;
	switch_driver *tmp;

	list_for_each_safe(pos, q, &drivers.list) {
		tmp = list_entry(pos, switch_driver, list);
		if (strcmp(tmp->name, name) == 0) {
			do_unregister(tmp);
			list_del(pos);
			kfree(tmp->name);
			kfree(tmp);

			return;
		}
	}
}

static int __init switch_init(void)
{
	if ((switch_root = proc_mkdir("switch", NULL)) == NULL) {
		printk("%s: proc_mkdir failed.\n", __FILE__);
		return -ENODEV;
	}

	INIT_LIST_HEAD(&drivers.list);

	return 0;
}

static void __exit switch_exit(void)
{
	remove_proc_entry("switch", NULL);
}

MODULE_AUTHOR("Felix Fietkau <openwrt@nbd.name>");
MODULE_LICENSE("GPL");

EXPORT_SYMBOL(switch_device_registered);
EXPORT_SYMBOL(switch_register_driver);
EXPORT_SYMBOL(switch_unregister_driver);
EXPORT_SYMBOL(switch_parse_vlan);
EXPORT_SYMBOL(switch_parse_media);
EXPORT_SYMBOL(switch_print_media);

module_init(switch_init);
module_exit(switch_exit);
