// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/compiler_attributes.h>
#include <linux/types.h>

#define BCM3384_VIPER_ALIAS_OFFSET	0x08000000UL
#define BCM3384_VIPER_UNCACHED_ALIAS	0xa0000000UL

#define ZEPHYR_RESET_VECTOR_VIPER	0x08000000UL
#define UART0_TX_FIFO			0xb4e00514UL

#define G2U_ZMIPS_BOOT_VECTOR		0xb51f8024UL
#define G2U_ZMIPS_BOOT_VECTOR_EN	0x00000008

#define PMC_TRIGGER			0xb3e010b4UL
#define PMC_BUSY			0xb3e010b8UL
#define PMC_STATUS			0xb3e01818UL
#define PMC_CMD0			0xb3e01c10UL
#define PMC_CMD1			0xb3e01c14UL
#define PMC_CMD2			0xb3e01c18UL
#define PMC_CMD3			0xb3e01c1cUL
#define PMC_CMD_POWER			0x00000081
#define PMC_TRIGGER_VALUE		0xa000afc8
#define PMC_STATUS_RESPONSE_READY	0x00000004
#define PMC_ZEPHYR_DEVICE		37
#define PMC_ZEPHYR_ZONE			0
#define PMC_POWER_OFF_ARG		0x00000100
#define PMC_POWER_ON_ARG		0x00000001
#define PMC_TIMEOUT			1000000

extern const u32 bcm3384_zephyr_trampoline[];
extern const u32 bcm3384_zephyr_trampoline_end[];

void __noreturn bcm3384_wrapper_main(void);

/*
 * This runs before Linux owns the CPU, from the Viper high-memory view of the
 * kernel image. Do not call normal kernel I/O helpers here: on MIPS they can
 * be emitted out-of-line at the linked low kernel address, which Viper cannot
 * safely execute during this split Viper/Zephyr entry path.
 */
static __always_inline u32 read32(unsigned long addr)
{
	return *(volatile u32 *)addr;
}

static __always_inline void write32(unsigned long addr, u32 val)
{
	*(volatile u32 *)addr = val;
}

static __always_inline void sync_mem(void)
{
	__asm__ __volatile__("sync" ::: "memory");
}

static __always_inline void delay(u32 count)
{
	while (count--)
		__asm__ __volatile__("" ::: "memory");
}

/*
 * Keep this file call-free on the Viper path.
 *
 * The stock bootloader executes this wrapper through the Viper high-memory
 * alias, while the kernel image is linked at the Zephyr low address. Any
 * helper that is not forced inline can become a normal jal to the low linked
 * address and fault before Zephyr is released. Therefore bcm3384_wrapper_main()
 * must be the only non-inline function in this file.
 */
void __noreturn bcm3384_wrapper_main(void)
{
	volatile u32 *trampoline_dst = (volatile u32 *)(BCM3384_VIPER_UNCACHED_ALIAS |
						       ZEPHYR_RESET_VECTOR_VIPER);
	const u32 *trampoline_src = (const u32 *)(BCM3384_VIPER_UNCACHED_ALIAS |
						 ((unsigned long)bcm3384_zephyr_trampoline & 0x1fffffff) |
						 BCM3384_VIPER_ALIAS_OFFSET);
	unsigned long trampoline_words = bcm3384_zephyr_trampoline_end -
					 bcm3384_zephyr_trampoline;

	/*
	 * GPL/eCos name: BCHP_G2U_REGS_GB_ZMIPS_BOOT_VECTOR, bit 3 en.
	 * eCos then power-cycles PMC logical 48, which maps to raw PMC
	 * device 37 zone 0 in the C6300BD firmware.
	 */
	for (u32 timeout = PMC_TIMEOUT; timeout; timeout--) {
		if (!read32(PMC_BUSY))
			goto power_off_busy_clear;
	}
	write32(UART0_TX_FIFO, '!');
	delay(128);
	goto park;

power_off_busy_clear:
	write32(PMC_CMD0, (1 << 16) | PMC_CMD_POWER);
	write32(PMC_CMD1, (PMC_ZEPHYR_DEVICE << 10) | PMC_ZEPHYR_ZONE);
	write32(PMC_CMD2, PMC_POWER_OFF_ARG);
	write32(PMC_CMD3, 0);
	write32(PMC_TRIGGER, PMC_TRIGGER_VALUE);
	sync_mem();

	for (u32 timeout = PMC_TIMEOUT; timeout; timeout--) {
		u32 status = read32(PMC_STATUS);

		if (status & PMC_STATUS_RESPONSE_READY) {
			write32(PMC_STATUS, status | PMC_STATUS_RESPONSE_READY);
			sync_mem();
			goto power_off_done;
		}
	}
	write32(UART0_TX_FIFO, '!');
	delay(128);
	goto park;

power_off_done:
	delay(100000);

	for (unsigned long i = 0; i < trampoline_words; i++)
		trampoline_dst[i] = trampoline_src[i];

	write32(G2U_ZMIPS_BOOT_VECTOR, G2U_ZMIPS_BOOT_VECTOR_EN);
	sync_mem();

	for (u32 timeout = PMC_TIMEOUT; timeout; timeout--) {
		if (!read32(PMC_BUSY))
			goto power_on_busy_clear;
	}
	write32(UART0_TX_FIFO, '?');
	delay(128);
	goto park;

power_on_busy_clear:
	write32(PMC_CMD0, (2 << 16) | PMC_CMD_POWER);
	write32(PMC_CMD1, (PMC_ZEPHYR_DEVICE << 10) | PMC_ZEPHYR_ZONE);
	write32(PMC_CMD2, PMC_POWER_ON_ARG);
	write32(PMC_CMD3, 0);
	write32(PMC_TRIGGER, PMC_TRIGGER_VALUE);
	sync_mem();

	for (u32 timeout = PMC_TIMEOUT; timeout; timeout--) {
		u32 status = read32(PMC_STATUS);

		if (status & PMC_STATUS_RESPONSE_READY) {
			write32(PMC_STATUS, status | PMC_STATUS_RESPONSE_READY);
			sync_mem();

			// Zephyr is now running, so we can stop Viper.
			goto park;
		}
	}
	write32(UART0_TX_FIFO, '?');
	delay(128);

park:
	while (1)
		__asm__ __volatile__("wait");
}
