/*
* prom.c 
**********************************************************************
* P . Sadik Oct 10, 2003
*
* Started change log
* idt_cpu_freq is make a kernel configuration parameter
* idt_cpu_freq is exported so that other modules can use it.
* Code cleanup
**********************************************************************
* P. Sadik Oct 20, 2003
*
* Removed NVRAM code from here, since they are already available under
* nvram directory.
* Added serial port initialisation.
**********************************************************************
**********************************************************************
* P. Sadik Oct 30, 2003
*
* Added reset_cons_port
**********************************************************************

  P.Christeas, 2005-2006
  Port to 2.6, add 2.6 cmdline parsing

*/

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/console.h>
#include <asm/bootinfo.h>
#include <linux/bootmem.h>
#include <linux/ioport.h>
#include <linux/blkdev.h>
#include <asm/rc32434/ddr.h>

#define PROM_ENTRY(x)   (0xbfc00000+((x)*8))
extern void __init setup_serial_port(void);

unsigned int idt_cpu_freq = 132000000;
EXPORT_SYMBOL(idt_cpu_freq);
char board_type[11];
EXPORT_SYMBOL(board_type);
unsigned int gpio_bootup_state = 0;
EXPORT_SYMBOL(gpio_bootup_state);


char mips_mac_address[18] = "08:00:06:05:40:01";
EXPORT_SYMBOL(mips_mac_address);

/* what to append to cmdline when button is [not] pressed */
#define GPIO_INIT_NOBUTTON ""
#define GPIO_INIT_BUTTON   " 2"

#ifdef CONFIG_MIKROTIK_RB500
unsigned soft_reboot = 0;
EXPORT_SYMBOL(soft_reboot);
#endif

#define SR_NMI			0x00180000      /* NMI */
#define SERIAL_SPEED_ENTRY	0x00000001

#ifdef CONFIG_REMOTE_DEBUG
extern int remote_debug;
#endif

extern unsigned long mips_machgroup;
extern unsigned long mips_machtype;

#define FREQ_TAG   "HZ="
#define GPIO_TAG   "gpio="
#define KMAC_TAG   "kmac="
#define MEM_TAG	   "mem="
#define BOARD_TAG  "board="
#define IGNORE_CMDLINE_MEM 1
#define DEBUG_DDR

void parse_soft_settings(unsigned *ptr, unsigned size);
void parse_hard_settings(unsigned *ptr, unsigned size);

void __init prom_setup_cmdline(void);

void __init prom_init(void)
{
	DDR_t ddr = (DDR_t) DDR_VirtualAddress; /* define the pointer to the DDR registers */
	phys_t memsize = 0-ddr->ddrmask;
	
	/* this should be the very first message, even before serial is properly initialized */
	prom_setup_cmdline();
	setup_serial_port();

	mips_machgroup = MACH_GROUP_MIKROTIK;
	soft_reboot = read_c0_status() & SR_NMI;
	pm_power_off = NULL;

	/*
	 * give all RAM to boot allocator,
	 * except for the first 0x400 and the last 0x200 bytes
	 */
	add_memory_region(ddr->ddrbase + 0x400, memsize - 0x600, BOOT_MEM_RAM);
}

void __init prom_free_prom_memory(void)
{
	/* No prom memory to free */
}

extern char _image_cmdline;
void __init prom_setup_cmdline(void){
	char cmd_line[CL_SIZE];
	char *cp;
	int prom_argc;
	char **prom_argv, **prom_envp;
	int i;
	
	prom_argc = fw_arg0;
	prom_argv = (char **) fw_arg1;
	prom_envp = (char **) fw_arg2;
	
	cp=cmd_line;
		/* Note: it is common that parameters start at argv[1] and not argv[0],
		however, our elf loader starts at [0] */
	for(i=0;i<prom_argc;i++){
		if (strncmp(prom_argv[i], FREQ_TAG, sizeof(FREQ_TAG) - 1) == 0) {
			idt_cpu_freq = simple_strtoul(prom_argv[i] + sizeof(FREQ_TAG) - 1, 0, 10);
			continue;
		}
#ifdef IGNORE_CMDLINE_MEM
		/* parses out the "mem=xx" arg */
		if (strncmp(prom_argv[i], MEM_TAG, sizeof(MEM_TAG) - 1) == 0) {
			continue;
		}
#endif
		if (i>0) *(cp++) = ' ';

		if (strncmp(prom_argv[i], BOARD_TAG, sizeof(BOARD_TAG) - 1) == 0) {
			strcpy(board_type, prom_argv[i] + sizeof(BOARD_TAG) -1);
		}
		if (strncmp(prom_argv[i], GPIO_TAG, sizeof(GPIO_TAG) - 1) == 0) {
			gpio_bootup_state =  simple_strtoul(prom_argv[i] + sizeof(GPIO_TAG) - 1, 0, 10);
		}
		strcpy(cp,prom_argv[i]);
		cp+=strlen(prom_argv[i]);
	}
	*(cp++) = ' ';
	strcpy(cp,(&_image_cmdline + 8));
	cp += strlen(&_image_cmdline);
	
	i=strlen(arcs_cmdline);
	if (i>0){
		*(cp++) = ' ';
		strcpy(cp,arcs_cmdline);
		cp+=strlen(arcs_cmdline);
	}
	if (gpio_bootup_state&0x02)
		strcpy(cp,GPIO_INIT_NOBUTTON);
	else
		strcpy(cp,GPIO_INIT_BUTTON);
	cmd_line[CL_SIZE-1] = '\0';
	
	strcpy(arcs_cmdline,cmd_line);
}

