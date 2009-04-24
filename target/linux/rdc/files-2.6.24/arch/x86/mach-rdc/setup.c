/*
 *	Machine specific setup for generic
 */

#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/arch_hooks.h>
#include <asm/io.h>
#include <asm/setup.h>

char * __init machine_specific_memory_setup(void)
{
	return "RDC R-321x";
}
