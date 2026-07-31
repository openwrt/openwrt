#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only

PROFILE_ID='vivo_sp1_mitrastar_gpt_2741gnac_n1'

validate_inputs() {
	case "${OMCI_INPUT_GPON_SN:-}" in
		[A-Za-z0-9][A-Za-z0-9][A-Za-z0-9][A-Za-z0-9][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]) ;;
		[0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]) ;;
		*) echo 'GPON SN must be VENDXXXXXXXX or 16 hexadecimal digits' >&2; return 1 ;;
	esac

	case "${OMCI_INPUT_SLID:-}" in
		''|*[![:print:]]*) echo 'SLID/GPON password must contain 1 to 32 printable ASCII characters' >&2; return 1 ;;
	esac
	[ "${#OMCI_INPUT_SLID}" -le 32 ] || {
		echo 'SLID/GPON password must contain at most 32 characters' >&2
		return 1
	}
}

case "$1" in
	describe)
		cat <<'JSON'
{"order":10,"id":"vivo_sp1_mitrastar_gpt_2741gnac_n1","name":"Vivo SP1 - Mitrastar GPT-2741GNAC-N1","description":"Emulates the Mitrastar GPT-2741GNAC-N1-SV identity used by Vivo SP1 with the Nokia OLT interoperability profile.","inputs":[{"name":"gpon_sn","label":"GPON SN","type":"text","required":true,"placeholder":"MSTC12345678","pattern":"^(?:[A-Za-z0-9]{4}[0-9A-Fa-f]{8}|[0-9A-Fa-f]{16})$","max_length":16},{"name":"slid","label":"SLID / GPON password","type":"password","required":true,"placeholder":"Enter the Vivo SLID","pattern":"^[ -~]{1,32}$","max_length":32}],"settings":["Vendor ID: MSTC","Equipment ID: GPT-2741GNAC-N1-SV","Version: GG-11000-C003","Hardware: GG-GAPL100v02","OMCC: 0x80","OLT profile: Nokia"]}
JSON
		;;
	apply)
		validate_inputs || exit 2
		uci -q batch <<EOF_UCI
set omci.main=agent
set omci.main.device='0'
set omci.main.enabled='1'
set omci.main.permissive='1'
set omci.main.fake_omci='1'
set omci.main.dying_gasp='1'
set omci.main.olt_profile='nokia'
set omci.main.olt_profile_force='nokia'
set omci.main.vendor_id='MSTC'
set omci.main.version='GG-11000-C003'
set omci.main.equipment_id='GPT-2741GNAC-N1-SV'
set omci.main.hardware_version='GG-GAPL100v02'
set omci.main.software_version_0='GG-11000-C003'
set omci.main.software_version_1='GG-11000-C003'
set omci.main.omcc_version='128'
set omci.main.profile='${PROFILE_ID}'
EOF_UCI
		status=$?
		[ "$status" -eq 0 ] || exit "$status"
		uci -q set "omci.main.serial=${OMCI_INPUT_GPON_SN}" || exit $?
		uci -q set "omci.main.password=${OMCI_INPUT_SLID}" || exit $?
		uci -q commit omci
		status=$?
		[ "$status" -eq 0 ] || exit "$status"
		printf '{"success":true,"profile":"%s"}\n' "$PROFILE_ID"
		;;
	*)
		echo "Usage: $0 {describe|apply}" >&2
		exit 2
		;;
esac
