#!/usr/bin/env bash
set -e

pkg_dir=$1

if [ -z $pkg_dir ] || [ ! -d $pkg_dir ]; then
	echo "Usage: ipkg-make-index <package_directory>" >&2
	exit 1
fi

which md5sum >/dev/null 2>&1 || alias md5sum=md5
empty=1

for pkg in `find $pkg_dir -name '*.ipk' | sort`; do
	empty=
	name="${pkg##*/}"
	name="${name%%_*}"
	[[ "$name" = "kernel" ]] && continue
	[[ "$name" = "libc" ]] && continue
	echo "Generating index for package $pkg" >&2
	file_size=$(ls -l $pkg | awk '{print $5}')
	md5sum=$(md5sum $pkg | awk '{print $1}')
	sha256sum=$(openssl dgst -sha256 $pkg | awk '{print $2}')
	# Take pains to make variable value sed-safe
	sed_safe_pkg=`echo $pkg | sed -e 's/^\.\///g' -e 's/\\//\\\\\\//g'`
	tar -xzOf $pkg ./control.tar.gz | tar xzOf - ./control | sed -e "s/^Description:/Filename: $sed_safe_pkg\\
Size: $file_size\\
MD5Sum: $md5sum\\
SHA256sum: $sha256sum\\
Description:/"
	echo ""
done
[ -n "$empty" ] && echo
exit 0
