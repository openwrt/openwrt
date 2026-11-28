netdev_name_set() {
	local dir="$1"
	local phase="$2"
	local name
	local netdev="${dir##*/}"

	[ -r "$dir/of_node/openwrt,netdev-name" ] || return 0
	read -r name < "$dir/of_node/openwrt,netdev-name"

	[ -n "$name" ] || return 0
	[ "$netdev" = "$name" ] && return 0

	# park: let the kernel pick a free temporary name (fits IFNAMSIZ)
	[ "$phase" = "park" ] && name="tmp%d"

	ip link set "$netdev" name "$name"
}
