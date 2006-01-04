#ifndef __SWITCH_CORE_H
#define __SWITCH_CORE_H

#include <linux/version.h>
#include <linux/list.h>
#define SWITCH_MAX_BUFSZ	4096

#define SWITCH_MEDIA_AUTO	1
#define SWITCH_MEDIA_100	2
#define SWITCH_MEDIA_FD		4

#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
#define LINUX_2_4
#endif

typedef int (*switch_handler)(void *driver, char *buf, int nr);

typedef struct {
	char *name;
	switch_handler read, write;
} switch_config;

typedef struct {
	struct list_head list;
	char *name;
	char *interface;
	int cpuport;
	int ports;
	int vlans;
	switch_config *driver_handlers, *port_handlers, *vlan_handlers;
	void *data;
	void *priv;
} switch_driver;

typedef struct {
	u32 port, untag, pvid;
} switch_vlan_config;


extern int switch_register_driver(switch_driver *driver);
extern void switch_unregister_driver(char *name);
extern switch_vlan_config *switch_parse_vlan(switch_driver *driver, char *buf);
extern int switch_parse_media(char *buf);
extern int switch_print_media(char *buf, int media);

#endif
