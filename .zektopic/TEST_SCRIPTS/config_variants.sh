#!/bin/bash
# config_variants.sh — Apply different OpenWrt config variants for x86/64
# Usage: ./config_variants.sh <variant>
#   variant: default | minimal | full | dev | hardened

set -e
cd /home/manu/openwrt

case "${1:-default}" in
  default)
    echo "=== x86/64 Default Config ==="
    cat > .config << 'EOF'
CONFIG_TARGET_x86=y
CONFIG_TARGET_x86_64=y
CONFIG_TARGET_x86_64_DEVICE_generic=y
CONFIG_TARGET_ROOTFS_EXT4FS=y
CONFIG_TARGET_ROOTFS_SQUASHFS=y
CONFIG_TARGET_ROOTFS_TARGZ=y
CONFIG_TARGET_IMAGES_GZIP=y
EOF
    make defconfig
    ;;

  minimal)
    echo "=== x86/64 Minimal Config (base only) ==="
    cat > .config << 'EOF'
CONFIG_TARGET_x86=y
CONFIG_TARGET_x86_64=y
CONFIG_TARGET_x86_64_DEVICE_generic=y
CONFIG_TARGET_ROOTFS_EXT4FS=y
CONFIG_TARGET_ROOTFS_SQUASHFS=y
CONFIG_TARGET_ROOTFS_TARGZ=y
CONFIG_TARGET_IMAGES_GZIP=y
# Disable most packages
# CONFIG_PACKAGE_dnsmasq is not set
# CONFIG_PACKAGE_firewall is not set
# CONFIG_PACKAGE_iwinfo is not set
# CONFIG_PACKAGE_ip6tables is not set
# CONFIG_PACKAGE_kmod-ipt-offload is not set
# CONFIG_PACKODE_odhcpd-ipv6only is not set
# CONFIG_PACKAGE_wpad-basic-mbedtls is not set
EOF
    make defconfig 2>/dev/null
    # Strip more
    for pkg in dnsmasq firewall iwinfo ip6tables kmod-ipt-offload odhcpd-ipv6only wpad-basic-mbedtls kmod-usb-core kmod-usb-uhci kmod-usb-ohci kmod-usb-xhci-hcd kmod-usb-storage; do
      sed -i "s/^CONFIG_PACKAGE_$pkg=y/# CONFIG_PACKAGE_$pkg is not set/" .config 2>/dev/null || true
    done
    make olddefconfig 2>/dev/null
    ;;

  full)
    echo "=== x86/64 Full Config (many packages) ==="
    cat > .config << 'EOF'
CONFIG_TARGET_x86=y
CONFIG_TARGET_x86_64=y
CONFIG_TARGET_x86_64_DEVICE_generic=y
CONFIG_TARGET_ROOTFS_EXT4FS=y
CONFIG_TARGET_ROOTFS_SQUASHFS=y
CONFIG_TARGET_ROOTFS_TARGZ=y
CONFIG_TARGET_IMAGES_GZIP=y
# Enable many packages
CONFIG_PACKAGE_wpad-openssl=y
CONFIG_PACKAGE_iperf3=y
CONFIG_PACKAGE_tcpdump=y
CONFIG_PACKAGE_mtr=y
CONFIG_PACKAGE_bind-dig=y
CONFIG_PACKAGE_curl=y
CONFIG_PACKAGE_kmod-sched-core=y
CONFIG_PACKAGE_kmod-sched-connmark=y
CONFIG_PACKAGE_kmod-sched-flower=y
CONFIG_PACKAGE_kmod-ifb=y
CONFIG_PACKAGE_kmod-bonding=y
CONFIG_PACKAGE_kmod-8021q=y
CONFIG_PACKAGE_kmod-bridge=y
CONFIG_PACKAGE_kmod-tun=y
CONFIG_PACKAGE_kmod-vxlan=y
CONFIG_PACKAGE_kmod-wireguard=y
CONFIG_PACKAGE_kmod-usb-core=y
CONFIG_PACKAGE_kmod-usb-xhci-hcd=y
CONFIG_PACKAGE_kmod-usb-storage=y
CONFIG_PACKAGE_kmod-fs-ext4=y
CONFIG_PACKAGE_kmod-fs-vfat=y
CONFIG_PACKAGE_kmod-fs-ntfs3=y
CONFIG_PACKAGE_kmod-nls-utf8=y
CONFIG_PACKAGE_iptables-mod-extra=y
CONFIG_PACKAGE_iptables-mod-filter=y
CONFIG_PACKAGE_iptables-mod-ipopt=y
EOF
    make defconfig
    ;;

  dev)
    echo "=== x86/64 Dev Config (with dev tools) ==="
    cat > .config << 'EOF'
CONFIG_TARGET_x86=y
CONFIG_TARGET_x86_64=y
CONFIG_TARGET_x86_64_DEVICE_generic=y
CONFIG_TARGET_ROOTFS_EXT4FS=y
CONFIG_TARGET_ROOTFS_SQUASHFS=y
CONFIG_TARGET_ROOTFS_TARGZ=y
CONFIG_TARGET_IMAGES_GZIP=y
# Development tools
CONFIG_PACKAGE_strace=y
CONFIG_PACKAGE_ltrace=y
CONFIG_PACKAGE_gdb=y
CONFIG_PACKAGE_perf=y
CONFIG_PACKAGE_tcpdump=y
CONFIG_PACKAGE_iperf3=y
CONFIG_PACKAGE_iptraf-ng=y
CONFIG_PACKAGE_nano=y
CONFIG_PACKAGE_tmux=y
CONFIG_PACKAGE_openssh-sftp-server=y
CONFIG_PACKAGE_kmod-sched-core=y
CONFIG_PACKAGE_kmod-ifb=y
EOF
    make defconfig
    ;;

  hardened)
    echo "=== x86/64 Hardened Config ==="
    cat > .config << 'EOF'
CONFIG_TARGET_x86=y
CONFIG_TARGET_x86_64=y
CONFIG_TARGET_x86_64_DEVICE_generic=y
CONFIG_TARGET_ROOTFS_EXT4FS=y
CONFIG_TARGET_ROOTFS_SQUASHFS=y
CONFIG_TARGET_ROOTFS_TARGZ=y
CONFIG_TARGET_IMAGES_GZIP=y
# Security hardening
CONFIG_KERNEL_CC_STACKPROTECTOR_REGULAR=y
CONFIG_KERNEL_CC_STACKPROTECTOR_STRONG=y
CONFIG_KERNEL_STACKPROTECTOR=y
CONFIG_KERNEL_FORTIFY_SOURCE=y
CONFIG_PACKAGE_arptables=y
CONFIG_PACKAGE_ebtables=y
CONFIG_PACKAGE_iptables-mod-conntrack-extra=y
CONFIG_PACKAGE_iptables-mod-ipopt=y
CONFIG_PACKAGE_kmod-ipt-raw=y
CONFIG_PACKAGE_kmod-ipt-conntrack-extra=y
EOF
    make defconfig
    ;;

  *)
    echo "Usage: $0 <variant>"
    echo "Variants: default, minimal, full, dev, hardened"
    exit 1
    ;;
esac

echo ""
echo "Config applied. Key settings:"
grep -E "^CONFIG_TARGET|^CONFIG_PACKAGE" .config | sort | head -30
echo ""
echo "Run 'make -j\$(nproc) V=s' to build"
