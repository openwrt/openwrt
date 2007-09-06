/* CF-mips driver
   This is a block driver for the direct (mmaped) interface to the CF-slot,
   found in Routerboard.com's RB532 board
   See SDK provided from routerboard.com.
   
   Module adapted By P.Christeas <p_christeas@yahoo.com>, 2005-6.
   Cleaned up and adapted to platform_device by Felix Fietkau <nbd@openwrt.org>

   This work is redistributed under the terms of the GNU General Public License.
*/

#ifndef __CFMIPS_ATA_H__
#define __CFMIPS_ATA_H__

#include <linux/interrupt.h>

#define CFG_DC_DEV1	(void*)0xb8010010
#define   CFG_DC_DEVBASE	0x0
#define   CFG_DC_DEVMASK	0x4
#define   CFG_DC_DEVC		0x8
#define   CFG_DC_DEVTC		0xC

#define CFDEV_BUF_SIZE	0x1000
#define ATA_CIS_OFFSET	0x200
#define ATA_REG_OFFSET	0x800
#define ATA_DBUF_OFFSET	0xC00

#define ATA_REG_FEAT	0x1
#define ATA_REG_SC	0x2
#define ATA_REG_SN	0x3
#define ATA_REG_CL	0x4
#define ATA_REG_CH	0x5
#define ATA_REG_DH	0x6
#define   ATA_REG_DH_BASE	0xa0
#define   ATA_REG_DH_LBA	0x40
#define   ATA_REG_DH_DRV	0x10
#define ATA_REG_CMD	0x7
#define ATA_REG_ST	0x7
#define   ATA_REG_ST_BUSY	0x80
#define   ATA_REG_ST_RDY	0x40
#define   ATA_REG_ST_DWF	0x20
#define   ATA_REG_ST_DSC	0x10
#define   ATA_REG_ST_DRQ	0x08
#define   ATA_REG_ST_CORR	0x04
#define   ATA_REG_ST_ERR	0x01
#define ATA_REG_ERR	0xd
#define ATA_REG_DC	0xe
#define   ATA_REG_DC_IEN	0x02
#define   ATA_REG_DC_SRST	0x04

#define ATA_CMD_READ_SECTORS	0x20
#define ATA_CMD_WRITE_SECTORS	0x30
#define ATA_CMD_EXEC_DRIVE_DIAG	0x90
#define ATA_CMD_READ_MULTIPLE	0xC4
#define ATA_CMD_WRITE_MULTIPLE	0xC5
#define ATA_CMD_SET_MULTIPLE	0xC6
#define ATA_CMD_IDENTIFY_DRIVE	0xEC
#define ATA_CMD_SET_FEATURES	0xEF

#define ATA_FEATURE_ENABLE_APM	0x05
#define ATA_FEATURE_DISABLE_APM	0x85
#define ATA_APM_DISABLED	0x00
#define ATA_APM_MIN_POWER	0x01
#define ATA_APM_WITH_STANDBY	0x7f
#define ATA_APM_WITHOUT_STANDBY	0x80
#define ATA_APM_MAX_PERFORMANCE	0xfe

#define CF_SECT_SIZE	0x200
/* That is the ratio CF_SECT_SIZE/512 (the kernel sector size) */
#define CF_KERNEL_MUL	1
#define ATA_MAX_SECT_PER_CMD	0x100

#define CF_TRANS_FAILED		0
#define CF_TRANS_OK		1
#define CF_TRANS_IN_PROGRESS	2


enum trans_state {
	TS_IDLE = 0,
	TS_AFTER_RESET,
	TS_READY,
	TS_CMD,
	TS_TRANS
};

// 
// #if DEBUG
// static unsigned long busy_time;
// #endif

/** Struct to hold the cfdev
Actually, all the data here only has one instance. However, for 
reasons of programming conformity, it is passed around as a pointer
*/
struct cf_mips_dev {
	void *base; /* base address for I/O */
	void *baddr; /* remapped address */

	int pin; /* gpio pin */
	int irq; /* gpio irq */
	
	unsigned head;
	unsigned cyl;
	unsigned spt;
	unsigned sectors;
	
	unsigned short block_size;
	unsigned dtype ; // ATA or CF
	struct request_queue *queue;
	struct gendisk  *gd;
	
	/* Transaction state */
	enum trans_state tstate;
	char *tbuf;
	unsigned long tbuf_size;
	sector_t tsect_start;
	unsigned tsector_count;
	unsigned tsectors_left;
	int tread;
	unsigned tcmd;
	int async_mode;
	unsigned long irq_enable_time;
	
	struct request *active_req; /* A request is being carried out. Is that different from tstate? */
	int users;
	struct timer_list to_timer;
	struct tasklet_struct tasklet;

	/** This lock ensures that the requests to this device are all done
	atomically. Transfers can run in parallel, requests are all queued
	one-by-one */
	spinlock_t lock;
};

int cf_do_transfer(struct cf_mips_dev* dev,sector_t sector, unsigned long nsect, 
	char* buffer, int is_write);
int cf_init(struct cf_mips_dev* dev);
void cf_cleanup(struct cf_mips_dev* dev);

void cf_async_trans_done(struct cf_mips_dev* dev, int result);
// void *cf_get_next_buf(unsigned long *buf_size);

#endif
