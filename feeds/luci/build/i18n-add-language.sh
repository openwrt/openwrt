#!/usr/bin/env bash

LANG=$1

case "$LANG" in
	[a-z][a-z]|[a-z][a-z]-[a-z][a-z]) : ;;
	*)
		echo "Usage: $0 <ISO_CODE>\n" >&2
		exit 1
	;;
esac

ADDED=0

for podir in ./*/*/po; do
	[ -d "$podir/templates" ] || continue

	mkdir "$podir/$LANG"
	for catalog in $(cd "$podir/templates"; echo *.pot); do
		if [ -f "$podir/templates/$catalog" -a ! -f "$podir/$LANG/${catalog%.pot}.po" ]; then
			msginit --no-translator -l "$LANG" -i "$podir/templates/$catalog" -o "$podir/$LANG/${catalog%.pot}.po"
			git add "$podir/$LANG/${catalog%.pot}.po"
			ADDED=$((ADDED + 1))
		fi
	done
done

if [ $ADDED -gt 0 ]; then
	echo ""
	echo "Added $ADDED new translation catalogs for language '$LANG'."
	echo "Please also edit 'luci.mk' and add"
	echo ""
	echo "  LUCI_LANG.$LANG=Native Language Name"
	echo ""
	echo "to properly package the translation files."
	echo ""
fi
