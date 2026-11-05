/* SPDX-License-Identifier: GPL-2.0 */
/*
 * gpond - AN7552/AN7563 GPON userspace daemon.
 *
 * The kernel driver (an7552-gpon) keeps the dynamic PON state and runs
 * PLOAM; gpond owns the persistent configuration (/etc/config/gpon),
 * applies it over generic netlink, runs the OMCI management channel
 * and exposes the "gpon" ubus object.
 */
#ifndef _GPOND_H
#define _GPOND_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <syslog.h>

#include <libubox/blobmsg.h>
#include <libubox/uloop.h>

#include <gpon_genl.h>

#define GPOND_PKG		"gpon"
#define GPON_NETDEV		"pon0"

extern int gpond_log_level;

#define LOG(_lvl, _fmt, ...) do {					\
	if ((_lvl) <= gpond_log_level)					\
		syslog(_lvl, _fmt, ##__VA_ARGS__);			\
} while (0)
#define ERR(_fmt, ...)		LOG(LOG_ERR, _fmt, ##__VA_ARGS__)
#define WARN(_fmt, ...)		LOG(LOG_WARNING, _fmt, ##__VA_ARGS__)
#define INFO(_fmt, ...)		LOG(LOG_INFO, _fmt, ##__VA_ARGS__)
#define DBG(_fmt, ...)		LOG(LOG_DEBUG, _fmt, ##__VA_ARGS__)

/* full-frame dump at debug level, "what (len): hex" */
void gpond_hexdump(const char *what, const void *data, size_t len);

#define GPON_SN_LEN		8
#define GPON_PASSWD_LEN		10
#define GPON_MAX_UNI		8
#define GPON_MAX_TCONT		31	/* kernel: 32 channels, 0 is the OMCC */

/* big-endian row decoders shared by the ME handlers */
static inline uint16_t rd_be16(const uint8_t *p)
{
	return (uint16_t)(p[0] << 8 | p[1]);
}

static inline uint32_t rd_be32(const uint8_t *p)
{
	return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 |
	       (uint32_t)p[2] << 8 | p[3];
}

static inline void wr_be16(uint8_t *p, uint16_t v)
{
	p[0] = v >> 8;
	p[1] = v & 0xff;
}

/* an all-zero IPv4 address: "no address" in the MIB */
static inline bool is_zero4(const uint8_t *p)
{
	return !(p[0] | p[1] | p[2] | p[3]);
}

static inline int hex_digit(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;

	return -1;
}

/* decode n bytes from a 2n-character hex string; -1 on a bad digit */
static inline int hex_decode(const char *s, uint8_t *out, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		int h = hex_digit(s[2 * i]), l = hex_digit(s[2 * i + 1]);

		if (h < 0 || l < 0)
			return -1;
		out[i] = h << 4 | l;
	}

	return 0;
}

/* /etc/config/gpon */
struct gpon_cfg {
	/* config global 'global' */
	bool		enabled;
	/*
	 * write /etc/config/gpon only: the network, omcproxy and easycwmp
	 * sections OMCI asks for are logged, not written, and "factory"
	 * cleans only /etc/config/gpon (global.own_config_only)
	 */
	bool		own_config_only;
	int		mode;			/* GPON_MODE_* */
	char		iface[32];		/* netifd interface on pon0 */
	int		log_level;
	bool		sn_set;
	uint8_t		sn[GPON_SN_LEN];
	bool		passwd_set;
	uint8_t		passwd[GPON_PASSWD_LEN];

	/* config gpon 'gpon' */
	int		sd, sf;			/* 0: kernel default */
	int		to1, to2;		/* ms, 0: kernel default */
	int		cfg_done_s;		/* OMCI quiet time -> CFG_DONE */

	/* config onu 'onu' (OMCI identity) */
	char		vendor_id[5];
	char		version[15];
	char		equipment_id[21];
	char		loid[25];
	char		lopw[13];
	uint8_t		omcc_version;
	uint16_t	product_code;
	int		tconts;
	bool		uni_ports_set;
	int		uni_ports;
	char		sw_ver_slot[2][15];
	int		sw_active;		/* image 0/1 */
	int		sw_committed;
	int		sw_valid[2];
	bool		sw_store;		/* keep downloaded images */
	char		sw_dir[128];
	int		reboot_mode;		/* 0: ignore, 1: reboot system */
	int		setup_cwmp;		/* 0: ignore, 1: write easycwmp */
	/* Linux devices behind the PPTP Ethernet UNIs, in ME order */
	char		uni_dev[GPON_MAX_UNI][16];
	int		n_uni_dev;
	/* HGU: the IP hosts (ME 134) the ONU creates alongside the VEIP (ME 329) */
	char		veip_proto[16];	/* netifd proto of the WAN interfaces */
	bool		veip_ipv6;	/* + a dhcpv6 interface */
	int		ip_hosts;
	/* IPTV: netifd interfaces behind the multicast proxy (omcproxy downlink) */
	char		mcast_downlink[64];
	/* gate the multicast GEM in the MAC while nobody listens (mcast_gate.c) */
	bool		mcast_gate;
};

extern struct gpon_cfg cfg;
/* factory is under way: the daemon stops, nothing reloads or re-stages */
extern bool gpond_stopping;
void gpond_reload(void);

/* config.c */
enum opt_type {
	T_BOOL,
	T_INT,
	T_U8,
	T_U16,
	T_U32,
	T_STR,
	T_MODE,
	T_SN,
	T_PASSWD,
	T_UNI_LIST,
};

struct opt {
	const char *section;		/* named section, type == name */
	const char *name;
	enum opt_type type;
	size_t off;
	size_t size;
	size_t flag_off;		/* "is set" flag for SN and passwd */
};

struct gpon_mode_name {
	const char *name;
	int mode;
};

int config_load(struct gpon_cfg *c);
int config_write(const struct gpon_cfg *c);
void config_dump(const struct gpon_cfg *c, struct blob_buf *b);
const char *config_mode_name(int mode);

/* nl.c - generic netlink client */
struct nl_msg;
struct nlattr;

int nl_init(void);
void nl_done(void);
struct nl_msg *nl_msg(int cmd, bool dump);
/* sends @msg (consumed); @cb gets the parsed attributes of each reply */
typedef void (*nl_reply_cb)(struct nlattr **tb, void *arg);

struct reply_ctx {
	nl_reply_cb cb;
	void *arg;
};

struct gem_encrypt_ctx {
	uint16_t gem;
	uint8_t encrypt;
	bool found;
};

int nl_request(struct nl_msg *msg, nl_reply_cb cb, void *arg);
int nl_cmd_u8(int cmd, int attr, uint8_t v);
int nl_put_u8(struct nl_msg *m, int attr, uint8_t v);
int nl_put_u16(struct nl_msg *m, int attr, uint16_t v);
int nl_put_u32(struct nl_msg *m, int attr, uint32_t v);
int nl_put_flag(struct nl_msg *m, int attr);
int nl_put(struct nl_msg *m, int attr, const void *data, int len);

/* kernel wrappers */
int kx_omci_tx(const void *data, size_t len);
int kx_gem_add(uint16_t gem, uint16_t alloc_id, bool mcast);
int kx_gem_del(uint16_t gem);
int kx_mcast_rx(bool on);
int kx_gem_flush(void);
int kx_gem_encrypt(uint16_t gem, uint8_t *encrypt);
int kx_tcont_add(uint16_t alloc_id);
int kx_tcont_del(uint16_t alloc_id);
int kx_us_gem(uint16_t gem);
int kx_us_rule(uint8_t rule, uint16_t vid, uint8_t pcp_mask, uint16_t gem);
int kx_us_rule_del(uint8_t rule);
int kx_pcp_queue(const uint8_t map[8]);
/* router VID rewrite for the PPE (CMD_VID_MAP_SET/DEL) */
int kx_vid_map(uint8_t rule, uint16_t uni_vid, uint8_t pcp_mask,
	       uint16_t ani_vid, uint8_t pcp);
int kx_vid_map_del(uint8_t rule);
/* P-bit of untagged IP frames per DSCP (CMD_DSCP_MAP); map NULL: only the flag */
int kx_dscp_map(const uint8_t map[64], bool enable);
int kx_qos_rate(uint16_t alloc_id, uint32_t kbps, uint32_t bucket);

int kx_onu_flag(int attr, uint8_t v);
int kx_onu_mib_reset(void);
int kx_cfg_done(void);
int kx_tod(uint32_t sf, uint32_t sec, uint32_t nsec);

/* persist.c - UCI writes according to config_mode */
struct uci_context;
struct uci_package;
struct uci_context *persist_open(const char *pkg, struct uci_package **p);
/* the section carries "comment gpond ...": gpond wrote it */
struct uci_section;
bool persist_ours(struct uci_context *ctx, struct uci_section *s);
int persist_set(struct uci_context *ctx, const char *pkg, const char *sect,
		const char *opt, const char *val);
/* option "comment" "gpond ...": what the section is for, to find it again */
int persist_comment(struct uci_context *ctx, const char *pkg, const char *sect,
		    const char *fmt, ...) __attribute__((format(printf, 4, 5)));
int persist_add_list(struct uci_context *ctx, const char *pkg,
		     const char *sect, const char *opt, const char *val);
int persist_del_list(struct uci_context *ctx, const char *pkg,
		     const char *sect, const char *opt, const char *val);
/* compare the package before and after a writer: no difference, no commit */
char *persist_snapshot(struct uci_context *ctx, struct uci_package *p);
int persist_close(struct uci_context *ctx, struct uci_package *p, bool commit);

/* ubus.c */
int ubus_iface_call(const char *iface, const char *method);
int ubus_network_reload(void);
int ubus_config_change(const char *pkg);
int ubus_system_reboot(void);

/* apply.c */
int apply_config(bool restart_link);
int link_set(bool up);

/* event.c - kernel notifications */
void event_handle(int cmd, struct nlattr **tb);

/* ubus.c (object) */
enum kt { K_U8, K_U16, K_U32, K_U64, K_BIN };

struct kattr_desc {
	int attr;
	const char *name;
	enum kt type;
};

enum { CNT_GEM, CNT_ALLOC, CNT_CLEAR, __CNT_MAX };
enum { INJ_FRAME, INJ_REPLAY, __INJ_MAX };
enum { DS_PRESENT, DS_CARRIER, DS_SPEED, DS_MAC, __DS_MAX };
enum { IS_UP, IS_PENDING, IS_ADDR, IS_ROUTE, IS_DNS, IS_SEARCH, __IS_MAX };

int ubus_init(void);
void ubus_done(void);
void ubus_notify_state(struct nlattr **tb);
void ubus_notify_tod(uint32_t sec, uint32_t nsec);

/* omci.c */
void omci_init(void);
void omci_rx(const uint8_t *msg, size_t len);
void omci_state(uint8_t ostate, int onu_id);
void omci_dump(struct blob_buf *b);
void omci_stats(struct blob_buf *b);
void omci_config_changed(void);
void omci_factory(void);

/* omci_alarm.c */
struct omci_inst;
void omci_alarm_dump(struct blob_buf *b);
void omci_alarm_inst_dump(struct blob_buf *b, struct omci_inst *in);

/*
 * monitor.c - alarm and AVC sources: kernel GPON alarms, EN7571 hwmon
 * (optical levels, laser bias, temperature, supply), UNI links (netifd)
 */
#ifndef GPOND_HWMON_DIR
#define GPOND_HWMON_DIR		"/sys/class/hwmon"
#endif
#define EN7571_HWMON_NAME	"en7571"
/* <dir>/<hwmonN (a d_name)>, and an attribute below it */
#define HWMON_PATH_MAX		(sizeof(GPOND_HWMON_DIR) + 256 + 1)
#define HWMON_ATTR_PATH_MAX	(HWMON_PATH_MAX + 64)

/* a condition: unknown (no source), off, on */
enum { C_UNKNOWN = -1, C_OFF, C_ON };

void monitor_init(void);

/* mcast_gate.c */
void mcast_gate_init(void);
void monitor_gpon_alarms(uint32_t alarms);
void monitor_resync(void);
void monitor_dump(struct blob_buf *b);
int monitor_hwmon_read(const char *attr, long *v);

/* link of a Linux device as netifd reports it (network.device status) */
struct gpon_link {
	bool present;
	bool carrier;
	int speed;		/* Mbit/s, 0: unknown */
	bool full_duplex;
	bool mac_valid;
	uint8_t mac[6];
};
int ubus_device_status(const char *dev, struct gpon_link *l);

struct monitor_uni {
	bool valid;
	struct gpon_link l;
};

struct monitor_state {
	bool gpon_valid;
	uint32_t gpon;			/* GPON_ATTR_ALARMS */

	char hwmon[HWMON_PATH_MAX];	/* hwmon directory, "" if not found */
	bool rx_valid, tx_valid;
	long rx_uw, tx_uw;
	int16_t rx_level, tx_level;	/* 0.002 dB from 1 mW */
	int rx_low, rx_high, tx_low, tx_high, bias;
	int temp_warn, temp_crit, volt_warn, volt_crit;

	struct monitor_uni uni[GPON_MAX_UNI];
};

/* a netifd interface as network.interface.<name> status reports it */
struct gpon_l3_status {
	bool valid;		/* the interface exists and is up */
	bool up, pending;
	uint8_t addr[4], mask[4], gw[4], dns[2][4];
	char domain[26];
};
int ubus_iface_status(const char *iface, struct gpon_l3_status *s);

/* eth.c - Ethernet side of OMCI */
void eth_init(void);
void eth_changed(void);
void eth_dump(struct blob_buf *b);
void eth_reset(void);
int eth_factory(void);
struct omci_inst;
void eth_l3_admin(struct omci_inst *in);
const char *eth_uni_dev(uint16_t pptp_id);
const char *eth_bridge_name(uint16_t id);
const char *eth_l3_dev(uint16_t class_id, uint16_t me);

/* swdl.c - software download */
#define SECTION_LEN		31
#define MAX_WINDOW		256

struct swdl_state {
	bool active;
	uint16_t inst;
	uint32_t size;
	uint32_t done;			/* bytes acknowledged */
	unsigned int window;		/* sections per window */
	uint8_t buf[MAX_WINDOW * SECTION_LEN];
	uint32_t fill;			/* highest byte of the open window */
	uint32_t crc;
	int fd;
	char path[192];
	char ver[15];
};

uint8_t swdl_start(uint16_t inst, const uint8_t *c, uint8_t *rc);
uint8_t swdl_section(uint16_t inst, const uint8_t *c, bool ack, uint8_t *rc);
uint8_t swdl_end(uint16_t inst, const uint8_t *c, uint8_t *rc);

#endif /* _GPOND_H */
