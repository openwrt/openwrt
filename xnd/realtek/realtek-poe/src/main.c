/* SPDX-License-Identifier: GPL-2.0 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>

#include <libubox/ustream.h>
#include <libubox/uloop.h>
#include <libubox/list.h>
#include <libubox/ulog.h>
#include <libubus.h>

#include <uci.h>
#include <uci_blob.h>

#define ULOG_DBG(fmt, ...) ulog(LOG_DEBUG, fmt, ## __VA_ARGS__)

typedef int (*poe_reply_handler)(unsigned char *reply);

#define MAX_PORT	8
#define GET_STR(a, b)	(a < ARRAY_SIZE(b) ? b[a] : NULL)

struct port_config {
	char name[16];
	unsigned char enable;
	unsigned char priority;
	unsigned char power_up_mode;
	unsigned char power_budget;
};

struct config {
	int debug;

	int budget;
	int budget_guard;

	int port_count;
	struct port_config ports[MAX_PORT];
};

struct port_state {
	char *status;
	float watt;
	char *poe_mode;
};

struct state {
	char *sys_mode;
	unsigned char sys_version;
	char *sys_mcu;
	char *sys_status;
	unsigned char sys_ext_version;
	float power_consumption;

	struct port_state ports[MAX_PORT];
};

struct cmd {
	struct list_head list;
	unsigned char cmd[12];
};

static struct uloop_timeout state_timeout;
static struct ubus_auto_conn conn;
static struct ustream_fd stream;
static LIST_HEAD(cmd_pending);
static unsigned char cmd_seq;
static struct state state;
static struct blob_buf b;

static struct config config = {
	.budget = 65,
	.budget_guard = 7,
	.port_count = 8,
};

static void
config_load_port(struct uci_section *s)
{
	enum {
		PORT_ATTR_ID,
		PORT_ATTR_NAME,
		PORT_ATTR_ENABLE,
		PORT_ATTR_PRIO,
		PORT_ATTR_POE_PLUS,
		__PORT_ATTR_MAX,
	};

	static const struct blobmsg_policy port_attrs[__PORT_ATTR_MAX] = {
		[PORT_ATTR_ID] = { .name = "id", .type = BLOBMSG_TYPE_INT32 },
		[PORT_ATTR_NAME] = { .name = "name", .type = BLOBMSG_TYPE_STRING },
		[PORT_ATTR_ENABLE] = { .name = "enable", .type = BLOBMSG_TYPE_INT32 },
		[PORT_ATTR_PRIO] = { .name = "priority", .type = BLOBMSG_TYPE_INT32 },
		[PORT_ATTR_POE_PLUS] = { .name = "poe_plus", .type = BLOBMSG_TYPE_INT32 },
	};

	const struct uci_blob_param_list port_attr_list = {
		.n_params = __PORT_ATTR_MAX,
		.params = port_attrs,
	};

	struct blob_attr *tb[__PORT_ATTR_MAX] = { 0 };
	unsigned int id;

	blob_buf_init(&b, 0);
	uci_to_blob(&b, s, &port_attr_list);
	blobmsg_parse(port_attrs, __PORT_ATTR_MAX, tb, blob_data(b.head), blob_len(b.head));

	if (!tb[PORT_ATTR_ID] || !tb[PORT_ATTR_NAME]) {
		ULOG_ERR("invalid port settings");
		return;
	}

	id = blobmsg_get_u32(tb[PORT_ATTR_ID]);
	if (!id || id > MAX_PORT) {
		ULOG_ERR("invalid port id");
		return;
	}
	id--;

	strncpy(config.ports[id].name, blobmsg_get_string(tb[PORT_ATTR_NAME]), 16);

	if (tb[PORT_ATTR_ENABLE])
		config.ports[id].enable = !!blobmsg_get_u32(tb[PORT_ATTR_ENABLE]);

	if (tb[PORT_ATTR_PRIO])
		config.ports[id].priority = blobmsg_get_u32(tb[PORT_ATTR_PRIO]);
	if (config.ports[id].priority > 3)
		config.ports[id].priority = 3;

	if (tb[PORT_ATTR_POE_PLUS] && blobmsg_get_u32(tb[PORT_ATTR_POE_PLUS]))
		config.ports[id].power_up_mode = 3;
}

static void
config_load_global(struct uci_section *s)
{
	enum {
		GLOBAL_ATTR_BUDGET,
		GLOBAL_ATTR_GUARD,
		__GLOBAL_ATTR_MAX,
	};

	static const struct blobmsg_policy global_attrs[__GLOBAL_ATTR_MAX] = {
		[GLOBAL_ATTR_BUDGET] = { .name = "budget", .type = BLOBMSG_TYPE_INT32 },
		[GLOBAL_ATTR_GUARD] = { .name = "guard", .type = BLOBMSG_TYPE_INT32 },
	};

	const struct uci_blob_param_list global_attr_list = {
		.n_params = __GLOBAL_ATTR_MAX,
		.params = global_attrs,
	};

	struct blob_attr *tb[__GLOBAL_ATTR_MAX] = { 0 };

	blob_buf_init(&b, 0);
	uci_to_blob(&b, s, &global_attr_list);
	blobmsg_parse(global_attrs, __GLOBAL_ATTR_MAX, tb, blob_data(b.head), blob_len(b.head));

	config.budget = 65;
	if (tb[GLOBAL_ATTR_BUDGET])
		config.budget = blobmsg_get_u32(tb[GLOBAL_ATTR_BUDGET]);

	if (tb[GLOBAL_ATTR_GUARD])
		config.budget_guard = blobmsg_get_u32(tb[GLOBAL_ATTR_GUARD]);
	else
		config.budget_guard = config.budget / 10;
}

static void
config_load(int init)
{
	struct uci_context *uci = uci_alloc_context();
        struct uci_package *package = NULL;

	memset(config.ports, 0, sizeof(config.ports));

	if (!uci_load(uci, "poe", &package)) {
		struct uci_element *e;

		if (init)
			uci_foreach_element(&package->sections, e) {
				struct uci_section *s = uci_to_section(e);

				if (!strcmp(s->type, "global"))
					config_load_global(s);
			}
		uci_foreach_element(&package->sections, e) {
			struct uci_section *s = uci_to_section(e);

			if (!strcmp(s->type, "port"))
				config_load_port(s);
		}
	}

	uci_unload(uci, package);
	uci_free_context(uci);
}

static void
poe_cmd_dump(char *type, unsigned char *data)
{
	int i;

	if (!config.debug)
		return;

	fprintf(stderr, "%s ->", type);
	for (i = 0; i < 12; i++)
		fprintf(stderr, " 0x%02x", data[i]);
	fprintf(stderr, "\n");
}

static int
poe_cmd_send(struct cmd *cmd)
{
	poe_cmd_dump("TX", cmd->cmd);
	ustream_write(&stream.stream, (char *)cmd->cmd, 12, false);

	return 0;
}

static int
poe_cmd_next(void)
{
	struct cmd *cmd;

	if (list_empty(&cmd_pending))
		return -1;

	cmd = list_first_entry(&cmd_pending, struct cmd, list);

	return poe_cmd_send(cmd);
}

static int
poe_cmd_queue(unsigned char *_cmd, int len)
{
	int i, empty = list_empty(&cmd_pending);
	struct cmd *cmd = malloc(sizeof(*cmd));

	memset(cmd, 0, sizeof(*cmd));
	memset(cmd->cmd, 0xff, 12);
	memcpy(cmd->cmd, _cmd, len);

	cmd_seq++;
	cmd->cmd[1] = cmd_seq;
	cmd->cmd[11] = 0;

	for (i = 0; i < 11; i++)
		cmd->cmd[11] += cmd->cmd[i];

	list_add_tail(&cmd->list, &cmd_pending);

	if (empty)
		return poe_cmd_send(cmd);

	return 0;
}

/* 0x00 - Set port enable
 *	0: Disable
 *	1: Enable
 */
static int
poe_cmd_port_enable(unsigned char port, unsigned char enable)
{
	unsigned char cmd[] = { 0x00, 0x00, port, enable };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

/* 0x06 - Set global port enable
 *	0: Disable PSE Functionality on all Ports
 *	1: Enable PSE Functionality on all Ports
 *	2: Enable Force power Functionality on all ports
 *	3: Enable Force Power with Disconnect Functionality on all Ports
 */
static int
poe_cmd_global_port_enable(unsigned char enable)
{
	unsigned char cmd[] = { 0x06, 0x00, enable };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

/* 0x10 - Set port detection type
 *	1: Legacy Capacitive Detection only
 *	2: IEEE 802.3af 4-Point Detection only (Default)
 *	3: IEEE 802.3af 4-Point followed by Legacy
 *	4: IEEE 802.3af 2-Point detection (Not Supported)
 *	5: IEEE 802.3af 2-Point followed by Legacy
 */
static int
poe_cmd_port_detection_type(unsigned char port, unsigned char type)
{
	unsigned char cmd[] = { 0x10, 0x00, port, type };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

/* 0x11 - Set port classification
 *	0: Disable
 *	1: Enable
 */
static int
poe_cmd_port_classification(unsigned char port, unsigned char classification)
{
	unsigned char cmd[] = { 0x11, 0x00, port, classification };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

/* 0x13 - Set port disconnect type
 *	0: none
 *	1: AC-disconnect
 *	2: DC-disconnect
 *	3: DC with delay
 */
static int
poe_cmd_port_disconnect_type(unsigned char port, unsigned char type)
{
	unsigned char cmd[] = { 0x13, 0x00, port, type };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

/* 0x15 - Set port power limit type
 *	0: None. Power limit is 16.2W if the connected device is “low power”,
 *	   or the set high power limit if the device is “high power”.
 *	1: Class based. The power limit for class 4 devices is determined by the high power limit.
 *	2: User defined
 */
static int
poe_cmd_port_power_limit_type(unsigned char port, unsigned char type)
{
	unsigned char cmd[] = { 0x15, 0x00, port, type };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

/* 0x16 - Set port power budget
 *	values in 0.2W increments
 */
static int
poe_cmd_port_power_budget(unsigned char port, unsigned char budget)
{
	unsigned char cmd[] = { 0x16, 0x00, port, budget };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

/* 0x17 - Set power management mode
 *	0: None (No Power Management mode) (Default in Semi-Auto mode)
 *	1: Static Power Management with Port Priority(Default in Automode)
 *	2: Dynamic Power Management with Port Priority
 *	3: Static Power Management without Port Priority
 *	4: Dynamic Power Management without Port Priority
 */
static int
poe_cmd_power_mgmt_mode(unsigned char mode)
{
	unsigned char cmd[] = { 0x17, 0x00, mode };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

/* 0x18 - Set global power budget */
static int
poe_cmd_global_power_budget(int budget, int guard)
{
	unsigned char cmd[] = { 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	cmd[3] = budget * 10 / 256;
	cmd[4] = budget * 10 % 256;
	cmd[5] = guard * 10 / 256;
	cmd[6] = guard * 10 % 256;

	return poe_cmd_queue(cmd, sizeof(cmd));
}

/* 0x1a - Set port priority
 *	0: Low
 *	1: Normal
 *	2: High
 *	3: Critical
 */
static int
poe_set_port_priority(unsigned char port, unsigned char priority)
{
	unsigned char cmd[] = { 0x1a, 0x00, port, priority };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

/* 0x1c - Set port power-up mode
 *	0: PoE
 *	1: legacy
 *	2: pre-PoE+
 *	3: PoE+
 */
static int
poe_set_port_power_up_mode(unsigned char port, unsigned char mode)
{
	unsigned char cmd[] = { 0x1c, 0x00, port, mode };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

/* 0x20 - Get system info */
static int
poe_cmd_status(void)
{
	unsigned char cmd[] = { 0x20 };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

static int
poe_reply_status(unsigned char *reply)
{
	static char *mode[]={
		"Semi-auto I2C",
		"Semi-auto UART",
		"Auto I2C",
		"Auto UART"
	};
	static char *mcu[]={
		"ST Micro ST32F100 Microcontroller",
		"Nuvoton M05xx LAN Microcontroller",
		"ST Micro STF030C8 Microcontroller",
		"Nuvoton M058SAN Microcontroller",
		"Nuvoton NUC122 Microcontroller"
	};
	static char *status[]={
		"Global Disable pin is de-asserted:No system reset from the previous query cmd:Configuration saved",
		"Global Disable pin is de-asserted:No system reset from the previous query cmd:Configuration Dirty",
		"Global Disable pin is de-asserted:System reseted:Configuration saved",
		"Global Disable pin is de-asserted:System reseted:Configuration Dirty",
		"Global Disable Pin is asserted:No system reset from the previous query cmd:Configuration saved",
		"Global Disable Pin is asserted:No system reset from the previous query cmd:Configuration Dirty",
		"Global Disable Pin is asserted:System reseted:Configuration saved",
		"Global Disable Pin is asserted:System reseted:Configuration Dirty"
	};

	state.sys_mode = GET_STR(reply[2], mode);
	config.port_count = reply[3];
	state.sys_version = reply[7];
	state.sys_mcu = GET_STR(reply[8], mcu);
	state.sys_status = GET_STR(reply[9], status);
	state.sys_ext_version = reply[10];

	return 0;
}

/* 0x23 - Get power statistics */
static int
poe_cmd_power_stats(void)
{
	unsigned char cmd[] = { 0x23 };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

static int
poe_reply_power_stats(unsigned char *reply)
{
	state.power_consumption = reply[2];
	state.power_consumption *= 256;
	state.power_consumption += reply[3];
	state.power_consumption /= 10;

	return 0;
}

/* 0x26 - Get extended port config */
static int
poe_cmd_port_ext_config(unsigned char port)
{
	unsigned char cmd[] = { 0x26, 0x00, port };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

static int
poe_reply_port_ext_config(unsigned char *reply)
{
	static char *mode[] = {
		"PoE",
		"Legacy",
		"pre-PoE+",
		"PoE+"
	};

	state.ports[reply[2]].poe_mode = GET_STR(reply[3], mode);

	return 0;
}

/* 0x2a - Get all port status */
static int
poe_cmd_port_overview(void)
{
	unsigned char cmd[] = { 0x2a, 0x00, 0x00 };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

static int
poe_reply_port_overview(unsigned char *reply)
{
	static char *status[]={
		"Disabled",
		"Searching",
		"Delivering power",
		"Fault",
		"Other fault",
		"Requesting power",
	};
	int i;

	for (i = 0; i < 8; i++)
		state.ports[i].status = GET_STR((reply[3 + i] & 0xf), status);

	return 0;
}

/* 0x30 - Get port power statistics */
static int
poe_cmd_port_power_stats(unsigned char port)
{
	unsigned char cmd[] = { 0x30, 0x00, port };

	return poe_cmd_queue(cmd, sizeof(cmd));
}

static int
poe_reply_port_power_stats(unsigned char *reply)
{
	float watt;

	watt = reply[9];
	watt *= 256;
	watt += reply[10];
	watt /= 10;

	state.ports[reply[2]].watt = watt;

	return 0;
}

static poe_reply_handler reply_handler[] = {
	[0x20] = poe_reply_status,
	[0x23] = poe_reply_power_stats,
	[0x26] = poe_reply_port_ext_config,
	[0x2a] = poe_reply_port_overview,
	[0x30] = poe_reply_port_power_stats,
};

static int
poe_reply_consume(unsigned char *reply)
{
	struct cmd *cmd = NULL;
	unsigned char sum = 0, i;

	poe_cmd_dump("RX", reply);

	if (list_empty(&cmd_pending)) {
		ULOG_ERR("received unsolicited reply\n");
		return -1;
	}

	cmd = list_first_entry(&cmd_pending, struct cmd, list);
	list_del(&cmd->list);

	for (i = 0; i < 11; i++)
		sum += reply[i];

	if (reply[11] != sum) {
		ULOG_DBG("received reply with bad checksum\n");
		return -1;
	}

	if (reply[0] != cmd->cmd[0]) {
		ULOG_DBG("received reply with bad command id\n");
		return -1;
	}

	if (reply[1] != cmd->cmd[1]) {
		ULOG_DBG("received reply with bad sequence number\n");
		return -1;
	}

	free(cmd);

	if (reply_handler[reply[0]])
		return reply_handler[reply[0]](reply);

	return 0;
}

static void
poe_stream_msg_cb(struct ustream *s, int bytes)
{
	int len;
	unsigned char *reply = (unsigned char *)ustream_get_read_buf(s, &len);

	if (len < 12)
		return;

	poe_reply_consume(reply);
	ustream_consume(s, 12);
	poe_cmd_next();
}

static void
poe_stream_notify_cb(struct ustream *s)
{
	if (!s->eof)
		return;

	ULOG_ERR("tty error, shutting down\n");
	exit(-1);
}

static int
poe_stream_open(char *dev, struct ustream_fd *s, speed_t speed)
{
	struct termios tio;
	int tty;

	tty = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (tty < 0) {
		ULOG_ERR("%s: device open failed: %s\n", dev, strerror(errno));
		return -1;
	}

	tcgetattr(tty, &tio);
	tio.c_cflag |= CREAD;
	tio.c_cflag |= CS8;
	tio.c_iflag |= IGNPAR;
	tio.c_lflag &= ~(ICANON);
	tio.c_lflag &= ~(ECHO);
	tio.c_lflag &= ~(ECHOE);
	tio.c_lflag &= ~(ISIG);
	tio.c_iflag &= ~(IXON | IXOFF | IXANY);
	tio.c_cflag &= ~CRTSCTS;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 0;
	cfsetispeed(&tio, speed);
	cfsetospeed(&tio, speed);
	tcsetattr(tty, TCSANOW, &tio);

	s->stream.string_data = false;
	s->stream.notify_read = poe_stream_msg_cb;
	s->stream.notify_state = poe_stream_notify_cb;

	ustream_fd_init(s, tty);
	tcflush(tty, TCIFLUSH);

	return 0;
}

static int
poe_port_setup(void)
{
	int i;

	for (i = 0; i < config.port_count; i++) {
		if (!config.ports[i].enable) {
			poe_cmd_port_enable(i, 0);
			continue;
		}

		poe_set_port_priority(i, config.ports[i].priority);
		poe_set_port_power_up_mode(i, config.ports[i].power_up_mode);
		if (config.ports[i].power_budget) {
			poe_cmd_port_power_budget(i, config.ports[i].power_budget);
			poe_cmd_port_power_limit_type(i, 2);
		} else {
			poe_cmd_port_power_limit_type(i, 1);
		}
		poe_cmd_port_disconnect_type(i, 2);
		poe_cmd_port_classification(i, 1);
		poe_cmd_port_detection_type(i, 3);
		poe_cmd_port_enable(i, 1);
	}

	return 0;
}

static int
poe_initial_setup(void)
{
	poe_cmd_status();
	poe_cmd_power_mgmt_mode(2);
	poe_cmd_global_power_budget(0, 0);
	poe_cmd_global_port_enable(0);
	poe_cmd_global_power_budget(config.budget, config.budget_guard);

	poe_port_setup();

	return 0;
}

static void
state_timeout_cb(struct uloop_timeout *t)
{
	int i;

	poe_cmd_power_stats();
	poe_cmd_port_overview();

	for (i = 0; i < config.port_count; i++) {
		poe_cmd_port_ext_config(i);
		poe_cmd_port_power_stats(i);
	}

	uloop_timeout_set(&state_timeout, 2 * 1000);
}

static int
ubus_poe_info_cb(struct ubus_context *ctx, struct ubus_object *obj,
		 struct ubus_request_data *req, const char *method,
		 struct blob_attr *msg)
{
	char tmp[16];
	void *c;
	int i;

	blob_buf_init(&b, 0);

	snprintf(tmp, sizeof(tmp), "v%u.%u.%u.%u",
		 state.sys_version >> 4, state.sys_version & 0xf,
		 state.sys_ext_version >> 4, state.sys_ext_version & 0xf);
	blobmsg_add_string(&b, "firmware", tmp);
	if (state.sys_mcu)
		blobmsg_add_string(&b, "mcu", state.sys_mcu);
	blobmsg_add_double(&b, "budget", config.budget);
	blobmsg_add_double(&b, "consumption", state.power_consumption);

	c = blobmsg_open_table(&b, "ports");
	for (i = 0; i < config.port_count; i++) {
		void *p;

		if (!config.ports[i].enable)
			continue;

		p = blobmsg_open_table(&b, config.ports[i].name);

		blobmsg_add_u32(&b, "priority", config.ports[i].priority);

		if (state.ports[i].poe_mode)
			blobmsg_add_string(&b, "mode", state.ports[i].poe_mode);
		if (state.ports[i].status)
			blobmsg_add_string(&b, "status", state.ports[i].status);
		else
			blobmsg_add_string(&b, "status", "unknown");
		if (state.ports[i].watt)
			blobmsg_add_double(&b, "consumption", state.ports[i].watt);

		blobmsg_close_table(&b, p);
	}
	blobmsg_close_table(&b, c);

	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}

static int
ubus_poe_reload_cb(struct ubus_context *ctx, struct ubus_object *obj,
		   struct ubus_request_data *req, const char *method,
		   struct blob_attr *msg)
{
	config_load(0);
	poe_port_setup();

	return UBUS_STATUS_OK;
}

static const struct ubus_method ubus_poe_methods[] = {
	UBUS_METHOD_NOARG("info", ubus_poe_info_cb),
	UBUS_METHOD_NOARG("reload", ubus_poe_reload_cb),
};

static struct ubus_object_type ubus_poe_object_type =
	UBUS_OBJECT_TYPE("poe", ubus_poe_methods);

static struct ubus_object ubus_poe_object = {
	.name = "poe",
	.type = &ubus_poe_object_type,
	.methods = ubus_poe_methods,
	.n_methods = ARRAY_SIZE(ubus_poe_methods),
};

static void
ubus_connect_handler(struct ubus_context *ctx)
{
	int ret;

	ret = ubus_add_object(ctx, &ubus_poe_object);
	if (ret)
		fprintf(stderr, "Failed to add object: %s\n", ubus_strerror(ret));
}

int
main(int argc, char ** argv)
{
	int ch;

	ulog_open(ULOG_STDIO | ULOG_SYSLOG, LOG_DAEMON, "realtek-poe");
	ulog_threshold(LOG_INFO);

	while ((ch = getopt(argc, argv, "d")) != -1) {
		switch (ch) {
		case 'd':
			config.debug = 1;
			break;
		}
	}

	config_load(1);

	uloop_init();
	conn.cb = ubus_connect_handler;
	ubus_auto_connect(&conn);

	if (poe_stream_open("/dev/ttyS1", &stream, B19200) < 0)
		return -1;

	poe_initial_setup();
	state_timeout.cb = state_timeout_cb;
	uloop_timeout_set(&state_timeout, 1000);
	uloop_run();
	uloop_done();

	return 0;
}
