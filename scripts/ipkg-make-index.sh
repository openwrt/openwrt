#!/bin/sh
set -e

pkg_dir=$1

if [ -z $pkg_dir ] || [ ! -d $pkg_dir ]; then
	echo "Usage: ipkg-make-index <package_directory>" >&2
	exit 1
fi

which md5sum >/dev/null 2>&1 || alias md5sum=md5

for pkg in `find $pkg_dir -name '*.ipk' | sort`; do
	echo "Generating index for package $pkg" >&2
	file_size=$(ls -l $pkg | awk '{print $5}')
	md5sum=$(md5sum $pkg | awk '{print $1}')
	# Take pains to make variable value sed-safe
	sed_safe_pkg=`echo $pkg | sed -e 's/^\.\///g' -e 's/\\//\\\\\\//g'`
	tar -xzOf $pkg ./control.tar.gz | tar xzOf - ./control | sed -e "s/^Description:/Filename: $sed_safe_pkg\\
Size: $file_size\\
MD5Sum: $md5sum\\
Description:/"
	echo ""
done
