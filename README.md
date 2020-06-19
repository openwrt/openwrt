# Install openwrt via uboot web:

### **NB: need to flash transition image firstly, or it will be brick.**

The transition image is [**s1300-factory-to-openwrt.img**](https://github.com/gl-inet/openwrt/blob/openwrt-18.06-s1300/s1300-factory-to-openwrt.img)
when you install openwrt. So first time installation you need to access uboot web twice.
Afterwards, openwrt sysupgrade can be used.

- **Steps:**

1. Push reset button and plug in power, wifi led will blink, release button when middle led light on
2. Set ip to 192.168.1.0/24 and use web browser to access http://192.168.1.1
3. Choose file [**s1300-factory-to-openwrt.img**](https://github.com/gl-inet/openwrt/blob/openwrt-18.06-s1300/s1300-factory-to-openwrt.img) and  click "Update Firmware"
  (Steps 3 will consume about 30 seconds, then):
4. Poweroff router
5. Repeat step 1, 2, and when step 3 choose your compiled openwrt firmware file and update firmware
  (When update finished, router will reboot automatically)

- **Why need transition image**

The kernel and rootfs of factory firmware is on eMMC, and openwrt firmware is on NOR flash. 
The transition image includes U-boot and partition table, which decides where to load kernel and rootfs.
After you install openwrt image, you can switch between factory and openwrt firmware by flashing transition image. 
Go back to factory firmware use transition image [**s1300-openwrt-to-factory.img**](https://github.com/gl-inet/openwrt/blob/openwrt-18.06-s1300/s1300-openwrt-to-factory.img)
