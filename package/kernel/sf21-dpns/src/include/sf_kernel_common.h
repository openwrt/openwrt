#ifndef _SF_KERNEL_COMMON_H
#define _SF_KERNEL_COMMON_H

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/sysfs.h>


struct module_notes_attrs {
	struct kobject *dir;
	unsigned int notes;
    struct bin_attribute attrs[];
};

struct _ddebug {
    /*
	 * These fields are used to drive the user interface
	 * for selecting and displaying debug callsites.
	 */
    const char *modname;
    const char *function;
    const char *filename;
    const char *format;
    unsigned int lineno:18;
	atomic_t enabled;
} __attribute__((aligned(8)));


#define SF_DYNAMIC_DEBUG_BRANCH(descriptor) \
	unlikely(atomic_read(&descriptor.enabled) > 0)

#define SF_DEFINE_DYNAMIC_DEBUG_METADATA(name, fmt)        \
    static struct _ddebug  __aligned(8)         \
    __section(".note.sf_dyndbg") name = {              \
        .modname = KBUILD_MODNAME,          \
        .function = __func__,               \
        .filename = __FILE__,               \
        .format = (fmt),                \
        .lineno = __LINE__,             \
		.enabled = ATOMIC_INIT(0)		\
    }

#define sf_dbg_ratelimited(fmt, ...)			\
do {                                    \
	SF_DEFINE_DYNAMIC_DEBUG_METADATA(descriptor, fmt);     \
	if (SF_DYNAMIC_DEBUG_BRANCH(descriptor) &&			\
			net_ratelimit())					\
			printk(fmt, ##__VA_ARGS__);		\
} while (0)


extern int sf_dynamic_debug_init(struct module *mod);
extern int sf_dynamic_debug_deinit(struct module *mod);

#endif
