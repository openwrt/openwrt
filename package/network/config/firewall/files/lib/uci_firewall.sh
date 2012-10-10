# This file is here for backwards compatibility and to override the
# uci_firewall.sh from an earlier version.
type fw_is_loaded >/dev/null || {
	. /lib/firewall/core.sh
}
