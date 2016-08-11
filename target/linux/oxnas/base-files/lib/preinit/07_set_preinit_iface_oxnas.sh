#!/bin/sh

oxnas_set_preinit_iface() {
	ifname=eth0
}

boot_hook_add preinit_main oxnas_set_preinit_iface
