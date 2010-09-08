#!/bin/sh

TOPDIR="$(pwd)"
TARGETDIR="${1}"

[ -f "$TOPDIR/scripts/${0##*/}" ] || {
	echo "Please execute within the toplevel directory" >&2
	exit 1
}

# Try to find package architecture from packages directory
PKGARCH=
for pkg in $TOPDIR/packages/*.ipk; do
	if [ -f "$pkg" ]; then
		PKGARCH="${pkg##*/}"
		PKGARCH="${PKGARCH#*_*_}"
		PKGARCH="${PKGARCH%.ipk}"
		[ "$PKGARCH" = all ] || break
	fi
done

# Try to find package architecture from the target directory
[ -n "$PKGARCH" ] || {
	PKGARCH="${TARGETDIR##*/root-}"
	[ "$PKGARCH" != "$TARGETDIR" ] || {
		echo "Cannot determine package architecture" >&2
		exit 1
	}
}

rm -f $TOPDIR/tmp/opkg.conf

[ -f $TOPDIR/repositories.conf ] && \
	$TOPDIR/staging_dir/host/bin/sed \
		-n -e "s/\$A/$PKGARCH/g" -e "/^[[:space:]*]src/p" \
		$TOPDIR/repositories.conf > $TOPDIR/tmp/opkg.conf

cat <<EOT >> $TOPDIR/tmp/opkg.conf
dest root /
arch all 100
arch $PKGARCH 200
option offline_root $TARGETDIR
src imagebuilder file:$TOPDIR/packages
EOT

exit 0
