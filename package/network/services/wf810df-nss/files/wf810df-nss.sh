#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only

wf810df_nss_log() {
	logger -t wf810df-nss "$*"
}

wf810df_nss_load() {
	local dir result
	# OpenWrt modprobe ignores CLI parameters; kmodloader reads them from a list.
	dir="$(mktemp -d /tmp/wf810df-nss.XXXXXX)" || return 1
	printf '%s\n' "$*" > "$dir/module" && kmodloader "$dir/"
	result=$?
	rm -r "$dir"
	[ "$result" = 0 ] && [ -d "/sys/module/$1" ]
}

wf810df_nss_host_wifi() {
	# Both host and NSS-capable ath11k default to host mode.
	modprobe ath11k || return 1
	modprobe ath11k_ahb
}

wf810df_nss_wan() {
	local path id dev wan=
	for path in /sys/class/net/*; do
		[ -r "$path/device/of_node/qcom,id" ] || continue
		id="$(hexdump -v -e '1/1 "%02x"' "$path/device/of_node/qcom,id")"
		[ "$id" = 00000001 ] || continue
		dev="${path##*/}"
		# A DSA conduit must never become the accelerated WAN.
		[ ! -d "$path/dsa" ] || return 1
		[ -z "$wan" ] || return 1
		wan="$dev"
	done
	[ -n "$wan" ] || return 1
	printf '%s\n' "$wan"
}

wf810df_nss_pools() {
	local allocated
	# Size the 512 MB profile before the two radios pin 4095 RX buffers each.
	allocated="$(sysctl -n dev.nss.n2hcfg.extra_pbuf_core0)" || return 1
	if [ "$allocated" = 0 ]; then
		sysctl -w dev.nss.n2hcfg.extra_pbuf_core0=3100000 || return 1
	fi
	allocated="$(sysctl -n dev.nss.n2hcfg.extra_pbuf_core0)" || return 1
	# The driver allocates whole pages; sysctl exit status alone is insufficient.
	[ "$allocated" -ge 3100000 ] 2>/dev/null || return 1
	sysctl -w dev.nss.n2hcfg.n2h_high_water_core0=30624 || return 1
	sysctl -w dev.nss.n2hcfg.n2h_wifi_pool_buf=8192 || return 1
}

wf810df_nss_start() {
	local enabled wifi wan attempts active loaded
	[ "$(board_name)" = "fpt,ax3000cv2" ] || return 0
	# Never change offload mode on a running Q6/ath11k instance.
	[ ! -d /sys/module/ath11k ] || {
		wf810df_nss_log 'ath11k is already loaded; reboot to change NSS mode'
		return 1
	}
	config_load nss
	config_get_bool enabled general enabled 0
	config_get_bool wifi general wifi_offload 1
	[ "$enabled" = 1 ] || {
		wf810df_nss_log 'disabled; starting host Wi-Fi'
		wf810df_nss_host_wifi
		return $?
	}
	wan="$(wf810df_nss_wan)" || {
		wf810df_nss_log 'unique native WAN not found; starting host Wi-Fi'
		wf810df_nss_host_wifi
		return $?
	}
	# Loading ath11k also depends on NSS symbols; decide before that load.
	wf810df_nss_load qca_nss_drv enabled=1 || {
		wf810df_nss_log 'NSS driver failed; starting host Wi-Fi'
		wf810df_nss_host_wifi
		return $?
	}
	read -r loaded < /sys/module/qca_nss_drv/parameters/enabled || return 1
	[ "$loaded" = Y ] || {
		wf810df_nss_log 'NSS enable parameter was not applied; reboot required'
		wf810df_nss_host_wifi
		return 1
	}
	attempts=0
	while [ "$attempts" -lt 30 ]; do
		active="$(cat /sys/module/qca_nss_drv/parameters/physical_interface_mask_active 2>/dev/null)"
		[ "$active" = 1 ] && break
		sleep 1
		attempts=$((attempts + 1))
	done
	[ "$active" = 1 ] || {
		wf810df_nss_log 'WAN handoff not ready; no ECM, host Wi-Fi; reboot with NSS disabled to restore WAN if claimed'
		wf810df_nss_host_wifi
		return 1
	}
	wf810df_nss_pools || {
		wf810df_nss_log 'NSS pool configuration failed; no ECM, host Wi-Fi; reboot required'
		wf810df_nss_host_wifi
		return 1
	}
	if [ "$wifi" = 1 ]; then
		wf810df_nss_load ath11k nss_offload=1 frame_mode=2 || return 1
	else
		modprobe ath11k || return 1
	fi
	modprobe ath11k_ahb || return 1
	/etc/init.d/qca-nss-ecm start || return 1
	wf810df_nss_log "native WAN $wan assigned to NSS; YT9215 LAN remains host-owned"
}
