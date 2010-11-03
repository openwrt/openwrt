#ifndef __CONTACT_H__
#define __CONTACT_H__

#include <ucimap.h>

struct account {
	struct ucimap_section_data map;
	const char *name;
	char *realm;
	char *username;
	char *password;
	int sip_port;
	char *stun_host;
	int stun_port;
	struct list_head head;
};

struct contact {
	struct ucimap_section_data map;
	const char *name;
	const char *identifier;
	const char *number;

	struct list_head head;
};

int contacts_init(void);
void contacts_free(void);
struct contact *contact_get(const char *number);

struct account *get_account(void);

#endif
