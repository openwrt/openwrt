/*
 * NVRAM variable manipulation (common)
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#include <typedefs.h>
#include <osl.h>
#include <bcmendian.h>
#include <bcmnvram.h>
#include <sbsdram.h>

extern struct nvram_tuple * BCMINIT(_nvram_realloc)(struct nvram_tuple *t, const char *name, const char *value);
extern void BCMINIT(_nvram_free)(struct nvram_tuple *t);
extern int BCMINIT(_nvram_read)(void *buf);

char * BCMINIT(_nvram_get)(const char *name);
int BCMINIT(_nvram_set)(const char *name, const char *value);
int BCMINIT(_nvram_unset)(const char *name);
int BCMINIT(_nvram_getall)(char *buf, int count);
int BCMINIT(_nvram_commit)(struct nvram_header *header);
int BCMINIT(_nvram_init)(void);
void BCMINIT(_nvram_exit)(void);

static struct nvram_tuple * BCMINITDATA(nvram_hash)[257];
static struct nvram_tuple * nvram_dead;

/* Free all tuples. Should be locked. */
static void  
BCMINITFN(nvram_free)(void)
{
	uint i;
	struct nvram_tuple *t, *next;

	/* Free hash table */
	for (i = 0; i < ARRAYSIZE(BCMINIT(nvram_hash)); i++) {
		for (t = BCMINIT(nvram_hash)[i]; t; t = next) {
			next = t->next;
			BCMINIT(_nvram_free)(t);
		}
		BCMINIT(nvram_hash)[i] = NULL;
	}

	/* Free dead table */
	for (t = nvram_dead; t; t = next) {
		next = t->next;
		BCMINIT(_nvram_free)(t);
	}
	nvram_dead = NULL;

	/* Indicate to per-port code that all tuples have been freed */
	BCMINIT(_nvram_free)(NULL);
}

/* String hash */
static INLINE uint
hash(const char *s)
{
	uint hash = 0;

	while (*s)
		hash = 31 * hash + *s++;

	return hash;
}

/* (Re)initialize the hash table. Should be locked. */
static int 
BCMINITFN(nvram_rehash)(struct nvram_header *header)
{
	char buf[] = "0xXXXXXXXX", *name, *value, *end, *eq;

	/* (Re)initialize hash table */
	BCMINIT(nvram_free)();

	/* Parse and set "name=value\0 ... \0\0" */
	name = (char *) &header[1];
	end = (char *) header + NVRAM_SPACE - 2;
	end[0] = end[1] = '\0';
	for (; *name; name = value + strlen(value) + 1) {
		if (!(eq = strchr(name, '=')))
			break;
		*eq = '\0';
		value = eq + 1;
		BCMINIT(_nvram_set)(name, value);
		*eq = '=';
	}

	/* Set special SDRAM parameters */
	if (!BCMINIT(_nvram_get)("sdram_init")) {
		sprintf(buf, "0x%04X", (uint16)(header->crc_ver_init >> 16));
		BCMINIT(_nvram_set)("sdram_init", buf);
	}
	if (!BCMINIT(_nvram_get)("sdram_config")) {
		sprintf(buf, "0x%04X", (uint16)(header->config_refresh & 0xffff));
		BCMINIT(_nvram_set)("sdram_config", buf);
	}
	if (!BCMINIT(_nvram_get)("sdram_refresh")) {
		sprintf(buf, "0x%04X", (uint16)((header->config_refresh >> 16) & 0xffff));
		BCMINIT(_nvram_set)("sdram_refresh", buf);
	}
	if (!BCMINIT(_nvram_get)("sdram_ncdl")) {
		sprintf(buf, "0x%08X", header->config_ncdl);
		BCMINIT(_nvram_set)("sdram_ncdl", buf);
	}

	return 0;
}

/* Get the value of an NVRAM variable. Should be locked. */
char * 
BCMINITFN(_nvram_get)(const char *name)
{
	uint i;
	struct nvram_tuple *t;
	char *value;

	if (!name)
		return NULL;

	/* Hash the name */
	i = hash(name) % ARRAYSIZE(BCMINIT(nvram_hash));

	/* Find the associated tuple in the hash table */
	for (t = BCMINIT(nvram_hash)[i]; t && strcmp(t->name, name); t = t->next);

	value = t ? t->value : NULL;

	return value;
}

/* Get the value of an NVRAM variable. Should be locked. */
int 
BCMINITFN(_nvram_set)(const char *name, const char *value)
{
	uint i;
	struct nvram_tuple *t, *u, **prev;

	/* Hash the name */
	i = hash(name) % ARRAYSIZE(BCMINIT(nvram_hash));

	/* Find the associated tuple in the hash table */
	for (prev = &BCMINIT(nvram_hash)[i], t = *prev; t && strcmp(t->name, name); prev = &t->next, t = *prev);

	/* (Re)allocate tuple */
	if (!(u = BCMINIT(_nvram_realloc)(t, name, value)))
		return -12; /* -ENOMEM */

	/* Value reallocated */
	if (t && t == u)
		return 0;

	/* Move old tuple to the dead table */
	if (t) {
		*prev = t->next;
		t->next = nvram_dead;
		nvram_dead = t;
	}

	/* Add new tuple to the hash table */
	u->next = BCMINIT(nvram_hash)[i];
	BCMINIT(nvram_hash)[i] = u;

	return 0;
}

/* Unset the value of an NVRAM variable. Should be locked. */
int 
BCMINITFN(_nvram_unset)(const char *name)
{
	uint i;
	struct nvram_tuple *t, **prev;

	if (!name)
		return 0;

	/* Hash the name */
	i = hash(name) % ARRAYSIZE(BCMINIT(nvram_hash));

	/* Find the associated tuple in the hash table */
	for (prev = &BCMINIT(nvram_hash)[i], t = *prev; t && strcmp(t->name, name); prev = &t->next, t = *prev);

	/* Move it to the dead table */
	if (t) {
		*prev = t->next;
		t->next = nvram_dead;
		nvram_dead = t;
	}

	return 0;
}

/* Get all NVRAM variables. Should be locked. */
int 
BCMINITFN(_nvram_getall)(char *buf, int count)
{
	uint i;
	struct nvram_tuple *t;
	int len = 0;

	bzero(buf, count);

	/* Write name=value\0 ... \0\0 */
	for (i = 0; i < ARRAYSIZE(BCMINIT(nvram_hash)); i++) {
		for (t = BCMINIT(nvram_hash)[i]; t; t = t->next) {
			if ((count - len) > (strlen(t->name) + 1 + strlen(t->value) + 1))
				len += sprintf(buf + len, "%s=%s", t->name, t->value) + 1;
			else
				break;
		}
	}

	return 0;
}

/* Regenerate NVRAM. Should be locked. */
int
BCMINITFN(_nvram_commit)(struct nvram_header *header)
{
	char *init, *config, *refresh, *ncdl;
	char *ptr, *end;
	int i;
	struct nvram_tuple *t;
	struct nvram_header tmp;
	uint8 crc;

	/* Regenerate header */
	header->magic = NVRAM_MAGIC;
	header->crc_ver_init = (NVRAM_VERSION << 8);
	if (!(init = BCMINIT(_nvram_get)("sdram_init")) ||
	    !(config = BCMINIT(_nvram_get)("sdram_config")) ||
	    !(refresh = BCMINIT(_nvram_get)("sdram_refresh")) ||
	    !(ncdl = BCMINIT(_nvram_get)("sdram_ncdl"))) {
		header->crc_ver_init |= SDRAM_INIT << 16;
		header->config_refresh = SDRAM_CONFIG;
		header->config_refresh |= SDRAM_REFRESH << 16;
		header->config_ncdl = 0;
	} else {
		header->crc_ver_init |= (simple_strtoul(init, NULL, 0) & 0xffff) << 16;
		header->config_refresh = simple_strtoul(config, NULL, 0) & 0xffff;
		header->config_refresh |= (simple_strtoul(refresh, NULL, 0) & 0xffff) << 16;
		header->config_ncdl = simple_strtoul(ncdl, NULL, 0);
	}

	/* Clear data area */
	ptr = (char *) header + sizeof(struct nvram_header);
	bzero(ptr, NVRAM_SPACE - sizeof(struct nvram_header));

	/* Leave space for a double NUL at the end */
	end = (char *) header + NVRAM_SPACE - 2;

	/* Write out all tuples */
	for (i = 0; i < ARRAYSIZE(BCMINIT(nvram_hash)); i++) {
		for (t = BCMINIT(nvram_hash)[i]; t; t = t->next) {
			if ((ptr + strlen(t->name) + 1 + strlen(t->value) + 1) > end)
				break;
			ptr += sprintf(ptr, "%s=%s", t->name, t->value) + 1;
		}
	}

	/* End with a double NUL */
	ptr += 2;

	/* Set new length */
	header->len = ROUNDUP(ptr - (char *) header, 4);

	/* Little-endian CRC8 over the last 11 bytes of the header */
	tmp.crc_ver_init = htol32(header->crc_ver_init);
	tmp.config_refresh = htol32(header->config_refresh);
	tmp.config_ncdl = htol32(header->config_ncdl);
	crc = hndcrc8((char *) &tmp + 9, sizeof(struct nvram_header) - 9, 0xff);

	/* Continue CRC8 over data bytes */
	crc = hndcrc8((char *) &header[1], header->len - sizeof(struct nvram_header), crc);

	/* Set new CRC8 */
	header->crc_ver_init |= crc;

	/* Reinitialize hash table */
	return BCMINIT(nvram_rehash)(header);
}

/* Initialize hash table. Should be locked. */
int 
BCMINITFN(_nvram_init)(void)
{
	struct nvram_header *header;
	int ret;

	if (!(header = (struct nvram_header *) kmalloc(NVRAM_SPACE, GFP_ATOMIC))) {
		return -12; /* -ENOMEM */
	}

	if ((ret = BCMINIT(_nvram_read)(header)) == 0 &&
	    header->magic == NVRAM_MAGIC)
		BCMINIT(nvram_rehash)(header);

	kfree(header);
	return ret;
}

/* Free hash table. Should be locked. */
void 
BCMINITFN(_nvram_exit)(void)
{
	BCMINIT(nvram_free)();
}

/*
 * Search the name=value vars for a specific one and return its value.
 * Returns NULL if not found.
 */
char*
getvar(char *vars, const char *name)
{
	char *s;
	int len;

	len = strlen(name);

	/* first look in vars[] */
	for (s = vars; s && *s;) {
		/* CSTYLED */
		if ((memcmp(s, name, len) == 0) && (s[len] == '='))
			return (&s[len+1]);

		while (*s++)
			;
	}

	/* then query nvram */
	return (nvram_get(name));
}

/*
 * Search the vars for a specific one and return its value as
 * an integer. Returns 0 if not found.
 */
int
getintvar(char *vars, const char *name)
{
	char *val;

	if ((val = getvar(vars, name)) == NULL)
		return (0);

	return (simple_strtoul(val, NULL, 0));
}


