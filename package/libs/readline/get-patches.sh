#!/bin/sh

pkgver=$(grep -Po "(?<=PKG_VERSION:=)(.*)" Makefile)
patchlevel=13

i=1
while [ $i -le $patchlevel ]; do
	patch=$(printf "%03d" $i)
	url=https://ftp.gnu.org/gnu/readline/readline-$pkgver-patches/readline${pkgver//./}-$patch
	## Download patch and remove 'patch -p0' requirement
	curl $url | sed \
		-e 's/..\/readline-'${pkgver}'.*\//a\//g' \
		-e 's!^--- !&b/!' \
		-e '/patch -p0/d' > patches/$patch-readline.patch
	i=$((i+1))
done
