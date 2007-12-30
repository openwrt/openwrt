/* OLPC machine specific definitions */

#ifndef ASM_OLPC_H_
#define ASM_OLPC_H_

#include <asm/geode.h>

struct olpc_platform_t {
	int flags;
	int boardrev;
	int ecver;
};

#define OLPC_F_PRESENT 0x01
#define OLPC_F_DCON    0x02
#define OLPC_F_VSA     0x04

enum
{
	OLPC_REV_A = 0,

	OLPC_REV_PRE_B1,
	OLPC_REV_B1,
	OLPC_REV_PRE_B2,
	OLPC_REV_B2,
	OLPC_REV_PRE_B3,
	OLPC_REV_B3,
	OLPC_REV_B4,
	OLPC_REV_C1,
	OLPC_REV_R1,
	OLPC_REV_UNKNOWN	/* this should always come last */
};

#ifndef CONFIG_OLPC

static inline int machine_is_olpc(void) { return 0; }
static inline int olpc_has_dcon(void) { return 0; }
static inline int olpc_has_vsa(void) { return 0; }
static inline int olpc_get_rev(void) { return OLPC_REV_UNKNOWN; }

#else

extern struct olpc_platform_t olpc_platform_info;

static inline int
machine_is_olpc(void)
{
	return (olpc_platform_info.flags & OLPC_F_PRESENT) ? 1 : 0;
}

static inline int
olpc_has_dcon(void)
{
	return (olpc_platform_info.flags & OLPC_F_DCON) ? 1 : 0;
}

static inline int
olpc_has_vsa(void)
{
	return (olpc_platform_info.flags & OLPC_F_VSA) ? 1 : 0;
}

static inline int
olpc_get_rev(void)
{
	return olpc_platform_info.boardrev;
}

static inline int
olpc_rev_after(int rev)
{
	return olpc_platform_info.boardrev > rev &&
			olpc_platform_info.boardrev != OLPC_REV_UNKNOWN;
}

static inline int
olpc_rev_before(int rev)
{
	return olpc_platform_info.boardrev < rev ||
			olpc_platform_info.boardrev == OLPC_REV_UNKNOWN;
}

#endif

/* EC functions */

int olpc_ec_cmd(unsigned char cmd, unsigned char *inbuf, size_t inlen,
		unsigned char *outbuf, size_t outlen);

void olpc_register_battery_callback(void (*f)(unsigned long));
void olpc_deregister_battery_callback(void);

/* EC commands and responses */

/* SCI source values */

#define EC_SCI_SRC_EMPTY   0x00
#define EC_SCI_SRC_GAME    0x01
#define EC_SCI_SRC_BATTERY 0x02
#define EC_SCI_SRC_BATSOC  0x04
#define EC_SCI_SRC_BATERR  0x08
#define EC_SCI_SRC_EBOOK   0x10
#define EC_SCI_SRC_WLAN    0x20
#define EC_SCI_SRC_ACPWR   0x40

/* GPIO assignments */

#define OLPC_GPIO_MIC_AC      1
#define OLPC_GPIO_DCON_IRQ    7
#define OLPC_GPIO_THRM_ALRM  10
#define OLPC_GPIO_WORKAUX    24
#define OLPC_GPIO_LID        26
#define OLPC_GPIO_ECSCI      27

#endif

