/*
 * Micron SPI-ER NAND Flash Memory
 *	This code uses the built in Ubicom flash controller
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
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/err.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#define DRIVER_NAME				"ubi32-nand-spi-er"
#define UBI32_NAND_SPI_ER_BLOCK_FROM_ROW(row)	(row >> 6)

#define UBI32_NAND_SPI_ER_STATUS_P_FAIL		(1 << 3)
#define UBI32_NAND_SPI_ER_STATUS_E_FAIL		(1 << 2)
#define UBI32_NAND_SPI_ER_STATUS_OIP		(1 << 0)

#define UBI32_NAND_SPI_ER_LAST_ROW_INVALID	0xFFFFFFFF
#define	UBI32_NAND_SPI_ER_BAD_BLOCK_MARK_OFFSET	0x08

struct ubi32_nand_spi_er_device {
	const char		*name;

	uint16_t		id;

	unsigned int		blocks;
	unsigned int		pages_per_block;
	unsigned int		page_size;
	unsigned int		write_size;
	unsigned int		erase_size;
};

struct ubi32_nand_spi_er {
	char				name[24];

	const struct ubi32_nand_spi_er_device	*device;

	struct mutex			lock;
	struct platform_device		*pdev;

	struct mtd_info			mtd;

	unsigned int			last_row;	/* the last row we fetched */

	/*
	 * Bad block table (MUST be last in strcuture)
	 */
	unsigned long			nbb;
	unsigned long			bbt[0];
};

/*
 * Chip supports a write_size of 512, but we cannot do partial
 * page with command 0x84.
 *
 * We need to use command 0x84 because we cannot fill the FIFO fast
 * enough to transfer the whole 512 bytes at a time. (maybe through
 * OCM?)
 */
const struct ubi32_nand_spi_er_device ubi32_nand_spi_er_devices[] = {
	{
		name:			"MT29F1G01ZDC",
		id:			0x2C12,
		blocks:			1024,
		pages_per_block:	64,
		page_size:		2048,
		write_size:		2048,
		erase_size:		64 * 2048,
	},
	{
		name:			"MT29F1G01ZDC",
		id:			0x2C13,
		blocks:			1024,
		pages_per_block:	64,
		page_size:		2048,
		write_size:		2048,
		erase_size:		64 * 2048,
	},
};

static int read_only = 0;
module_param(read_only, int, 0);
MODULE_PARM_DESC(read_only, "Leave device locked");

/*
 * Ubicom32 FLASH Command Set
 */
#define FLASH_PORT		RA

#define FLASH_FC_INST_CMD	0x00	/* for SPI command only transaction */
#define FLASH_FC_INST_WR	0x01	/* for SPI write transaction */
#define FLASH_FC_INST_RD	0x02	/* for SPI read transaction */

#define FLASH_COMMAND_KICK_OFF(io)								\
	asm volatile(										\
		"	bset	"D(IO_INT_CLR)"(%0), #0, #%%bit("D(IO_XFL_INT_DONE)")	\n\t"	\
		"	jmpt.t	.+4							\n\t"	\
		"	bset	"D(IO_INT_SET)"(%0), #0, #%%bit("D(IO_XFL_INT_START)")	\n\t"	\
		:										\
		: "a" (io)									\
		: "cc"										\
		);

#define FLASH_COMMAND_WAIT_FOR_COMPLETION(io)							\
	asm volatile(										\
		"	btst	"D(IO_INT_STATUS)"(%0), #%%bit("D(IO_XFL_INT_DONE)")	\n\t"	\
		"	jmpeq.f	.-4							\n\t"	\
		:										\
		: "a" (io)									\
		: "cc"										\
	);

#define FLASH_COMMAND_EXEC(io)				\
		FLASH_COMMAND_KICK_OFF(io)		\
		FLASH_COMMAND_WAIT_FOR_COMPLETION(io)

/*
 * ubi32_nand_spi_er_get_feature
 *	Get Feature register
 */
static uint8_t ubi32_nand_spi_er_get_feature(uint32_t reg)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)FLASH_PORT;

	/*
	 * Note that this will produce the sequence:
	 * 	SI [0F][REG][00][00]
	 * 	SO ---------[SR][SR][SR]
	 * Since the flash controller can only output 24 bits of address, this is
	 * ok for this command since the data will just repeat as long as the CS
	 * is asserted and the clock is running.
	 */
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(1) |
		    IO_XFL_CTL1_FC_ADDR;
	io->ctl2 = IO_XFL_CTL2_FC_CMD(0x0F) | IO_XFL_CTL2_FC_ADDR(reg << 16);
	FLASH_COMMAND_EXEC(io);

	return io->status1 & 0xFF;
}

/*
 * ubi32_nand_spi_er_write_buf
 *	writes a buffer to the bus
 *
 * Writes 511 + 1 bytes to the bus, we have to stuff one data byte into the address.
 */
static void ubi32_nand_spi_er_write_buf(const uint8_t *buf, uint32_t col)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)FLASH_PORT;
	uint32_t tmp;

	asm volatile (
		"	bset		"D(IO_INT_SET)"(%[port]), #0, #%%bit("D(IO_PORTX_INT_FIFO_TX_RESET)")	\n\t"
		"	pipe_flush	0									\n\t"
		:
		: [port] "a" (FLASH_PORT)
		: "cc"
	);

	/*
	 * Write the data into the cache
	 */
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
#ifdef SUPPORT_512_FIFO
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_WR) | IO_XFL_CTL1_FC_DATA(511) |
#endif
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_WR) | IO_XFL_CTL1_FC_DATA(31) |
		    IO_XFL_CTL1_FC_ADDR;

	/*
	 * Construct the address with the first byte of data
	 */
	tmp = (col << 8) | *buf++;
	io->ctl2 = IO_XFL_CTL2_FC_CMD(0x84) | IO_XFL_CTL2_FC_ADDR(tmp);

	asm volatile (

		/*
		 * Move 32 bytes
		 *
		 * The first word needs to be [11][22][33][33] to work around a flash
		 * controller bug.
		 */
		"	move.2		%[tmp], (%[data])2++							\n\t"
		"	shmrg.1		%[tmp], (%[data]), %[tmp]						\n\t"
		"	shmrg.1		%[tmp], (%[data])1++, %[tmp]						\n\t"
		"	move.4		"D(IO_TX_FIFO)"(%[port]), %[tmp]					\n\t"

		/*
		 * We're aligned again!
		 */
		"	.rept 7											\n\t"
		"	move.4		"D(IO_TX_FIFO)"(%[port]), (%[data])4++					\n\t"
		"	.endr											\n\t"

		/*
		 * Kick off the flash command
		 */
		"	bset	"D(IO_INT_CLR)"(%[port]), #0, #%%bit("D(IO_XFL_INT_DONE)")			\n\t"
		"	jmpt.t	.+4										\n\t"
		"	bset	"D(IO_INT_SET)"(%[port]), #0, #%%bit("D(IO_XFL_INT_START)")			\n\t"

#ifdef SUPPORT_512_FIFO
		/*
		 * Fill the remaining 120 words as space becomes available
		 */
		"1:												\n\t"
		"	cmpi		"D(IO_FIFO_LEVEL)"(%[port]), #4						\n\t"
		"	jmpgt.s.t	1b									\n\t"
		"	move.4		"D(IO_TX_FIFO)"(%[port]), (%[data])4++					\n\t"
		"	move.4		"D(IO_TX_FIFO)"(%[port]), (%[data])4++					\n\t"
		"	move.4		"D(IO_TX_FIFO)"(%[port]), (%[data])4++					\n\t"
		"	move.4		"D(IO_TX_FIFO)"(%[port]), (%[data])4++					\n\t"
		"	add.4		%[cnt], #-4, %[cnt]							\n\t"
		"	jmpgt.t		1b									\n\t"
#endif
		/*
		 * Wait for the transaction to finish
		 */
		"	btst	"D(IO_INT_STATUS)"(%[port]), #%%bit("D(IO_XFL_INT_DONE)")			\n\t"
		"	jmpeq.f	.-4										\n\t"

		: [tmp] "=&d" (tmp),
		  [data] "+&a" (buf)
		: [column] "d" (col),
		  [port] "a" (FLASH_PORT),
		  [cnt] "d" (120)		// see above comment
		: "cc"
	);
}

/*
 * ubi32_nand_spi_er_send_rd_addr
 *	perform FC_RD: CMD + address
 */
static void ubi32_nand_spi_er_send_rd_addr(uint8_t command, uint32_t address)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)FLASH_PORT;

	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(4) |
		    IO_XFL_CTL1_FC_ADDR;
	io->ctl2 = IO_XFL_CTL2_FC_CMD(command) | IO_XFL_CTL2_FC_ADDR(address);
	FLASH_COMMAND_EXEC(io);
}

/*
 * ubi32_nand_spi_er_send_cmd_addr
 *	perform FC_(xxx): CMD + address
 */
static void ubi32_nand_spi_er_send_cmd_addr(uint8_t command, uint32_t address)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)FLASH_PORT;

	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD) | IO_XFL_CTL1_FC_ADDR;
	io->ctl2 = IO_XFL_CTL2_FC_CMD(command) | IO_XFL_CTL2_FC_ADDR(address);
	FLASH_COMMAND_EXEC(io);
}

/*
 * ubi32_nand_spi_er_write_disable
 *	clear the write enable bit
 */
static void ubi32_nand_spi_er_write_disable(void)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)FLASH_PORT;

	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD);
	io->ctl2 = IO_XFL_CTL2_FC_CMD(0x04);
	FLASH_COMMAND_EXEC(io);
}

/*
 * ubi32_nand_spi_er_write_enable
 *	set the write enable bit
 */
static void ubi32_nand_spi_er_write_enable(void)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)FLASH_PORT;

	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD);
	io->ctl2 = IO_XFL_CTL2_FC_CMD(0x06);
	FLASH_COMMAND_EXEC(io);
}

/*
 * ubi32_nand_spi_er_busywait
 *	Wait until the chip is not busy
 */
static uint8_t ubi32_nand_spi_er_busywait(void)
{
	int i;
	uint8_t data;

	/*
	 * tRD is 100us, so don't delay too long, however, tERS is
	 * 10ms so you'd better loop enough.
	 */
	for (i = 0; i < 200; i++) {
		data = ubi32_nand_spi_er_get_feature(0xC0);
		if (!(data & UBI32_NAND_SPI_ER_STATUS_OIP)) {
			break;
		}

		udelay(50);
	}

	return data;
}

/*
 * ubi32_nand_spi_er_erase
 *	Erase a block, parameters must be block aligned
 */
static int ubi32_nand_spi_er_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct ubi32_nand_spi_er *chip = mtd->priv;
	int res;

	DEBUG(MTD_DEBUG_LEVEL3, "%s: erase addr:%x len:%x\n", chip->name, instr->addr, instr->len);

	if ((instr->addr + instr->len) > mtd->size) {
		return -EINVAL;
	}

	if (instr->addr & (chip->device->erase_size - 1)) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: erase address is not aligned %x\n", chip->name, instr->addr);
		return -EINVAL;
	}

	if (instr->len & (chip->device->erase_size - 1)) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: erase len is not aligned %x\n", chip->name, instr->len);
		return -EINVAL;
	}

	mutex_lock(&chip->lock);
	chip->last_row = UBI32_NAND_SPI_ER_LAST_ROW_INVALID;

	while (instr->len) {
		uint32_t block = instr->addr >> 17;
		uint32_t row = block << 6;
		uint8_t stat;
		DEBUG(MTD_DEBUG_LEVEL3, "%s: block erase row:%x block:%x addr:%x rem:%x\n", chip->name, row, block, instr->addr, instr->len);

		/*
		 * Test for bad block
		 */
		if (test_bit(block, chip->bbt)) {
			instr->fail_addr = block << 17;
			instr->state = MTD_ERASE_FAILED;
			res = -EBADMSG;
			goto done;
		}

		ubi32_nand_spi_er_write_enable();

		/*
		 * Block erase
		 */
		ubi32_nand_spi_er_send_cmd_addr(0xD8, row);

		/*
		 * Wait
		 */
		stat = ubi32_nand_spi_er_busywait();
		if (stat & UBI32_NAND_SPI_ER_STATUS_OIP) {
			instr->fail_addr = block << 17;
			instr->state = MTD_ERASE_FAILED;
			DEBUG(MTD_DEBUG_LEVEL1, "%s: chip is busy or nonresponsive stat=%02x\n", chip->name, stat);

			/*
			 * Chip is stuck?
			 */
			res = -EIO;
			goto done;
		}

		/*
		 * Check the status register
		 */
		if (stat & UBI32_NAND_SPI_ER_STATUS_E_FAIL) {
			DEBUG(MTD_DEBUG_LEVEL1, "%s: E_FAIL signalled (%02x)\n", chip->name, stat);
			instr->fail_addr = block << 17;
			instr->state = MTD_ERASE_FAILED;
			goto done;
		}

		/*
		 * Next
		 */
		block++;
		instr->len -= chip->device->erase_size;
		instr->addr += chip->device->erase_size;
	}

	instr->state = MTD_ERASE_DONE;

	mutex_unlock(&chip->lock);
	return 0;

done:
	ubi32_nand_spi_er_write_disable();

	mutex_unlock(&chip->lock);

	mtd_erase_callback(instr);
	return 0;
}

/*
 * ubi32_nand_spi_er_read
 *
 * return -EUCLEAN: ecc error recovered
 * return -EBADMSG: ecc error not recovered
*/
static int ubi32_nand_spi_er_read(struct mtd_info *mtd, loff_t from, size_t len,
				  size_t *retlen, u_char *buf)
{
	struct ubi32_nand_spi_er *chip = mtd->priv;
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)FLASH_PORT;

	uint32_t row;
	uint32_t column;
	int retval = 0;
	uint32_t *pbuf = (uint32_t *)buf;

	*retlen = 0;
	DEBUG(MTD_DEBUG_LEVEL2, "%s: read block from %llx len %d into %p\n", chip->name, from, len, buf);

	/*
	 * buf should be aligned
	 */
	if ((uint32_t)buf & 0x03) {
		return -EINVAL;
	}

	/*
	 * Zero length reads, nothing to do
	 */
	if (len == 0) {
		return 0;
	}

	/*
	 * Reject reads which go over the end of the flash
	 */
	if ((from + len) > mtd->size) {
		return -EINVAL;
	}

	/*
	 * Get the row and column address to start at
	 */
	row = from >> 11;
	column = from & 0x7FF;
	DEBUG(MTD_DEBUG_LEVEL3, "%s: row=%x %d column=%x %d last_row=%x %d\n", chip->name, row, row, column, column, chip->last_row, chip->last_row);

	/*
	 * Read the data from the chip
	 */
	mutex_lock(&chip->lock);
	while (len) {
		uint8_t stat;
		size_t toread;
		int i;
		int tmp;

		/*
		 * Figure out how much to read
		 *
		 * If we are reading from the middle of a page then the most we
		 * can read is to the end of the page
		 */
		toread = len;
		if (toread > (chip->device->page_size - column)) {
			toread = chip->device->page_size - column;
		}

		DEBUG(MTD_DEBUG_LEVEL3, "%s: buf=%p toread=%x row=%x column=%x last_row=%x\n", chip->name, pbuf, toread, row, column, chip->last_row);

		if (chip->last_row != row) {
			/*
			 * Check if the block is bad
			 */
			if (test_bit(UBI32_NAND_SPI_ER_BLOCK_FROM_ROW(row), chip->bbt)) {
				mutex_unlock(&chip->lock);
				return -EBADMSG;
			}

			/*
			 * Load the appropriate page
			 */
			ubi32_nand_spi_er_send_cmd_addr(0x13, row);

			/*
			 * Wait
			 */
			stat = ubi32_nand_spi_er_busywait();
			if (stat & UBI32_NAND_SPI_ER_STATUS_OIP) {
				DEBUG(MTD_DEBUG_LEVEL1, "%s: chip is busy or nonresponsive stat=%02x\n", chip->name, stat);

				/*
				 * Chip is stuck?
				 */
				mutex_unlock(&chip->lock);
				return -EIO;
			}

			/*
			 * Check the ECC bits
			 */
			stat >>= 4;
			if (stat == 1) {
				DEBUG(MTD_DEBUG_LEVEL1, "%s: ECC recovered, row=%x\n", chip->name, row);
				retval = -EUCLEAN;
			}
			if (stat == 2) {
				DEBUG(MTD_DEBUG_LEVEL0, "%s: failed ECC, row=%x\n", chip->name, row);
				chip->last_row = UBI32_NAND_SPI_ER_LAST_ROW_INVALID;
				mutex_unlock(&chip->lock);
				return -EBADMSG;
			}

		}

		chip->last_row = row;

		/*
		 * Read out the data:
		 *	We can always read a little too much since there is the
		 *	OOB after byte addr 2047.  The most we'll overread is 3 bytes.
		 */
		if (((uint32_t)pbuf & 0x03) == 0) {
			/*
			 * Aligned read
			 */
			tmp = toread & (~0x03);
			for (i = 0; i < tmp; i += 4) {
				ubi32_nand_spi_er_send_rd_addr(0x03, column << 8);
				*pbuf++ = io->status1;
				column += 4;
			}
		} else {
			/*
			 * Unaligned read
			 */
			tmp = toread & (~0x03);
			for (i = 0; i < tmp; i += 4) {
				ubi32_nand_spi_er_send_rd_addr(0x03, column << 8);
				memcpy(pbuf, &io->status1, 4);
				column += 4;
			}
		}

		/*
		 * Fill in any single bytes
		 */
		tmp = toread & 0x03;
		if (tmp) {
			uint8_t *bbuf = pbuf;
			uint32_t val;
			ubi32_nand_spi_er_send_rd_addr(0x03, column << 8);
			val = io->status1;
			for (i = 0; i < tmp; i++) {
				*bbuf++ = val >> 24;
				val <<= 8;
			}
		}

		len -= toread;
		*retlen += toread;

		/*
		 * For the next page, increment the row and always start at column 0
		 */
		column = 0;
		row++;
	}

	mutex_unlock(&chip->lock);
	return retval;
}

/*
 * ubi32_nand_spi_er_write
 */
#define WRITE_NOT_ALIGNED(x) ((x & (device->write_size - 1)) != 0)
static int ubi32_nand_spi_er_write(struct mtd_info *mtd, loff_t to, size_t len,
				   size_t *retlen, const u_char *buf)
{
	struct ubi32_nand_spi_er *chip = mtd->priv;
	const struct ubi32_nand_spi_er_device *device = chip->device;
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)FLASH_PORT;
	uint32_t row;
	uint32_t col;
	int res = 0;
	size_t towrite;

	DEBUG(MTD_DEBUG_LEVEL2, "%s: write block to %llx len %d from %p\n", chip->name, to, len, buf);

	*retlen = 0;

	/*
	 * nothing to write
	 */
	if (!len) {
		return 0;
	}

	/*
	 * Reject writes which go over the end of the flash
	 */
	if ((to + len) > mtd->size) {
		return -EINVAL;
	}

	/*
	 * buf should be aligned to 16 bits
	 */
	if ((uint32_t)buf & 0x01) {
		return -EINVAL;
	}

	/*
	 * Check to see if everything is page aligned
	 */
	if (WRITE_NOT_ALIGNED(to) || WRITE_NOT_ALIGNED(len)) {
		printk(KERN_NOTICE "ubi32_nand_spi_er_write: Attempt to write non page aligned data\n");
		return -EINVAL;
	}

	mutex_lock(&chip->lock);

	io->ctl0 |= IO_XFL_CTL0_MCB_LOCK;

	chip->last_row = UBI32_NAND_SPI_ER_LAST_ROW_INVALID;

	/*
	 * If the first write is a partial write then write at most the number of
	 * bytes to get us page aligned and then the remainder will be
	 * page aligned.  The last bit may be a partial page as well.
	 */
	col = to & (device->page_size - 1);
	towrite = device->page_size - col;
	if (towrite > len) {
		towrite = len;
	}

	/*
	 * Write the data
	 */
	row = to >> 11;
	while (len) {
		uint8_t stat;
		uint32_t my_towrite;

		DEBUG(MTD_DEBUG_LEVEL3, "%s: write %p to row:%x col:%x len:%x rem:%x\n", chip->name, buf, row, col, towrite, len);

		ubi32_nand_spi_er_write_enable();

		/*
		 * Move the data into the cache
		 */
		my_towrite = towrite;
		while (my_towrite) {
			uint32_t len = my_towrite;
			if (len > 32) {
				len = 32;
			}

			ubi32_nand_spi_er_write_buf(buf, col);
			buf += len;
			col += len;
			my_towrite -= len;
		}

		/*
		 * Program execute
		 */
		ubi32_nand_spi_er_send_cmd_addr(0x10, row);

		/*
		 * Wait
		 */
		stat = ubi32_nand_spi_er_busywait();
		if (stat & UBI32_NAND_SPI_ER_STATUS_OIP) {
			DEBUG(MTD_DEBUG_LEVEL1, "%s: chip is busy or nonresponsive stat=%02x\n", chip->name, stat);

			/*
			 * Chip is stuck?
			 */
			res = -EIO;
			goto done;
		}

		if (stat & (1 << 3)) {
			res = -EBADMSG;
			goto done;
		}

		row++;
		len -= towrite;
		*retlen += towrite;

		/*
		 * At this point, we are always page aligned so start at column 0.
		 * Note we may not have a full page to write at the end, hence the
		 * check if towrite > len.
		 */
		col = 0;
		towrite = device->page_size;
		if (towrite > len) {
			towrite = len;
		}
	}

	io->ctl0 &= ~IO_XFL_CTL0_MCB_LOCK;

	mutex_unlock(&chip->lock);
	return res;

done:
	ubi32_nand_spi_er_write_disable();

	io->ctl0 &= ~IO_XFL_CTL0_MCB_LOCK;

	mutex_unlock(&chip->lock);

	return res;
}

/*
 * ubi32_nand_spi_er_isbad
 */
static int ubi32_nand_spi_er_isbad(struct mtd_info *mtd, loff_t ofs)
{
	struct ubi32_nand_spi_er *chip = mtd->priv;
	uint32_t block;

	if (ofs & (chip->device->erase_size - 1)) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: address not aligned %llx\n", chip->name, ofs);
		return -EINVAL;
	}

	block = ofs >> 17;

	return test_bit(block, chip->bbt);
}

/*
 * ubi32_nand_spi_er_markbad
 */
static int ubi32_nand_spi_er_markbad(struct mtd_info *mtd, loff_t ofs)
{
	struct ubi32_nand_spi_er *chip = mtd->priv;
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)FLASH_PORT;
	uint32_t block;
	uint32_t row;
	int res = 0;
	uint8_t stat;

	if (ofs & (chip->device->erase_size - 1)) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: address not aligned %llx\n", chip->name, ofs);
		return -EINVAL;
	}

	block = ofs >> 17;

	/*
	 * If it's already marked bad, no need to mark it
	 */
	if (test_bit(block, chip->bbt)) {
		return 0;
	}

	/*
	 * Mark it in our cache
	 */
	__set_bit(block, chip->bbt);

	/*
	 * Write the user bad block mark.  If it fails, then we really
	 * can't do anything about it.
	 */
	mutex_lock(&chip->lock);
	chip->last_row = UBI32_NAND_SPI_ER_LAST_ROW_INVALID;

	ubi32_nand_spi_er_write_enable();

	/*
	 * Write the mark
	 */
	io->ctl0 |= IO_XFL_CTL0_MCB_LOCK;
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_WR) | IO_XFL_CTL1_FC_DATA(6);
	io->ctl2 = IO_XFL_CTL2_FC_CMD(0x84);

	asm volatile (
		"	bset		"D(IO_INT_SET)"(%[port]), #0, #%%bit("D(IO_PORTX_INT_FIFO_TX_RESET)")	\n\t"
		"	pipe_flush	0									\n\t"

		/*
		 * Move the data into the FIFO
		 */
		"	move.4		"D(IO_TX_FIFO)"(%[port]), %[word1]					\n\t"
		"	move.4		"D(IO_TX_FIFO)"(%[port]), %[word2]					\n\t"

		/*
		 * Kick off the flash command
		 */
		"	bset	"D(IO_INT_CLR)"(%[port]), #0, #%%bit("D(IO_XFL_INT_DONE)")			\n\t"
		"	jmpt.t	.+4										\n\t"
		"	bset	"D(IO_INT_SET)"(%[port]), #0, #%%bit("D(IO_XFL_INT_START)")			\n\t"

		/*
		 * Wait for the transaction to finish
		 */
		"	btst	"D(IO_INT_STATUS)"(%[port]), #%%bit("D(IO_XFL_INT_DONE)")			\n\t"
		"	jmpeq.f	.-4										\n\t"

		:
		: [word1] "d" (0x0800dead | (UBI32_NAND_SPI_ER_BAD_BLOCK_MARK_OFFSET << 16)),
		  [word2] "d" (0xbeef0000),
		  [port] "a" (FLASH_PORT)
		: "cc"
	);

	io->ctl0 &= ~IO_XFL_CTL0_MCB_LOCK;

	/*
	 * Program execute
	 */
	row = block << 6;
	ubi32_nand_spi_er_send_cmd_addr(0x10, row);

	/*
	 * Wait
	 */
	stat = ubi32_nand_spi_er_busywait();
	if (stat & UBI32_NAND_SPI_ER_STATUS_OIP) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: chip is busy or nonresponsive stat=%02x\n", chip->name, stat);

		/*
		 * Chip is stuck?
		 */
		res = -EIO;
		goto done;
	}

	if (stat & (1 << 3)) {
		res = -EBADMSG;
	}

done:
	ubi32_nand_spi_er_write_disable();

	mutex_unlock(&chip->lock);

	return res;
}

/*
 * ubi32_nand_spi_er_read_bbt
 */
static int ubi32_nand_spi_er_read_bbt(struct ubi32_nand_spi_er *chip)
{
	int j;
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)FLASH_PORT;

	for (j = 0; j < chip->device->blocks; j++) {
		unsigned short row = j << 6;
		uint8_t stat;

		/*
		 * Read Page
		 */
		ubi32_nand_spi_er_send_cmd_addr(0x13, row);

		/*
		 * Wait
		 */
		stat = ubi32_nand_spi_er_busywait();
		if (stat & UBI32_NAND_SPI_ER_STATUS_OIP) {
			DEBUG(MTD_DEBUG_LEVEL1, "%s: chip is busy or nonresponsive stat=%02x\n", chip->name, stat);

			/*
			 * Chip is stuck?
			 */
			return -EIO;
		}

		/*
		 * Check factory bad block mark
		 */
		ubi32_nand_spi_er_send_rd_addr(0x03, 0x080000);

		if ((io->status1 >> 24) != 0xFF) {
			chip->nbb++;
			__set_bit(j, chip->bbt);
			continue;
		}

		ubi32_nand_spi_er_send_rd_addr(0x03, 0x080000 | (UBI32_NAND_SPI_ER_BAD_BLOCK_MARK_OFFSET << 8));
		if (io->status1 == 0xdeadbeef) {
			chip->nbb++;
			__set_bit(j, chip->bbt);
		}
	}

#if defined(CONFIG_MTD_DEBUG) && (MTD_DEBUG_LEVEL3 <= CONFIG_MTD_DEBUG_VERBOSE)
	printk("%s: Bad Block Table:", chip->name);
	for (j = 0; j < chip->device->blocks; j++) {
		if ((j % 64) == 0) {
			printk("\n%s: block %03x: ", chip->name, j);
		}
		printk("%c", test_bit(j, chip->bbt) ? 'X' : '.');
	}
	printk("\n%s: Bad Block Numbers: ", chip->name);
	for (j = 0; j < chip->device->blocks; j++) {
		if (test_bit(j, chip->bbt)) {
			printk("%x ", j);
		}
	}
	printk("\n");
#endif

	return 0;
}

#ifndef MODULE
/*
 * Called at boot time:
 *
 * ubi32_nand_spi_er=read_only
 *	if read_only specified then do not unlock device
 */
static int __init ubi32_nand_spi_er_setup(char *str)
{
	if (str && (strncasecmp(str, "read_only", 9) == 0)) {
		read_only = 1;
	}
	return 0;
}

__setup("ubi32_nand_spi_er=", ubi32_nand_spi_er_setup);
#endif

/*
 * ubi32_nand_spi_er_probe
 *	Detect and initialize ubi32_nand_spi_er device.
 */
static int __devinit ubi32_nand_spi_er_probe(struct platform_device *pdev)
{
	uint32_t i;
	uint32_t id;
	int res;
	size_t bbt_bytes;
	struct ubi32_nand_spi_er *chip;
	const struct ubi32_nand_spi_er_device *device;
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)FLASH_PORT;

	/*
	 * Reset
	 */
	for (i = 0; i < 2; i++) {
		io->ctl1 &= ~IO_XFL_CTL1_MASK;
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD);
		io->ctl2 = IO_XFL_CTL2_FC_CMD(0xFF);
		FLASH_COMMAND_EXEC(io);
		udelay(250);
	}
	udelay(1000);

	/*
	 * Read out ID
	 */
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(2) |
		    IO_XFL_CTL1_FC_ADDR;
	io->ctl2 = IO_XFL_CTL2_FC_CMD(0x9F);
	FLASH_COMMAND_EXEC(io);

	id = io->status1 >> 16;
	device = ubi32_nand_spi_er_devices;
	for (i = 0; i < ARRAY_SIZE(ubi32_nand_spi_er_devices); i++) {
		if (device->id == id) {
			break;
		}
		device++;
	}
	if (i == ARRAY_SIZE(ubi32_nand_spi_er_devices)) {
		return -ENODEV;
	}

	/*
	 * Initialize our chip structure
	 */
	bbt_bytes = DIV_ROUND_UP(device->blocks, BITS_PER_BYTE);
	chip = kzalloc(sizeof(struct ubi32_nand_spi_er) + bbt_bytes, GFP_KERNEL);
	if (!chip) {
		return -ENOMEM;
	}
	snprintf(chip->name, sizeof(chip->name), "%s", device->name);

	chip->device = device;
	chip->last_row = UBI32_NAND_SPI_ER_LAST_ROW_INVALID;

	mutex_init(&chip->lock);

	chip->mtd.type = MTD_NANDFLASH;
	chip->mtd.flags = MTD_WRITEABLE;

	/*
	 * #blocks * block size * n blocks
	 */
	chip->mtd.size = device->blocks * device->pages_per_block * device->page_size;
	chip->mtd.erasesize = device->erase_size;

	/*
	 * 1 page, optionally we can support partial write (512)
	 */
	chip->mtd.writesize = device->write_size;
	chip->mtd.name = device->name;
	chip->mtd.erase = ubi32_nand_spi_er_erase;
	chip->mtd.read = ubi32_nand_spi_er_read;
	chip->mtd.write = ubi32_nand_spi_er_write;
	chip->mtd.block_isbad = ubi32_nand_spi_er_isbad;
	chip->mtd.block_markbad = ubi32_nand_spi_er_markbad;
	chip->mtd.priv = chip;

	/*
	 * Cache the bad block table
	 */
	res = ubi32_nand_spi_er_read_bbt(chip);
	if (res) {
		kfree(chip);
		return res;
	}

	/*
	 * Un/lock the chip
	 */
	io->ctl0 |= IO_XFL_CTL0_MCB_LOCK;
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_WR) | IO_XFL_CTL1_FC_DATA(2);
	io->ctl2 = IO_XFL_CTL2_FC_CMD(0x1F);

	if (read_only) {
		i = 0xa0380000;
	} else {
		i = 0xa0000000;
	}
	asm volatile (
		"	bset		"D(IO_INT_SET)"(%[port]), #0, #%%bit("D(IO_PORTX_INT_FIFO_TX_RESET)")	\n\t"
		"	pipe_flush	0									\n\t"

		/*
		 * Move the data into the FIFO
		 */
		"	move.4		"D(IO_TX_FIFO)"(%[port]), %[word1]					\n\t"

		/*
		 * Kick off the flash command
		 */
		"	bset	"D(IO_INT_CLR)"(%[port]), #0, #%%bit("D(IO_XFL_INT_DONE)")			\n\t"
		"	jmpt.t	.+4										\n\t"
		"	bset	"D(IO_INT_SET)"(%[port]), #0, #%%bit("D(IO_XFL_INT_START)")			\n\t"

		/*
		 * Wait for the transaction to finish
		 */
		"	btst	"D(IO_INT_STATUS)"(%[port]), #%%bit("D(IO_XFL_INT_DONE)")			\n\t"
		"	jmpeq.f	.-4										\n\t"

		:
		: [word1] "d" (i),
		  [port] "a" (FLASH_PORT)
		: "cc"
	);
	io->ctl0 &= ~IO_XFL_CTL0_MCB_LOCK;

	dev_set_drvdata(&pdev->dev, chip);

	printk(KERN_INFO "%s: added device size: %u KBytes %lu bad blocks %s\n", chip->mtd.name, DIV_ROUND_UP(chip->mtd.size, 1024), chip->nbb, read_only ? "[read only]" : "");
	return add_mtd_device(&chip->mtd);
}

/*
 * ubi32_nand_spi_er_remove
 */
static int __devexit ubi32_nand_spi_er_remove(struct platform_device *pdev)
{
	struct ubi32_nand_spi_er *chip = dev_get_drvdata(&pdev->dev);
	int status;

	DEBUG(MTD_DEBUG_LEVEL1, "%s: remove\n", chip->name);

	status = del_mtd_device(&chip->mtd);
	if (status == 0) {
		kfree(chip);
	}

	dev_set_drvdata(&pdev->dev, NULL);
	return status;
}

static struct platform_device *ubi32_nand_spi_er_device;

static struct platform_driver ubi32_nand_spi_er_driver = {
	.driver = {
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},

	.probe		= ubi32_nand_spi_er_probe,
	.remove		= ubi32_nand_spi_er_remove,
};

/*
 * ubi32_nand_spi_er_init
 */
static int __init ubi32_nand_spi_er_init(void)
{
	int ret;

	ret = platform_driver_register(&ubi32_nand_spi_er_driver);

	if (ret) {
		return ret;
	}

	ubi32_nand_spi_er_device = platform_device_alloc(DRIVER_NAME, 0);
	if (!ubi32_nand_spi_er_device) {
		return -ENOMEM;
	}

	ret = platform_device_add(ubi32_nand_spi_er_device);
	if (ret) {
		platform_device_put(ubi32_nand_spi_er_device);
		platform_driver_unregister(&ubi32_nand_spi_er_driver);
	}

	return ret;
}
module_init(ubi32_nand_spi_er_init);

/*
 * ubi32_nand_spi_er_exit
 */
static void __exit ubi32_nand_spi_er_exit(void)
{
	platform_device_unregister(ubi32_nand_spi_er_device);
	platform_driver_unregister(&ubi32_nand_spi_er_driver);
}
module_exit(ubi32_nand_spi_er_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Tjin");
MODULE_DESCRIPTION("MTD ubi32_nand_spi_er driver for ubicom32 SPI flash controller.");
