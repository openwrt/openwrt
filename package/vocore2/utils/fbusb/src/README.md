# Compile Example

make -C /Volumes/OpenWrt/vocore2/build_dir/target-mipsel_24kc_musl/linux-ramips_mt76x8/linux-4.14.131 M=$PWD CROSS_COMPILE="/Volumes/OpenWrt/vocore2/staging_dir/toolchain-mipsel_24kc_gcc-7.3.0_musl/bin/mipsel-openwrt-linux-" ARCH="mips" modules

because framebuffer only support 16bit or 32bit color, so it is very slow to convert data from 32bit to 24bit or from 16bit to 24bit, new version solve this problem.
If you want better colors, need to use libusb send data to the screen, and make it in R-G-B-R-G-B... order. R,G,B each takes one byte.
