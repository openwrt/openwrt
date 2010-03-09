#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/power_supply.h>
#include <linux/suspend.h>

#include <linux/i2c.h>

#include <asm/mach-jz4740/irq.h>
#include <asm/mach-jz4740/gpio.h>
#include <asm/mach-jz4740/board-n516.h>

static int batt_level=0;
module_param(batt_level, int, 0);

struct n516_lpc_chip {
	struct i2c_client	*i2c_client;
	struct input_dev	*input;
	unsigned int		battery_level;
	unsigned int		suspending:1, can_sleep:1;
};

static struct n516_lpc_chip *the_lpc;

struct i2c_device_id n516_lpc_i2c_ids[] = {
	{"LPC524", 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, n516_lpc_i2c_ids);

static const unsigned short normal_i2c[] = {0x54, I2C_CLIENT_END};

static const unsigned int n516_lpc_keymap[] = {
	[0x01] = KEY_4,
	[0x02] = KEY_3,
	[0x03] = KEY_2,
	[0x04] = KEY_1,
	[0x05] = KEY_0,
	[0x07] = KEY_9,
	[0x08] = KEY_8,
	[0x09] = KEY_7,
	[0x0a] = KEY_6,
	[0x0b] = KEY_5,
	[0x0d] = KEY_PLAYPAUSE,
	[0x0e] = KEY_MENU,
	[0x0f] = KEY_SEARCH,
	[0x10] = KEY_DIRECTION,
	[0x11] = KEY_SPACE,
	[0x13] = KEY_ENTER,
	[0x14] = KEY_UP,
	[0x15] = KEY_DOWN,
	[0x16] = KEY_RIGHT,
	[0x17] = KEY_LEFT,
	[0x19] = KEY_PAGEDOWN,
	[0x1a] = KEY_PAGEUP,
	[0x1c] = KEY_POWER,
	[0x1d] = KEY_ESC,
	[0x1e] = KEY_SLEEP,
	[0x1f] = KEY_WAKEUP,
};

static const unsigned int batt_charge[] = {0, 7, 20, 45, 65, 80, 100};
#define MAX_BAT_LEVEL	6

/* Insmod parameters */
I2C_CLIENT_INSMOD_1(n516_lpc);

static inline int n516_bat_charging(void)
{
	return !gpio_get_value(GPIO_CHARG_STAT_N);
}

static int n516_bat_get_status(struct power_supply *b)
{
	if (power_supply_am_i_supplied(b)) {
		if (n516_bat_charging())
			return POWER_SUPPLY_STATUS_CHARGING;
		else
			return POWER_SUPPLY_STATUS_FULL;
	} else {
		return POWER_SUPPLY_STATUS_DISCHARGING;
	}
}

static int n516_bat_get_charge(struct power_supply *b)
{
	return batt_charge[the_lpc->battery_level];
}

static int n516_bat_get_property(struct power_supply *b,
		enum power_supply_property psp,
		union power_supply_propval *val)
{
	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = n516_bat_get_status(b);
		break;
	case POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN:
		val->intval = 100;
		break;
	case POWER_SUPPLY_PROP_CHARGE_EMPTY_DESIGN:
		val->intval = 0;
		break;
	case POWER_SUPPLY_PROP_CHARGE_NOW:
		val->intval = n516_bat_get_charge(b);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static void n516_bat_power_changed(struct power_supply *p)
{
	if (power_supply_am_i_supplied(p) && !n516_bat_charging())
		the_lpc->battery_level = MAX_BAT_LEVEL;

	power_supply_changed(p);
}

static enum power_supply_property n516_bat_properties[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN,
	POWER_SUPPLY_PROP_CHARGE_EMPTY_DESIGN,
	POWER_SUPPLY_PROP_CHARGE_NOW,
};

static struct power_supply n516_battery = {
	.name		= "n516-battery",
	.get_property	= n516_bat_get_property,
	.properties	= n516_bat_properties,
	.num_properties	= ARRAY_SIZE(n516_bat_properties),
	.external_power_changed = n516_bat_power_changed,
};

static irqreturn_t n516_bat_charge_irq(int irq, void *dev)
{
	struct power_supply *psy = dev;

	dev_dbg(psy->dev, "Battery charging IRQ\n");

	if (power_supply_am_i_supplied(psy) && !n516_bat_charging())
		the_lpc->battery_level = MAX_BAT_LEVEL;

	power_supply_changed(psy);

	return IRQ_HANDLED;
}

static int n516_lpc_send_message(struct n516_lpc_chip *chip, unsigned char val)
{
	struct i2c_client *client = chip->i2c_client;
	struct i2c_msg msg = {client->addr, client->flags, 1, &val};
	int ret = 0;

	ret = i2c_transfer(client->adapter, &msg, 1);
	return ret > 0 ? 0 : ret;
}

static void n516_key_event(struct n516_lpc_chip *chip, unsigned char keycode)
{
	struct i2c_client *client = chip->i2c_client;
	bool long_press = false;

	if (keycode & 0x40) {
		keycode &= ~0x40;
		long_press = true;
	}

	dev_dbg(&client->dev, "keycode: 0x%02x, long_press: 0x%02x\n", keycode, (unsigned int)long_press);

	if (keycode >= ARRAY_SIZE(n516_lpc_keymap) || n516_lpc_keymap[keycode] == 0)
		return;

	if (long_press)
		input_report_key(chip->input, KEY_LEFTALT, 1);

	input_report_key(chip->input, n516_lpc_keymap[keycode], 1);
	input_sync(chip->input);
	input_report_key(chip->input, n516_lpc_keymap[keycode], 0);

	if (long_press)
		input_report_key(chip->input, KEY_LEFTALT, 0);
	input_sync(chip->input);
}

static void n516_battery_event(struct n516_lpc_chip *chip, unsigned char battery_level)
{
	if (battery_level != chip->battery_level) {
		chip->battery_level = battery_level;
		power_supply_changed(&n516_battery);
	}
}

static irqreturn_t n516_lpc_irq_thread(int irq, void *devid)
{
	struct n516_lpc_chip *chip = (struct n516_lpc_chip*)devid;
	int ret;
	unsigned char raw_msg;
	struct i2c_client *client = chip->i2c_client;
	struct i2c_msg msg = {client->addr, client->flags | I2C_M_RD, 1, &raw_msg};

	if (client->dev.power.status != DPM_ON)
		return IRQ_HANDLED;

	ret = i2c_transfer(client->adapter, &msg, 1);
	if (ret != 1) {
		dev_dbg(&client->dev, "I2C error: %d\n", ret);
		return IRQ_HANDLED;
	}

	dev_dbg(&client->dev, "msg: 0x%02x\n", raw_msg);

	/* Ack wakeup event */
	if ((raw_msg & ~0x40) < ARRAY_SIZE(n516_lpc_keymap))
		n516_key_event(chip, raw_msg);
	else if ((raw_msg >= 0x81) && (raw_msg <= 0x87))
		n516_battery_event(chip, raw_msg - 0x81);
	else if (raw_msg == 0x7e)
		n516_lpc_send_message(chip, 0x00);
	else
		dev_warn(&client->dev, "Unknown message: %x\n", raw_msg);

	if (chip->suspending)
		chip->can_sleep = 0;

	return IRQ_HANDLED;
}

static void n516_lpc_power_off(void)
{
	struct i2c_client *client = the_lpc->i2c_client;
	unsigned char val = 0x01;
	struct i2c_msg msg = {client->addr, client->flags, 1, &val};

	printk("Issue LPC POWEROFF command...\n");
	while (1)
		i2c_transfer(client->adapter, &msg, 1);
}

static int n516_lpc_detect(struct i2c_client *client, int kind, struct i2c_board_info *info)
{
	return 0;
}

static int n516_lpc_suspend_notifier(struct notifier_block *nb,
		                                unsigned long event,
						void *dummy)
{
	switch(event) {
	case PM_SUSPEND_PREPARE:
		the_lpc->suspending = 1;
		the_lpc->can_sleep = 1;
		break;
	case PM_POST_SUSPEND:
		the_lpc->suspending = 0;
		the_lpc->can_sleep = 1;
		break;
	default:
		return NOTIFY_DONE;
	}
	return NOTIFY_OK;
}

static struct notifier_block n516_lpc_notif_block = {
	.notifier_call = n516_lpc_suspend_notifier,
};

static int __devinit n516_lpc_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct n516_lpc_chip *chip;
	struct input_dev *input;
	int ret = 0;
	int i;

	chip = kzalloc(sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	the_lpc = chip;
	chip->i2c_client = client;
	if ((batt_level > 0) && (batt_level < ARRAY_SIZE(batt_charge)))
		chip->battery_level = batt_level;
	else
		chip->battery_level = 1;

	i2c_set_clientdata(client, chip);

	ret = gpio_request(GPIO_LPC_INT, "LPC interrupt request");
	if (ret) {
		dev_err(&client->dev, "Unable to reguest LPC INT GPIO\n");
		goto err_gpio_req_lpcint;
	}

	ret = gpio_request(GPIO_CHARG_STAT_N, "LPC charging status");
	if (ret) {
		dev_err(&client->dev, "Unable to reguest CHARG STAT GPIO\n");
		goto err_gpio_req_chargstat;
	}

	/* Enter normal mode */
	n516_lpc_send_message(chip, 0x2);

	input = input_allocate_device();
	if (!input) {
		dev_err(&client->dev, "Unable to allocate input device\n");
		ret = -ENOMEM;
		goto err_input_alloc;
	}

	chip->input = input;

	__set_bit(EV_KEY, input->evbit);

	for (i = 0; i < ARRAY_SIZE(n516_lpc_keymap); i++)
		__set_bit(n516_lpc_keymap[i], input->keybit);

	__set_bit(KEY_LEFTALT, input->keybit);

	input->name = "n516-keys";
	input->phys = "n516-keys/input0";
	input->dev.parent = &client->dev;
	input->id.bustype = BUS_I2C;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0100;

	ret = input_register_device(input);
	if (ret < 0) {
		dev_err(&client->dev, "Unable to register input device\n");
		goto err_input_register;
	}

	ret = power_supply_register(NULL, &n516_battery);
	if (ret) {
		dev_err(&client->dev, "Unable to register N516 battery\n");
		goto err_bat_reg;
	}

	ret = request_threaded_irq(gpio_to_irq(GPIO_LPC_INT), NULL,
					n516_lpc_irq_thread,
					IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
					"lpc", chip);
	if (ret) {
		dev_err(&client->dev, "request_irq failed: %d\n", ret);
		goto err_request_lpc_irq;
	}

	ret = request_irq(gpio_to_irq(GPIO_CHARG_STAT_N), n516_bat_charge_irq,
				IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
				"battery charging", &n516_battery);
	if (ret) {
		dev_err(&client->dev, "Unable to claim battery charging IRQ\n");
		goto err_request_chrg_irq;
	}

	pm_power_off = n516_lpc_power_off;
	ret = register_pm_notifier(&n516_lpc_notif_block);
	if (ret) {
		dev_err(&client->dev, "Unable to register PM notify block\n");
		goto err_reg_pm_notifier;
	}

	device_init_wakeup(&client->dev, 1);

	return 0;

	unregister_pm_notifier(&n516_lpc_notif_block);
err_reg_pm_notifier:
	free_irq(gpio_to_irq(GPIO_CHARG_STAT_N), &n516_battery);
err_request_chrg_irq:
	free_irq(gpio_to_irq(GPIO_LPC_INT), chip);
err_request_lpc_irq:
	power_supply_unregister(&n516_battery);
err_bat_reg:
	input_unregister_device(input);
err_input_register:
	input_free_device(input);
err_input_alloc:
	gpio_free(GPIO_CHARG_STAT_N);
err_gpio_req_chargstat:
	gpio_free(GPIO_LPC_INT);
err_gpio_req_lpcint:
	i2c_set_clientdata(client, NULL);
	kfree(chip);

	return ret;
}

static int __devexit n516_lpc_remove(struct i2c_client *client)
{
	struct n516_lpc_chip *chip = i2c_get_clientdata(client);

	unregister_pm_notifier(&n516_lpc_notif_block);
	pm_power_off = NULL;
	free_irq(gpio_to_irq(GPIO_CHARG_STAT_N), &n516_battery);
	free_irq(gpio_to_irq(GPIO_LPC_INT), chip);
	power_supply_unregister(&n516_battery);
	input_unregister_device(chip->input);
	gpio_free(GPIO_CHARG_STAT_N);
	gpio_free(GPIO_LPC_INT);
	i2c_set_clientdata(client, NULL);
	kfree(chip);

	return 0;
}

#if CONFIG_PM
static int n516_lpc_suspend(struct i2c_client *client, pm_message_t msg)
{
	if (!the_lpc->can_sleep)
		return -EBUSY;

	if (device_may_wakeup(&client->dev))
		enable_irq_wake(gpio_to_irq(GPIO_LPC_INT));

	return 0;
}

static int n516_lpc_resume(struct i2c_client *client)
{
	if (device_may_wakeup(&client->dev))
		disable_irq_wake(gpio_to_irq(GPIO_LPC_INT));

	return 0;
}
#else
#define n516_lpc_suspend NULL
#define n516_lpc_resume NULL
#endif


static struct i2c_driver n516_lpc_driver = {
	.class		= I2C_CLASS_HWMON,
	.driver		= {
		.name	= "n516-keys",
		.owner	= THIS_MODULE,
	},
	.probe		= n516_lpc_probe,
	.remove		= __devexit_p(n516_lpc_remove),
	.detect		= n516_lpc_detect,
	.id_table	= n516_lpc_i2c_ids,
	.address_data	= &addr_data,
	.suspend	= n516_lpc_suspend,
	.resume		= n516_lpc_resume,
};

static int __init n516_lpc_init(void)
{
	return i2c_add_driver(&n516_lpc_driver);
}

static void __exit n516_lpc_exit(void)
{
	i2c_del_driver(&n516_lpc_driver);
}


module_init(n516_lpc_init);
module_exit(n516_lpc_exit);

MODULE_AUTHOR("Yauhen Kharuzhy");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Keys and power controller driver for N516");
MODULE_ALIAS("platform:n516-keys");

