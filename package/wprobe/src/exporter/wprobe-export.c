/*
**     exporter.c - example exporter
**
**     Copyright Fraunhofer FOKUS
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <ipfix_def.h>
#include <ipfix_def_fokus.h>
#include <ipfix_fields_fokus.h>

#include <ipfix.h>
#include <mlog.h>
#include <wprobe.h>
#include <stdbool.h>

static ipfix_datarecord_t g_data  = { NULL, NULL, 0 };
static int do_close = 0;

struct wprobe_mapping {
	int id;
	bool counter;
	const char *wprobe_id;
	struct wprobe_value *val;
};

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_array) (sizeof(_array) / sizeof((_array)[0]))
#endif

#define WMAP(_id, _name, ...) \
	{ \
		.counter = false, \
		.id = IPFIX_FT_WPROBE_##_id##_N, \
		.wprobe_id = _name \
		, ## __VA_ARGS__ \
	}

#define WMAP_COUNTER(_id, _name, ...) \
	{ \
		.counter = true, \
		.id = IPFIX_FT_WPROBE_##_id, \
		.wprobe_id = _name \
		, ## __VA_ARGS__ \
	}


#define WPROBE_OFFSET	2

static struct wprobe_mapping map_globals[] = {
	WMAP(NOISE, "noise"),
	WMAP(PHY_BUSY, "phy_busy"),
	WMAP(PHY_RX, "phy_rx"),
	WMAP(PHY_TX, "phy_tx"),
	WMAP_COUNTER(FRAMES, "frames"),
	WMAP_COUNTER(PROBEREQ, "probereq"),
};

static struct wprobe_mapping map_perlink[] = {
	WMAP(IEEE_TX_RATE, "tx_rate"),
	WMAP(IEEE_RX_RATE, "rx_rate"),
	WMAP(RSSI, "rssi"),
	WMAP(SIGNAL, "signal"),
	WMAP(RETRANSMIT_200, "retransmit_200"),
	WMAP(RETRANSMIT_400, "retransmit_400"),
	WMAP(RETRANSMIT_800, "retransmit_800"),
	WMAP(RETRANSMIT_1600, "retransmit_1600"),
};

static unsigned char link_local[6];
static char link_default[6];
static int nfields = 0;

#define FOKUS_USERID	12325

static void
match_template(struct wprobe_mapping *map, int n, struct list_head *list)
{
	struct wprobe_attribute *attr;
	int i, j, last = -1;

	list_for_each_entry(attr, list, list) {
		for (i = 0; i < n; i++) {
			j = (last + 1 + i) % n;
			if (!strcmp(attr->name, map[j].wprobe_id))
				goto found;
		}
		continue;
found:
		last = j;
		map[j].val = &attr->val;
		memset(&attr->val, 0, sizeof(attr->val));
		nfields++;
	}
}

/* name: export_ipfix_get_template()
 */
static ipfix_template_t *
prepare_template(ipfix_t *handle)
{
    ipfix_template_t *t = NULL;
	int size = 3 * nfields + WPROBE_OFFSET;
    int i;

    if (ipfix_new_data_template( handle, &t, size) < 0) {
        mlogf( 0, "ipfix_new_template() failed: %s\n", strerror(errno) ); 
		exit(1);
    }

	ipfix_add_field(handle, t, 0, IPFIX_FT_SOURCEMACADDRESS, 6);
	ipfix_add_field(handle, t, 0, IPFIX_FT_DESTINATIONMACADDRESS, 6);

	g_data.lens = calloc(size, sizeof(g_data.lens[0]));
	g_data.lens[0] = 6;
	g_data.lens[1] = 6;
	for (i = WPROBE_OFFSET; i < size; i++)
		g_data.lens[i] = 4;

	g_data.addrs = calloc(size, sizeof(g_data.addrs[0]));
	g_data.addrs[0] = link_local;
	g_data.maxfields = WPROBE_OFFSET;
	return t;
}

static void
add_template_fields(ipfix_t *handle, ipfix_template_t *t, struct wprobe_mapping *map, int n)
{
	int f = g_data.maxfields;
	int i;

    for (i = 0; i < n; i++) {
		if (!map[i].val)
			continue;

		if (map[i].counter)
			g_data.addrs[f++] = &map[i].val->U32;
		else
			g_data.addrs[f++] = &map[i].val->n;

        if (ipfix_add_field( handle, t, FOKUS_USERID, map[i].id + 0, 4) < 0)
            exit(1);

		if (map[i].counter)
			continue;

		g_data.lens[f] = 8;
		g_data.addrs[f++] = &map[i].val->s;

		g_data.lens[f] = 8;
		g_data.addrs[f++] = &map[i].val->ss;
        if (ipfix_add_field( handle, t, FOKUS_USERID, map[i].id + 1, 8) < 0)
            exit(1);
        if (ipfix_add_field( handle, t, FOKUS_USERID, map[i].id + 2, 8) < 0)
            exit(1);
    }
	g_data.maxfields = f;
}

static void
wprobe_dump_data(ipfix_t *ipfixh, ipfix_template_t *ipfixt, struct wprobe_iface *dev)
{
	struct wprobe_link *link;

	wprobe_update_links(dev);
	wprobe_request_data(dev, NULL);
	if (list_empty(&dev->links)) {
		g_data.addrs[1] = link_default;
		ipfix_export_array(ipfixh, ipfixt, g_data.maxfields, g_data.addrs, g_data.lens);
		ipfix_export_flush(ipfixh);
	}
	list_for_each_entry(link, &dev->links, list) {
		g_data.addrs[1] = link->addr;
		wprobe_request_data(dev, link->addr);
		ipfix_export_array(ipfixh, ipfixt, g_data.maxfields, g_data.addrs, g_data.lens);
		ipfix_export_flush(ipfixh);
	}
}

int main ( int argc, char **argv )
{
	struct wprobe_iface *dev = NULL;
    ipfix_template_t  *ipfixt = NULL;
    ipfix_t *ipfixh = NULL;
    int protocol = IPFIX_PROTO_TCP;
    char *chost = NULL;
	char *ifname = NULL;
    int sourceid = 12345;
    int port = IPFIX_PORTNO;
    int verbose_level = 0;
    int opt, i = 10;
	char *err = NULL;

	while ((opt = getopt(argc, argv, "hi:c:p:vstu")) != EOF) {
		switch (opt) {
		case 'p':
			if ((port=atoi(optarg)) <0) {
				fprintf( stderr, "Invalid -p argument!\n" );
				exit(1);
			}
			break;
		case 'i':
			ifname = optarg;
			break;
		case 'c':
			chost = optarg;
			break;

		case 's':
			protocol = IPFIX_PROTO_SCTP;
			break;

		case 't':
			protocol = IPFIX_PROTO_TCP;
			break;

		case 'u':
			protocol = IPFIX_PROTO_UDP;
			break;

		case 'v':
			verbose_level ++;
			break;

		case 'h':
		default:
			fprintf(stderr, "usage: %s [-hstuv] -i <interface> -c <collector> [-p portno]\n"
					 "  -h               this help\n"
					 "  -i <interface>   wprobe interface\n"
					 "  -c <collector>   collector address\n"
					 "  -p <portno>      collector port number (default=%d)\n"
					 "  -s               send data via SCTP\n"
					 "  -t               send data via TCP (default)\n"
					 "  -u               send data via UDP\n"
					 "  -v               increase verbose level\n\n",
					 argv[0], IPFIX_PORTNO  );
			exit(1);
		}
	}

	if (!ifname) {
		fprintf(stderr, "No interface specified\n");
		return -1;
	}

	if (!chost) {
		fprintf(stderr, "No collector specified\n");
		return -1;
	}

	dev = wprobe_get_auto(ifname, &err);
	if (!dev || (list_empty(&dev->global_attr) && list_empty(&dev->link_attr))) {
		fprintf(stderr, "Cannot connect to wprobe on interface '%s': %s\n", ifname, (err ? err : "Unknown error"));
		return -1;
	}

	match_template(map_globals, ARRAY_SIZE(map_globals), &dev->global_attr);
	match_template(map_perlink, ARRAY_SIZE(map_perlink), &dev->link_attr);
	if (nfields == 0) {
		fprintf(stderr, "No usable attributes found\n");
		return -1;
	}

    mlog_set_vlevel( verbose_level );
    if (ipfix_init() < 0) {
        fprintf( stderr, "cannot init ipfix module: %s\n", strerror(errno) );
        exit(1);
    }

    ipfix_add_vendor_information_elements(ipfix_ft_fokus);
    if (ipfix_open(&ipfixh, sourceid, IPFIX_VERSION) < 0) {
        fprintf( stderr, "ipfix_open() failed: %s\n", strerror(errno) );
        exit(1);
    }

    if (ipfix_add_collector( ipfixh, chost, port, protocol ) < 0) {
        fprintf( stderr, "ipfix_add_collector(%s,%d) failed: %s\n", 
                 chost, port, strerror(errno));
        exit(1);
    }

	fprintf(stderr, "Local link address: %02x:%02x:%02x:%02x:%02x:%02x\n",
		link_local[0], link_local[1], link_local[2],
		link_local[3], link_local[4], link_local[5]);

	ipfixt = prepare_template(ipfixh);
	add_template_fields(ipfixh, ipfixt, map_globals, ARRAY_SIZE(map_globals));
	add_template_fields(ipfixh, ipfixt, map_perlink, ARRAY_SIZE(map_perlink));

	while (!do_close) {
		sleep(1);
		wprobe_dump_data(ipfixh, ipfixt, dev);
    }

    ipfix_delete_template( ipfixh, ipfixt );
    ipfix_close( ipfixh );
    ipfix_cleanup();
    exit(0);
}
