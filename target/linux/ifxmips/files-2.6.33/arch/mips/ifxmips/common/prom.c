#include <linux/init.h>
#include <linux/module.h>
#include <linux/bootmem.h>
#include <linux/etherdevice.h>

#include <asm/bootinfo.h>
#include <machine.h>

#include <ifxmips.h>
#include <ifxmips_prom.h>

/* for Multithreading (APRP) on MIPS34K */
unsigned long physical_memsize;

void
prom_free_prom_memory(void)
{
}

extern unsigned char ifxmips_ethaddr[6];
int cmdline_mac = 0;

static int __init
ifxmips_set_ethaddr(char *str)
{
#define IS_HEX(x) \
	(((x >= '0' && x <= '9') || (x >= 'a' && x <= 'f') \
		|| (x >= 'A' && x <= 'F')) ? (1) : (0))
	int i;
	str = strchr(str, '=');
	if (!str)
		goto out;
	str++;
	if (strlen(str) != 17)
		goto out;
	for (i = 0; i < 6; i++) {
		if (!IS_HEX(str[3 * i]) || !IS_HEX(str[(3 * i) + 1]))
			goto out;
		if ((i != 5) && (str[(3 * i) + 2] != ':'))
			goto out;
		ifxmips_ethaddr[i] = simple_strtoul(&str[3 * i], NULL, 16);
	}
	if (is_valid_ether_addr(ifxmips_ethaddr))
		cmdline_mac = 1;
out:
	return 1;
}
__setup("ethaddr", ifxmips_set_ethaddr);

static void __init prom_detect_machtype(void)
{
	mips_machtype = IFXMIPS_MACH_EASY50712;
}

static void __init prom_init_cmdline(void)
{
	int argc = fw_arg0;
	char **argv = (char **) fw_arg1;
	char **envp = (char **) fw_arg2;

	int memsize = 16; /* assume 16M as default */
	int i;

	if (argc)
	{
		argv = (char **)KSEG1ADDR((unsigned long)argv);
		arcs_cmdline[0] = '\0';
		for (i = 1; i < argc; i++)
		{
			char *a = (char *)KSEG1ADDR(argv[i]);
			if (!argv[i])
				continue;
			if (strlen(arcs_cmdline) + strlen(a + 1) >= sizeof(arcs_cmdline))
			{
				early_printf("cmdline overflow, skipping: %s\n", a);
				break;
			}
			strcat(arcs_cmdline, a);
			strcat(arcs_cmdline, " ");
		}
		if (!*arcs_cmdline)
			strcpy(&(arcs_cmdline[0]),
				"console=ttyS0,115200 rootfstype=squashfs,jffs2");
	}
	envp = (char **)KSEG1ADDR((unsigned long)envp);
	while (*envp)
	{
		char *e = (char *)KSEG1ADDR(*envp);

		if (!strncmp(e, "memsize=", 8))
		{
			e += 8;
			memsize = simple_strtoul(e, NULL, 10);
		}
		envp++;
	}
	memsize *= 1024 * 1024;

	add_memory_region(0x00000000, memsize, BOOT_MEM_RAM);
}

void __init
prom_init(void)
{
	prom_detect_machtype();
	prom_init_cmdline();
}
