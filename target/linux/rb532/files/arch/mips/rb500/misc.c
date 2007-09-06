#include <linux/module.h>
#include <linux/kernel.h>   /* printk() */
#include <linux/types.h>    /* size_t */
#include <linux/pci.h>
#include <linux/spinlock.h>
#include <asm/rc32434/rb.h>

#define GPIO_BADDR  0xb8050000


static volatile unsigned char *devCtl3Base = 0;
static unsigned char latchU5State = 0;
static spinlock_t clu5Lock = SPIN_LOCK_UNLOCKED;

void set434Reg(unsigned regOffs, unsigned bit, unsigned len, unsigned val) {
    unsigned flags, data;
    unsigned i = 0;
    spin_lock_irqsave(&clu5Lock, flags);
    data = *(volatile unsigned *) (IDT434_REG_BASE + regOffs);
    for (i = 0; i != len; ++i) {
	if (val & (1 << i)) data |= (1 << (i + bit));
	else data &= ~(1 << (i + bit));
    }
    *(volatile unsigned *) (IDT434_REG_BASE + regOffs) = data;
    spin_unlock_irqrestore(&clu5Lock, flags);
}

void changeLatchU5(unsigned char orMask, unsigned char nandMask) {
    unsigned flags;
    spin_lock_irqsave(&clu5Lock, flags);
    latchU5State = (latchU5State | orMask) & ~nandMask;
    if( !devCtl3Base) devCtl3Base = (volatile unsigned char *)
	        KSEG1ADDR(*(volatile unsigned *) KSEG1ADDR(0x18010030));
    *devCtl3Base = latchU5State;
    spin_unlock_irqrestore(&clu5Lock, flags);
}

u32 gpio_get(gpio_func func)
{
	return readl((void *) GPIO_BADDR + func);
}

void gpio_set(gpio_func func, u32 mask, u32 value)
{
	u32 val = readl((void *) GPIO_BADDR + func);
	
	val &= ~mask;
	val |= value & mask;
	
	writel(val, (void *) GPIO_BADDR + func);
}

EXPORT_SYMBOL(gpio_set);
EXPORT_SYMBOL(gpio_get);
EXPORT_SYMBOL(set434Reg);
EXPORT_SYMBOL(changeLatchU5);
