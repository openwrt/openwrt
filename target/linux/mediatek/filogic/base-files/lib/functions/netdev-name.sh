netdev_name_set() {
	local dir="$1"
	local attr
	local label
	local name
	local netdev="${dir##*/}"

	for attr in label openwrt,netdev-name; do
		[ -r "$dir/of_node/$attr" ] || continue
		read -r label < "$dir/of_node/$attr"
		name="$label"
	done

	[ -n "$name" ] || return 0
	[ "$netdev" = "$name" ] && return 0

	ip link set "$netdev" name "$name"
}
