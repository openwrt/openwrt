/*
 * drivers/mtd/devices/ubi32-m25p80.c
 *   NOR flash driver, Ubicom processor internal SPI flash interface.
 *
 *   This code instantiates the serial flash that contains the
 *   original bootcode.  The serial flash start at address 0x60000000
 *   in both Ubicom32V3 and Ubicom32V4 ISAs.
 *
 *   This piece of flash is made to appear as a Memory Technology
 *   Device (MTD) with this driver to allow Read/Write/Erase operations.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/types.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>

#include <asm/ip5000.h>
#include <asm/devtree.h>

#define UBICOM32_FLASH_BASE	0x60000000
#define UBICOM32_FLASH_MAX_SIZE 0x01000000
#define UBICOM32_FLASH_START	0x00000000
#define UBICOM32_KERNEL_OFFSET	0x00010000 /* The kernel starts after Ubicom
					    * .protect section. */

static struct mtd_partition ubicom32_flash_partitions[] = {
	{
		.name	= "Bootloader",		/* Protected Section
						 * Partition */
		.size	= 0x10000,
		.offset	= UBICOM32_FLASH_START,
//		.mask_flags = MTD_WRITEABLE	/* Mark Read-only */
	},
	{
		.name	= "Kernel",		/* Kernel Partition. */
		.size	= 0,			/* this will be set up during
						 * probe stage. At that time we
						 * will know end of linux image
						 * in flash. */
		.offset	= MTDPART_OFS_APPEND,	/* Starts right after Protected
						 * section. */
//		.mask_flags = MTD_WRITEABLE	/* Mark Read-only */
	},
	{
		.name	= "Rest",		/* Rest of the flash. */
		.size	= 0x200000,		/* Use up what remains in the
						 * flash. */
		.offset	= MTDPART_OFS_NXTBLK,	/* Starts right after Protected
						 * section. */
	}
};

static struct flash_platform_data ubicom32_flash_data = {
	.name = "ubicom32_boot_flash",
	.parts = ubicom32_flash_partitions,
	.nr_parts = ARRAY_SIZE(ubicom32_flash_partitions),
};

static struct resource ubicom32_flash_resource[] = {
	{
		.start	= UBICOM32_FLASH_BASE,
		.end	= UBICOM32_FLASH_BASE +
		UBICOM32_FLASH_MAX_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device ubicom32_flash_device = {
	.name = "ubicom32flashdriver",
	.id = 0, /* Bus number */
	.num_resources = ARRAY_SIZE(ubicom32_flash_resource),
	.resource = ubicom32_flash_resource,
	.dev = {
		.platform_data = &ubicom32_flash_data,
	},
};

static struct platform_device *ubicom32_flash_devices[] = {
	&ubicom32_flash_device,
};

static int __init ubicom32_flash_init(void)
{
	printk(KERN_INFO "%s(): registering device resources\n",
	       __FUNCTION__);
	platform_add_devices(ubicom32_flash_devices,
			     ARRAY_SIZE(ubicom32_flash_devices));
	return 0;
}

arch_initcall(ubicom32_flash_init);

/*
 * MTD SPI driver for ST M25Pxx (and similar) serial flash chips through
 * Ubicom32 SPI controller.
 *
 * Author: Mike Lavender, mike@steroidmicros.com
 *
 * Copyright (c) 2005, Intec Automation Inc.
 *
 * Some parts are based on lart.c by Abraham Van Der Merwe
 *
 * Cleaned up and generalized based on mtd_dataflash.c
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#define FLASH_PAGESIZE		256

/* Flash opcodes. */
#define	OPCODE_WREN		0x06	/* Write enable */
#define	OPCODE_RDSR		0x05	/* Read status register */
#define	OPCODE_READ		0x03	/* Read data bytes (low frequency) */
#define	OPCODE_FAST_READ	0x0b	/* Read data bytes (high frequency) */
#define	OPCODE_PP		0x02	/* Page program (up to 256 bytes) */
#define	OPCODE_BE_4K		0x20	/* Erase 4KiB block */
#define	OPCODE_BE_32K		0x52	/* Erase 32KiB block */
#define	OPCODE_SE		0xd8	/* Sector erase (usually 64KiB) */
#define	OPCODE_RDID		0x9f	/* Read JEDEC ID */

/* Status Register bits. */
#define	SR_WIP			1	/* Write in progress */
#define	SR_WEL			2	/* Write enable latch */
/* meaning of other SR_* bits may differ between vendors */
#define	SR_BP0			4	/* Block protect 0 */
#define	SR_BP1			8	/* Block protect 1 */
#define	SR_BP2			0x10	/* Block protect 2 */
#define	SR_SRWD			0x80	/* SR write protect */

/* Define max times to check status register before we give up. */
#define	MAX_READY_WAIT_COUNT	100000


#ifdef CONFIG_MTD_PARTITIONS
#define	mtd_has_partitions()	(1)
#else
#define	mtd_has_partitions()	(0)
#endif

/*
 * Ubicom32 FLASH Command Set
 */
#define FLASH_FC_INST_CMD	0x00	/* for SPI command only transaction */
#define FLASH_FC_INST_WR	0x01	/* for SPI write transaction */
#define FLASH_FC_INST_RD	0x02	/* for SPI read transaction */

#define ALIGN_DOWN(v, a) ((v) & ~((a) - 1))
#define ALIGN_UP(v, a) (((v) + ((a) - 1)) & ~((a) - 1))

#define	FLASH_COMMAND_KICK_OFF(io)							\
	asm volatile(									\
	"	bset	"D(IO_INT_CLR)"(%0), #0, #%%bit("D(IO_XFL_INT_DONE)")	\n\t"	\
	"	jmpt.t	.+4							\n\t"	\
	"	bset	"D(IO_INT_SET)"(%0), #0, #%%bit("D(IO_XFL_INT_START)")	\n\t"	\
		:									\
		: "a" (io)								\
		: "memory", "cc"							\
	);

#define	FLASH_COMMAND_WAIT_FOR_COMPLETION(io)						\
	asm volatile(									\
	"	btst	"D(IO_INT_STATUS)"(%0), #%%bit("D(IO_XFL_INT_DONE)")	\n\t"	\
	"	jmpeq.f	.-4							\n\t"	\
		:									\
		: "a" (io)								\
		: "memory", "cc"									\
	);

#define	FLASH_COMMAND_EXEC(io)								\
	FLASH_COMMAND_KICK_OFF(io)							\
	FLASH_COMMAND_WAIT_FOR_COMPLETION(io)


#define OSC1_FREQ 12000000
#define TEN_MICRO_SECONDS (OSC1_FREQ * 10 / 1000000)

/*
 * We will have to eventually replace this null definition with the real thing.
 */
#define WATCHDOG_RESET()

#define EXTFLASH_WRITE_FIFO_SIZE 32
#define EXTFLASH_WRITE_BLOCK_SIZE EXTFLASH_WRITE_FIFO_SIZE /* limit the size to
							    * FIFO capacity, so
							    * the thread can be
							    * suspended. */

#define JFFS2_FILESYSTEM_SIZE 0x100000

/****************************************************************************/

struct m25p {
	struct platform_device	*plt_dev;
	struct mutex		lock;
	struct mtd_info		mtd;
	unsigned		partitioned:1;
	u8			erase_opcode;
	u8			command[4];
};

static inline struct m25p *mtd_to_m25p(struct mtd_info *mtd)
{
	return container_of(mtd, struct m25p, mtd);
}

/****************************************************************************/

/*
 * Internal helper functions
 */

/*
 * Read the status register, returning its value in the location
 * Return the status register value.
 * Returns negative if error occurred.
 */
static int read_sr(struct m25p *flash)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)RA;

	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) |
		IO_XFL_CTL1_FC_DATA(1);
	io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_RDSR);
	FLASH_COMMAND_EXEC(io);

	return io->status1 & 0xff;
}

/*
 * mem_flash_io_read_u32()
 */
static u32 mem_flash_io_read_u32(u32 addr)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)RA;
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) |
		IO_XFL_CTL1_FC_DATA(4) | IO_XFL_CTL1_FC_DUMMY(1) |
		IO_XFL_CTL1_FC_ADDR;
	io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_FAST_READ) |
		IO_XFL_CTL2_FC_ADDR(addr);
	FLASH_COMMAND_EXEC(io);
	return io->status1;
}

/*
 * mem_flash_read_u8()
 */
static u8 mem_flash_read_u8(u32 addr)
{
	u32 tmp_addr = ALIGN_DOWN(addr, 4);
	u32 tmp_data = mem_flash_io_read_u32(tmp_addr);
	u8 *ptr = (u8 *)&tmp_data;
	return ptr[addr & 0x3];
}

/*
 * mem_flash_read()
 *	No need to lock as read is implemented with ireads (same as normal flash
 *	execution).
 */
static void mem_flash_read(u32 addr, void *dst, size_t length)
{
	/*
	 * Range check
	 */
	/*
	 * Fix source alignment.
	 */
	while (addr & 0x03) {
		if (length == 0) {
			return;
		}
		*((u8 *)dst) = mem_flash_read_u8(addr++);
		dst++;
		length--;
	}

	while (length >= 4) {
		u32 tmp_data = mem_flash_io_read_u32(addr);
		addr += 4;
		length -= 4;

		/*
		 * Send the data to the destination.
		 */
		memcpy((void *)dst, (void *)&tmp_data, 4);
		dst += 4;
	}

	while (length--) {
		*((u8 *)dst) = mem_flash_read_u8(addr++);
		dst++;
	}
}

/*
 * mem_flash_wait_until_complete()
 */
static void mem_flash_wait_until_complete(void)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)RA;

	do {
		/*
		 * Put a delay here to deal with flash programming problem.
		 */
		u32 mptval = UBICOM32_IO_TIMER->mptval + TEN_MICRO_SECONDS;
		while (UBICOM32_IO_TIMER->mptval < mptval)
			;

		io->ctl1 &= ~IO_XFL_CTL1_MASK;
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) |
			IO_XFL_CTL1_FC_DATA(1);
		io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_RDSR);
		FLASH_COMMAND_EXEC(io);
	} while (io->status1 & SR_WIP);
}

/*
 * mem_flash_write_next()
 */
static size_t mem_flash_write_next(u32 addr, u8 *buf, size_t length)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)RA;
	u32 data_start = addr;
	u32 data_end = addr + length;
	size_t count;
	u32 i, j;

	/*
	 * Top limit address.
	 */
	u32 block_start = ALIGN_DOWN(data_start, 4);
	u32 block_end = block_start + EXTFLASH_WRITE_BLOCK_SIZE;

	union {
		u8 byte[EXTFLASH_WRITE_BLOCK_SIZE];
		u32 word[EXTFLASH_WRITE_BLOCK_SIZE / 4];
	} write_buf;

	u32 *flash_addr = (u32 *)block_start;

	/*
	 * The write block must be limited by FLASH internal buffer.
	 */
	u32 block_end_align = ALIGN_DOWN(block_end, 256);
	bool write_needed;

	block_end = (block_end_align > block_start)
		? block_end_align : block_end;
	data_end = (data_end <= block_end) ? data_end : block_end;
	block_end = ALIGN_UP(data_end, 4);
	count = data_end - data_start;

	/*
	 * Transfer data to a buffer.
	 */
	for (i = 0; i < (block_end - block_start) / 4; i++) {
		/*
		 * The FLASH read can hold D-cache for a long time.
		 * Use I/O operation to read FLASH to avoid starving other
		 * threads, especially HRT.  (Do this for application only)
		 */
		write_buf.word[i] = mem_flash_io_read_u32(
			(u32)(&flash_addr[i]));
	}

	write_needed = false;
	for (i = 0, j = (data_start - block_start);
	     i < (data_end - data_start); i++, j++) {
		write_needed = write_needed || (write_buf.byte[j] != buf[i]);
		write_buf.byte[j] &= buf[i];
	}


	/*
	 * If the data in FLASH is identical to what to be written. Then skip
	 * it.
	 */
	if (write_needed) {
		/*
		 * Write to flash.
		 */
		void *tmp __attribute__((unused));
		s32 extra_words;

		asm volatile(
		"	move.4	%0, %2									\n\t"
		"	bset	"D(IO_INT_SET)"(%1), #0, #%%bit("D(IO_PORTX_INT_FIFO_TX_RESET)")	\n\t"
		"	pipe_flush 0									\n\t"
		"	.rept	"D(EXTFLASH_WRITE_FIFO_SIZE / 4)"					\n\t"
		"	move.4	"D(IO_TX_FIFO)"(%1), (%0)4++						\n\t"
		"	.endr										\n\t"
			: "=&a" (tmp)
			: "a" (io), "r" (&write_buf.word[0])
			: "memory", "cc"
		);

		/* Lock FLASH for write access. */
		io->ctl0 |= IO_XFL_CTL0_MCB_LOCK;

		/* Command: WREN */
		io->ctl1 &= ~IO_XFL_CTL1_MASK;
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD);
		io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_WREN);
		FLASH_COMMAND_EXEC(io);

		/* Command: BYTE PROGRAM */
		io->ctl1 &= ~IO_XFL_CTL1_MASK;
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_WR) |
			IO_XFL_CTL1_FC_DATA(block_end - block_start) |
			IO_XFL_CTL1_FC_ADDR;
		io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_PP) |
			IO_XFL_CTL2_FC_ADDR(block_start);
		FLASH_COMMAND_KICK_OFF(io);

		extra_words = (s32)(block_end - block_start -
				    EXTFLASH_WRITE_FIFO_SIZE) / 4;
		if (extra_words > 0) {
			asm volatile(
			"	move.4		%0, %3				\n\t"
			"1:	cmpi		"D(IO_FIFO_LEVEL)"(%1), #4	\n\t"
			"	jmpgt.s.t	1b				\n\t"
			"	move.4		"D(IO_TX_FIFO)"(%1), (%0)4++	\n\t"
			"	add.4		%2, #-1, %2			\n\t"
			"	jmpgt.t		1b				\n\t"
				: "=&a" (tmp)
				: "a" (io), "d" (extra_words),
				  "r" (&write_buf.word[EXTFLASH_WRITE_FIFO_SIZE / 4])
				: "memory", "cc"
			);
		}
		FLASH_COMMAND_WAIT_FOR_COMPLETION(io);

		mem_flash_wait_until_complete();


		/* Unlock FLASH for cache access. */
		io->ctl0 &= ~IO_XFL_CTL0_MCB_LOCK;
	}

	/*
	 * Complete.
	 */
	return count;
}

/*
 * mem_flash_write()
 */
static void mem_flash_write(u32 addr, const void *src, size_t length)
{
	/*
	 * Write data
	 */
	u8_t *ptr = (u8_t *)src;
	while (length) {
		size_t count = mem_flash_write_next(addr, ptr, length);
		addr += count;
		ptr += count;
		length -= count;
	}
}

/*
 * Service routine to read status register until ready, or timeout occurs.
 * Returns non-zero if error.
 */
static int wait_till_ready(struct m25p *flash)
{
	int count;
	int sr;

	/* one chip guarantees max 5 msec wait here after page writes,
	 * but potentially three seconds (!) after page erase.
	 */
	for (count = 0; count < MAX_READY_WAIT_COUNT; count++) {
		u32 mptval;
		sr = read_sr(flash);
		if (sr < 0)
			break;
		else if (!(sr & SR_WIP))
			return 0;

		/*
		 * Put a 10us delay here to deal with flash programming problem.
		 */
		mptval = UBICOM32_IO_TIMER->mptval + TEN_MICRO_SECONDS;
		while ((s32)(mptval - UBICOM32_IO_TIMER->mptval) > 0) {
			WATCHDOG_RESET();
		}
		/* REVISIT sometimes sleeping would be best */
	}

	return 1;
}

/*
 * mem_flash_erase_page()
 */
static void mem_flash_erase_page(u32 addr)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)RA;

	/* Lock FLASH for write access. */
	io->ctl0 |= IO_XFL_CTL0_MCB_LOCK;

	/* Command: WREN */
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD);
	io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_WREN);
	FLASH_COMMAND_EXEC(io);

	/* Command: ERASE */
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD) |
		IO_XFL_CTL1_FC_ADDR;
	io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_SE) |
		IO_XFL_CTL2_FC_ADDR(addr);
	FLASH_COMMAND_EXEC(io);

	mem_flash_wait_until_complete();

	/* Unlock FLASH for cache access. */
	io->ctl0 &= ~IO_XFL_CTL0_MCB_LOCK;
}

/*
 * mem_flash_erase()
 */
static u32 mem_flash_erase(u32 addr, u32 length)
{
	/*
	 * Calculate the endaddress to be the first address of the page
	 * just beyond this erase section of pages.
	 */
	u32 endaddr = addr + length;

	/*
	 * Erase.
	 */
	while (addr < endaddr) {
		u32 test_addr = addr;
		mem_flash_erase_page(addr);

		/*
		 * Test how much was erased as actual flash page at this address
		 * may be smaller than the expected page size.
		 */
		while (test_addr < endaddr) {
			/*
			 * The FLASH read can hold D-cache for a long time.  Use
			 * I/O operation to read FLASH to avoid starving other
			 * threads, especially HRT.  (Do this for application
			 * only)
			 */
			if (mem_flash_io_read_u32(test_addr) != 0xFFFFFFFF) {
				break;
			}
			test_addr += 4;
		}
		if (test_addr == addr) {
			printk("erase failed at address 0x%x, skipping",
			       test_addr);
			test_addr += 4;
			return 1;
		}
		addr = test_addr;
	}
	return 0;
}


/****************************************************************************/

/*
 * MTD implementation
 */

/*
 * Erase an address range on the flash chip.  The address range may extend
 * one or more erase sectors.  Return an error is there is a problem erasing.
 */
static int ubicom32_flash_driver_erase(struct mtd_info *mtd,
				       struct erase_info *instr)
{
	struct m25p *flash = mtd_to_m25p(mtd);
	u32 addr, len;

	DEBUG(MTD_DEBUG_LEVEL2, "%s: %s %s 0x%08x, len %lld\n",
	      dev_name(&flash->plt_dev->dev), __FUNCTION__, "at",
	      (u32)instr->addr, instr->len);

	/* sanity checks */
	if (instr->addr + instr->len > flash->mtd.size)
		return -EINVAL;
	if ((instr->addr % mtd->erasesize) != 0
			|| (instr->len % mtd->erasesize) != 0) {
		return -EINVAL;
	}

	addr = instr->addr + UBICOM32_FLASH_BASE;
	len = instr->len;

	mutex_lock(&flash->lock);

	/* REVISIT in some cases we could speed up erasing large regions
	 * by using OPCODE_SE instead of OPCODE_BE_4K
	 */

	/* now erase those sectors */
	if (mem_flash_erase(addr, len)) {
		instr->state = MTD_ERASE_FAILED;
		mutex_unlock(&flash->lock);
		return -EIO;
	}

	mutex_unlock(&flash->lock);
	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);
	return 0;
}

/*
 * Read an address range from the flash chip.  The address range
 * may be any size provided it is within the physical boundaries.
 */
static int ubicom32_flash_driver_read(struct mtd_info *mtd, loff_t from,
				      size_t len, size_t *retlen, u_char *buf)
{
	struct m25p *flash = mtd_to_m25p(mtd);
	u32 base_addr = UBICOM32_FLASH_BASE + from;

	DEBUG(MTD_DEBUG_LEVEL2, "%s: %s %s 0x%08x, len %d\n",
	      dev_name(&flash->plt_dev->dev), __FUNCTION__, "from",
	      (u32)from, len);

	/* sanity checks */
	if (!len)
		return 0;

	if (from + len > flash->mtd.size)
		return -EINVAL;

	/* Byte count starts at zero. */
	if (retlen)
		*retlen = 0;

	mutex_lock(&flash->lock);

	/* Wait till previous write/erase is done. */
	if (wait_till_ready(flash)) {
		/* REVISIT status return?? */
		mutex_unlock(&flash->lock);
		return 1;
	}

	mem_flash_read(base_addr, (void *)buf, len);

	if (retlen)
		*retlen = len;

	mutex_unlock(&flash->lock);

	return 0;
}

/*
 * Write an address range to the flash chip.  Data must be written in
 * FLASH_PAGESIZE chunks.  The address range may be any size provided
 * it is within the physical boundaries.
 */
static int ubicom32_flash_driver_write(struct mtd_info *mtd, loff_t to,
				       size_t len, size_t *retlen,
				       const u_char *buf)
{
	struct m25p *flash = mtd_to_m25p(mtd);
	u32 base_addr = UBICOM32_FLASH_BASE + to;
	DEBUG(MTD_DEBUG_LEVEL2, "%s: %s %s 0x%08x, len %d\n",
	      dev_name(&flash->plt_dev->dev), __FUNCTION__, "to",
	      (u32)to, len);

	if (retlen)
		*retlen = 0;

	/* sanity checks */
	if (!len)
		return 0;

	if (to + len > flash->mtd.size)
		return -EINVAL;

	mutex_lock(&flash->lock);

	mem_flash_write(base_addr, (void *) buf, len);

	/* Wait until finished previous write command. */
	if (wait_till_ready(flash)) {
		mutex_unlock(&flash->lock);
		return 1;
	}

	if (retlen)
		*retlen = len;

	mutex_unlock(&flash->lock);
	return 0;
}


/****************************************************************************/

/*
 * SPI device driver setup and teardown
 */

struct flash_info {
	char		*name;

	/* JEDEC id zero means "no ID" (most older chips); otherwise it has
	 * a high byte of zero plus three data bytes: the manufacturer id,
	 * then a two byte device id.
	 */
	u32		jedec_id;

	/* The size listed here is what works with OPCODE_SE, which isn't
	 * necessarily called a "sector" by the vendor.
	 */
	unsigned	sector_size;
	u16		n_sectors;

	u16		flags;
#define	SECT_4K		0x01		/* OPCODE_BE_4K works uniformly */
};


/* NOTE: double check command sets and memory organization when you add
 * more flash chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 */
static struct flash_info __devinitdata m25p_data[] = {

	/* Atmel -- some are (confusingly) marketed as "DataFlash" */
	{ "at25fs010",  0x1f6601, 32 * 1024, 4, SECT_4K, },
	{ "at25fs040",  0x1f6604, 64 * 1024, 8, SECT_4K, },

	{ "at25df041a", 0x1f4401, 64 * 1024, 8, SECT_4K, },

	{ "at26f004",   0x1f0400, 64 * 1024, 8, SECT_4K, },
	{ "at26df081a", 0x1f4501, 64 * 1024, 16, SECT_4K, },
	{ "at26df161a", 0x1f4601, 64 * 1024, 32, SECT_4K, },
	{ "at26df321",  0x1f4701, 64 * 1024, 64, SECT_4K, },

	/* Spansion -- single (large) sector size only, at least
	 * for the chips listed here (without boot sectors).
	 */
	{ "s25sl004a", 0x010212, 64 * 1024, 8, },
	{ "s25sl008a", 0x010213, 64 * 1024, 16, },
	{ "s25sl016a", 0x010214, 64 * 1024, 32, },
	{ "s25sl032a", 0x010215, 64 * 1024, 64, },
	{ "s25sl064a", 0x010216, 64 * 1024, 128, },

	/* SST -- large erase sizes are "overlays", "sectors" are 4K */
	{ "sst25vf040b", 0xbf258d, 64 * 1024, 8, SECT_4K, },
	{ "sst25vf080b", 0xbf258e, 64 * 1024, 16, SECT_4K, },
	{ "sst25vf016b", 0xbf2541, 64 * 1024, 32, SECT_4K, },
	{ "sst25vf032b", 0xbf254a, 64 * 1024, 64, SECT_4K, },

	/* ST Microelectronics -- newer production may have feature updates */
	{ "m25p05",  0x202010,  32 * 1024, 2, },
	{ "m25p10",  0x202011,  32 * 1024, 4, },
	{ "m25p20",  0x202012,  64 * 1024, 4, },
	{ "m25p40",  0x202013,  64 * 1024, 8, },
	{ "m25p80",         0,  64 * 1024, 16, },
	{ "m25p16",  0x202015,  64 * 1024, 32, },
	{ "m25p32",  0x202016,  64 * 1024, 64, },
	{ "m25p64",  0x202017,  64 * 1024, 128, },
	{ "m25p128", 0x202018, 256 * 1024, 64, },

	{ "m45pe80", 0x204014,  64 * 1024, 16, },
	{ "m45pe16", 0x204015,  64 * 1024, 32, },

	{ "m25pe80", 0x208014,  64 * 1024, 16, },
	{ "m25pe16", 0x208015,  64 * 1024, 32, SECT_4K, },

	/* Winbond -- w25x "blocks" are 64K, "sectors" are 4KiB */
	{ "w25x10", 0xef3011, 64 * 1024, 2, SECT_4K, },
	{ "w25x20", 0xef3012, 64 * 1024, 4, SECT_4K, },
	{ "w25x40", 0xef3013, 64 * 1024, 8, SECT_4K, },
	{ "w25x80", 0xef3014, 64 * 1024, 16, SECT_4K, },
	{ "w25x16", 0xef3015, 64 * 1024, 32, SECT_4K, },
	{ "w25x32", 0xef3016, 64 * 1024, 64, SECT_4K, },
	{ "w25x64", 0xef3017, 64 * 1024, 128, SECT_4K, },

	/* Macronix -- mx25lxxx */
	{ "mx25l32",  0xc22016, 64 * 1024,  64, },
	{ "mx25l64",  0xc22017, 64 * 1024, 128, },
	{ "mx25l128", 0xc22018, 64 * 1024, 256, },

};

struct flash_info *__devinit jedec_probe(struct platform_device *spi)
{
	int			tmp;
	u32			jedec;
	struct flash_info	*info;
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)RA;

	/*
	 * Setup and run RDID command on the flash.
	 */
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) |
		IO_XFL_CTL1_FC_DATA(3);
	io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_RDID);
	FLASH_COMMAND_EXEC(io);

	jedec = io->status1 & 0x00ffffff;

	for (tmp = 0, info = m25p_data;
			tmp < ARRAY_SIZE(m25p_data);
			tmp++, info++) {
		if (info->jedec_id == jedec)
			return info;
	}
	dev_err(&spi->dev, "unrecognized JEDEC id %06x\n", jedec);
	return NULL;
}


/*
 * board specific setup should have ensured the SPI clock used here
 * matches what the READ command supports, at least until this driver
 * understands FAST_READ (for clocks over 25 MHz).
 */
static int __devinit ubicom32_flash_probe(struct platform_device *spi)
{
	struct flash_platform_data	*data;
	struct m25p			*flash;
	struct flash_info		*info;
	unsigned			i;

	/* Platform data helps sort out which chip type we have, as
	 * well as how this board partitions it.  If we don't have
	 * a chip ID, try the JEDEC id commands; they'll work for most
	 * newer chips, even if we don't recognize the particular chip.
	 */
	data = spi->dev.platform_data;
	if (data && data->type) {
		for (i = 0, info = m25p_data;
				i < ARRAY_SIZE(m25p_data);
				i++, info++) {
			if (strcmp(data->type, info->name) == 0)
				break;
		}

		/* unrecognized chip? */
		if (i == ARRAY_SIZE(m25p_data)) {
			DEBUG(MTD_DEBUG_LEVEL0, "%s: unrecognized id %s\n",
			      dev_name(&spi->dev), data->type);
			info = NULL;

		/* recognized; is that chip really what's there? */
		} else if (info->jedec_id) {
			struct flash_info	*chip = jedec_probe(spi);

			if (!chip || chip != info) {
				dev_warn(&spi->dev, "found %s, expected %s\n",
						chip ? chip->name : "UNKNOWN",
						info->name);
				info = NULL;
			}
		}
	} else
		info = jedec_probe(spi);

	if (!info)
		return -ENODEV;

	flash = kzalloc(sizeof *flash, GFP_KERNEL);
	if (!flash)
		return -ENOMEM;

	flash->plt_dev = spi;
	mutex_init(&flash->lock);
	dev_set_drvdata(&spi->dev, flash);

	if (data && data->name)
		flash->mtd.name = data->name;
	else
		flash->mtd.name = dev_name(&spi->dev);

	flash->mtd.type = MTD_NORFLASH;
	flash->mtd.writesize = 1;
	flash->mtd.flags = MTD_CAP_NORFLASH;
	flash->mtd.size = info->sector_size * info->n_sectors;
	flash->mtd.erase = ubicom32_flash_driver_erase;
	flash->mtd.read = ubicom32_flash_driver_read;
	flash->mtd.write = ubicom32_flash_driver_write;

	/* prefer "small sector" erase if possible */
	/*
	 * The Ubicom erase code does not use the opcode for smaller sectors,
	 * so disable that functionality and keep erasesize == sector_size
	 * so that the test in ubicom32_flash_driver_erase works properly.
	 *
	 * This was: `if (info->flags & SECT_4K) {' instead of `if (0) {'
	 */
	if (0) {
		flash->erase_opcode = OPCODE_BE_4K;
		flash->mtd.erasesize = 4096;
	} else {
		flash->erase_opcode = OPCODE_SE;
		flash->mtd.erasesize = info->sector_size;
	}

	dev_info(&spi->dev, "%s (%lld Kbytes)\n", info->name,
		 flash->mtd.size / 1024);

	DEBUG(MTD_DEBUG_LEVEL2,
		"mtd .name = %s, .size = 0x%.8llx (%lluMiB) "
			".erasesize = 0x%.8x (%uKiB) .numeraseregions = %d\n",
		flash->mtd.name,
		flash->mtd.size, flash->mtd.size / (1024*1024),
		flash->mtd.erasesize, flash->mtd.erasesize / 1024,
		flash->mtd.numeraseregions);

	if (flash->mtd.numeraseregions)
		for (i = 0; i < flash->mtd.numeraseregions; i++)
			DEBUG(MTD_DEBUG_LEVEL2,
				"mtd.eraseregions[%d] = { .offset = 0x%.8llx, "
				".erasesize = 0x%.8x (%uKiB), "
				".numblocks = %d }\n",
				i, flash->mtd.eraseregions[i].offset,
				flash->mtd.eraseregions[i].erasesize,
				flash->mtd.eraseregions[i].erasesize / 1024,
				flash->mtd.eraseregions[i].numblocks);


	/* partitions should match sector boundaries; and it may be good to
	 * use readonly partitions for writeprotected sectors (BP2..BP0).
	 */
	if (mtd_has_partitions()) {
		struct mtd_partition	*parts = NULL;
		int			nr_parts = 0;

#ifdef CONFIG_MTD_CMDLINE_PARTS
		static const char *part_probes[] = { "cmdlinepart", NULL, };

		nr_parts = parse_mtd_partitions(&flash->mtd,
				part_probes, &parts, 0);
#endif

		if (nr_parts <= 0 && data && data->parts) {
			parts = data->parts;
			nr_parts = data->nr_parts;
			if (nr_parts >= 2) {
				/*
				 * Set last partition size to be 1M.
				 */
				parts[1].size = flash->mtd.size -
					parts[0].size - JFFS2_FILESYSTEM_SIZE;
				parts[2].size = JFFS2_FILESYSTEM_SIZE;
			}
		}

		if (nr_parts > 0) {
			for (i = 0; i < nr_parts; i++) {
				DEBUG(MTD_DEBUG_LEVEL2, "partitions[%d] = "
					"{.name = %s, .offset = 0x%.8llx, "
						".size = 0x%.8llx (%lluKiB) }\n",
					i, parts[i].name,
					parts[i].offset,
					parts[i].size,
					parts[i].size / 1024);
			}
			flash->partitioned = 1;
			return add_mtd_partitions(&flash->mtd, parts, nr_parts);
		}
	} else if (data->nr_parts)
		dev_warn(&spi->dev, "ignoring %d default partitions on %s\n",
				data->nr_parts, data->name);

	return add_mtd_device(&flash->mtd) == 1 ? -ENODEV : 0;
}


static int __devexit ubicom32_flash_remove(struct spi_device *spi)
{
	struct m25p	*flash = dev_get_drvdata(&spi->dev);
	int		status;

	/* Clean up MTD stuff. */
	if (mtd_has_partitions() && flash->partitioned)
		status = del_mtd_partitions(&flash->mtd);
	else
		status = del_mtd_device(&flash->mtd);
	if (status == 0)
		kfree(flash);
	return 0;
}

static struct platform_driver ubicom32_flash_driver = {
	.driver = {
		.name	= "ubicom32flashdriver",
		.bus	= &platform_bus_type,
		.owner	= THIS_MODULE,
	},
	.probe	= ubicom32_flash_probe,
	.remove	= NULL,
};

static int ubicom32_flash_driver_init(void)
{
	return platform_driver_register(&ubicom32_flash_driver);
}


static void ubicom32_flash_driver_exit(void)
{
	platform_driver_unregister(&ubicom32_flash_driver);
}


module_init(ubicom32_flash_driver_init);
module_exit(ubicom32_flash_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mike Lavender");
MODULE_DESCRIPTION("Ubicom32 MTD SPI driver for ST M25Pxx flash chips");
