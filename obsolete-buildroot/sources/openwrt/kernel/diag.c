// replacement diag module
// (c) 2004 openwrt 
// mbm at alt dot org
//
// initial release 2004/03/28

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sysctl.h>
#include <asm/io.h>
#include <typedefs.h>
#include <bcm4710.h>
#include <sbutils.h>

static void *sbh;

// v2.x - - - - -
#define DIAG_GPIO (1<<1)
#define DMZ_GPIO  (1<<7)

static void set_gpio(uint32 mask, uint32 value) {
	sb_gpiocontrol(sbh,mask,0);
	sb_gpioouten(sbh,mask,mask);
	sb_gpioout(sbh,mask,value);
}

static void v2_set_diag(u8 state) {
	set_gpio(DIAG_GPIO,state);
}
static void v2_set_dmz(u8 state) {
	set_gpio(DMZ_GPIO,state);
}

// v1.x - - - - -
#define LED_DIAG   0x13
#define LED_DMZ    0x12

static void v1_set_diag(u8 state) {
	if (!state) {
		*(volatile u8*)(KSEG1ADDR(BCM4710_EUART)+LED_DIAG)=0xFF;
	} else {
		*(volatile u8*)(KSEG1ADDR(BCM4710_EUART)+LED_DIAG);
	}
}
static void v1_set_dmz(u8 state) {
	if (!state) {
		*(volatile u8*)(KSEG1ADDR(BCM4710_EUART)+LED_DMZ)=0xFF;
	} else {
		*(volatile u8*)(KSEG1ADDR(BCM4710_EUART)+LED_DMZ);
	}
}

// - - - - -
static void ignore(u8 ignored) {};

// - - - - -
#define BIT_DMZ         0x01
#define BIT_DIAG        0x04

void (*set_diag)(u8 state);
void (*set_dmz)(u8 state);

static unsigned int diag = 0;

static void diag_change()
{
	printk(KERN_INFO "led -> %02x\n",diag);

	set_diag(0xFF); // off
	set_dmz(0xFF); // off

	if(diag & BIT_DIAG)
		set_diag(0x00); // on
	if(diag & BIT_DMZ)
		set_dmz(0x00); // on
}

static int proc_diag(ctl_table *table, int write, struct file *filp,
		void *buffer, size_t *lenp)
{
	int r;
	r = proc_dointvec(table, write, filp, buffer, lenp);
	if (write && !r) {
		diag_change();
	}
	return r;
}

// - - - - -
static unsigned char reset_gpio = 0;
static unsigned char reset_polarity = 0;
static unsigned int reset = 0;

static int proc_reset(ctl_table *table, int write, struct file *filp,
		void *buffer, size_t *lenp)
{

	if (reset_gpio) {
		sb_gpiocontrol(sbh,reset_gpio,reset_gpio);
		sb_gpioouten(sbh,reset_gpio,0);
		reset=!(sb_gpioin(sbh)&reset_gpio);

		if (reset_polarity) reset=!reset;
	} else {
		reset=0;
	}

	return proc_dointvec(table, write, filp, buffer, lenp);
}

// - - - - -
static struct ctl_table_header *diag_sysctl_header;

static ctl_table sys_diag[] = {
         { 
	   ctl_name: 2000,
	   procname: "diag", 
	   data: &diag,
	   maxlen: sizeof(diag), 
	   mode: 0644,
	   proc_handler: proc_diag
	 },
	 {
	   ctl_name: 2001,
	   procname: "reset",
	   data: &reset,
	   maxlen: sizeof(reset),
	   mode: 0444,
	   proc_handler: proc_reset 
	 },
         { 0 }
};

static int __init diag_init()
{
	char *buf;
	u32 board_type;
	sbh = sb_kattach();
	sb_gpiosetcore(sbh);

	board_type = sb_boardtype(sbh);
	printk(KERN_INFO "diag boardtype: %08x\n",board_type);

	set_diag=ignore;
	set_dmz=ignore;
	
	if (board_type & 0x400) {
		board_type=1;
		set_diag=v1_set_diag;
		set_dmz=v1_set_dmz;

		buf=nvram_get("boardtype")?:"";

		if (!strcmp(buf,"bcm94710dev")) {
			buf=nvram_get("boardnum")?:"";
			if (!strcmp(buf,"42")) {
				// wrt54g v1.x
				set_diag=v1_set_diag;
				set_dmz=v1_set_dmz;
				reset_gpio=(1<<6);
				reset_polarity=0;
			} else (!strcmp(buf,"asusX")) {
				//asus wl-500g
				//no leds
				reset_gpio=(1<<6);
				reset_polarity=1;
			}
		} else if (!strcmp(buf,"bcm94710ap")) {
			// buffalo
			set_diag=ignore;
			set_dmz=v2_set_dmz;
			reset_gpio=(1<<4);
			reset_polarity=1;
		}
	} else {
		board_type=2;
		set_diag=v2_set_diag;
		set_dmz=v2_set_dmz;
		reset_gpio=(1<<6);
		reset_polarity=0;
	}
	printk(KERN_INFO "using v%d hardware\n",board_type);

	diag_sysctl_header = register_sysctl_table(sys_diag, 0);
	diag_change();

	return 0;
}

static void __exit diag_exit()
{
	unregister_sysctl_table(diag_sysctl_header);
}

module_init(diag_init);
module_exit(diag_exit);
