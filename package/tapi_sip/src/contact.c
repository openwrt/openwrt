#include <malloc.h>
#include <string.h>

#include <uci.h>
#include <ucimap.h>

#include "list.h"
#include "contact.h"

static struct uci_context *ctx;
static struct uci_package *pkg;
static struct list_head contact_list;
static struct list_head account_list;

static int contact_init(struct uci_map *map, void *section,
	struct uci_section *s)
{
    struct contact *p = section;
	p->name = strdup(s->e.name);
	return 0;
}

static int contact_add(struct uci_map *map, void *section)
{
    struct contact *c = section;
	printf("add contact: %s\n", c->name);
	list_add_tail(&c->head, &contact_list);
	return 0;
}

static struct uci_optmap contact_uci_map[] = {
	{
		UCIMAP_OPTION(struct contact, identifier),
		.type = UCIMAP_STRING,
		.name = "identifier",
	},
	{
		UCIMAP_OPTION(struct contact, number),
		.type = UCIMAP_STRING,
		.name = "number",
	},
};

static struct uci_sectionmap contact_sectionmap = {
	UCIMAP_SECTION(struct contact, map),
	.type = "contact",
	.init = contact_init,
	.add = contact_add,
	.options = contact_uci_map,
	.n_options = ARRAY_SIZE(contact_uci_map),
	.options_size = sizeof(struct uci_optmap),
};

static int account_init(struct uci_map *map, void *section,
	struct uci_section *s)
{
    struct account *a = section;
	a->name = strdup(s->e.name);
	return 0;
}

static int account_add(struct uci_map *map, void *section)
{
    struct account *a = section;
	list_add_tail(&a->head, &account_list);
	return 0;
}

static struct uci_optmap account_uci_map[] = {
	{
		UCIMAP_OPTION(struct account, realm),
		.type = UCIMAP_STRING,
		.name = "realm",
	},
	{
		UCIMAP_OPTION(struct account, username),
		.type = UCIMAP_STRING,
		.name = "username",
	},
	{
		UCIMAP_OPTION(struct account, sip_port),
		.type = UCIMAP_INT,
		.name = "sip_port",
	},
	{
		UCIMAP_OPTION(struct account, password),
		.type = UCIMAP_STRING,
		.name = "password",
	},
	{
		UCIMAP_OPTION(struct account, stun_host),
		.type = UCIMAP_STRING,
		.name = "stun_host",
	},
	{
		UCIMAP_OPTION(struct account, stun_port),
		.type = UCIMAP_INT,
		.name = "stun_port",
	},
};

static struct uci_sectionmap account_sectionmap = {
	UCIMAP_SECTION(struct account, map),
	.type = "account",
	.init = account_init,
	.add = account_add,
	.options = account_uci_map,
	.n_options = ARRAY_SIZE(account_uci_map),
	.options_size = sizeof(struct uci_optmap),
};

static struct uci_sectionmap *network_smap[] = {
    &contact_sectionmap,
	&account_sectionmap,
};

static struct uci_map contact_map = {
	.sections = network_smap,
	.n_sections = ARRAY_SIZE(network_smap),
};

int contacts_init(void)
{
	int ret;

	INIT_LIST_HEAD(&contact_list);
	INIT_LIST_HEAD(&account_list);
	ctx = uci_alloc_context();

	ucimap_init(&contact_map);
	ret = uci_load(ctx, "telephony", &pkg);
	if (ret)
		return ret;

	ucimap_parse(&contact_map, pkg);

	return 0;
}

void contacts_free(void)
{
}

struct contact *contact_get(const char *number)
{
	struct contact *contact;
	list_for_each_entry(contact, &contact_list, head)
	{
		if (strcmp(contact->number, number) == 0)
			return contact;
	}

	return NULL;
}

struct account *get_account(void)
{
	if (list_empty(&account_list))
		return NULL;

	return list_first_entry(&account_list, struct account, head);
}
