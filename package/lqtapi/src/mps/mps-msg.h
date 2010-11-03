#ifndef __MPS_MSG_H__
#define __MPS_MSG_H__

#include <linux/slab.h>

struct mps_message {
	uint32_t header;
	uint32_t data[0];
};

#define MPS_MSG_HEADER(_rw, _sc, _bc, _cmd, _chan, _mode, _ecmd, _length) \
	(((_rw) << 31) | ((_sc) << 30) | ((_bc) << 29) | ((_cmd) << 24) | \
	((_chan) << 16) | ((_mode) << 13) | ((_ecmd) << 8) | (_length))

#define MPS_MSG_INIT(_msg, _rw, _sc, _bc, _cmd, _chan, _mode, _ecmd, _length) \
	do { \
		(_msg)->header = MPS_MSG_HEADER(_rw, _sc, _bc, _cmd, _chan, _mode, \
			_ecmd, _length); \
	} while(0)


static inline void mps_msg_init(struct mps_message *msg, uint32_t rw, uint32_t sc,
	uint32_t bc, uint32_t cmd, uint32_t chan, uint32_t mode, uint32_t ecmd,
	uint32_t length)
{
	msg->header = MPS_MSG_HEADER(rw, sc, bc, cmd, chan, mode, ecmd, length);
}

#define DECLARE_MESSAGE(_name, _size) struct mps_message _name; \
	uint32_t __mps_msg_data_ ## __FUNCTION__ ## __LINE__[_size]

static inline struct mps_message *mps_message_alloc(size_t size)
{
	return kmalloc(sizeof(struct mps_message) + size * sizeof(uint32_t), GFP_KERNEL);
}

static inline size_t mps_message_size(const struct mps_message *msg)
{
	return msg->header & 0xff;
}

enum {
	MPS_MSG_WRITE = 0,
	MPS_MSG_READ = 1,
};

enum {
	MPS_CMD_ALI = 1,
	MPS_CMD_DECT = 3,
	MPS_CMD_SDD = 4,
	MPS_CMD_EOP = 6,
};

#define MOD_PCM 0
#define MOD_SDD 0
#define MOD_ALI 1
#define MOD_SIGNALING 2
#define MOD_CODER 3
#define MOD_RESOURCE 6
#define MOD_SYSTEM 7
#define ECMD_SYS_VER 6
#define SYS_CAP_ECMD 7
#define ECMD_CIDS_DATA 9
#define ECMD_DCCTL_DEBUG 0x0a

#define ALI_CHAN_CMD 6
#define ALI_CHAN_ECMD 1

#define MPS_MSG_HEADER_W(_sc, _bc, _cmd, _chan, _mode, _ecmd, _length) \
	MPS_MSG_HEADER(MPS_MSG_WRITE, _sc, _bc, _cmd, _chan, _mode, _ecmd, _length)

#define MPS_MSG_HEADER_R(_sc, _bc, _cmd, _chan, _mode, _ecmd, _length) \
	MPS_MSG_HEADER(MPS_MSG_READ, _sc, _bc, _cmd, _chan, _mode, _ecmd, _length)

#define MPS_MSG_CMD_EOP(_mode, _ecmd, _length) \
	MPS_MSG_HEADER_R(0, 0, MPS_CMD_EOP, 0, _mode, _ecmd, _length)

#define MPS_MSG_CMD_EOP_SYSTEM(_ecmd, _length) \
	MPS_MSG_CMD_EOP(MOD_SYSTEM, _ecmd, _length)

#define MPS_CMD_GET_VERSION \
	MPS_MSG_CMD_EOP_SYSTEM(ECMD_SYS_VER, 4)

#define MPS_CMD_ALI(_chan) \
	MPS_MSG_HEADER_W(0, 0, ALI_CHAN_CMD, _chan, MOD_ALI, ALI_CHAN_ECMD, 12)

#endif
