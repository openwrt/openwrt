#include <linux/types.h>
#include <linux/io.h>

#include <adm5120_defs.h>
#include <adm5120_switch.h>

/* CFRDY is connected to GPIO4/INTX_1 */
#define ADM5120_CF_GPIO_NUM     4
#define ADM5120_CF_IRQ_LEVEL_BIT        0x20    /* GPIO4 = 0x20, GPIO2 = 0x10 */
#define ADM5120_IRQ_CFRDY       5
#define EXTIO_WAIT_EN		(0x1 << 6)
#define EXTIO_CS1_INT1_EN	(0x1 << 5)
#define EXTIO_CS0_INT0_EN	(0x1 << 4)

struct cf_device {
        int gpio_pin;
        void *dev;
        struct gendisk *gd;
};
