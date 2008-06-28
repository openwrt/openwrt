#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <asm/processor.h>

#include "cpu.h"

static struct cpu_dev rdc_cpu_dev __cpuinitdata = {
        .c_vendor       = "RDC",
        .c_models = {
                { .vendor = X86_VENDOR_RDC, .family = 4, .model_names =
                  {
                          [0] = "R861x(-G)",
                  }
                },
        },
};

int __init rdc_init_cpu(void)
{
        cpu_devs[X86_VENDOR_RDC] = &rdc_cpu_dev;
        return 0;
}
