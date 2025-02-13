#!/usr/bin/python3

"""Automated test for OpenWrt image in qemu.

This starts a existing OpenWrt image in qemu system emulation and then 
executes some automated tests. It is currently only working on Linux 
systems and expects that internet access is available.

To run this on a Malta BE build run it like this:
./.github/workflows/qemu-start.py malta be
"""

import pexpect
import sys
import time
import argparse


def start_qemu(target, subtarget):
    # Starts qemu and waits till the system is available
    qemu_cmd = f"./scripts/qemustart {target} {subtarget} --netdev user,id=wan -netdev user,id=lan,net=192.168.1.0/24,dhcpstart=192.168.1.100,restrict=yes,hostfwd=tcp::8022-:22 -net nic,netdev=wan -net nic,netdev=lan"
    print(f'calling: "{qemu_cmd}"')
    child = pexpect.spawn(qemu_cmd, logfile=sys.stdout.buffer)

    child.expect("Please press Enter to activate this console.", timeout=80)
    child.expect("link becomes ready", timeout=90)
    child.send("\n")

    child.expect(":/#", timeout=10)
    child.sendline("uname -a")
    # Wait till network is really available
    time.sleep(5)
    return child


def configure_network(child):
    # Configures the LAN network to DHCP client mode
    # OpenWrt will get an IP address from the qemu DHCP server
    child.expect(":/#")
    child.sendline("ip addr")
    child.expect("192.168.1.1/24")

    child.expect(":/#")
    child.sendline("uci set network.lan.proto=dhcp")

    child.expect(":/#")
    child.sendline("uci changes")
    child.expect("network.lan.proto='dhcp'")

    child.expect(":/#")
    child.sendline("uci commit")

    child.expect(":/#")
    child.sendline("service network reload")
    child.expect("entered forwarding state")
    # Wait for DHCP configuration
    time.sleep(8)

    child.expect(":/#")
    child.sendline("ip addr")
    child.expect("10.0.2.15/24")
    # Make sure the full output was printed
    child.expect("root@")
    return child


def check_https_download(child, url, expect_str, expect_code=0):
    child.expect(":/#")
    child.sendline(f"wget {url}")
    child.expect(expect_str)

    child.expect(":/#")
    child.sendline("echo $?")
    child.expect(str(expect_code))
    return child


def check_download(child):
    # Test a http(s) connection to multiple server and check the results
    child = check_https_download(
        child,
        "https://downloads.openwrt.org/releases/21.02.2/targets/armvirt/64/config.buildinfo",
        "Download completed ",
    )

    child.expect(":/#")
    child.sendline("sha256sum config.buildinfo")
    child.expect(
        "26b85383a138594b1197e581bd13c6825c0b6b5f23829870a6dbc5d37ccf6cd8  config.buildinfo"
    )

    child.expect(":/#")
    child.sendline("rm config.buildinfo")

    child = check_https_download(
        child,
        "http://http.badssl.com/",
        "Download completed ",
    )

    child.expect(":/#")
    child.sendline("rm index.html")

    child = check_https_download(
        child,
        "https://letsencrypt.org",
        "Download completed ",
    )

    child.expect(":/#")
    child.sendline("rm index.html")

    child = check_https_download(
        child,
        "https://www.mozilla.org/",
        "Download completed ",
    )

    child = check_https_download(
        child,
        "https://untrusted-root.badssl.com/",
        "Connection error: Invalid SSL certificate",
        5,
    )

    child = check_https_download(
        child,
        "https://wrong.host.badssl.com",
        "Connection error: Server hostname does not match SSL certificate",
        5,
    )

    child = check_https_download(
        child,
        "https://expired.badssl.com",
        "Connection error: Invalid SSL certificate",
        5,
    )

    child = check_https_download(
        child,
        "https://rc4.badssl.com",
        "Connection error: Connection failed",
        4,
    )

    return child


def opkg_install_iperf3(child):
    child.expect(":/#")
    child.sendline("opkg update")
    child.expect("https://downloads.openwrt.org/")
    child.expect("Signature check passed.")

    child.expect(":/#")
    child.sendline("opkg list-installed")
    child.expect("procd ")

    child.expect(":/#")
    child.sendline("opkg install iperf3")
    child.expect("Installing iperf3")
    child.expect("Configuring iperf3")

    child.expect(":/#")
    child.sendline("opkg list-installed")
    child.expect("iperf3 ")

    child.expect(":/#")
    child.sendline("iperf3 -v")
    child.expect("cJSON")

    return child


def check_ssh():
    # Login to the OpenWrt running in qemu over SSH from the host
    cmd = "ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null root@localhost -p 8022"
    print(f"\nlogin with: {cmd}")
    ssh = pexpect.spawn(cmd, logfile=sys.stdout.buffer)

    ssh.expect("W I R E L E S S   F R E E D O M")
    ssh.expect("root@OpenWrt:~# ")
    ssh.sendline("uname -a")
    ssh.expect("root@OpenWrt:~# ")
    ssh.sendline("exit")
    ssh.expect("Connection to localhost closed.")
    ssh.wait()


def poweroff(child):
    child.expect(":/#")
    child.sendline("poweroff")

    child.expect(":/#")
    child.expect("Power down")
    return child


parser = argparse.ArgumentParser(description="Start test in qemu")
parser.add_argument("target", help="target name")
parser.add_argument("subtarget", help="sub target name")

args = parser.parse_args()

child = start_qemu(args.target, args.subtarget)
check_download(child)
opkg_install_iperf3(child)
configure_network(child)
check_ssh()
poweroff(child)
child.wait()
