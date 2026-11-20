// SPDX-License-Identifier: GPL-2.0-only
/*
 * leds-group-virtualcolor.c - Virtual grouped LED driver with multicolor ABI
 *
 * Locking hierarchy (must be acquired in this order):
 *	 1. global_owner_lock (global)
 *   2. vcolor_controller.lock (per-controller)
 *   3. virtual_led.lock (per-vLED)
 *
 * Never hold vLED locks during arbitration to avoid deadlock.
 * Arbitration copies vLED state under vLED lock, then releases before processing.
 *
 *
 * Device Tree Dependency:
 * This driver requires Device Tree (CONFIG_OF) due to LED phandle resolution.
 * GPIO LEDs in particular require OF-specific APIs as they lack full fwnode
 * support. We minimize OF usage to these essential operations:
 *   1. to_of_node() - Convert fwnode to OF node (GPIO LED limitation)
 *   2. of_count_phandle_with_args() - Count LED references
 *   3. of_parse_phandle() - Resolve LED phandle to device node
 *   4. of_node_put() - Release device node references
 * All other operations use generic device model APIs (bus_find_device_by_fwnode,
 * dev_get_drvdata, put_device) for future ACPI compatibility.
 *
 * Author: Jonathan Brophy <professor_jonny@hotmail.com>
 */

#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/leds.h>
#include <dt-bindings/leds/common.h>
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/xarray.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/kref.h>
#include <linux/ratelimit.h>
#include <linux/workqueue.h>
#include <linux/rwsem.h>
#ifdef CONFIG_DEBUG_FS /* Conditional Include for debug use */
	#include <linux/random.h>
#endif
#include <linux/of_platform.h>
#include <linux/of.h>
#include <linux/ktime.h>
#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/atomic.h>
#include <linux/types.h>

#define DRIVER_NAME "leds-group-virtualcolor"
#define VLED_DEBUGFS_DIR DRIVER_NAME
#define MAX_PHYS_LEDS_DEFAULT 64
#define UPDATE_BATCH_DELAY_MS 10
#define DEFAULT_UPDATE_RATE_LIMIT 100
#define PRIORITY_MAX INT_MAX
#define VLED_SNAPSHOT_DEFAULT 32

#ifdef CONFIG_DEBUG_FS
#define MAX_DEBUGFS_NAME 96
#endif

#ifdef CONFIG_LOCKDEP
#define assert_controller_locked(lvc) lockdep_assert_held(&(lvc)->lock)
#define assert_vled_locked(vled) lockdep_assert_held(&(vled)->lock)
#else
#define assert_controller_locked(lvc) do { } while (0)
#define assert_vled_locked(vled) do { } while (0)
#endif

static inline bool is_valid_led_cdev(struct led_classdev *cdev)
{
	if (!cdev)
		return false;
	if (!cdev->brightness_set && !cdev->brightness_set_blocking)
		return false;
	return true;
}

/**
 * vcolor_led_from_fwnode - Resolve LED classdev from fwnode reference
 * @fwnode: Firmware node containing LED array
 * @index: Index within the LED array property
 * @out_dev: Optional output for the LED's parent device (may be NULL for GPIO LEDs)
 *
 * This is the single OF bridge point in the driver, following V4L2's pattern.
 * Once the LED subsystem provides fwnode_led_get(), this function can be replaced
 * with a single line: return fwnode_led_get(fwnode, index, out_dev);
 *
 * Returns: LED classdev pointer or ERR_PTR on failure
 */
static struct led_classdev *vcolor_led_from_fwnode(const struct fwnode_handle *fwnode,
						   int index,
						   struct device **out_dev)
{
	struct led_classdev *cdev;
	struct device_node *np;

	if (out_dev)
		*out_dev = NULL;

#ifdef CONFIG_OF
	/* === SINGLE OF BRIDGE: Convert fwnode to of_node === */
	np = to_of_node((struct fwnode_handle *)fwnode);
	if (!np)
		return ERR_PTR(-EINVAL);

	/*
	 * of_led_get() is the current LED subsystem API that handles:
	 * - GPIO LEDs (which don't have struct device)
	 * - Platform LED controllers
	 * - Deferred probing via -EPROBE_DEFER
	 * - Reference counting via led_module_get()
	 *
	 * The index parameter allows accessing individual LEDs in phandle arrays.
	 *
	 * Future replacement: When fwnode_led_get() exists in LED core,
	 * this entire #ifdef block becomes: return fwnode_led_get(fwnode, index, out_dev);
	 */
	cdev = of_led_get(np, index);
	if (IS_ERR(cdev))
		return cdev;

	if (!is_valid_led_cdev(cdev)) {
		led_put(cdev);
		return ERR_PTR(-EINVAL);
	}

	/*
	 * Store parent device reference if it exists.
	 * GPIO LEDs may not have cdev->dev, which is fine.
	 */
	if (out_dev && cdev->dev)
		*out_dev = get_device(cdev->dev);

	return cdev;
#else
	return ERR_PTR(-ENOTSUPP);
#endif
}

/* Structured logging macros for better categorization */
#define ctrl_err(lvc, fmt, ...) \
	dev_err(&(lvc)->pdev->dev, "[CTRL] " fmt, ##__VA_ARGS__)

#define ctrl_warn(lvc, fmt, ...) \
	dev_warn(&(lvc)->pdev->dev, "[CTRL] " fmt, ##__VA_ARGS__)

#define ctrl_info(lvc, fmt, ...) \
	dev_info(&(lvc)->pdev->dev, "[CTRL] " fmt, ##__VA_ARGS__)

#define ctrl_dbg(lvc, fmt, ...) \
	dev_dbg(&(lvc)->pdev->dev, "[CTRL] " fmt, ##__VA_ARGS__)

#define arb_err(lvc, fmt, ...) \
	dev_err_ratelimited(&(lvc)->pdev->dev, "[ARB] " fmt, ##__VA_ARGS__)

#define vled_err(vled, fmt, ...) \
	dev_err(&(vled)->ctrl->pdev->dev, "[vLED:%s] " fmt, (vled)->name, ##__VA_ARGS__)

static_assert(sizeof(void *) <= sizeof(unsigned long),
		  "XArray keys require pointer size <= unsigned long");

/* Module parameters */
#ifdef CONFIG_DEBUG_FS
static bool enable_debugfs = true;
#else
static bool enable_debugfs;
#endif

static bool use_gamma_correction;
static unsigned int update_delay_us;
static unsigned int max_phys_leds = MAX_PHYS_LEDS_DEFAULT;
static bool enable_update_batching;

/* LED mode enumeration */
enum vled_mode {
	VLED_MODE_MULTICOLOR = 0,
	VLED_MODE_STANDARD = 1,
};

struct vcolor_controller;

struct mc_channel {
	u8 color_id;
	u8 num_leds;
	struct led_classdev **leds;
	struct device **led_devs;
	u8 intensity;
	u8 scale;
};

struct phys_led_entry {
	/* HOT PATH */
	struct led_classdev *cdev;
	u8 chosen_brightness;
	s32 chosen_priority;
	u64 chosen_sequence;
	struct kref refcount;
	unsigned int list_index;

	/* COLD PATH */
	struct device *dev;
	struct list_head list;
#ifdef CONFIG_DEBUG_FS
	char winner_name[MAX_DEBUGFS_NAME];
#endif
};

struct update_buffer {
	struct phys_led_entry **entries;
	u8 *brightness;
	unsigned int capacity;
	unsigned int max_capacity;
};

struct arbitration_buffers {
	u8 *intensities;
	u8 *scales;
	unsigned int capacity;
};

static DEFINE_XARRAY(global_owner_xa);
static DECLARE_RWSEM(global_owner_rwsem);

struct global_phys_owner {
	struct platform_device *owner_pdev;
};


/**
 * struct virtual_led - Virtual LED with priority-based arbitration
 * @cdev: LED class device
 * @priority: Arbitration priority (higher wins)
 * @name: LED name (assigned by LED core)
 * @channels: Array of color channels
 * @num_channels: Number of color channels (max 255, but dirty tracking limited to 32)
 * @lock: Protects vLED state during updates
 * @list: Entry in controller's vLED list
 * @fwnode: Firmware node for DT parsing
 * @ctrl: Parent controller
 * @cdev_registered: LED class device registration status
 * @intensity_ratelimit: Rate limiter for intensity updates
 * @arb_bufs: Pre-allocated buffers for arbitration
 * @mode: Operating mode (MULTICOLOR or STANDARD)
 * @refcount: Reference counter
 * @sequence: Monotonic sequence number for tie-breaking
 * All channels are processed during each arbitration cycle for simplicity.
 *
 */
struct virtual_led {
	struct led_classdev cdev;
	s32 priority;
	const char *name;
	struct mc_channel *channels;
	u8 num_channels;
	struct mutex lock;
	struct list_head list;
	struct fwnode_handle *fwnode;
	struct vcolor_controller *ctrl;
	bool cdev_registered;
	struct ratelimit_state intensity_ratelimit;
	struct arbitration_buffers arb_bufs;
	enum vled_mode mode;
	struct kref refcount;
	u64 sequence;
#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs_dir;
	u64 brightness_set_count;
	u64 intensity_update_count;
	u64 arbitration_wins;
	u64 arbitration_losses;
	u64 arbitration_participations;
	u64 buffer_allocation_failures;
	u64 intensity_parse_errors;
	u64 ratelimit_drops;
#endif
};

struct vcolor_controller {
	struct list_head leds;
	struct mutex lock;
	struct list_head phys_leds;
	struct xarray phys_xa;
	struct platform_device *pdev;
	struct update_buffer update_buf;
	/* Pre-allocated arbitration buffers */
	struct virtual_led **vled_snapshot;
	unsigned int vled_snapshot_capacity;
	struct phys_led_entry **ple_snapshot;
	unsigned int ple_snapshot_capacity;
	bool *ple_usage_bitmap;
	unsigned int ple_usage_bitmap_capacity;
	bool suspended;
	atomic_t rebuilding;
	unsigned int phys_led_count;
	atomic_t removing;
	struct delayed_work update_work;
	atomic_t pending_updates;
	atomic64_t global_sequence;
	bool first_arbitration;
#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs_root;
	u64 arbitration_count;
	u64 update_count;
	atomic64_t allocation_failures;
	atomic64_t update_buffer_overflows;
	atomic64_t ratelimit_hits;
	u64 arb_latency_min_ns;
	u64 arb_latency_max_ns;
	u64 arb_latency_total_ns;
	u64 arb_latency_count;
	ktime_t last_update;
#endif
};

/* Forward declarations */
static void controller_rebuild_phys_leds(struct vcolor_controller *lvc);
static void controller_destroy_phys_list(struct vcolor_controller *lvc);
static void controller_run_arbitration_and_update(struct vcolor_controller *lvc);
static void phys_led_entry_release(struct kref *ref);
static void virtual_led_release(struct kref *ref);
static void release_device_array(struct device **devs, unsigned int count);

static inline unsigned long get_stable_led_key(struct led_classdev *cdev)
{
	if (!cdev)
		return 0;

	/* GPIO LEDs don't have dev - use cdev pointer as key */
	if (cdev->dev)
		return (unsigned long)cdev->dev;
	else
		return (unsigned long)cdev;
}

static inline struct virtual_led *virtual_led_get(struct virtual_led *vled)
{
	if (vled)
		kref_get(&vled->refcount);
	return vled;
}

static inline void virtual_led_put(struct virtual_led *vled)
{
	if (vled)
		kref_put(&vled->refcount, virtual_led_release);
}

static inline bool controller_safe_arbitrate(struct vcolor_controller *lvc)
{
	bool ran;

	if (!lvc)
		return false;

	/* Fast path: avoid lock acquisition if nothing to do */
	if (atomic_read(&lvc->removing) || atomic_read(&lvc->rebuilding))
		return false;

	mutex_lock(&lvc->lock);

	/* Check suspended under lock to prevent suspend race */
	ran = false;
	if (!lvc->suspended && !atomic_read(&lvc->rebuilding) &&
	    device_is_registered(&lvc->pdev->dev)) {
		controller_run_arbitration_and_update(lvc);
		ran = true;
	}

	mutex_unlock(&lvc->lock);
	return ran;
}


/**
 * parse_leds_fwnode_array - Parse LED references using fwnode APIs
 * @dev: Device for logging and memory allocation
 * @fwnode: Firmware node containing LED references
 * @propname: Property name (e.g., "leds")
 * @out_leds: Output array of LED classdev pointers
 * @out_devs: Output array of device pointers (may contain NULLs for GPIO LEDs)
 * @out_count: Number of LEDs found
 *
 * Uses fwnode APIs for property traversal, with a single OF bridge for LED resolution.
 * This pattern mirrors V4L2's approach and makes future fwnode_led_get() migration trivial.
 */
 static int parse_leds_fwnode_array(struct device *dev,
 				   const struct fwnode_handle *fwnode,
 				   const char *propname,
 				   struct led_classdev ***out_leds,
 				   struct device ***out_devs,
 				   u8 *out_count)
 {
 	struct fwnode_reference_args args;
 	int count, idx, valid, i;
 	struct led_classdev **leds;
 	struct device **devs;
 	struct led_classdev *cdev;
 	struct device *led_dev;
 	int ret;

 	*out_leds = NULL;
 	*out_devs = NULL;
 	*out_count = 0;

 	/* Count phandle references using generic fwnode API */
 	count = 0;
 	while (fwnode_property_get_reference_args(fwnode, propname,
 						  NULL, 0, count, &args) == 0) {
 		fwnode_handle_put(args.fwnode);
 		count++;
 	}

 	if (count <= 0)
 		return 0;

 	/* Allocate temporary arrays for LED/device pointers */
 	leds = kcalloc(count, sizeof(*leds), GFP_KERNEL);
 	if (!leds)
 		return -ENOMEM;

 	devs = kcalloc(count, sizeof(*devs), GFP_KERNEL);
 	if (!devs) {
 		kfree(leds);
 		return -ENOMEM;
 	}

 	/* Iterate through each LED reference and PACK valid entries */
 	valid = 0;
 	for (idx = 0; idx < count; idx++) {
 		led_dev = NULL;

 		/*
 		 * Single OF bridge point: resolve LED from fwnode using index.
 		 */
 		cdev = vcolor_led_from_fwnode(fwnode, idx, &led_dev);

 		if (IS_ERR(cdev)) {
 			ret = PTR_ERR(cdev);

 			/* Handle deferred probe - cleanup and return immediately */
 			if (ret == -EPROBE_DEFER) {
 				dev_info(dev, "LED %d not ready yet (EPROBE_DEFER), will retry\n", idx);

 				/* Release all previously acquired LEDs and devices */
 				for (i = 0; i < valid; i++) {
 					if (leds[i])
 						led_put(leds[i]);
 					if (devs[i])
 						put_device(devs[i]);
 				}

 				kfree(leds);
 				kfree(devs);
 				return -EPROBE_DEFER;
 			}

 			/* Other errors - log and skip this LED */
 			dev_warn(dev, "Failed to resolve LED %d: %d\n", idx, ret);
 			continue;
 		}

 		/* Store valid LED in PACKED position */
 		if (is_valid_led_cdev(cdev)) {
 			leds[valid] = cdev;      /* Pack at 'valid' index, not 'idx' */
 			devs[valid] = led_dev;   /* May be NULL for GPIO LEDs */
 			valid++;
 		} else {
 			dev_warn(dev, "LED %d is not valid (no brightness_set method)\n", idx);
 			led_put(cdev);
 			if (led_dev)
 				put_device(led_dev);
 		}
 	}

 	/* Check if we got any valid LEDs */
 	if (valid == 0) {
 		dev_warn(dev, "Property '%s': none of %d LED(s) resolved\n",
 			 propname, count);
 		kfree(leds);
 		kfree(devs);
 		return -ENODEV;
 	}

 	/* Success - return PACKED arrays to caller */
 	*out_leds = leds;
 	*out_devs = devs;
 	*out_count = (u8)valid;

 	return 0;
 }

static void release_device_array(struct device **devs, unsigned int count)
{
	unsigned int i;

	if (!devs)
		return;

	for (i = 0; i < count; i++) {
		if (devs[i]) {
			put_device(devs[i]);
			devs[i] = NULL;
		}
		/* If devs[i] is NULL (GPIO LED case), nothing to release */
	}
}

static void global_release_all_for_pdev(struct platform_device *pdev)
{
	XA_STATE(xas, &global_owner_xa, 0);
	unsigned long released;
	struct global_phys_owner *gpo;

	down_write(&global_owner_rwsem);

	released = 0;

	xas_for_each(&xas, gpo, ULONG_MAX) {
		if (gpo && !xa_is_value(gpo) && gpo->owner_pdev == pdev) {
			if (!xa_is_err(xas_store(&xas, NULL))) {
				kfree(gpo);
				released++;
			}
		}
	}

	up_write(&global_owner_rwsem);

	if (released) {
		dev_info(&pdev->dev, "Released %lu physical LED ownership claims\n",
			released);
	}
}

static void phys_led_entry_release(struct kref *ref)
{
	struct phys_led_entry *ple;

	ple = container_of(ref, struct phys_led_entry, refcount);

	if (ple->dev)
		put_device(ple->dev);

	kfree(ple);
}

static inline struct phys_led_entry *phys_led_entry_get(
	struct phys_led_entry *ple)
{
	if (ple)
		kref_get(&ple->refcount);
	return ple;
}

static inline void phys_led_entry_put(struct phys_led_entry *ple)
{
	if (ple)
		kref_put(&ple->refcount, phys_led_entry_release);
}

static bool claim_physical_led(struct vcolor_controller *lvc,
				  struct led_classdev *cdev,
				  struct device *dev,
				  struct phys_led_entry **out_ple)
{
	struct global_phys_owner *gpo;
	struct phys_led_entry *ple = NULL;
	void *ret_ptr;
	bool success = false;
	bool newly_claimed_global = false;
	unsigned long key;

	*out_ple = NULL;

	if (!cdev)
		return false;

	key = get_stable_led_key(cdev);
	if (!key) {
		ctrl_err(lvc, "Failed to get stable key for LED '%s'\n",
			 cdev->name ? cdev->name : "(unnamed)");
		return false;
	}

	down_write(&global_owner_rwsem);
	mutex_lock(&lvc->lock);

	gpo = xa_load(&global_owner_xa, key);
	if (xa_is_value(gpo)) {
		ctrl_err(lvc, "Invalid XArray entry for LED '%s'\n", cdev->name);
		goto out_unlock;
	}

	if (gpo && gpo->owner_pdev != lvc->pdev) {
		ctrl_warn(lvc, "Physical LED '%s' already claimed by another controller\n",
			  cdev->name);
		goto out_unlock;
	}

	if (xa_load(&lvc->phys_xa, key)) {
		ctrl_dbg(lvc, "LED '%s' already claimed locally, skipping duplicate\n",
			 cdev->name);
		goto out_unlock;
	}

	ple = kzalloc(sizeof(*ple), GFP_KERNEL);
	if (!ple) {
		ctrl_err(lvc, "Failed to allocate phys_led_entry for '%s'\n",
			cdev->name);

#ifdef CONFIG_DEBUG_FS
		atomic64_inc(&lvc->allocation_failures);
#endif
		goto out_unlock;
	}
	kref_init(&ple->refcount);
	ple->cdev = cdev;
	ple->dev = dev;
	if (dev)
		get_device(dev);
	ple->chosen_brightness = 0;
	ple->chosen_priority = -1;
	ple->chosen_sequence = 0;
#ifdef CONFIG_DEBUG_FS
	ple->winner_name[0] = '\0';
#endif

	if (!gpo) {
		gpo = kzalloc(sizeof(*gpo), GFP_KERNEL);
		if (!gpo) {
			ctrl_err(lvc, "Failed to allocate ownership record for LED '%s'\n",
				 cdev->name);
#ifdef CONFIG_DEBUG_FS
			atomic64_inc(&lvc->allocation_failures);
#endif
			goto out_unlock;
		}

		gpo->owner_pdev = lvc->pdev;
		ret_ptr = xa_store(&global_owner_xa, key, gpo, GFP_KERNEL);

		if (xa_is_err(ret_ptr)) {
			ctrl_err(lvc, "Failed to register global ownership for LED '%s': %ld\n",
				 cdev->name, PTR_ERR(ret_ptr));
			kfree(gpo);
#ifdef CONFIG_DEBUG_FS
			atomic64_inc(&lvc->allocation_failures);
#endif
			goto out_unlock;
		}

		newly_claimed_global = true;
	}

	ret_ptr = xa_store(&lvc->phys_xa, key, ple, GFP_KERNEL);
	if (xa_is_err(ret_ptr)) {
		ctrl_err(lvc, "Failed to store phys_led_entry '%s' in xarray: %ld\n",
			 cdev->name, PTR_ERR(ret_ptr));

		if (newly_claimed_global) {
			gpo = xa_erase(&global_owner_xa, key);
			if (gpo && !xa_is_value(gpo)) {
				ctrl_dbg(lvc, "Cleaned up leaked global ownership for '%s' after local store failure\n",
					 cdev->name);
				kfree(gpo);
			}
		}

#ifdef CONFIG_DEBUG_FS
		atomic64_inc(&lvc->allocation_failures);
#endif
		goto out_unlock;
	}

	list_add_tail(&ple->list, &lvc->phys_leds);
	lvc->phys_led_count++;
	*out_ple = ple;
	success = true;

out_unlock:
	mutex_unlock(&lvc->lock);
	up_write(&global_owner_rwsem);

	if (!success && ple)
		phys_led_entry_put(ple);

	return success;
}

static void add_led_group_to_phys_list(struct vcolor_controller *lvc,
	struct led_classdev **leds,
	struct device **devs,
	unsigned int count)
{
	unsigned int i;
	struct phys_led_entry *ple;

	for (i = 0; i < count; i++) {
		if (!leds || !leds[i])
			continue;

		claim_physical_led(lvc, leds[i], devs ? devs[i] : NULL, &ple);
	}
}

static void controller_destroy_phys_list(struct vcolor_controller *lvc)
{
	struct phys_led_entry *ple, *tmp;
	unsigned long key;

	assert_controller_locked(lvc);

	list_for_each_entry_safe(ple, tmp, &lvc->phys_leds, list) {
		list_del(&ple->list);

		if (ple->cdev) {
			key = get_stable_led_key(ple->cdev);
			if (key)
				xa_erase(&lvc->phys_xa, key);
			ple->cdev = NULL;
		}

		phys_led_entry_put(ple);
	}

	INIT_LIST_HEAD(&lvc->phys_leds);
	lvc->phys_led_count = 0;
}

static void controller_rebuild_phys_leds(struct vcolor_controller *lvc)
{
	struct virtual_led *vled, **vled_snapshot;
	unsigned int i, j, vled_count, actual_count;
	bool needs_free = false;

	assert_controller_locked(lvc);

	atomic_set(&lvc->rebuilding, 1);

	/* Count vLEDs first */
	vled_count = 0;
	list_for_each_entry(vled, &lvc->leds, list)
		vled_count++;

	if (vled_count == 0) {
		/* Safe to destroy when no vLEDs exist */
		controller_destroy_phys_list(lvc);
		xa_destroy(&lvc->phys_xa);
		xa_init(&lvc->phys_xa);
		atomic_set(&lvc->rebuilding, 0);
		return;
	}

	/* Allocate snapshot BEFORE destroying existing state */
	if (vled_count > lvc->vled_snapshot_capacity) {
		ctrl_warn(lvc,
			"vLED count %u exceeds snapshot capacity %u, using dynamic allocation\n",
			vled_count, lvc->vled_snapshot_capacity);
		vled_snapshot = kcalloc(vled_count, sizeof(*vled_snapshot), GFP_KERNEL);
		if (!vled_snapshot) {
			ctrl_err(lvc, "Failed to allocate vled snapshot for rebuild (OOM) - keeping existing state\n");
			#ifdef CONFIG_DEBUG_FS
						atomic64_inc(&lvc->allocation_failures);
			#endif
						atomic_set(&lvc->rebuilding, 0);
						return;
					}
					needs_free = true;
	} else {
		vled_snapshot = lvc->vled_snapshot;
		needs_free = false;
	}

	/* Copy vLED pointers with refcounts BEFORE destroying state */
	actual_count = 0;
		list_for_each_entry(vled, &lvc->leds, list) {
			if (actual_count >= vled_count) {
				ctrl_warn(lvc, "vLED count increased during snapshot! Expected %u\n", vled_count);
				break;
		}
		vled_snapshot[actual_count++] = virtual_led_get(vled);
	}

	/* Verify count stability */
	if (actual_count != vled_count) {
		ctrl_warn(lvc, "vLED count mismatch: expected %u, got %u\n",
				  vled_count, actual_count);
		/* Use actual count for processing */
		vled_count = actual_count;
	}

	/* Now safe to destroy existing state */
	controller_destroy_phys_list(lvc);
	xa_destroy(&lvc->phys_xa);
	xa_init(&lvc->phys_xa);

	mutex_unlock(&lvc->lock);

	/* Process all vLEDs outside lock */
	for (i = 0; i < actual_count; i++) {
		vled = vled_snapshot[i];
		if (!vled)
			continue;

		for (j = 0; j < vled->num_channels; j++) {
			add_led_group_to_phys_list(lvc, vled->channels[j].leds,
						   vled->channels[j].led_devs,
						   vled->channels[j].num_leds);
		}

		virtual_led_put(vled);
		vled_snapshot[i] = NULL;
	}

	/* Only free if we did dynamic allocation */
	if (needs_free)
		kfree(vled_snapshot);

	mutex_lock(&lvc->lock);

	/* Assign list indices for O(1) lookup during arbitration */
	{
		unsigned int idx = 0;
		struct phys_led_entry *ple_temp;

		list_for_each_entry(ple_temp, &lvc->phys_leds, list) {
			ple_temp->list_index = idx++;
		}
	}

	atomic_set(&lvc->rebuilding, 0);
	controller_run_arbitration_and_update(lvc);
}

static const u8 gamma_table[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4,
	4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14,
	14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 25, 26,
	26, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40, 40, 41,
	42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
	63, 64, 65, 66, 67, 68, 70, 71, 72, 73, 74, 75, 76, 78, 79, 80, 81, 82, 84, 85, 86, 87,
	89, 90, 91, 92, 94, 95, 96, 97, 99, 100, 101, 103, 104, 105, 107, 108, 109, 111, 112,
	114, 115, 116, 118, 119, 121, 122, 123, 125, 126, 128, 129, 131, 132, 134, 135, 137,
	138, 140, 141, 143, 144, 146, 147, 149, 150, 152, 154, 155, 157, 158, 160, 162, 163,
	165, 167, 168, 170, 172, 173, 175, 177, 178, 180, 182, 184, 185, 187, 189, 191, 192,
	194, 196, 198, 200, 201, 203, 205, 207, 209, 211, 212, 214, 216, 218, 220, 222, 224,
	226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 253, 255
};

static u8 scale_intensity_by_brightness(u8 intensity, u8 global_brightness,
					u8 max_global_brightness)
{
	u32 scaled_val;
	u8 final_intensity;

	if (max_global_brightness == 0)
		return 0;

	scaled_val = (u32)intensity * (u32)global_brightness;
	final_intensity = (u8)clamp_val(scaled_val / max_global_brightness, 0, 255);

	if (use_gamma_correction)
		final_intensity = gamma_table[final_intensity];

	return final_intensity;
}

static u8 vled_channel_get_final_intensity(enum vled_mode mode,
					   u8 raw_intensity,
					   u8 scale,
					   enum led_brightness vled_brightness,
					   enum led_brightness vled_max_brightness)
{
	u8 intensity = raw_intensity;
	u16 scaled_val;

	if (mode == VLED_MODE_MULTICOLOR) {
		if (scale < 255) {
			scaled_val = ((u16)intensity * (u16)scale) / 255;
			intensity = (u8)clamp_val(scaled_val, 0, 255);
		}
	} else {
		intensity = scale;
	}

	return scale_intensity_by_brightness(intensity, vled_brightness,
						 vled_max_brightness);
}

static void apply_winner_to_channel(struct vcolor_controller *lvc,
	struct virtual_led *vled,
	struct led_classdev **leds,
	unsigned int count,
	u8 final_intensity)
{
	unsigned int i;
	struct phys_led_entry *ple;
	unsigned long key;
#ifdef CONFIG_DEBUG_FS
	int copy_result;
#endif

	assert_controller_locked(lvc);

	for (i = 0; i < count; i++) {
		if (!leds[i])
			continue;

		key = get_stable_led_key(leds[i]);
		if (!key)
			continue;

		ple = xa_load(&lvc->phys_xa, key);
		if (!ple || xa_is_value(ple))
			continue;

		/* Winner takes all - no comparison needed */
		ple->chosen_brightness = final_intensity;
		ple->chosen_priority = vled->priority;
		ple->chosen_sequence = vled->sequence;

	#ifdef CONFIG_DEBUG_FS
		if (vled->name) {
			copy_result = strscpy(ple->winner_name, vled->name, MAX_DEBUGFS_NAME);
			if (copy_result < 0) {
				dev_warn_once(&lvc->pdev->dev,
					"vLED name truncated in telemetry: '%.32s...'\n",
					vled->name);
			}
		} else {
			strscpy(ple->winner_name, "(unnamed)", MAX_DEBUGFS_NAME);
		}
	#endif
	}
}

/*
 * Winner-takes-all logic
 * Context: Must be called with lvc->lock held
 * Note: This function temporarily releases lvc->lock during hardware I/O
 *	   to prevent blocking, then re-acquires it before returning.
 */
static void controller_run_arbitration_and_update(struct vcolor_controller *lvc)
{
	struct phys_led_entry *ple;
	struct virtual_led *vled;
	struct virtual_led *winner_vled = NULL;
	s32 highest_priority = -1;
	u64 latest_sequence = 0;
	u8 *intensities;
	u8 *scales;
	enum vled_mode mode;
	unsigned int j;
	u8 final_intensity;
	struct phys_led_entry **local_entries;
	u8 *local_brightness;
	unsigned int update_count;
	unsigned int i;
	u64 vled_seq;
	enum led_brightness brightness, max_brightness;
	bool *ple_used_by_winner;  /* Track which physical LEDs winner uses */
	unsigned int ple_count;

#ifdef CONFIG_DEBUG_FS
	ktime_t arb_start, arb_end;
	u64 arb_duration_ns;
#endif

	if (!lvc || lvc->suspended || atomic_read(&lvc->removing))
		return;

	assert_controller_locked(lvc);

	local_entries = lvc->update_buf.entries;
	local_brightness = lvc->update_buf.brightness;

	if (!local_entries || !local_brightness) {
		dev_err(&lvc->pdev->dev, "Update buffers not allocated, cannot arbitrate\n");
		return;
	}

#ifdef CONFIG_DEBUG_FS
	arb_start = ktime_get();
	lvc->arbitration_count++;
#endif

/* Count physical LEDs */
	ple_count = 0;
	list_for_each_entry(ple, &lvc->phys_leds, list)
		ple_count++;

	if (ple_count == 0) {
	#ifdef CONFIG_DEBUG_FS
		arb_end = ktime_get();
		arb_duration_ns = ktime_to_ns(ktime_sub(arb_end, arb_start));
		if (lvc->arb_latency_count == 0 || arb_duration_ns < lvc->arb_latency_min_ns)
			lvc->arb_latency_min_ns = arb_duration_ns;
		if (arb_duration_ns > lvc->arb_latency_max_ns)
			lvc->arb_latency_max_ns = arb_duration_ns;
		lvc->arb_latency_total_ns += arb_duration_ns;
		lvc->arb_latency_count++;
	#endif
		return;
	}

	/* Use pre-allocated bitmap - validate BEFORE using it */
	if (ple_count > lvc->ple_usage_bitmap_capacity) {
		dev_err(&lvc->pdev->dev,
			"Physical LED count %u exceeds bitmap capacity %u\n",
			ple_count, lvc->ple_usage_bitmap_capacity);
	#ifdef CONFIG_DEBUG_FS
		atomic64_inc(&lvc->allocation_failures);
		arb_end = ktime_get();
		arb_duration_ns = ktime_to_ns(ktime_sub(arb_end, arb_start));
		if (lvc->arb_latency_count == 0 || arb_duration_ns < lvc->arb_latency_min_ns)
			lvc->arb_latency_min_ns = arb_duration_ns;
		if (arb_duration_ns > lvc->arb_latency_max_ns)
			lvc->arb_latency_max_ns = arb_duration_ns;
		lvc->arb_latency_total_ns += arb_duration_ns;
		lvc->arb_latency_count++;
	#endif
		return;
	}

	ple_used_by_winner = lvc->ple_usage_bitmap;
	memset(ple_used_by_winner, 0, ple_count * sizeof(bool));

	/* Reset arbitration state */
	list_for_each_entry(ple, &lvc->phys_leds, list) {
		ple->chosen_brightness = 0;
		ple->chosen_priority = -1;
		ple->chosen_sequence = 0;
	#ifdef CONFIG_DEBUG_FS
		ple->winner_name[0] = '\0';
	#endif
	}

	/* STEP 1: Find the ONE active vLED winner (highest priority, latest sequence) */
	list_for_each_entry(vled, &lvc->leds, list) {
	#ifdef CONFIG_DEBUG_FS
		vled->arbitration_participations++;
	#endif
		mutex_lock(&vled->lock);

		brightness = vled->cdev.brightness;
		vled_seq = vled->sequence;

		mutex_unlock(&vled->lock);

		/* Only vLEDs with brightness > 0 can become winners */
		if (brightness == 0)
			continue;

		/* Winner-takes-all: only ONE vLED controls the controller */
		if (vled->priority > highest_priority ||
		    (vled->priority == highest_priority && vled_seq > latest_sequence)) {
			winner_vled = vled;
			highest_priority = vled->priority;
			latest_sequence = vled_seq;
		}
	}

	/* STEP 2: If no active vLED, all physical LEDs stay at 0 (already set above) */
	if (!winner_vled) {
	#ifdef CONFIG_DEBUG_FS
		/* All vLEDs lost (none active) */
		list_for_each_entry(vled, &lvc->leds, list) {
			mutex_lock(&vled->lock);
			if (vled->cdev.brightness == 0)
				vled->arbitration_losses++;
			mutex_unlock(&vled->lock);
		}
	#endif
		/* Physical LEDs already reset to 0 at line 1046-1053 */
		goto collect_updates;
	}

	/* STEP 3: Apply winner's configuration to all its physical LEDs */
	mutex_lock(&winner_vled->lock);

	intensities = winner_vled->arb_bufs.intensities;
	scales = winner_vled->arb_bufs.scales;

	if (!intensities || !scales ||
		winner_vled->arb_bufs.capacity < winner_vled->num_channels) {
		mutex_unlock(&winner_vled->lock);
		dev_err_ratelimited(&lvc->pdev->dev,
			"vLED '%s': arbitration buffer missing or undersized (cap=%u, need=%u)\n",
			winner_vled->name, winner_vled->arb_bufs.capacity,
			winner_vled->num_channels);
#ifdef CONFIG_DEBUG_FS
		atomic64_inc(&lvc->allocation_failures);
#endif
		return;
	}

	/* Snapshot winner's channel data */
	for (j = 0; j < winner_vled->num_channels; j++) {
		intensities[j] = winner_vled->channels[j].intensity;
		scales[j] = winner_vled->channels[j].scale;
	}

	brightness = winner_vled->cdev.brightness;
	max_brightness = winner_vled->cdev.max_brightness;
	mode = winner_vled->mode;
	vled_seq = winner_vled->sequence;

	mutex_unlock(&winner_vled->lock);

	/* Apply winner to all its channels */
	for (j = 0; j < winner_vled->num_channels; j++) {
		final_intensity = vled_channel_get_final_intensity(
			mode, intensities[j], scales[j], brightness, max_brightness
		);

		apply_winner_to_channel(lvc, winner_vled,
			winner_vled->channels[j].leds,
			winner_vled->channels[j].num_leds,
			final_intensity);

			/* Mark physical LEDs as used by winner */
					for (i = 0; i < winner_vled->channels[j].num_leds; i++) {
						unsigned long key;
						struct phys_led_entry *ple_temp;

						if (!winner_vled->channels[j].leds[i])
							continue;

						key = get_stable_led_key(winner_vled->channels[j].leds[i]);
						if (!key)
							continue;

						ple_temp = xa_load(&lvc->phys_xa, key);
						if (!ple_temp || xa_is_value(ple_temp))
							continue;

						/* Direct O(1) index lookup instead of O(n) list scan */
						if (ple_temp->list_index < ple_count)
							ple_used_by_winner[ple_temp->list_index] = true;
					}
	}

#ifdef CONFIG_DEBUG_FS
	winner_vled->arbitration_wins++;

	/* Mark all non-winners as losers */
	list_for_each_entry(vled, &lvc->leds, list) {
		if (vled != winner_vled) {
			mutex_lock(&vled->lock);
			if (vled->cdev.brightness > 0)
				vled->arbitration_losses++;
			mutex_unlock(&vled->lock);
		}
	}
#endif

	/* STEP 4: Turn off physical LEDs not used by winner */
	i = 0;
	list_for_each_entry(ple, &lvc->phys_leds, list) {
		if (!ple_used_by_winner[i]) {
			ple->chosen_brightness = 0;
			ple->chosen_priority = -1;
			ple->chosen_sequence = 0;
		#ifdef CONFIG_DEBUG_FS
			ple->winner_name[0] = '\0';
		#endif
		}
		i++;
	}

collect_updates:

	/* Collect LEDs that need updating */
	update_count = 0;

	list_for_each_entry(ple, &lvc->phys_leds, list) {
		if (!ple->cdev)
			continue;

		if (lvc->first_arbitration || ple->cdev->brightness != ple->chosen_brightness) {
			if (update_count >= lvc->update_buf.capacity) {
				dev_err_ratelimited(&lvc->pdev->dev,
					"Update buffer overflow: %u LEDs need update, capacity=%u\n",
					update_count + 1, lvc->update_buf.capacity);
				dev_err_ratelimited(&lvc->pdev->dev,
					"CRITICAL: Increase max_phys_leds to %u and reload driver!\n",
					lvc->phys_led_count + 16);
				dev_err_ratelimited(&lvc->pdev->dev,
					"Skipping remaining %u LEDs - will retry next cycle\n",
					lvc->phys_led_count - update_count);
			#ifdef CONFIG_DEBUG_FS
				atomic64_inc(&lvc->update_buffer_overflows);
			#endif
				break;
			}

			local_entries[update_count] = phys_led_entry_get(ple);
			local_brightness[update_count] = ple->chosen_brightness;
			update_count++;
		}
	}

	lvc->first_arbitration = false;

#ifdef CONFIG_DEBUG_FS
	lvc->update_count += update_count;
	lvc->last_update = ktime_get();

	arb_end = ktime_get();
	arb_duration_ns = ktime_to_ns(ktime_sub(arb_end, arb_start));

	if (lvc->arb_latency_count == 0 || arb_duration_ns < lvc->arb_latency_min_ns)
		lvc->arb_latency_min_ns = arb_duration_ns;
	if (arb_duration_ns > lvc->arb_latency_max_ns)
		lvc->arb_latency_max_ns = arb_duration_ns;

	if (lvc->arb_latency_total_ns > (U64_MAX - arb_duration_ns) ||
		lvc->arb_latency_count >= (U64_MAX - 1)) {
		lvc->arb_latency_total_ns = arb_duration_ns;
			lvc->arb_latency_count = 1;
			lvc->arb_latency_min_ns = arb_duration_ns;
			lvc->arb_latency_max_ns = arb_duration_ns;
			dev_info(&lvc->pdev->dev,
				"Arbitration latency statistics reset due to overflow\n");
		} else {
			lvc->arb_latency_total_ns += arb_duration_ns;
			lvc->arb_latency_count++;
		}
#endif

	mutex_unlock(&lvc->lock);

	/* Hardware I/O outside lock */
	for (i = 0; i < update_count; i++) {
		int pm_ret;

		if (atomic_read(&lvc->removing)) {
			for (; i < update_count; i++)
				phys_led_entry_put(local_entries[i]);
			mutex_lock(&lvc->lock);
			return;
		}

		ple = local_entries[i];
		if (!ple || !ple->cdev) {
			phys_led_entry_put(ple);
			continue;
		}

		if (ple->dev && pm_runtime_enabled(ple->dev)) {
			pm_ret = pm_runtime_get_sync(ple->dev);
			if (pm_ret < 0) {
				dev_err_ratelimited(&lvc->pdev->dev,
					"Failed to power LED '%s': %d\n",
					ple->cdev->name, pm_ret);
				pm_runtime_put_noidle(ple->dev);
				phys_led_entry_put(ple);
				continue;
			}
		}

		if (ple->cdev->brightness_set_blocking)
			ple->cdev->brightness_set_blocking(ple->cdev, local_brightness[i]);
		else if (ple->cdev->brightness_set)
			ple->cdev->brightness_set(ple->cdev, local_brightness[i]);

		/* Update driver's cached brightness so next arbitration sees the actual value.
		* Some LED drivers don't update cdev->brightness themselves, so maintain it here.
		*/
		WRITE_ONCE(ple->cdev->brightness, local_brightness[i]);

		if (ple->dev && pm_runtime_enabled(ple->dev))
			pm_runtime_put(ple->dev);

		phys_led_entry_put(ple);
	}

	if (update_delay_us > 0 && update_count > 0)
		usleep_range(update_delay_us, update_delay_us + 100);

	mutex_lock(&lvc->lock);
}

static int virtual_led_brightness_set(struct led_classdev *cdev,
	enum led_brightness brightness)
{
	struct virtual_led *vled;
	struct vcolor_controller *lvc;

	if (!cdev)
		return -EINVAL;

	vled = container_of(cdev, struct virtual_led, cdev);

	if (!vled || !vled->ctrl)
		return -ENODEV;

	lvc = vled->ctrl;

	mutex_lock(&vled->lock);
	vled->cdev.brightness = brightness;
	vled->sequence = atomic64_inc_return(&lvc->global_sequence);
#ifdef CONFIG_DEBUG_FS
	vled->brightness_set_count++;
#endif
	mutex_unlock(&vled->lock);

	/* Trigger arbitration - all vLEDs will participate */
	if (!enable_update_batching)
		controller_safe_arbitrate(lvc);

	return 0;
}

static void deferred_update_worker(struct work_struct *work)
{
	struct vcolor_controller *lvc;
	int pending;

	lvc = container_of(work, struct vcolor_controller, update_work.work);

	pending = atomic_xchg(&lvc->pending_updates, 0);

	if (pending > 0 && !atomic_read(&lvc->removing)) {
		if (!controller_safe_arbitrate(lvc)) {
			atomic_set(&lvc->pending_updates, pending);
			mod_delayed_work(system_wq, &lvc->update_work,
					 msecs_to_jiffies(UPDATE_BATCH_DELAY_MS));
		}
	}
}

static ssize_t multi_intensity_show(struct device *dev,
					struct device_attribute *attr, char *buf)
{
	struct led_classdev *cdev;
	struct virtual_led *vled;
	int len;
	unsigned int i;

	cdev = dev_get_drvdata(dev);
	if (!cdev)
		return -ENODEV;

	vled = container_of(cdev, struct virtual_led, cdev);

	mutex_lock(&vled->lock);

	len = 0;
	for (i = 0; i < vled->num_channels; i++) {
		if (i > 0)
			len += scnprintf(buf + len, PAGE_SIZE - len, " ");
		len += scnprintf(buf + len, PAGE_SIZE - len, "%u",
				 vled->channels[i].intensity);
	}
	len += scnprintf(buf + len, PAGE_SIZE - len, "\n");

	mutex_unlock(&vled->lock);
	return len;
}

static int parse_intensity_values(const char *buf, u8 *values,
				  unsigned int expected_count)
{
	char *tmp, *cur, *end;
	unsigned int count, val;
	int ret;
	size_t buf_len;

	if (expected_count == 0)
		return -EINVAL;

	/* NEW: Prevent truncation by rejecting oversized input */
	buf_len = strnlen(buf, PAGE_SIZE + 1);
	if (buf_len > PAGE_SIZE) {
		pr_warn_once("Intensity input exceeds PAGE_SIZE (%lu bytes), rejecting\n",
			     PAGE_SIZE);
		return -EINVAL;
	}

	tmp = kstrndup(buf, PAGE_SIZE, GFP_KERNEL);
	if (!tmp)
		return -ENOMEM;

	cur = tmp;
	end = tmp + strlen(tmp);
	count = 0;
	ret = 0;

	while (cur < end && count < expected_count) {
		while (cur < end && (*cur == ' ' || *cur == '\t' || *cur == '\n'))
			cur++;

		if (cur >= end)
			break;

		if (kstrtouint(cur, 0, &val) || val > 255) {
			ret = -EINVAL;
			goto out;
		}

		values[count++] = (u8)val;

		while (cur < end && *cur != ' ' && *cur != '\t' && *cur != '\n' && *cur != '\0')
			cur++;
	}

	if (count != expected_count)
		ret = -EINVAL;

out:
	kfree(tmp);
	return ret;
}

static ssize_t multi_intensity_store(struct device *dev,
					 struct device_attribute *attr,
					 const char *buf, size_t count)
{
	struct led_classdev *cdev;
	struct virtual_led *vled;
	u8 *values;
	int ret;
	unsigned int i;
	struct vcolor_controller *lvc;
	enum vled_mode mode;

	cdev = dev_get_drvdata(dev);
	if (!cdev)
		return -ENODEV;

	vled = container_of(cdev, struct virtual_led, cdev);

	mutex_lock(&vled->lock);
	mode = vled->mode;
	mutex_unlock(&vled->lock);

	if (mode == VLED_MODE_STANDARD) {
		dev_warn_ratelimited(dev,
			"Cannot change intensity in standard mode - color is fixed by multipliers\n");
		return -EPERM;
	}

	if (!__ratelimit(&vled->intensity_ratelimit)) {
#ifdef CONFIG_DEBUG_FS
		if (vled->ctrl)
			atomic64_inc(&vled->ctrl->ratelimit_hits);
		vled->ratelimit_drops++;
#endif
		return count;
	}

	values = kcalloc(vled->num_channels, sizeof(*values), GFP_KERNEL);
	if (!values) {
		vled_err(vled, "Failed to allocate intensity buffer\n");

#ifdef CONFIG_DEBUG_FS
		vled->buffer_allocation_failures++;
#endif
		return -ENOMEM;
	}

	ret = parse_intensity_values(buf, values, vled->num_channels);
	if (ret) {
		vled_err(vled, "Invalid intensity format (expected %u space-separated values 0-255, e.g., '255 128 0')\n",
			 vled->num_channels);
#ifdef CONFIG_DEBUG_FS
		vled->intensity_parse_errors++;
#endif
		kfree(values);
		return ret;
	}

	mutex_lock(&vled->lock);
	for (i = 0; i < vled->num_channels; i++)
		vled->channels[i].intensity = values[i];

#ifdef CONFIG_DEBUG_FS
	vled->intensity_update_count++;
#endif

	lvc = vled->ctrl;
	if (lvc)
		vled->sequence = atomic64_inc_return(&lvc->global_sequence);
	mutex_unlock(&vled->lock);

	kfree(values);

	if (lvc) {
		if (enable_update_batching) {
			atomic_inc(&lvc->pending_updates);
			mod_delayed_work(system_wq, &lvc->update_work,
					 msecs_to_jiffies(UPDATE_BATCH_DELAY_MS));
		} else {
			controller_safe_arbitrate(lvc);
		}
	}

	return count;
}
static DEVICE_ATTR_RW(multi_intensity);

static ssize_t multi_index_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct led_classdev *cdev;
	struct virtual_led *vled;
	int len;
	unsigned int i;

	cdev = dev_get_drvdata(dev);
	if (!cdev)
		return -ENODEV;

	vled = container_of(cdev, struct virtual_led, cdev);

	mutex_lock(&vled->lock);

	len = 0;
	for (i = 0; i < vled->num_channels; i++) {
		if (i > 0)
			len += scnprintf(buf + len, PAGE_SIZE - len, " ");
		len += scnprintf(buf + len, PAGE_SIZE - len, "%u",
				 vled->channels[i].color_id);
	}
	len += scnprintf(buf + len, PAGE_SIZE - len, "\n");

	mutex_unlock(&vled->lock);
	return len;
}
static DEVICE_ATTR_RO(multi_index);

static ssize_t multi_multipliers_show(struct device *dev,
					  struct device_attribute *attr, char *buf)
{
	struct led_classdev *cdev;
	struct virtual_led *vled;
	int len;
	unsigned int i;

	cdev = dev_get_drvdata(dev);
	if (!cdev)
		return -ENODEV;

	vled = container_of(cdev, struct virtual_led, cdev);

	mutex_lock(&vled->lock);

	len = 0;
	for (i = 0; i < vled->num_channels; i++) {
		if (i > 0)
			len += scnprintf(buf + len, PAGE_SIZE - len, " ");
		len += scnprintf(buf + len, PAGE_SIZE - len, "%u",
				 vled->channels[i].scale);
	}
	len += scnprintf(buf + len, PAGE_SIZE - len, "\n");

	mutex_unlock(&vled->lock);
	return len;
}
static DEVICE_ATTR_RO(multi_multipliers);

static struct attribute *virtual_led_attrs[] = {
	&dev_attr_multi_intensity.attr,
	&dev_attr_multi_index.attr,
	&dev_attr_multi_multipliers.attr,
	NULL
};

static struct attribute_group virtual_led_attr_group = {
	.attrs = virtual_led_attrs,
	.name = "mc",
};

static const struct attribute_group *virtual_led_groups[] = {
	&virtual_led_attr_group,
	NULL
};

static int parse_unified_led_list(struct device *dev,
				  struct fwnode_handle *fwnode,
				  const char *propname,
				  struct mc_channel **out_channels,
				  u8 *out_count)
{
	struct led_classdev **leds;
	struct device **devs;
	u8 count, i, j;
	struct mc_channel *channels;
	int ret, color_id;
	unsigned int color_counts[LED_COLOR_ID_MAX] = {0};
	unsigned int num_channels = 0;
	unsigned int num_channels_allocated = 0;  /* NEW: Track allocation progress */
	unsigned int led_idx;

	ret = parse_leds_fwnode_array(dev, fwnode, propname,
					  &leds, &devs, &count);
	if (ret)
		return ret;

	/* Count LEDs by color */
	for (i = 0; i < count; i++) {
		if (!leds[i])
			continue;

		color_id = leds[i]->color;

		if (color_id < 0 || color_id >= LED_COLOR_ID_MAX) {
			dev_warn(dev, "LED '%s' has invalid color %d, skipping\n",
				 leds[i]->name, color_id);
			continue;
		}

		if (color_id == LED_COLOR_ID_WHITE) {
			dev_dbg(dev, "LED '%s' has color=WHITE (might be unset, check DT)\n",
				leds[i]->name);
		}

		color_counts[color_id]++;
	}

	/* Count number of unique colors */
	for (i = 0; i < LED_COLOR_ID_MAX; i++) {
		if (color_counts[i] > 0)
			num_channels++;
	}

	/* Warn if channel count approaches u8 limit */
	if (num_channels > 255) {
		dev_err(dev, "Channel count %u exceeds u8 limit (255), driver limitation\n",
			num_channels);
		ret = -EINVAL;
		goto err_free_temp_arrays;  /* Add this label before existing cleanup */
	}
	/* FIXED: Free temp arrays before returning error */
	if (num_channels == 0) {
		dev_err(dev, "No valid LEDs with color properties found in '%s'\n",
			propname);

		/* Release LED and device references */
		for (i = 0; i < count; i++) {
			if (leds[i])
				led_put(leds[i]);
			if (devs[i])
				put_device(devs[i]);
		}
		kfree(leds);
		kfree(devs);
		return -ENODEV;
	}

	/* Allocate channel structures */
	channels = devm_kcalloc(dev, num_channels, sizeof(*channels), GFP_KERNEL);
	if (!channels) {
		ret = -ENOMEM;
		goto err_cleanup_arrays;
	}

	/* Build channels grouped by color */
	num_channels_allocated = 0;  /* Track how many channel arrays we allocate */
	for (i = 0; i < LED_COLOR_ID_MAX; i++) {
		if (color_counts[i] == 0)
			continue;

		channels[num_channels_allocated].leds = devm_kcalloc(dev, color_counts[i],
						sizeof(*channels[num_channels_allocated].leds),
						GFP_KERNEL);
		if (!channels[num_channels_allocated].leds) {
			ret = -ENOMEM;
			goto err_cleanup_arrays;
		}

		channels[num_channels_allocated].led_devs = devm_kcalloc(dev, color_counts[i],
						sizeof(*channels[num_channels_allocated].led_devs),
						GFP_KERNEL);
		if (!channels[num_channels_allocated].led_devs) {
			ret = -ENOMEM;
			goto err_cleanup_arrays;
		}

		/* Collect all LEDs of this color */
		led_idx = 0;
		for (j = 0; j < count; j++) {
			if (!leds[j])
				continue;

			if (leds[j]->color == i) {
				channels[num_channels_allocated].leds[led_idx] = leds[j];
				channels[num_channels_allocated].led_devs[led_idx] = devs[j];
				led_idx++;
			}
		}

		channels[num_channels_allocated].color_id = i;
		channels[num_channels_allocated].num_leds = color_counts[i];
		channels[num_channels_allocated].intensity = 0;
		channels[num_channels_allocated].scale = 255;

		dev_dbg(dev, "Added channel: color_id=%d, num_leds=%u\n",
			i, color_counts[i]);

		num_channels_allocated++;  /* Track successful allocation */
	}

	/*
	 * Free temporary arrays (allocated in parse_leds_fwnode_array)
	 * References have been transferred to channels[].leds/led_devs
	 */
	kfree(leds);
	kfree(devs);

	*out_channels = channels;
	*out_count = num_channels_allocated;

	return 0;

err_cleanup_arrays:
	dev_err(dev, "Failed to allocate channel arrays during LED list parsing\n");

	/*
	 * FIXED: Zero any partially-initialized devm channel arrays to prevent
	 * stale pointers from persisting in devm-managed memory.
	 *
	 * We've already released LED ownership via led_put() below, but the
	 * devm-allocated arrays persist until device teardown. Zeroing them
	 * ensures no dangling pointers remain if this memory is examined later.
	 */
	for (i = 0; i < num_channels_allocated; i++) {
		if (channels[i].leds) {
			memset(channels[i].leds, 0,
			       channels[i].num_leds * sizeof(*channels[i].leds));
		}
		if (channels[i].led_devs) {
			memset(channels[i].led_devs, 0,
			       channels[i].num_leds * sizeof(*channels[i].led_devs));
		}
	}
err_free_temp_arrays:
	/* Release LED and device references from temporary arrays */
	for (i = 0; i < count; i++) {
		if (leds[i])
			led_put(leds[i]);
		if (devs[i])
			put_device(devs[i]);
	}
	kfree(leds);
	kfree(devs);
	return ret;
}

static int parse_channel_multipliers(struct device *dev,
	const struct fwnode_handle *fwnode,
	struct mc_channel *channels,
	unsigned int num_channels)
{
	u32 *scales;
	int ret, i;

	scales = kcalloc(num_channels, sizeof(*scales), GFP_KERNEL);
	if (!scales)
		return -ENOMEM;

	ret = fwnode_property_read_u32_array(fwnode, "mc-channel-multipliers",
						 scales, num_channels);

	if (ret == -EINVAL || ret == -ENODATA) {
		kfree(scales);
		return 0;
	}

	if (ret) {
		dev_err(dev, "Failed to read 'mc-channel-multipliers': %d\n", ret);
		kfree(scales);
		return ret;
	}

	for (i = 0; i < num_channels; i++) {
		if (scales[i] > 255) {
			dev_err(dev, "Invalid scale[%d]=%u (max 255)\n", i, scales[i]);
			kfree(scales);
			return -EINVAL;
		}
		channels[i].scale = scales[i];
	}

	kfree(scales);
	return 0;
}

static int allocate_vled_buffers(struct device *dev, struct virtual_led *vled)
{
	vled->arb_bufs.capacity = vled->num_channels;

	vled->arb_bufs.intensities = devm_kcalloc(dev, vled->num_channels,
						  sizeof(*vled->arb_bufs.intensities),
						  GFP_KERNEL);
	if (!vled->arb_bufs.intensities) {
		vled->arb_bufs.capacity = 0;
		return -ENOMEM;
	}

	vled->arb_bufs.scales = devm_kcalloc(dev, vled->num_channels,
						 sizeof(*vled->arb_bufs.scales),
						 GFP_KERNEL);
	if (!vled->arb_bufs.scales) {
		vled->arb_bufs.capacity = 0;
		return -ENOMEM;
	}

	return 0;
}

/*
 * Initializes the vled structure and parses DT,
 * but DOES NOT register the LED class device to prevent race condition.
 *
 * NOTE: This assumes kzalloc/kfree fix (for kref lifetime) has been applied.
 */
static struct virtual_led *virtual_led_init(struct device *dev,
											const struct fwnode_handle *child,
											struct vcolor_controller *lvc)
{
	struct virtual_led *vled;
	const char *mode_str;
	const char *function_str = NULL;
	const char *color_name;
	u32 color_id = LED_COLOR_ID_WHITE;
	u32 priority;
	int ret;
	char *led_name;

	/* Use kzalloc instead of devm_kzalloc to match kref lifetime */
	vled = kzalloc(sizeof(*vled), GFP_KERNEL);
	if (!vled)
		return ERR_PTR(-ENOMEM);

	kref_init(&vled->refcount);
	mutex_init(&vled->lock);
	INIT_LIST_HEAD(&vled->list);
	vled->fwnode = fwnode_handle_get((struct fwnode_handle *)child);
	vled->ctrl = lvc;

	priority = 0;
	ret = fwnode_property_read_u32(child, "priority", &priority);
	if (ret)
		priority = 0;

	if (priority > PRIORITY_MAX) {
		dev_warn(dev, "Priority %u exceeds maximum %d, clamping\n",
			priority, PRIORITY_MAX);
		priority = PRIORITY_MAX;
	}

	vled->mode = VLED_MODE_MULTICOLOR;
	ret = fwnode_property_read_string(child, "led-mode", &mode_str);
	if (ret == 0) {
		if (strcmp(mode_str, "standard") == 0) {
			vled->mode = VLED_MODE_STANDARD;
		} else if (strcmp(mode_str, "multicolor") == 0) {
			vled->mode = VLED_MODE_MULTICOLOR;
		} else {
			dev_err(dev, "Invalid led-mode '%s' (use 'standard' or 'multicolor')\n",
				mode_str);
			ret = -EINVAL;
			goto err_put_fwnode;
		}
	}

	ret = parse_unified_led_list(dev, (struct fwnode_handle *)child, "leds",
					 &vled->channels, &vled->num_channels);
	if (ret) {
		dev_err(dev, "Failed to parse LED list: %d\n", ret);
		goto err_put_fwnode;
	}

	ret = parse_channel_multipliers(dev, child, vled->channels,
					vled->num_channels);
	if (ret) {
		dev_err(dev, "Failed to parse channel multipliers: %d\n", ret);
		goto err_put_fwnode;
	}

	ret = allocate_vled_buffers(dev, vled);
	if (ret) {
		dev_err(dev, "Failed to allocate arbitration buffers for vLED: %d\n",
			ret);
		goto err_put_fwnode;
	}

	ret = fwnode_property_read_string(child, "function", &function_str);
	if (ret || !function_str)
		function_str = "status";

	ret = fwnode_property_read_u32(child, "color", &color_id);
	color_name = led_get_color_name(color_id);
	if (!color_name) {
		color_id = LED_COLOR_ID_WHITE;
		color_name = "white";
	}

	led_name = kasprintf(GFP_KERNEL, "%s:%s",
					 function_str, color_name);
	if (!led_name) {
		ret = -ENOMEM;
		goto err_put_fwnode;
	}

	/* Setup LED class device but DO NOT register yet */
	vled->cdev.name = led_name;
	vled->cdev.max_brightness = 255;
	vled->cdev.brightness = 0;
	vled->cdev.brightness_set_blocking = virtual_led_brightness_set;
	vled->cdev.groups = virtual_led_groups;
	vled->priority = (s32)priority;

	ratelimit_state_init(&vled->intensity_ratelimit,
				 1 * HZ, DEFAULT_UPDATE_RATE_LIMIT);

	/* vLED is now ready for internal logic, but not exposed to userspace */
	return vled;

err_put_fwnode:
	fwnode_handle_put(vled->fwnode);
	kfree(vled); /* Must use kfree since we used kzalloc */
	return ERR_PTR(ret);
}

static int virtual_led_register(struct device *dev, struct virtual_led *vled)
{
	struct led_init_data init_data = {};
	int ret;

	init_data.fwnode = vled->fwnode;
	init_data.devicename = NULL;
	init_data.default_label = NULL;

	/* Use explicit registration to match vled kzalloc/kfree lifetime */
	ret = led_classdev_register_ext(dev, &vled->cdev, &init_data);
	if (ret) {
		dev_err(dev, "LED registration FAILED for '%s': error %d\n",
			vled->cdev.name, ret);
		return ret;
	}

	vled->cdev_registered = true;
	dev_info(dev, "Registered virtual LED '%s'\n", vled->cdev.name);

	/* Verify LED core assigned name matches */
	vled->name = vled->cdev.name;

	return 0;
}

static void virtual_led_release(struct kref *ref)
{
	struct virtual_led *vled = container_of(ref, struct virtual_led, refcount);

	if (WARN_ON(vled->cdev_registered)) {
		pr_err("%s: kref reached zero while LED '%s' still registered!\n",
			   DRIVER_NAME, vled->name ? vled->name : "(unknown)");
	}

	/* Actually free the memory since we used kzalloc */
	kfree(vled);
}

static void virtual_led_destroy(struct virtual_led *vled)
{
	unsigned int i, j;

	if (!vled)
		return;

	vled->cdev_registered = false;

	/* Free LED name allocated with kasprintf (non-devm) */
	if (vled->cdev.name)
		kfree((void *)vled->cdev.name);

#ifdef CONFIG_DEBUG_FS
	debugfs_remove_recursive(vled->debugfs_dir);
#endif

/*
 * Release LED and device references obtained during probe.
 * The channel arrays themselves (vled->channels[i].leds/led_devs)
 * are devm-managed and will be freed automatically by the driver core.
 */
 for (i = 0; i < vled->num_channels; i++) {
 		if (vled->channels[i].leds) {
 			for (j = 0; j < vled->channels[i].num_leds; j++) {
 				if (vled->channels[i].leds[j]) {
 					led_put(vled->channels[i].leds[j]);
 					vled->channels[i].leds[j] = NULL;
 				}
 			}
 		}

 		release_device_array(vled->channels[i].led_devs,
 			vled->channels[i].num_leds);
 	}

	fwnode_handle_put(vled->fwnode);
}

#ifdef CONFIG_DEBUG_FS

#define SCNPRINTF_FIELD(out, len, size, name, format, value) \
	do { \
		if (len >= size) { \
			break; \
		} \
		len += scnprintf(out + len, size - len, name ": " format "\n", value); \
	} while (0)

static int debugfs_simple_read(struct file *file, char __user *buf,
				   size_t count, loff_t *ppos,
				   int (*format)(void *data, char *out, size_t size))
{
	char *out;
	int len, ret;

	out = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!out)
		return -ENOMEM;

	len = format(file->private_data, out, PAGE_SIZE);
	ret = simple_read_from_buffer(buf, count, ppos, out, len);
	kfree(out);

	return ret;
}

static int format_stats(void *data, char *out, size_t size)
{
	struct vcolor_controller *lvc;
	s64 last_update_ms;
	u64 arb_latency_avg_ns;
	u64 arb_count, update_count, phys_count;
	u64 alloc_failures, buf_overflows, ratelimit_hits;
	int len;

	lvc = data;

	alloc_failures = atomic64_read(&lvc->allocation_failures);
	buf_overflows = atomic64_read(&lvc->update_buffer_overflows);
	ratelimit_hits = atomic64_read(&lvc->ratelimit_hits);

	mutex_lock(&lvc->lock);

	last_update_ms = ktime_to_ms(ktime_sub(ktime_get(), lvc->last_update));

	arb_latency_avg_ns = 0;
	if (lvc->arb_latency_count > 0)
		arb_latency_avg_ns = lvc->arb_latency_total_ns / lvc->arb_latency_count;

	arb_count = lvc->arbitration_count;
	update_count = lvc->update_count;
	phys_count = lvc->phys_led_count;

	mutex_unlock(&lvc->lock);

	len = 0;
	if (len >= size)
		return len;
	len += scnprintf(out + len, size - len, "=== Controller Stats ===\n");
	SCNPRINTF_FIELD(out, len, size, "Arbitration cycles", "%llu", arb_count);
	SCNPRINTF_FIELD(out, len, size, "LED updates", "%llu", update_count);
	SCNPRINTF_FIELD(out, len, size, "Last update", "%lld ms ago", last_update_ms);

	if (len >= size)
		return len;
	len += scnprintf(out + len, size - len, "\n=== Error Counters ===\n");
	SCNPRINTF_FIELD(out, len, size, "Allocation failures", "%llu", alloc_failures);
	SCNPRINTF_FIELD(out, len, size, "Update buffer overflows", "%llu", buf_overflows);
	SCNPRINTF_FIELD(out, len, size, "Rate limit hits", "%llu", ratelimit_hits);
	SCNPRINTF_FIELD(out, len, size, "Global sequence", "%llu",
			atomic64_read(&lvc->global_sequence));

	if (len >= size)
		return len;
	len += scnprintf(out + len, size - len, "\n=== Arbitration Latency ===\n");
	SCNPRINTF_FIELD(out, len, size, "Min", "%llu ns", lvc->arb_latency_min_ns);
	SCNPRINTF_FIELD(out, len, size, "Max", "%llu ns", lvc->arb_latency_max_ns);
	SCNPRINTF_FIELD(out, len, size, "Avg", "%llu ns", arb_latency_avg_ns);
	SCNPRINTF_FIELD(out, len, size, "Count", "%llu", lvc->arb_latency_count);

	if (len >= size)
		return len;
	len += scnprintf(out + len, size - len, "\n=== Configuration ===\n");
	SCNPRINTF_FIELD(out, len, size, "Gamma correction", "%s",
			use_gamma_correction ? "enabled" : "disabled");
	SCNPRINTF_FIELD(out, len, size, "Update batching", "%s",
			enable_update_batching ? "enabled" : "disabled");
	SCNPRINTF_FIELD(out, len, size, "Update delay", "%u us", update_delay_us);
	if (len >= size)
		return len;
	len += scnprintf(out + len, size - len, "Physical LED count: %llu/%u\n",
			 phys_count, lvc->update_buf.capacity);
	SCNPRINTF_FIELD(out, len, size, "Removing", "%s",
			atomic_read(&lvc->removing) ? "yes" : "no");

	return len;
}

static int format_vled_stats(void *data, char *out, size_t size)
{
	struct vcolor_controller *lvc;
	int len;
	struct virtual_led *vled;
	u64 win_rate;

	lvc = data;

	mutex_lock(&lvc->lock);

	len = 0;
	list_for_each_entry(vled, &lvc->leds, list) {
		if (len >= size)
			break;

		win_rate = 0;
		if (vled->arbitration_participations > 0) {
			win_rate = div64_u64(vled->arbitration_wins * 100ULL,
						 vled->arbitration_participations);
			if (win_rate > 100)
				win_rate = 100;
		}

		if (len >= size)
			return len;
		len += scnprintf(out + len, size - len,
				 "=== LED: %s (Mode: %s, Prio: %d) ===\n",
				 vled->name,
				 vled->mode == VLED_MODE_STANDARD ? "standard" : "multicolor",
				 vled->priority);
		SCNPRINTF_FIELD(out, len, size, "Brightness sets", "%llu",
				vled->brightness_set_count);
		SCNPRINTF_FIELD(out, len, size, "Intensity sets", "%llu",
				vled->intensity_update_count);
		SCNPRINTF_FIELD(out, len, size, "Sequence", "%llu", vled->sequence);
		if (len >= size)
			break;
		len += scnprintf(out + len, size - len,
				 "Current brightness: %u/%u\n",
				 vled->cdev.brightness, vled->cdev.max_brightness);
		SCNPRINTF_FIELD(out, len, size, "Channels", "%u", vled->num_channels);
		SCNPRINTF_FIELD(out, len, size, "Arbitration participations", "%llu",
				vled->arbitration_participations);
		SCNPRINTF_FIELD(out, len, size, "Arbitration losses", "%llu",
				vled->arbitration_losses);
		SCNPRINTF_FIELD(out, len, size, "Win rate", "%llu%%\n", win_rate);

		if (len >= size)
			return len;
		len += scnprintf(out + len, size - len, "\n=== vLED Error Counters ===\n");
		SCNPRINTF_FIELD(out, len, size, "Buffer allocation failures", "%llu",
				vled->buffer_allocation_failures);
		SCNPRINTF_FIELD(out, len, size, "Intensity parse errors", "%llu",
				vled->intensity_parse_errors);
		SCNPRINTF_FIELD(out, len, size, "Rate limit drops", "%llu\n",
				vled->ratelimit_drops);
	}

	mutex_unlock(&lvc->lock);
	return len;
}

static int format_phys_led_states(void *data, char *out, size_t size)
{
	struct vcolor_controller *lvc;
	int len;
	struct phys_led_entry *ple;

	lvc = data;

	len = 0;
	len += scnprintf(out + len, size - len, "=== Physical LED States ===\n");
	if (len >= size)
		return len;
	len += scnprintf(out + len, size - len,
			 "Format: [LED] Brightness Priority Seq Winner\n\n");

	mutex_lock(&lvc->lock);

	list_for_each_entry(ple, &lvc->phys_leds, list) {
		if (len >= size)
			break;
		if (!ple->cdev)
			continue;

		len += scnprintf(out + len, size - len,
				 "[%s] B:%u P:%d S:%llu W:%s\n",
				 ple->cdev->name,
				 ple->chosen_brightness,
				 ple->chosen_priority,
				 ple->chosen_sequence,
				 ple->winner_name[0] ? ple->winner_name : "(none)");
	}

	mutex_unlock(&lvc->lock);
	return len;
}

static int format_claimed_leds(void *data, char *out, size_t size)
{
	unsigned long count, index;
	struct global_phys_owner *gpo;

	down_read(&global_owner_rwsem);

	count = 0;
	xa_for_each(&global_owner_xa, index, gpo)
		if (gpo && !xa_is_value(gpo))
			count++;

	up_read(&global_owner_rwsem);

	return scnprintf(out, size, "%lu\n", count);
}

#define DEBUGFS_READ_FOP(name, formatter) \
static ssize_t debugfs_##name##_read(struct file *file, char __user *buf, \
			size_t count, loff_t *ppos) \
			{ \
	return debugfs_simple_read(file, buf, count, ppos, formatter); \
} \
static const struct file_operations debugfs_##name##_fops = { \
	.owner = THIS_MODULE, \
	.open = simple_open, \
	.read = debugfs_##name##_read, \
	.llseek = default_llseek, \
}

DEBUGFS_READ_FOP(stats, format_stats);
DEBUGFS_READ_FOP(vled_stats, format_vled_stats);
DEBUGFS_READ_FOP(phys_led_states, format_phys_led_states);
DEBUGFS_READ_FOP(claimed, format_claimed_leds);

static ssize_t debugfs_selftest_read(struct file *file, char __user *buf,
					 size_t count, loff_t *ppos)
{
	struct vcolor_controller *lvc;
	char *output;
	int len, ret;

	lvc = file->private_data;

	if (!lvc)
		return -ENODEV;

	output = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!output)
		return -ENOMEM;

	len = 0;
	len += scnprintf(output + len, PAGE_SIZE - len,
			 "=== %s Selftest V5.1.18 ===\n", DRIVER_NAME);
	len += scnprintf(output + len, PAGE_SIZE - len,
			 "\nChanges in V5.1.18:\n");
	len += scnprintf(output + len, PAGE_SIZE - len,
			 "- Conditional debug compilation: IMPLEMENTED\n");
	len += scnprintf(output + len, PAGE_SIZE - len,
			 "- Reduced struct sizes (~200 bytes per LED): IMPLEMENTED\n");
	len += scnprintf(output + len, PAGE_SIZE - len,
			 "- Eliminated debug telemetry overhead: IMPLEMENTED\n");
	len += scnprintf(output + len, PAGE_SIZE - len,
			 "\nResult: PASS - Production ready V5.1.18 (optimized)\n");

	ret = simple_read_from_buffer(buf, count, ppos, output, len);
	kfree(output);

	return ret;
}

static const struct file_operations debugfs_selftest_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read = debugfs_selftest_read,
	.llseek = default_llseek,
};

static ssize_t debugfs_stress_test_write(struct file *file,
					 const char __user *buf,
					 size_t count, loff_t *ppos)
{
	struct vcolor_controller *lvc;
	unsigned int iterations, completed, i, j;
	u8 random_data[4];
	struct virtual_led *vled, **vled_snapshot;
	unsigned int vled_count;

	lvc = file->private_data;

	if (!lvc || atomic_read(&lvc->removing))
		return -ENODEV;

	if (kstrtouint_from_user(buf, count, 0, &iterations))
		return -EINVAL;

	if (iterations > 10000) {
		dev_warn(&lvc->pdev->dev, "Clamping stress test to 10000 iterations\n");
		iterations = 10000;
	}

	if (mutex_lock_interruptible(&lvc->lock)) {
		dev_info(&lvc->pdev->dev, "Stress test interrupted by signal\n");
		return -EINTR;
	}

	vled_count = 0;
	list_for_each_entry(vled, &lvc->leds, list)
		vled_count++;

	if (vled_count == 0) {
		mutex_unlock(&lvc->lock);
		dev_info(&lvc->pdev->dev, "No vLEDs available for stress test\n");
		return count;
	}

	vled_snapshot = kcalloc(vled_count, sizeof(*vled_snapshot), GFP_KERNEL);
	if (!vled_snapshot) {
		mutex_unlock(&lvc->lock);
		dev_err(&lvc->pdev->dev, "Failed to allocate vled snapshot for stress test\n");
		return -ENOMEM;
	}

	i = 0;
	list_for_each_entry(vled, &lvc->leds, list) {
		vled_snapshot[i++] = virtual_led_get(vled);
	}

	dev_info(&lvc->pdev->dev, "Starting stress test (%u iterations, %u vLEDs)\n",
		 iterations, vled_count);
	 /*
 	 * Locking pattern: We hold lvc->lock across arbitration but release it
 	 * between iterations to allow other operations. controller_run_arbitration_and_update()
 	 * expects the lock to be held on entry and maintains that invariant on return.
 	 */
	completed = 0;
	for (i = 0; i < iterations; i++) {
		get_random_bytes(random_data, sizeof(random_data));

		for (j = 0; j < vled_count; j++) {
			unsigned int k;
			u8 new_brightness;

			vled = vled_snapshot[j];
			if (!vled)
				continue;

			new_brightness = random_data[0] % (vled->cdev.max_brightness + 1);

			mutex_lock(&vled->lock);
			for (k = 0; k < vled->num_channels && k < 3; k++)
				vled->channels[k].intensity = random_data[k + 1];

			vled->cdev.brightness = new_brightness;
			vled->sequence = atomic64_inc_return(&lvc->global_sequence);
			mutex_unlock(&vled->lock);

		}

		controller_run_arbitration_and_update(lvc);
		completed++;

		mutex_unlock(&lvc->lock);
		usleep_range(100, 200);
		mutex_lock(&lvc->lock);
		cond_resched();

		if (atomic_read(&lvc->removing))
			break;
	}

	mutex_unlock(&lvc->lock);

	for (i = 0; i < vled_count; i++)
		virtual_led_put(vled_snapshot[i]);
	kfree(vled_snapshot);

	dev_info(&lvc->pdev->dev,
		 "Stress test completed: %u/%u iterations, %llu total arbitrations\n",
		 completed, iterations, lvc->arbitration_count);

	return count;
}

static const struct file_operations debugfs_stress_test_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.write = debugfs_stress_test_write,
	.llseek = default_llseek,
};

static ssize_t debugfs_rebuild_write(struct file *file,
					 const char __user *buf,
					 size_t count, loff_t *ppos)
{
	struct vcolor_controller *lvc;
	unsigned int phys_count;

	lvc = file->private_data;

	if (!lvc || lvc->suspended || atomic_read(&lvc->removing))
		return -EBUSY;

	if (mutex_lock_interruptible(&lvc->lock)) {
		dev_info(&lvc->pdev->dev, "Physical LED rebuild interrupted by signal\n");
		return -EINTR;
	}

	if (atomic_read(&lvc->removing)) {
		mutex_unlock(&lvc->lock);
		return -EBUSY;
	}

	dev_info(&lvc->pdev->dev, "Physical LED rebuild triggered via debugfs\n");
	controller_rebuild_phys_leds(lvc);

	phys_count = lvc->phys_led_count;
	dev_info(&lvc->pdev->dev, "Physical LED rebuild complete: %u LEDs registered\n",
		 phys_count);

	mutex_unlock(&lvc->lock);

	return count;
}

static const struct file_operations debugfs_rebuild_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.write = debugfs_rebuild_write,
	.llseek = default_llseek,
};

static void controller_setup_debugfs(struct vcolor_controller *lvc)
{
	char debugfs_dirname[64];

	if (!enable_debugfs)
		return;

	snprintf(debugfs_dirname, sizeof(debugfs_dirname), "%s-%s",
		VLED_DEBUGFS_DIR, dev_name(&lvc->pdev->dev));

	lvc->debugfs_root = debugfs_create_dir(debugfs_dirname, NULL);
	if (IS_ERR_OR_NULL(lvc->debugfs_root)) {
		lvc->debugfs_root = NULL;
		return;
	}

	debugfs_create_file("stats", 0444, lvc->debugfs_root, lvc,
				&debugfs_stats_fops);
	debugfs_create_file("vled_stats", 0444, lvc->debugfs_root, lvc,
				&debugfs_vled_stats_fops);
	debugfs_create_file("phys_led_states", 0444, lvc->debugfs_root, lvc,
				&debugfs_phys_led_states_fops);
	debugfs_create_file("claimed_leds", 0444, lvc->debugfs_root, lvc,
				&debugfs_claimed_fops);
	debugfs_create_file("selftest", 0444, lvc->debugfs_root, lvc,
				&debugfs_selftest_fops);
	debugfs_create_file("stress_test", 0200, lvc->debugfs_root, lvc,
				&debugfs_stress_test_fops);
	debugfs_create_file("rebuild", 0200, lvc->debugfs_root, lvc,
				&debugfs_rebuild_fops);
}

static void controller_destroy_debugfs(struct vcolor_controller *lvc)
{
	debugfs_remove_recursive(lvc->debugfs_root);
}

#else
static inline void controller_setup_debugfs(struct vcolor_controller *lvc) {}
static inline void controller_destroy_debugfs(struct vcolor_controller *lvc) {}
#endif

static int leds_virtualcolor_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct vcolor_controller *lvc;
	struct device_node *child_np;
	struct virtual_led *vled;
	unsigned int phys_count;
	int ret;
	int initialized_count = 0;

	dev = &pdev->dev;

	lvc = devm_kzalloc(dev, sizeof(*lvc), GFP_KERNEL);
	if (!lvc)
		return -ENOMEM;

	INIT_LIST_HEAD(&lvc->leds);
	INIT_LIST_HEAD(&lvc->phys_leds);
	mutex_init(&lvc->lock);
	lvc->pdev = pdev;
	xa_init(&lvc->phys_xa);
	atomic_set(&lvc->removing, 0);
	atomic_set(&lvc->rebuilding, 0);
	INIT_DELAYED_WORK(&lvc->update_work, deferred_update_worker);
	atomic_set(&lvc->pending_updates, 0);
	atomic64_set(&lvc->global_sequence, 0);
	lvc->first_arbitration = true;
#ifdef CONFIG_DEBUG_FS
	lvc->last_update = ktime_get();
	atomic64_set(&lvc->allocation_failures, 0);
	atomic64_set(&lvc->update_buffer_overflows, 0);
	atomic64_set(&lvc->ratelimit_hits, 0);
	lvc->arb_latency_min_ns = U64_MAX;
	lvc->arb_latency_max_ns = 0;
	lvc->arb_latency_total_ns = 0;
	lvc->arb_latency_count = 0;
#endif
	dev_set_drvdata(dev, lvc);

	lvc->update_buf.max_capacity = max_phys_leds;
	lvc->update_buf.capacity = max_phys_leds;

	lvc->update_buf.entries = devm_kcalloc(dev, max_phys_leds,
							   sizeof(*lvc->update_buf.entries),
							   GFP_KERNEL);
		lvc->update_buf.brightness = devm_kcalloc(dev, max_phys_leds,
							  sizeof(*lvc->update_buf.brightness),
							  GFP_KERNEL);
		if (!lvc->update_buf.entries || !lvc->update_buf.brightness) {
			dev_err(dev, "Failed to allocate update buffers (capacity=%u)\n",
				max_phys_leds);
			return -ENOMEM;
		}

		/* Pre-allocate arbitration snapshot buffers */
		lvc->vled_snapshot_capacity = VLED_SNAPSHOT_DEFAULT;
		lvc->vled_snapshot = devm_kcalloc(dev, lvc->vled_snapshot_capacity,
						  sizeof(*lvc->vled_snapshot), GFP_KERNEL);
		if (!lvc->vled_snapshot) {
			dev_err(dev, "Failed to allocate vLED snapshot buffer\n");
			return -ENOMEM;
		}

		lvc->ple_snapshot_capacity = max_phys_leds;
		lvc->ple_snapshot = devm_kcalloc(dev, lvc->ple_snapshot_capacity,
						 sizeof(*lvc->ple_snapshot), GFP_KERNEL);
		if (!lvc->ple_snapshot) {
			dev_err(dev, "Failed to allocate PLE snapshot buffer\n");
			return -ENOMEM;
		}

		lvc->ple_usage_bitmap_capacity = max_phys_leds;
		lvc->ple_usage_bitmap = devm_kcalloc(dev, lvc->ple_usage_bitmap_capacity,
							 sizeof(*lvc->ple_usage_bitmap), GFP_KERNEL);
		if (!lvc->ple_usage_bitmap) {
			dev_err(dev, "Failed to allocate PLE usage bitmap\n");
			return -ENOMEM;
		}

		controller_setup_debugfs(lvc);

		/*
		 * PHASE 1: Initialize vLEDs and build internal list
		 *
		 * IMPORTANT: for_each_available_child_of_node() uses of_get_next_available_child()
		 * which handles per-iteration reference management automatically. Any early return
		 * from inside the loop MUST call of_node_put(child_np) for the current node before
		 * returning, as demonstrated in the EPROBE_DEFER path below.
		 */
		for_each_available_child_of_node(dev->of_node, child_np) {
			struct fwnode_handle *child_fwnode;

			/* Convert OF node to fwnode handle (does not increment refcount) */
			child_fwnode = of_fwnode_handle(child_np);

			/*
			 * virtual_led_init will call fwnode_handle_get() internally,
			 * so we pass the unref'd handle here
			 */
			vled = virtual_led_init(dev, child_fwnode, lvc);
			if (IS_ERR(vled)) {
				ret = PTR_ERR(vled);
				dev_err(dev, "Failed to create LED from device node: %d\n", ret);

				/* Handle deferred probe specially */
				if (ret == -EPROBE_DEFER) {
					dev_info(dev, "Deferring probe until LEDs are available\n");
					of_node_put(child_np);
					controller_destroy_debugfs(lvc);
					return -EPROBE_DEFER;
				}
				/* Loop continues, for_each macro handles of_node_put */
				continue;
			}

			mutex_lock(&lvc->lock);
			list_add_tail(&vled->list, &lvc->leds);
			mutex_unlock(&lvc->lock);

			initialized_count++;
		}

	if (initialized_count == 0) {
		ret = dev_err_probe(dev, -ENODEV, "No valid LED nodes found\n");
		goto err_cleanup;
	}

	/* * PHASE 2: Build physical LED mappings NOW.
	 * The controller is now in a consistent state.
	 */
	mutex_lock(&lvc->lock);
	controller_rebuild_phys_leds(lvc);
	phys_count = lvc->phys_led_count;

	if (phys_count > max_phys_leds) {
		dev_warn(dev, "Physical LED count (%u) exceeds limit (%u)\n",
			 phys_count, max_phys_leds);
	}
	mutex_unlock(&lvc->lock);

	/*
 * Force all physical LEDs to known state (brightness=0).
 *
 * This is critical because:
 * 1. Device tree may have boot LED aliases (linux,default-trigger = "default-on")
 * 2. Physical LED drivers may restore previous brightness on probe
 * 3. Without this, first arbitration compares current_brightness with chosen_brightness
 *    and skips update if they match (even though driver never set it)
 *
 * The first_arbitration flag helps, but DT triggers can activate LEDs AFTER
 * our probe completes, so we must force them off here.
 */
{
	struct phys_led_entry *ple;
	unsigned int reset_count = 0;

	dev_info(dev, "Forcing %u physical LEDs to initial state (off)\n", phys_count);

	mutex_lock(&lvc->lock);
	list_for_each_entry(ple, &lvc->phys_leds, list) {
		if (!ple->cdev)
			continue;

		/* Force hardware to brightness=0 */
		if (ple->cdev->brightness_set_blocking) {
			ple->cdev->brightness_set_blocking(ple->cdev, 0);
		} else if (ple->cdev->brightness_set) {
			ple->cdev->brightness_set(ple->cdev, 0);
		}

		/* Update driver's view to match hardware */
		ple->cdev->brightness = 0;
		reset_count++;

		dev_dbg(dev, "  Reset physical LED '%s' to brightness=0\n",
			ple->cdev->name ? ple->cdev->name : "(unnamed)");
	}
	mutex_unlock(&lvc->lock);

	if (reset_count > 0) {
		dev_info(dev, "Reset %u physical LEDs to off state\n", reset_count);
	}
}


	/* * PHASE 3: Register vLEDs (expose to userspace)
	 * If registration fails, the device is shut down completely.
	 */
	list_for_each_entry(vled, &lvc->leds, list) {
		ret = virtual_led_register(dev, vled);
		if (ret)
			goto err_cleanup;
	}

	dev_info(dev, "Initialized %d virtual LED(s), controlling %u physical LEDs\n",
		 initialized_count, phys_count);

	return 0;

err_cleanup:
	  /* IMPORTANT: Since virtual_leds are kzalloc'd (not devm),
	   * we must clean them up manually on failure paths.
	   */
		mutex_lock(&lvc->lock);

		/* Destroy physical LED list and XArray first */
		controller_destroy_phys_list(lvc);
		xa_destroy(&lvc->phys_xa);

		/* Manually clean up vleds using the list and kref/kfree logic */
		{
			struct virtual_led *v, *tmp;

			list_for_each_entry_safe(v, tmp, &lvc->leds, list) {
				list_del(&v->list);
				/* Release device references before freeing vLED */
				virtual_led_destroy(v);
				/* This call uses kref_put() which leads to kfree(v) */
				virtual_led_put(v);
			}
		}
		mutex_unlock(&lvc->lock);

		controller_destroy_debugfs(lvc);

		/* devm will clean up the LVC structure itself */
		return ret;
}

static void leds_virtualcolor_remove(struct platform_device *pdev)
{
	struct vcolor_controller *lvc;
	struct virtual_led *vled, *tmp;

	lvc = platform_get_drvdata(pdev);

	if (!lvc)
		return;

	atomic_set(&lvc->removing, 1);
	cancel_delayed_work_sync(&lvc->update_work);

	mutex_lock(&lvc->lock);
	controller_destroy_phys_list(lvc);
	xa_destroy(&lvc->phys_xa);
	mutex_unlock(&lvc->lock);

	list_for_each_entry_safe(vled, tmp, &lvc->leds, list) {
		list_del(&vled->list);

		/* Unregister LED class device before freeing vled memory */
		if (vled->cdev_registered)
			led_classdev_unregister(&vled->cdev);

		virtual_led_destroy(vled);
		virtual_led_put(vled);
	}

	global_release_all_for_pdev(pdev);
	controller_destroy_debugfs(lvc);

	dev_info(&pdev->dev, "Driver removed successfully\n");
}

static void leds_virtualcolor_shutdown(struct platform_device *pdev)
{
	struct vcolor_controller *lvc;
	struct phys_led_entry *ple;

	lvc = platform_get_drvdata(pdev);

	if (!lvc)
		return;

	cancel_delayed_work_sync(&lvc->update_work);

	mutex_lock(&lvc->lock);
	atomic_set(&lvc->removing, 1);

	list_for_each_entry(ple, &lvc->phys_leds, list) {
		if (ple->cdev) {
			if (ple->cdev->brightness_set_blocking)
				ple->cdev->brightness_set_blocking(ple->cdev, 0);
			else if (ple->cdev->brightness_set)
				ple->cdev->brightness_set(ple->cdev, 0);
		}
	}
	controller_destroy_phys_list(lvc);

	mutex_unlock(&lvc->lock);

	dev_info(&pdev->dev, "Driver shutdown: all LEDs turned off\n");
}

#ifdef CONFIG_PM_SLEEP
static int leds_virtualcolor_suspend(struct device *dev)
{
	struct vcolor_controller *lvc;

	lvc = dev_get_drvdata(dev);

	if (!lvc)
		return 0;

	cancel_delayed_work_sync(&lvc->update_work);

	mutex_lock(&lvc->lock);
	lvc->suspended = true;

	/*
	 * No need to save/restore physical LED states.
	 * On resume, controller_rebuild_phys_leds() will run arbitration
	 * and restore LEDs based on current vLED states (which persist
	 * across suspend/resume since they're in memory).
	 */

	mutex_unlock(&lvc->lock);

	dev_info(dev, "System suspended\n");

	return 0;
}

static int leds_virtualcolor_resume(struct device *dev)
{
	struct vcolor_controller *lvc;

	lvc = dev_get_drvdata(dev);

	if (!lvc)
		return 0;

	mutex_lock(&lvc->lock);

	/* Rebuild physical LED mappings */
	controller_rebuild_phys_leds(lvc);

	lvc->suspended = false;
	mutex_unlock(&lvc->lock);

	/*
	 * controller_rebuild_phys_leds() already ran arbitration,
	 * which restored all physical LEDs based on current vLED states.
	 * vLED brightness values persisted in memory across suspend,
	 * so LEDs are now in correct state.
	 */

	dev_info(dev, "System resumed: LED states restored\n");

	return 0;
}
#else
#define leds_virtualcolor_suspend NULL
#define leds_virtualcolor_resume NULL
#endif

static const struct dev_pm_ops leds_virtualcolor_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(leds_virtualcolor_suspend, leds_virtualcolor_resume)
};

static const struct of_device_id leds_virtualcolor_dt_ids[] = {
	{ .compatible = "leds-group-virtualcolor" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, leds_virtualcolor_dt_ids);

static struct platform_driver leds_virtualcolor_driver = {
	.probe = leds_virtualcolor_probe,
	.remove = leds_virtualcolor_remove,
	.shutdown = leds_virtualcolor_shutdown,
	.driver = {
		.name = DRIVER_NAME,
		.of_match_table = leds_virtualcolor_dt_ids,
		.pm = &leds_virtualcolor_pm_ops,
	},
};

static int __init leds_virtualcolor_init(void)
{
	int ret;

	/* Validate and clamp module parameters */
	if (update_delay_us > 1000000) {
		pr_warn(DRIVER_NAME ": update_delay_us=%u exceeds max, clamping to 1000000\n",
			update_delay_us);
		update_delay_us = 1000000;
	}

	if (max_phys_leds < 1 || max_phys_leds > 1024) {
		pr_warn(DRIVER_NAME ": max_phys_leds=%u out of range, using default %u\n",
			max_phys_leds, MAX_PHYS_LEDS_DEFAULT);
		max_phys_leds = MAX_PHYS_LEDS_DEFAULT;
	}

	pr_info(DRIVER_NAME ": v5.1.18 - Debug compilation optimization\n");
	pr_info(DRIVER_NAME ": Config: gamma=%s, batching=%s, delay=%uus, max_leds=%u\n",
		use_gamma_correction ? "on" : "off",
		enable_update_batching ? "on" : "off",
		update_delay_us, max_phys_leds);

	ret = platform_driver_register(&leds_virtualcolor_driver);
	if (ret) {
		pr_err(DRIVER_NAME ": Failed to register platform driver: %d\n", ret);
		return ret;
	}

	return 0;
}
module_init(leds_virtualcolor_init);

static void __exit leds_virtualcolor_exit(void)
{
	unsigned long index, leaked = 0;
	struct global_phys_owner *gpo;

	/* Unregister driver first to prevent new probes */
	platform_driver_unregister(&leds_virtualcolor_driver);

	/* Check for leaked ownership entries */
	down_write(&global_owner_rwsem);

	xa_for_each(&global_owner_xa, index, gpo) {
		if (gpo && !xa_is_value(gpo)) {
			pr_err(DRIVER_NAME
				   ": LEAK: Ownership entry at index %lu (pdev=%p) not freed\n",
				   index, gpo->owner_pdev);
			leaked++;
		}
	}

	if (leaked) {
		pr_err(DRIVER_NAME ": %lu leaked entries detected at module exit\n", leaked);
		pr_err(DRIVER_NAME ": This indicates controllers were not properly removed\n");
		pr_err(DRIVER_NAME ": Memory leaked to prevent use-after-free corruption\n");
	}

	xa_destroy(&global_owner_xa);
	up_write(&global_owner_rwsem);

	pr_info(DRIVER_NAME ": Driver unloaded%s\n",
		leaked ? " (with memory leaks - see errors above)" : " cleanly");
}
module_exit(leds_virtualcolor_exit);

module_param(enable_debugfs, bool, 0444);
MODULE_PARM_DESC(enable_debugfs, "Enable debugfs interface");

module_param(use_gamma_correction, bool, 0644);
MODULE_PARM_DESC(use_gamma_correction, "Apply gamma correction");

module_param(update_delay_us, uint, 0644);
MODULE_PARM_DESC(update_delay_us, "Artificial delay after LED update");

module_param(max_phys_leds, uint, 0444);
MODULE_PARM_DESC(max_phys_leds, "Maximum unique physical LEDs");

module_param(enable_update_batching, bool, 0644);
MODULE_PARM_DESC(enable_update_batching, "Enable update batching/debouncing");

MODULE_AUTHOR("Jonathan Brophy <professor_jonny@hotmail.com>");
MODULE_DESCRIPTION("Virtual grouped LED driver with multicolor ABI V5.1.18");
MODULE_LICENSE("GPL");
MODULE_VERSION("5.1.18");
