/* SPDX-License-Identifier: GPL-2.0 */
/*
 * OMCI (ITU-T G.988) - managed entity definitions and MIB.
 */
#ifndef _GPOND_OMCI_H
#define _GPOND_OMCI_H

#include <stdbool.h>
#include <stdint.h>

#include <libubox/blobmsg.h>
#include <libubox/list.h>
#include <libubox/uloop.h>

/* access (SDK omci_types.h ATTR_ACCESS_*) */
#define OMCI_ACC_R		0x1
#define OMCI_ACC_W		0x2
#define OMCI_ACC_SBC		0x4	/* set-by-create */

/* format (SDK ATTR_FORMAT_*) */
#define OMCI_FMT_POINTER	1
#define OMCI_FMT_BIT_FIELD	2
#define OMCI_FMT_SIGNED		3
#define OMCI_FMT_UNSIGNED	4
#define OMCI_FMT_STRING		5
#define OMCI_FMT_ENUM		6
#define OMCI_FMT_TABLE		7

/* ME access (SDK OMCI_ME_ACCESS_*) */
#define OMCI_ME_BY_ONU		1
#define OMCI_ME_BY_OLT		2
#define OMCI_ME_BY_BOTH		3

#define OMCI_ME_SUPPORTED	0x1
#define OMCI_ME_NO_MIB_UPLOAD	0x2

#define OMCI_MAX_ATTR		16

/* attribute flags */
#define OMCI_ATTR_AVC		0x1	/* SDK avcId column is an OMCI_AVC_ID_* */

/* G.988 alarm bitmap: 28 bytes, alarm number n is bit 7 - n % 8
 * of byte n / 8 (MSB first) */
#define OMCI_ALARM_BYTES	28
#define OMCI_MAX_ALARM		(OMCI_ALARM_BYTES * 8)

/* baseline message: contents bytes, message types sent unsolicited */
#define OMCI_CONTENTS		32
/* the header of an extended message, the ACK of a Test is built from it */
#define OMCI_DEV_BASELINE	0x0a
#define OMCI_DEV_EXTENDED	0x0b
#define OMCI_EXT_HDR		10
#define OMCI_EXT_CONT_MAX	1966
#define OMCI_MT_ALARM		16
#define OMCI_MT_AVC		17

#define CLASS_PPTP_ETH		11
#define CLASS_ONU_G		256
#define CLASS_ANI_G		263
#define PPTP_ID(_n)		(0x100 + (_n) + 1)

enum {
	MT_CREATE = 4,
	MT_DELETE = 6,
	MT_SET = 8,
	MT_GET = 9,
	MT_GET_ALL_ALARMS = 11,
	MT_GET_ALL_ALARMS_NEXT = 12,
	MT_MIB_UPLOAD = 13,
	MT_MIB_UPLOAD_NEXT = 14,
	MT_MIB_RESET = 15,
	MT_TEST = 18,
	MT_START_SW_DOWNLOAD = 19,
	MT_DOWNLOAD_SECTION = 20,
	MT_END_SW_DOWNLOAD = 21,
	MT_ACTIVATE_SW = 22,
	MT_COMMIT_SW = 23,
	MT_SYNC_TIME = 24,
	MT_REBOOT = 25,
	MT_GET_NEXT = 26,
	MT_TEST_RESULT = 27,
	MT_GET_CURRENT_DATA = 28,
	MT_SET_TABLE = 29,		/* extended message set only */
};

enum {
	RES_OK = 0,
	RES_PROC_ERR = 1,
	RES_NOT_SUPPORTED = 2,
	RES_PARAM_ERR = 3,
	RES_UNKNOWN_ME = 4,
	RES_UNKNOWN_INST = 5,
	RES_BUSY = 6,
	RES_INST_EXISTS = 7,
	RES_ATTR_FAILED = 9,
};

struct omci_attr_def {
	uint8_t		idx;		/* 1..16, mask bit 16 - idx */
	uint8_t		size;		/* bytes; table: row size */
	uint8_t		access;
	uint8_t		format;
	uint8_t		flags;		/* OMCI_ATTR_* */
	const char	*name;
};

struct omci_me_def {
	uint16_t	class_id;
	const char	*name;
	uint8_t		access;
	uint8_t		flags;
	uint8_t		nattr;
	const struct omci_attr_def *attr;
	/* alarm numbers the ME reports (SDK alarmIdMapTable) */
	uint8_t		nalarm;
	const uint8_t	*alarm;
	/* attribute indexes the SDK looks up by name, 0: none */
	uint8_t		arc;		/* "ARC" */
	uint8_t		arc_intv;	/* "ARC interval" */
	uint8_t		admin;		/* "admin state" */
};

extern const struct omci_me_def omci_me_defs[];
extern const unsigned int omci_me_defs_num;

struct omci_inst {
	struct list_head	list;
	const struct omci_me_def *def;
	uint16_t		id;
	bool			by_olt;
	bool			restored;	/* read from /usr/lib/gpon/gpon.json */
	uint8_t			*val[OMCI_MAX_ATTR + 1];	/* by idx */
	uint16_t		len[OMCI_MAX_ATTR + 1];		/* table: bytes */
	/* omci_alarm.c */
	uint16_t		olt_mask;	/* attributes of an ONU-created
						 * instance the OLT has written */
	uint8_t			alarm[OMCI_ALARM_BYTES];	/* active alarms */
	bool			arc_on;		/* alarm reports inhibited */
	struct uloop_timeout	arc_tmo;	/* end of the ARC interval */
	void			*pm;		/* omci_pm.c, PM MEs */
};

static inline uint16_t omci_mask(uint8_t idx)
{
	return 0x8000 >> (idx - 1);
}

const struct omci_me_def *omci_me_find(uint16_t class_id);
const struct omci_attr_def *omci_attr(const struct omci_me_def *d,
				      uint8_t idx);

/* omci.c */
#define ALLOC_UNASSIGNED(_a)	((_a) == 0x00ff || (_a) == 0xffff)

/* MIB upload snapshot: one entry per upload-next message */
struct upload_chunk {
	uint16_t class_id;
	uint16_t id;
	uint16_t mask;
	uint8_t data[26];
	uint8_t len;
};

struct omci_upload_state {
	struct upload_chunk *chunks;
	unsigned int n;
};

/* get-next snapshot of one table attribute */
struct omci_table_get_state {
	uint16_t class_id, id, mask;
	uint8_t *data;
	uint32_t len;
};

struct omci_last_resp {
	uint16_t tci;
	uint8_t mt;
	uint8_t msg[OMCI_EXT_HDR + OMCI_EXT_CONT_MAX];
	size_t len;
	bool valid;
};

struct omci_stats_cnt {
	uint64_t rx, tx, errors, retrans, rx_ext, rx_ak;
};

struct omci_upload_ext_state {
	uint8_t *data;
	size_t len;
	size_t *start;			/* of each block */
	size_t *size;			/* contents of each block */
	unsigned int n;
};

struct br_fdb_rec {
	uint8_t mac[6];
	uint8_t port_no;
	uint8_t is_local;
	uint32_t ageing_timer;
	uint8_t port_hi;
	uint8_t pad0;
	uint16_t unused;
};

extern int omci_onu_id;
bool omci_notify_ok(void);
void omci_onu_msg_tx(uint16_t tci, uint8_t mt, uint16_t class_id,
		     uint16_t id, const uint8_t *contents);
void omci_notify_tx(uint8_t mt, uint16_t class_id, uint16_t id,
		    const uint8_t *contents);

/* omci_test.c - Test (MT 18), Test result (MT 27) */
struct test_result {
	struct uloop_timeout	tmo;
	uint16_t		tci, class_id, id;
	uint8_t			c[OMCI_CONTENTS];
};

uint8_t omci_test(uint16_t tci, struct omci_inst *in, const uint8_t *req,
		  const uint8_t *c);

/* omci_alarm.c - alarms (MT 16), AVCs (MT 17), alarm reporting control */
/* get all alarms snapshot: one row per get all alarms next */
struct alarm_row {
	uint16_t class_id;
	uint16_t id;
	uint8_t bitmap[OMCI_ALARM_BYTES];
};

struct alarm_snap {
	struct alarm_row *rows;
	unsigned int n;
};

int omci_alarm_set(uint16_t class_id, uint16_t id, uint8_t num, bool on,
		   bool notify);
void omci_alarm_inst_free(struct omci_inst *in);
void omci_alarm_arc_set(struct omci_inst *in);
void omci_alarm_unlocked(struct omci_inst *in);
void omci_alarm_mib_reset(void);
/* alarm numbers an ME can report: the SDK alarm list, or the TCAs of a PM ME */
unsigned int omci_alarm_list(const struct omci_me_def *d, const uint8_t **l);
uint16_t omci_get_all_alarms(uint8_t mode);
void omci_get_all_alarms_next(uint16_t seq, uint8_t *contents);
int omci_onu_set(struct omci_inst *in, uint8_t idx, const void *v,
		 uint16_t len);
int omci_onu_set_uint(struct omci_inst *in, uint8_t idx, uint32_t v);
int omci_onu_set_str(struct omci_inst *in, uint8_t idx, const char *s);

/* omci_mib.c */
extern struct list_head omci_mib;
struct omci_inst *mib_find(uint16_t class_id, uint16_t id);
struct omci_inst *mib_first(uint16_t class_id);
struct omci_inst *mib_create(uint16_t class_id, uint16_t id, bool by_olt);
void mib_delete(struct omci_inst *in);
void mib_reset(void);
int mib_set_raw(struct omci_inst *in, uint8_t idx, const void *v, uint16_t len);
uint8_t *mib_val(struct omci_inst *in, uint8_t idx);
uint32_t mib_get_uint(struct omci_inst *in, uint8_t idx);
void mib_set_uint(struct omci_inst *in, uint8_t idx, uint32_t v);
void mib_set_str(struct omci_inst *in, uint8_t idx, const char *s);

/*
 * mibstore.c - the MIB the OLT built, kept in /usr/lib/gpon/gpon.json (text, hex): one
 * line per OLT-created instance and per ONU-created instance the OLT wrote
 * to.  Written only by "ubus call xpon submit"; read at every start,
 * bind_replay() then puts it into the kernel whenever the ONU reaches O5.
 */
struct raw_frame {
	uint8_t *data;
	size_t len;
};
void mibstore_changed(void);
void mibstore_record_frame(const uint8_t *msg, size_t len);
int mibstore_submit(void);
int mibstore_erase(void);
bool mibstore_dirty(void);
void mibstore_load(void);
int mibstore_replay(void);

/* ME 268 GEM port network CTP attribute 3 "Direction" (G.988 9.2.3) */
#define GEM_DIR_UNI_TO_ANI	1
#define GEM_DIR_ANI_TO_UNI	2
#define GEM_DIR_BIDIR		3

/* omci_bind.c - MIB -> kernel driver */
bool bind_check(struct omci_inst *in, uint8_t idx, const uint8_t *v,
		bool create);
int bind_create(struct omci_inst *in);
int bind_set(struct omci_inst *in, uint16_t mask, const uint8_t *old[17]);
int bind_delete(struct omci_inst *in);
int bind_iwtp_gem(uint16_t iwtp_id);
void bind_mib_reset(void);
bool bind_replay(void);

#endif /* _GPOND_OMCI_H */
