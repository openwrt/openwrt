# uxc container package-hook helpers, sourced by the install scripts that
# container-<name> packages ship (the default_postinst() pattern). The package
# scripts stay thin: source this and call the matching helper with the container
# name. Everything here runs on the live target only.

UXC_STOP_WAIT=15

uxc_preinst() {
	local name="$1" newvol="$2" newsize="$3"
	local reg="/tmp/run/uvol/.meta/uxc/$name.json"
	local oldvol free i

	[ -n "$IPKG_INSTROOT" ] && return 0

	if [ ! -L /tmp/run/uvol/.meta ]; then
		echo "uxc: cannot install '$name': the uvol metadata volume is not mounted at /tmp/run/uvol/.meta" >&2
		echo "uxc: no usable uvol backend; install 'autopart' (block), or provision an LVM VG / UBI space for uvol" >&2
		return 1
	fi

	[ -f "$reg" ] || return 0

	oldvol="$(sed -n 's|.*"path"[: ]*"/tmp/run/uvol/\([^"]*\)".*|\1|p' "$reg")"
	[ -z "$oldvol" ] && return 0
	[ "$oldvol" = "$newvol" ] && return 0

	uxc kill "$name" KILL 2>/dev/null
	i=0
	while [ "$i" -lt "$UXC_STOP_WAIT" ] && uxc list 2>/dev/null | grep -qE "^$name[[:space:]].*[[:space:]]running"; do
		sleep 1
		i=$((i + 1))
	done

	free="$(uvol free 2>/dev/null)"
	if [ -n "$free" ] && [ -n "$newsize" ] && [ "$free" -lt "$newsize" ]; then
		uvol down "$oldvol" 2>/dev/null
		uvol remove "$oldvol" 2>/dev/null
	fi

	return 0
}

uxc_postinst() {
	local name="$1"

	[ -n "$IPKG_INSTROOT" ] && return 0
	uxc create "$name" 2>/dev/null
	uxc start "$name" 2>/dev/null

	return 0
}

uxc_prerm() {
	local name="$1"

	[ -n "$IPKG_INSTROOT" ] && return 0
	uxc kill "$name" 2>/dev/null

	return 0
}

uxc_postrm() {
	local name="$1" left

	[ -n "$IPKG_INSTROOT" ] && return 0

	rm -rf "/tmp/run/uvol/.meta/uxc/state/$name" "/tmp/run/uvol/.meta/uxc/settings/$name.json"

	left="$(uvol list 2>/dev/null | while read -r vn rest; do
		case "$vn" in "$name"|"$name".*) echo "$vn";; esac
	done)"
	[ -z "$left" ] && return 0

	echo "container '$name' removed; persistent data volumes were kept. To delete this data, run:" >&2
	echo "$left" | sed 's/^/  uvol remove /' >&2

	return 0
}

# Stack (composition) hooks, called by a stack-<app> package's install scripts.
# The guard lives here, in a real shell file, on purpose: an in-Makefile
# `[ -n "$IPKG_INSTROOT" ]` guard inside a $(call)-expanded define is silently
# eaten by make ($I -> empty, leaving the always-true literal "PKG_INSTROOT"),
# which neutered the bring-up. Keeping it here is immune to that.
uxc_stack_postinst() {
	local app="$1"

	[ -n "$IPKG_INSTROOT" ] && return 0
	uxc-stack up "$app"

	return 0
}

uxc_stack_prerm() {
	local app="$1"

	[ -n "$IPKG_INSTROOT" ] && return 0
	uxc-stack down "$app"

	return 0
}
