// SPDX-License-Identifier: GPL-2.0-only
/* RTX830 FPGA controls, derived from the preceding GPL RTX830 Linux port.
 * Preserve its tested SPI0/CS2 protocol and restart preparation. GPIO keys
 * use the standard GPIO input driver; SPI flash and FPGA programming are absent.
 */
#include <linux/module.h>
#include "rtx830_board.h"
#include <linux/device.h>
#include <linux/gpio/consumer.h>
#include <linux/jiffies.h>
#include <linux/leds.h>
#include <linux/mutex.h>
#include <linux/reboot.h>
#include <linux/spi/spi.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

static struct spi_device *board_spi;
static u8 *spi_tx, *spi_rx;
static struct gpio_desc *power_switch;
static struct delayed_work power_work;
static bool stopping, poweroff_latched;
static unsigned int off_samples;
static unsigned long next_hold, next_poweroff;
static struct device *board_dev;
static DEFINE_MUTEX(board_lock);
static bool initialized, notifier_registered;
static bool boot_complete;
static bool restart_prepared;

bool rtx830_restart_is_prepared(void)
{
	return READ_ONCE(restart_prepared);
}
EXPORT_SYMBOL_GPL(rtx830_restart_is_prepared);

struct rtx830_led {
	struct led_classdev cdev;
	u8 mask;
};

static struct rtx830_led leds[3];
static unsigned int num_leds;
static const char * const names[] = {
	"rtx830:status", "rtx830:usb", "rtx830:microsd",
};

/* board_lock held. One message keeps CS2 asserted across both bytes.
 * Old CFG 0x1818 is mode 3, 8 bits, TCLK/16 (15.625 MHz).
 */
static int reg_transfer(u8 reg, bool write, u8 *value)
{
	struct spi_transfer transfers[] = {
		{ .tx_buf = spi_tx, .len = 1,
		  .delay = { .value = 2, .unit = SPI_DELAY_UNIT_USECS } },
		{ .tx_buf = spi_tx + 1, .rx_buf = spi_rx, .len = 1,
		  .delay = { .value = 2, .unit = SPI_DELAY_UNIT_USECS } },
	};
	int ret;

	if (reg != 2 && reg != 4 && reg != 22 &&
	    !(reg == 11 && write && (*value == 0 || *value == 0x82)) &&
	    !(reg == 24 && write && *value == 0))
		return -EINVAL;
	spi_tx[0] = (reg << 2) | (write ? 0 : 0x80);
	spi_tx[1] = write ? *value : 0xff;
	ret = spi_sync_transfer(board_spi, transfers, ARRAY_SIZE(transfers));
	if (!ret && !write)
		*value = spi_rx[0];
	return ret;
}

/* No GPIO key event: this input announces loss of the switched supply.
 * Refresh even during debounce; once confirmed, OFF cannot be cancelled.
 * The FPGA cuts power about 8 seconds after the last successful refresh.
 */
static void power_poll(struct work_struct *work)
{
	bool request_poweroff = false;
	u8 hold = 0;
	int value, ret;

	mutex_lock(&board_lock);
	if (stopping || !initialized)
		goto out;
	value = gpiod_get_value_cansleep(power_switch);
	if (value < 0) {
		dev_err_ratelimited(board_dev, "power-switch read failed: %d\n", value);
		if (!poweroff_latched)
			off_samples = 0;
	} else if (!poweroff_latched) {
		if (value) {
			if (++off_samples >= 3) {
				poweroff_latched = true;
				next_poweroff = jiffies;
				dev_info(board_dev, "power switch OFF; requesting orderly poweroff\n");
			}
		} else {
			off_samples = 0;
		}
	}
	if ((poweroff_latched || off_samples) &&
	    (!next_hold || time_after_eq(jiffies, next_hold))) {
		ret = reg_transfer(24, true, &hold);
		if (ret)
			dev_err_ratelimited(board_dev, "power hold refresh failed: %d\n", ret);
		next_hold = jiffies + msecs_to_jiffies(500);
	}
	/* The input may arrive before procd installs its shutdown signal handler.
	 * Retrying also remains harmless while a normal shutdown is in progress.
	 */
	if (poweroff_latched && time_after_eq(jiffies, next_poweroff)) {
		request_poweroff = true;
		next_poweroff = jiffies + 5 * HZ;
	}
	schedule_delayed_work(&power_work, msecs_to_jiffies(50));
out:
	mutex_unlock(&board_lock);
	if (request_poweroff)
		orderly_poweroff(true);
}

/* Only the measured power, indicator and restart bits may change. */
static int reg_update(u8 reg, u8 mask, u8 bits)
{
	u8 original = 0, value, desired, restored;
	int ret, rollback_write, rollback_read;

	if ((reg == 2 && (mask & ~3)) ||
	    (reg == 4 && (mask & ~7)) ||
	    (reg == 22 && !((mask == BIT(0) && bits == 0) ||
		(mask == BIT(2) && !(bits & ~BIT(2))))))
		return -EINVAL;
	if (reg != 2 && reg != 4 && reg != 22)
		return -EINVAL;
	ret = reg_transfer(reg, false, &original);
	if (ret)
		return ret;
	if ((reg == 22 && (original & ~(BIT(0) | BIT(2) | BIT(4)))) ||
	    original == 0xff)
		return -EINVAL;
	desired = (original & ~mask) | (bits & mask);
	if (desired == original)
		return 0;
	value = desired;
	ret = reg_transfer(reg, true, &value);
	if (!ret)
		ret = reg_transfer(reg, false, &value);
	if (!ret && value == desired)
		return 0;
	restored = original;
	rollback_write = reg_transfer(reg, true, &restored);
	rollback_read = reg_transfer(reg, false, &restored);
	pr_err("rtx830_board: reg%u update failed (%d), rollback=%d/%d value=%02x expected=%02x\n",
	       reg, ret, rollback_write, rollback_read, restored, original);
	return ret ? ret : -EIO;
}

static int led_set(struct led_classdev *cdev, enum led_brightness brightness)
{
	struct rtx830_led *led = container_of(cdev, struct rtx830_led, cdev);
	int ret;

	mutex_lock(&board_lock);
	ret = initialized ? reg_update(4, led->mask, brightness ? led->mask : 0) : -ENODEV;
	mutex_unlock(&board_lock);
	return ret;
}

/* POWER has a hardware blink/steady mode rather than a real OFF mode. */
static int boot_complete_set(const char *value, const struct kernel_param *param)
{
	bool ready;
	u8 previous;
	int ret = kstrtobool(value, &ready);

	if (ret)
		return ret;
	mutex_lock(&board_lock);
	/* Change POWER and STATUS in one locked operation; no separate timer. */
	ret = initialized ? reg_transfer(22, false, &previous) : -ENODEV;
	if (!ret)
		ret = reg_update(22, BIT(2), ready ? BIT(2) : 0);
	if (!ret) {
		ret = reg_update(4, BIT(0), ready ? 0 : BIT(0));
		if (ret && reg_update(22, BIT(2), previous & BIT(2)))
			pr_err("rtx830_board: POWER rollback after STATUS failure failed\n");
	}
	if (!ret) {
		boot_complete = ready;
		leds[0].cdev.brightness = ready ? 0 : 1;
	}
	mutex_unlock(&board_lock);
	return ret;
}

static const struct kernel_param_ops boot_complete_ops = {
	.set = boot_complete_set, .get = param_get_bool,
};
module_param_cb(boot_complete, &boot_complete_ops, &boot_complete, 0644);
MODULE_PARM_DESC(boot_complete, "0: POWER blinking/STATUS on; 1: POWER steady/STATUS off");

static int buzzer_set(const char *value, const struct kernel_param *param)
{
	unsigned int ms;
	u8 command;
	int ret = 0, stop_ret;

	if (kstrtouint(value, 10, &ms) || ms > 500)
		return -EINVAL;
	mutex_lock(&board_lock);
	if (!initialized) {
		mutex_unlock(&board_lock);
		return -ENODEV;
	}
	if (ms) {
		command = 0x82;
		ret = reg_transfer(11, true, &command);
		if (!ret)
			msleep(ms);
	}
	command = 0;
	stop_ret = reg_transfer(11, true, &command);
	if (stop_ret)
		pr_err("rtx830_board: buzzer stop failed: %d\n", stop_ret);
	mutex_unlock(&board_lock);
	return stop_ret ? stop_ret : ret;
}

static const struct kernel_param_ops buzzer_ops = { .set = buzzer_set };
module_param_cb(buzzer_ms, &buzzer_ops, NULL, 0200);
MODULE_PARM_DESC(buzzer_ms, "Explicit bounded short beep, 0..500 ms; 0 stops");

static int prepare_restart(struct notifier_block *nb, unsigned long action, void *data)
{
	int ret;

	if (action != SYS_RESTART)
		return NOTIFY_DONE;
	mutex_lock(&board_lock);
	/* An OFF request must finish at the hardware cutoff, not reboot into
	 * a writable root while the last hold-refresh grace is expiring.
	 * Check the live input too, covering OFF before polling debounces it.
	 */
	ret = initialized ? gpiod_get_value_cansleep(power_switch) : -ENODEV;
	if (poweroff_latched || ret > 0)
		ret = -ESHUTDOWN;
	if (!ret)
		ret = reg_update(22, BIT(0), 0);
	WRITE_ONCE(restart_prepared, !ret);
	mutex_unlock(&board_lock);
	if (ret)
		pr_err("rtx830_board: restart preparation failed: %d\n", ret);
	else
		pr_info("rtx830_board: restart flag cleared; POWER mode preserved\n");
	return NOTIFY_DONE;
}

static struct notifier_block reboot_nb = { .notifier_call = prepare_restart };

/* Called while the SPI controller is still usable. Stop polling before
 * teardown, then grant the hardware its final measured shutdown grace.
 */
static void board_stop(struct spi_device *spi)
{
	u8 hold = 0;
	int ret = 0;

	mutex_lock(&board_lock);
	stopping = true;
	mutex_unlock(&board_lock);
	cancel_delayed_work_sync(&power_work);
	mutex_lock(&board_lock);
	if (initialized && poweroff_latched)
		ret = reg_transfer(24, true, &hold);
	initialized = false;
	mutex_unlock(&board_lock);
	if (ret)
		dev_err(&spi->dev, "final power hold refresh failed: %d\n", ret);
}

static void board_cleanup(struct spi_device *spi)
{
	board_stop(spi);
	if (notifier_registered) {
		unregister_reboot_notifier(&reboot_nb);
		notifier_registered = false;
	}
	while (num_leds)
		led_classdev_unregister(&leds[--num_leds].cdev);
	mutex_lock(&board_lock);
	initialized = false;
	board_spi = NULL;
	board_dev = NULL;
	mutex_unlock(&board_lock);
}

static int board_probe(struct spi_device *spi)
{
	u8 snapshot = 0;
	unsigned int i;
	int ret;

	/* The DT binding fixes this board protocol; reject incompatible modes. */
	if (spi_get_chipselect(spi, 0) != 2 || spi->mode != SPI_MODE_3 ||
	    !spi->max_speed_hz || spi->max_speed_hz > 15625000 ||
	    (spi->bits_per_word && spi->bits_per_word != 8))
		return -EINVAL;
	spi->bits_per_word = 8;
	ret = spi_setup(spi);
	if (ret)
		return ret;
	mutex_lock(&board_lock);
	if (board_spi) {
		mutex_unlock(&board_lock);
		return -EBUSY;
	}
	/* SPI buffers must remain DMA-safe even if the controller changes. */
	spi_tx = devm_kmalloc(&spi->dev, 2, GFP_KERNEL);
	spi_rx = devm_kmalloc(&spi->dev, 1, GFP_KERNEL);
	if (!spi_tx || !spi_rx) {
		mutex_unlock(&board_lock);
		return -ENOMEM;
	}
	power_switch = devm_gpiod_get(&spi->dev, "power-switch", GPIOD_IN);
	if (IS_ERR(power_switch)) {
		ret = PTR_ERR(power_switch);
		mutex_unlock(&board_lock);
		return dev_err_probe(&spi->dev, ret, "power-switch GPIO unavailable\n");
	}
	board_spi = spi;
	board_dev = &spi->dev;
	stopping = false;
	poweroff_latched = false;
	off_samples = 0;
	next_hold = 0;
	next_poweroff = 0;
	boot_complete = false;
	WRITE_ONCE(restart_prepared, false);
	INIT_DELAYED_WORK(&power_work, power_poll);
	ret = reg_update(2, 3, 3);
	if (!ret)
		ret = reg_update(4, BIT(0), BIT(0));
	if (!ret)
		ret = reg_update(22, BIT(2), 0);
	if (!ret)
		ret = reg_transfer(4, false, &snapshot);
	initialized = !ret;
	mutex_unlock(&board_lock);
	if (ret)
		goto fail;
	for (i = 0; i < ARRAY_SIZE(leds); i++) {
		leds[i].mask = BIT(i);
		leds[i].cdev.name = names[i];
		leds[i].cdev.max_brightness = 1;
		leds[i].cdev.brightness = !!(snapshot & BIT(i));
		leds[i].cdev.brightness_set_blocking = led_set;
		leds[i].cdev.flags = LED_RETAIN_AT_SHUTDOWN;
		ret = led_classdev_register(board_dev, &leds[i].cdev);
		if (ret)
			goto fail;
		num_leds++;
	}
	ret = register_reboot_notifier(&reboot_nb);
	if (ret)
		goto fail;
	notifier_registered = true;
	schedule_delayed_work(&power_work, 0);
	dev_info(board_dev, "storage power, LEDs, buzzer, OFF input and restart preparation ready\n");
	return 0;
fail:
	board_cleanup(spi);
	return ret;
}

static const struct of_device_id board_of_match[] = {
	{ .compatible = "yamaha,rtx830-board" },
	{ }
};
MODULE_DEVICE_TABLE(of, board_of_match);
static const struct spi_device_id board_ids[] = {
	{ "rtx830-board", 0 },
	{ }
};
MODULE_DEVICE_TABLE(spi, board_ids);
static struct spi_driver board_driver = {
	.driver = {
		.name = "rtx830-board",
		.of_match_table = board_of_match,
	},
	.probe = board_probe,
	.remove = board_cleanup,
	.shutdown = board_stop,
	.id_table = board_ids,
};
module_spi_driver(board_driver);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RTX830 SPI FPGA LEDs, power-switch shutdown and restart preparation");
