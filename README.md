Fritzbox 7490 has two SOCs, similar to 3490, more information [here](http://www.aboehler.at/doku/doku.php/projects:fritz3490). Most credits go to him.
In the end there will be essentially two Openwrts running on the 7490. One on the Lantiq with access to DSL Modem, USB and Storage and the other one on the SOC with access to the two Wifis.
More information in the threads for [3490](https://forum.openwrt.org/t/port-to-avm-fritz-box-3490/52692) and [7490](https://forum.openwrt.org/t/support-fritzbox-7490/4112).

Prepare the OpenWrt Build environment on Linux as described [here](https://openwrt.org/docs/guide-developer/build-system/install-buildsystem).
Make also sure to install bspatch utility (bsdiff package).

The SSIDs for wireless are OpenWrt and OpenWrt-5G and the pre-shared key is extracted from urlader partition as printed on the sticker on the back of the device.

To create the images:
1. `git clone https://github.com/kestrel1974/openwrt.git`
2. `cd openwrt`
3. `git checkout fritzbox7490`
4. `scripts/feeds update`
5. `scripts/feeds install -a`
6. `make menuconfig` - first you need to build the wasp image for the SOC with the two wifis. So select ATH79 as target system, subtarget generic and target profile AVM FRITZ!Box 7490 WASP. To enable LUCI web GUI, Enable LUCI->Collections->luci (with Uhttpd), the minimum stuff is preselected.
7. `make`

Then the WASP image is ready. I then saved the config and started from scratch for the Lantiq image, which will also contain the WASP image plus some firmware files from the orginal AVM software. For that task I copied some code from freetz for the unquashfs-avm-be utility and created a new tool fritzboximage-downloadextractor (in tools). It also installs the vdsl firmware from the AVM image:

8. `cp .config .config_7490wasp`
9. `make menuconfig` - Select Lantiq as target system, subtarget XRX200 and target profile AVM FRITZ!Box 7490. To enable LUCI web GUI, Enable LUCI->Collections->luci (with Uhttpd), the minimum stuff is preselected.
10. `make`

Then the images are ready. Please note that if both images are too large, the ramboot process does not work anymore. Its around 11-13MB, when it does not work anymore.
To ramboot the image, follow the procedure mentioned [here for the 7362](https://openwrt.org/toh/avm/avm_7362_sl), essentially configure static address (e.g. 192.168.178.2) on a device connected to the 7490, power on the 7490, start ftp to 192.168.178.1 immediately after the initial blink of all leds, logon with adam2/adam2 and exit with bye. EVA-FTP-Client.ps1 from eva-tools will also work.

Run:

11. `scripts/flashing/eva_ramboot.py 192.168.178.1 bin/targets/lantiq/xrx200/openwrt-lantiq-xrx200-avm_fritz7490-initramfs-kernel.bin`

Booting for the Lantiq image is about 90-120s, then there will be additional 60-120 seconds for the WASP image. Then both Openwrt instances will be available at 192.168.1.1 and 192.168.1.2 (WASP). I have tried to automate startup, but in case there are timing issues and the WASP image does not start, then you can start WASP manually using the script /opt/wasp/upload-wasp.sh.
Have fun.

Problems:
* DECT and ISDN/POTS are not working and probably never will
* When the images are too large, the eva bootloader is stuck. I suspect this is due to the filesystem is embedded as initramfs in the kernel, whereas the freetz or AVM images have the rootfs appended to the kernel when using initramfs or image2ram. So if scripts/flashing/eva_ramboot.py hangs and does not time out, then the initramfs image is too big (should be < 16MB).
* I have not tried sysupgrade, so no responsibility from my side, if there is something not working. I have only tested the initramfs so far.
* Due to size issues, I have placed the vdsl annex b firmware only into the file system for the Lantiq.

For now I cannot put more work or time into it. Use it as is and I will not submit a pull request to Openwrt.

![OpenWrt logo](include/logo.png)

OpenWrt Project is a Linux operating system targeting embedded devices. Instead
of trying to create a single, static firmware, OpenWrt provides a fully
writable filesystem with package management. This frees you from the
application selection and configuration provided by the vendor and allows you
to customize the device through the use of packages to suit any application.
For developers, OpenWrt is the framework to build an application without having
to build a complete firmware around it; for users this means the ability for
full customization, to use the device in ways never envisioned.

Sunshine!

## Development

To build your own firmware you need a GNU/Linux, BSD or MacOSX system (case
sensitive filesystem required). Cygwin is unsupported because of the lack of a
case sensitive file system.

### Requirements

You need the following tools to compile OpenWrt, the package names vary between
distributions. A complete list with distribution specific packages is found in
the [Build System Setup](https://openwrt.org/docs/guide-developer/build-system/install-buildsystem)
documentation.

```
binutils bzip2 diff find flex gawk gcc-6+ getopt grep install libc-dev libz-dev
make4.1+ perl python3.6+ rsync subversion unzip which
```

### Quickstart

1. Run `./scripts/feeds update -a` to obtain all the latest package definitions
   defined in feeds.conf / feeds.conf.default

2. Run `./scripts/feeds install -a` to install symlinks for all obtained
   packages into package/feeds/

3. Run `make menuconfig` to select your preferred configuration for the
   toolchain, target system & firmware packages.

4. Run `make` to build your firmware. This will download all sources, build the
   cross-compile toolchain and then cross-compile the GNU/Linux kernel & all chosen
   applications for your target system.

### Related Repositories

The main repository uses multiple sub-repositories to manage packages of
different categories. All packages are installed via the OpenWrt package
manager called `opkg`. If you're looking to develop the web interface or port
packages to OpenWrt, please find the fitting repository below.

* [LuCI Web Interface](https://github.com/openwrt/luci): Modern and modular
  interface to control the device via a web browser.

* [OpenWrt Packages](https://github.com/openwrt/packages): Community repository
  of ported packages.

* [OpenWrt Routing](https://github.com/openwrt-routing/packages): Packages
  specifically focused on (mesh) routing.

## Support Information

For a list of supported devices see the [OpenWrt Hardware Database](https://openwrt.org/supported_devices)

### Documentation

* [Quick Start Guide](https://openwrt.org/docs/guide-quick-start/start)
* [User Guide](https://openwrt.org/docs/guide-user/start)
* [Developer Documentation](https://openwrt.org/docs/guide-developer/start)
* [Technical Reference](https://openwrt.org/docs/techref/start)

### Support Community

* [Forum](https://forum.openwrt.org): For usage, projects, discussions and hardware advise.
* [Support Chat](https://webchat.freenode.net/#openwrt): Channel `#openwrt` on freenode.net.

### Developer Community

* [Bug Reports](https://bugs.openwrt.org): Report bugs in OpenWrt
* [Dev Mailing List](https://lists.openwrt.org/mailman/listinfo/openwrt-devel): Send patches
* [Dev Chat](https://webchat.freenode.net/#openwrt-devel): Channel `#openwrt-devel` on freenode.net.

## License

OpenWrt is licensed under GPL-2.0
