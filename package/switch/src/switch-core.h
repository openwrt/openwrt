#ifndef __SWITCH_CORE_H
#define __SWITCH_CORE_H

#include <linux/version.h>
#include <linux/list.h>
#define SWITCH_MAX_BUFSZ	4096
#define SWITCH_NAME_BUFSZ	16

#define SWITCH_MEDIA_AUTO	1
#define SWITCH_MEDIA_100	2
#define SWITCH_MEDIA_FD		4
#define SWITCH_MEDIA_1000	8

typedef int (*switch_handler)(void *driver, char *buf, int nr);

typedef struct {
	const char *name;
	switch_handler read, write;
} switch_config;

typedef struct {
	struct list_head list;
	const char *name;
	const char *version;
	const char *interface;
	int cpuport;
	int ports;
	int vlans;
	const switch_config *driver_handlers, *port_handlers, *vlan_handlers;
	void *data;
	void *priv;
	char dev_name[SWITCH_NAME_BUFSZ];
} switch_driver;

typedef struct {
	u32 port, untag, pvid;
} switch_vlan_config;


extern int switch_device_registered (char* device);
extern int switch_register_driver(switch_driver *driver);
extern void switch_unregister_driver(char *name);
extern switch_vlan_config *switch_parse_vlan(switch_driver *driver, char *buf);
extern int switch_parse_media(char *buf);
extern int switch_print_media(char *buf, int media);

static inline char *strdup(const char *str)
{
	char *new = kmalloc(strlen(str) + 1, GFP_KERNEL);
	strcpy(new, str);
	return new;
}


#endif
