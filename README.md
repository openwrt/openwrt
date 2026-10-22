**English** | [Tiếng Việt](README.vi.md)

![OpenWrt logo](include/logo.png)

Unofficial [**OpenWrt**](https://openwrt.org/) for **FPT AX3000CV2 (also known as Actiontec/CIG WF-810DF)**.  
See the official OpenWrt README for general build instructions and project information: [click here](https://github.com/openwrt/openwrt/blob/master/README.md)

This build mainly targets the upcoming [**OpenWrt 25.12 mainstream release**](https://github.com/openwrt/openwrt/tree/openwrt-25.12). We keep all changes as close to upstream OpenWrt as possible.

Additional work may include device bring-up, hardware validation, driver fixes, calibration data integration, LED and GPIO mapping, and performance tuning specific to the AX3000CV2 platform.

> [!CAUTION]
> **FOR EDUCATIONAL AND RESEARCH PURPOSES ONLY**
>
> This firmware is provided **“AS IS”** strictly for **educational and research purposes**.  
> **Use it entirely at your own risk.**

## Specifications

- **SoC:** Qualcomm IPQ5018
- **RAM:** [512 MB NYANA `NT5CC256M16ER-EK`](https://www.nanya.com/en/product/4248/NT5CC256M16ER-EK)
- **NAND:** [256 MB GigaDevice `GD5F2GM7REYIG`](https://www.gigadevice.com/product/flash/spi-nand-flash/gd5f2gm7re)
- **Wi-Fi:**
  - Internal (2.4GHz)
  - Qualcomm QCN6102 (5GHz AX)
- **Ethernet:**
  - [3x ports 1GbE Switch Motorcomm YT9215S](https://en.motor-comm.com/download?kw=&category=135&wd=1&tp=1)
  - 1x port 1GbE WAN Internal

## Hardware working status

- [x] Internal IPQ5018 WAN
- [x] 3x 1GbE Switch Motorcomm YT9215S (thanks [@ppptran](https://github.com/ppptran))
- [x] 2.4GHz Wi-Fi (SoC)
- [x] 5GHz Wi-Fi (QCN6102)
- [x] NAND Flash
- [x] LEDs (Red/White)
- [x] Buttons (Reset/WPS (aka Mesh button))

### Known issues

- NSS not working, so no hardware offloading for routing and Wi-Fi.

## External resources

These resources were dumped from the original firmware:

- WLAN Firmware `WLAN.HK.2.8-01357-QCAHKSWPL_SILICONZ-1.56794.4 v1`
  - [IPQ5081](https://github.com/michioxd/upstream-wifi-fw/tree/main/ath11k-firmware/IPQ5018_QCN6122_QCN6122/hw1.0/2.8/WLAN.HK.2.8-01357-QCAHKSWPL_SILICONZ-1.56794.4)
  - [QCN6122  (usable on QCN6102)](https://github.com/michioxd/upstream-wifi-fw/tree/main/ath11k-firmware/IPQ5018_QCN6122_QCN6122/hw1.0/2.8/WLAN.HK.2.8-01357-QCAHKSWPL_SILICONZ-1.56794.4/qcn6122)
- WLAN Calibration data:
  - [WF-810DF 2.4GHz](https://github.com/michioxd/upstream-wifi-fw/blob/main/ath11k-firmware/IPQ5018_QCN6122_QCN6122/hw1.0/2.8/WLAN.HK.2.8-01357-QCAHKSWPL_SILICONZ-1.56794.4/cal-ahb-c000000.wifi.bin)
  - [WF-810DF 5GHz](https://github.com/michioxd/upstream-wifi-fw/blob/main/ath11k-firmware/IPQ5018_QCN6122_QCN6122/hw1.0/2.8/WLAN.HK.2.8-01357-QCAHKSWPL_SILICONZ-1.56794.4/qcn6122/cal-ahb-b00a040.wifi.bin)
- WLAN Board data:
  - [WF-810DF 2.4GHz](https://github.com/michioxd/upstream-wifi-fw/blob/main/ath11k-firmware/IPQ5018_QCN6122_QCN6122/hw1.0/2.8/WLAN.HK.2.8-01357-QCAHKSWPL_SILICONZ-1.56794.4/board-2.bin)
  - [WF-810DF 5GHz](https://github.com/michioxd/upstream-wifi-fw/blob/main/ath11k-firmware/IPQ5018_QCN6122_QCN6122/hw1.0/2.8/WLAN.HK.2.8-01357-QCAHKSWPL_SILICONZ-1.56794.4/qcn6122/board-2.bin)

## Download

Check out the latest version in the [Releases](https://github.com/michioxd/openwrt-wf810df/releases/latest) section.

## Installation

> [!CAUTION]
> **IMPORTANT DISCLAIMER – READ CAREFULLY**
>
> This firmware is provided **AS IS** and is intended for **advanced users only**.  
> **USE IT ENTIRELY AT YOUR OWN RISK.**
>
> I **explicitly disclaim all responsibility and liability** for any damage, data loss, device malfunction, or other issues that may arise from its use.
> This includes, but is **not limited to**, **permanently bricked devices**, system instability, loss of functionality, or irreversible hardware/software damage.
>
> By proceeding, you **acknowledge that you fully understand the risks involved**, accept that firmware flashing is inherently dangerous, and agree that **you alone are solely and fully responsible for all consequences**, whether expected or unexpected.
>
> **If you do not fully understand what you are doing, STOP NOW and DO NOT CONTINUE.**

<details>
<summary>I'm sure, lemme "rock" my device</summary>

### Prerequisites

Currently, you must to use U-Boot to flash the firmware. Flash directly from the original firmware is not supported (at least for now).

- FPT AX3000CV2 running stock QSDK firmware.
- Access to the U-Boot command line (via UART).
  <details>
  <summary>Check out my setup</summary>
  ESP32 used as a UART bridge

  ![img](https://github.com/user-attachments/assets/3e4140af-9982-4330-b0f6-5b8d51853c84)

  Code to use ESP32 as UART bridge:

  ```c
  #include <HardwareSerial.h>
  
  #define RX 14
  #define TX 13
  
  HardwareSerial sr(2);
  
  void setup() {
    Serial.begin(115200);
    sr.begin(115200, SERIAL_8N1, RX, TX);
  }
  
  void loop() {
    while (Serial.available()) sr.write(Serial.read());
    while (sr.available())     Serial.write(sr.read());
  }
  ```
  
  </details>
- A computer with TFTP software (Tftpd or similar), and you must know your PC/Gateway IP address.

### 0. Back up the original firmware (dump the entire NAND)

> **Save your stock firmware!**
>
> Do not skip this step.  
> You may think it is unnecessary since you do not plan to return to the “dark past”, but I assure you that the stock firmware is still useful for restoring missing settings, drivers, and for further research.

1. Power on the device and repeatedly press the **Escape (ESC)** key to enter the U-Boot command-line interface.
2. Set the environment variables so U-Boot can connect to the local network and locate your TFTP server:
   ```sh
   # Adjust these values to match your setup
   setenv ipaddr 192.168.2.240
   setenv gatewayip 192.168.2.1
   setenv serverip 192.168.2.104
   # Save to NAND for later use or debugging
   saveenv
   ```
3. For safety, the dump will be split into 4 parts. Each part is 64 MB, since the NAND size is 256 MB.
   ```sh
   mw.b 0x44000000 0xff 0x4000000
   nand read 0x44000000 0x0 0x4000000
   tftpput 0x44000000 0x4000000 part1.bin

   mw.b 0x44000000 0xff 0x4000000
   nand read 0x44000000 0x4000000 0x4000000
   tftpput 0x44000000 0x4000000 part2.bin

   mw.b 0x44000000 0xff 0x4000000
   nand read 0x44000000 0x8000000 0x4000000
   tftpput 0x44000000 0x4000000 part3.bin

   mw.b 0x44000000 0xff 0x4000000
   nand read 0x44000000 0xC000000 0x4000000
   tftpput 0x44000000 0x4000000 part4.bin
   ```
4. If you wanna restore the original firmware:
   ```sh
   tftpboot 0x44000000 part1.bin
   nand erase 0x0 0x4000000
   nand write 0x44000000 0x0 0x4000000

   tftpboot 0x44000000 part2.bin
   nand erase 0x4000000 0x4000000
   nand write 0x44000000 0x4000000 0x4000000

   tftpboot 0x44000000 part3.bin
   nand erase 0x8000000 0x4000000
   nand write 0x44000000 0x8000000 0x4000000

   tftpboot 0x44000000 part4.bin
   nand erase 0xC000000 0x4000000
   nand write 0x44000000 0xC000000 0x4000000
   ```

### 1. Flash firmware

1. Erase the old firmware, starting from `0xd00000` (~13 MB) with a size of `0xf300000` (~249 MB):
   ```
   nand scrub 0xd00000 0xf300000
   ```
   If prompted, just press `y` and then **Enter**.
2. Make sure you have downloaded the file ending with `...-qualcommax-ipq50xx-fpt_ax3000cv2-squashfs-factory.ubi` from the [Releases](https://github.com/michioxd/openwrt-wf810df/releases/latest) page. Rename it to something easier to type. For example: `update.ubi`.
4. Load the image into memory:
   ```
   tftpboot 0x44000000 update.ubi
   ```
5. Flash to NAND
   ```sh
   nand write 0x44000000 0xd00000 ${filesize}
   ```
   Wait a moment for the process to finish.

### 2. Set the startup environment

After flashing the image, you need to create a custom boot command for U-Boot. The `bootipq` command uses fixed partitions, so it does not recognize our firmware.

```sh
setenv mtdids nand0=nand0
setenv bootargs "ubi.mtd=20 root=/dev/ubiblock0_1 rootfstype=squashfs rootwait"
setenv bootowrt "setenv mtdparts mtdparts=nand0:0xf300000@0xd00000(rootfs); ubi part rootfs; ubi read 0x44000000 kernel; bootm 0x44000000"
setenv bootcmd "run bootowrt"
saveenv
```

Now you can use `run bootowrt` to boot into the new firmware, or simply run `reset` to reboot :)

</details>

## License

OpenWrt License: [GPL-2.0](https://openwrt.org/license)

This repository:  GPL-2.0

## Thanks

- OpenWrt Project and all its contributors
- Contributors of [this repository](https://github.com/michioxd/openwrt-wf810df/graphs/contributors)
