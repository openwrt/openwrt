/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mdio.h>
#include <linux/mii.h>
#include <linux/module.h>
#include <linux/of_mdio.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/version.h>

MODULE_DESCRIPTION("MDIO by GPIO");
MODULE_AUTHOR("Winder Sung <winder@annapurnalabs.com>");
MODULE_LICENSE("GPL");

#define GPIO_MDC_PIN 0
#define GPIO_MDIO_PIN 1

#define ISIS_CHIP_ID 0x18
#define ISIS_CHIP_REG 0

#define MARVELL_CHIP_ID 0x1302

static void al_mdio_gpio_mdc_pulse(void) /*Clock line */
{
  /* 1 Mhz */
  udelay(1);
  gpio_set_value(GPIO_MDC_PIN, 0);
  udelay(1);
  gpio_set_value(GPIO_MDC_PIN, 1);
  udelay(1);
}

static void al_mdio_gpio_idle(void) {
  gpio_direction_output(GPIO_MDIO_PIN, 1);
  al_mdio_gpio_mdc_pulse();
}

static void al_mdio_gpio_preamble(void) {
  char i;

  gpio_direction_output(GPIO_MDIO_PIN, 1);
  /* Transmit al_mdio_gpio_preamble 11....11(32 bits) */
  for (i = 0; i < 32; i++)
    al_mdio_gpio_mdc_pulse();
  /* Transmit Start of Frame '01' */
  gpio_set_value(GPIO_MDIO_PIN, 0);
  al_mdio_gpio_mdc_pulse();
  gpio_set_value(GPIO_MDIO_PIN, 1);
  al_mdio_gpio_mdc_pulse();
}

static unsigned int al_mdio_gpio_phy_reg_read(char phy_addr, char phy_reg) {
  char i;
  u16 phy_val;

  al_mdio_gpio_preamble();
  /*OP Code 10*/
  gpio_direction_output(GPIO_MDIO_PIN, 1);
  al_mdio_gpio_mdc_pulse();
  gpio_set_value(GPIO_MDIO_PIN, 0);
  al_mdio_gpio_mdc_pulse();
  /*5 bits PHY addr*/
  for (i = 0; i < 5; i++) {
    if (phy_addr & 0x10) {
      gpio_set_value(GPIO_MDIO_PIN, 1);
    } else {
      gpio_set_value(GPIO_MDIO_PIN, 0);
    }
    al_mdio_gpio_mdc_pulse();
    phy_addr <<= 1;
  }
  /*5 bits PHY reg*/
  for (i = 0; i < 5; i++) {
    if (phy_reg & 0x10) {
      gpio_set_value(GPIO_MDIO_PIN, 1);
    } else {
      gpio_set_value(GPIO_MDIO_PIN, 0);
    }
    al_mdio_gpio_mdc_pulse();
    phy_reg <<= 1;
  }
  /*Turnaround Z*/
  gpio_set_value(GPIO_MDIO_PIN, 1);
  al_mdio_gpio_mdc_pulse();
  gpio_direction_input(GPIO_MDIO_PIN);
  /*Read 16 bits Data*/
  phy_val = 0x0000;
  for (i = 0; i < 16; i++) {
    al_mdio_gpio_mdc_pulse();
    if (1 == gpio_get_value(GPIO_MDIO_PIN))
      phy_val |= 0x0001;
    if (i < 15)
      phy_val <<= 1;
  }
  al_mdio_gpio_idle();
  gpio_direction_output(GPIO_MDIO_PIN, 1);

  return phy_val;
}

static int al_mdio_gpio_phy_reg_write(char phy_addr, char phy_reg,
                                      unsigned int phy_val) {
  char i;
  u16 Temp;

  al_mdio_gpio_preamble();
  /*OP Code 01*/
  gpio_direction_output(GPIO_MDIO_PIN, 0);
  al_mdio_gpio_mdc_pulse();
  gpio_set_value(GPIO_MDIO_PIN, 1);
  al_mdio_gpio_mdc_pulse();
  /*5 bits PHY addr*/
  for (i = 0; i < 5; i++) {
    if (phy_addr & 0x10) {
      gpio_set_value(GPIO_MDIO_PIN, 1);
    } else {
      gpio_set_value(GPIO_MDIO_PIN, 0);
    }
    al_mdio_gpio_mdc_pulse();
    phy_addr <<= 1;
  }
  /*5 bits PHY reg*/
  for (i = 0; i < 5; i++) {
    if (phy_reg & 0x10) {
      gpio_set_value(GPIO_MDIO_PIN, 1);
    } else {
      gpio_set_value(GPIO_MDIO_PIN, 0);
    }
    al_mdio_gpio_mdc_pulse();
    phy_reg <<= 1;
  }
  /*Turnaround 10*/
  gpio_set_value(GPIO_MDIO_PIN, 1);
  al_mdio_gpio_mdc_pulse();
  gpio_set_value(GPIO_MDIO_PIN, 0);
  al_mdio_gpio_mdc_pulse();
  /*Write 16 bits Data*/
  Temp = 0x8000;
  for (i = 0; i < 16; i++) {
    if (phy_val & Temp) {
      gpio_set_value(GPIO_MDIO_PIN, 1);
    } else {
      gpio_set_value(GPIO_MDIO_PIN, 0);
    }
    al_mdio_gpio_mdc_pulse();
    Temp >>= 1;
  }
  al_mdio_gpio_idle();

  return 0;
}

static int al_mdio_gpio_read(struct mii_bus *bus, int mii_id, int reg) {
  return al_mdio_gpio_phy_reg_read(mii_id, reg);
}

static int al_mdio_gpio_write(struct mii_bus *bus, int mii_id, int reg,
                              u16 val) {
  return al_mdio_gpio_phy_reg_write(mii_id, reg, val);
}

static struct mii_bus *al_mdio_gpio_init(struct device *dev) {
  struct mii_bus *bus;
  u16 phy_val;
  int ret;

  /* MDC */
  ret = gpio_request(GPIO_MDC_PIN, "gpio_as_mdc");
  if (ret)
    return NULL;

  gpio_direction_output(GPIO_MDC_PIN, 1);

  /* MDIO */
  ret = gpio_request(GPIO_MDIO_PIN, "gpio_as_mdio");
  if (ret)
    goto fail_free_mdc_pin;

  gpio_direction_output(GPIO_MDIO_PIN, 0);

  al_mdio_gpio_phy_reg_write(ISIS_CHIP_ID, ISIS_CHIP_REG, 0x0);
  phy_val = al_mdio_gpio_phy_reg_read(0x10, 0x0);
  if (phy_val != MARVELL_CHIP_ID)
    goto fail_free_mdio_pin;

  bus = mdiobus_alloc();
  if (bus == NULL)
    goto fail_free_mdio_pin;

  bus->parent = dev;
  bus->name = "mdio-al-gpio";
  snprintf(bus->id, MII_BUS_ID_SIZE, "%s-0", bus->name);
  bus->read = &al_mdio_gpio_read;
  bus->write = &al_mdio_gpio_write;

  dev_set_drvdata(dev, bus);

  return bus;

fail_free_mdio_pin:
  gpio_free(GPIO_MDIO_PIN);

fail_free_mdc_pin:
  gpio_free(GPIO_MDC_PIN);

  return NULL;
}

static void al_mdio_gpio_deinit(struct device *dev) {
  struct mii_bus *bus = dev_get_drvdata(dev);

  mdiobus_free(bus);
  gpio_free(GPIO_MDC_PIN);
  gpio_free(GPIO_MDIO_PIN);
}

static void al_mdio_gpio_destroy(struct device *dev) {
  struct mii_bus *bus = dev_get_drvdata(dev);

  mdiobus_unregister(bus);
  al_mdio_gpio_deinit(dev);
}

static int al_mdio_gpio_probe(struct platform_device *pdev) {
  struct mii_bus *bus;
  int ret;

  bus = al_mdio_gpio_init(&pdev->dev);
  if (!bus)
    return -ENODEV;

  ret = of_mdiobus_register(bus, pdev->dev.of_node);
  if (ret)
    al_mdio_gpio_deinit(&pdev->dev);

  return ret;
}

static void al_mdio_gpio_remove(struct platform_device *pdev) {
  al_mdio_gpio_destroy(&pdev->dev);
}

static const struct of_device_id al_mdio_gpio_of_match[] = {
    {
        .compatible = "annapurna-labs,mdio-al-gpio",
    },
    {/* sentinel */}};
MODULE_DEVICE_TABLE(of, al_mdio_gpio_of_match);

static struct platform_driver al_mdio_gpio_driver = {
    .probe = al_mdio_gpio_probe,
    .remove = al_mdio_gpio_remove,
    .driver =
        {
            .name = "mdio-al-gpio",
            .of_match_table = al_mdio_gpio_of_match,
        },
};

module_platform_driver(al_mdio_gpio_driver);
