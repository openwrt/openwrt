# Mediatek EIP93 Crypto Engine

Linux Crypto Driver for the EIP-93. This Crypto engine is
available in the Mediatek MT7621 SoC.

This should be added to your device DTS or better yet to the mt7621.dtsi:

	crypto: crypto@1e004000 {
		status = "okay";

		compatible = "mediatek,mtk-eip93";
		reg = <0x1e004000 0x1000>;
		
		interrupt-parent = <&gic>;
		interrupts = <GIC_SHARED 19 IRQ_TYPE_LEVEL_HIGH>;
	};

It enables hardware crypto for:
* des ecb/cbc
* 3des ecb/cbc
* aes ecb / cbc / ctr /rfc3686 with 128/192/256 keysize.

Authentication:
* authenc(hmac(md5/sha1/sha224/sha256), des / 3des - cbc)
* authenc(hmac(md5/sha1/224/256, des / 3des - cbc)
* authenc(hmac(sha1/sha256), cbc / rfc3686(ctr) - aes) with 128/192/256 keysize

Testing has been done on Linux Kernel v5.10.76 with all the extended tests enabled.

copy or clone into "package/kernel"

For ESP HW offload: copy the 999-add-esp-hw-offload.patch to "target/linux/ramips/patches-5.10"
in the OpenWrt build system. This is a work-around to add XFRM_OFFLOAD to ESP4.
DO NOT! use esp4/6-offload from the kernel: this will replace it!

Experimental feature: Use Polling vs IRQ should not be used together with ESP HW offload.
it is intended to be used when you want to use the driver from userspace e.g. with openssl and /dev/crypto

For best performance move the interupt to another CPU. On the MT7621A I suggest moving it to CPU2:
cat /proc/interrupt

echo 4 >/proc/irq/21/smp_affinity (replace 21 with another number if its different, like on the MT7621S)

TODO:
finish the ansi prng implementation.

finish ahash implemention for simple sha1/256 and hmac(sha1/sha256).
