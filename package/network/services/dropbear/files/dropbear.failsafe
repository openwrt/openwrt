#!/bin/sh

failsafe_dropbear () {
	dropbearkey -t rsa -s 1024 -f /tmp/dropbear_rsa_failsafe_host_key
	dropbearkey -t ed25519 -f /tmp/dropbear_ed25519_failsafe_host_key
	dropbear -r /tmp/dropbear_rsa_failsafe_host_key -r /tmp/dropbear_ed25519_failsafe_host_key <> /dev/null 2>&1
}

boot_hook_add failsafe failsafe_dropbear
