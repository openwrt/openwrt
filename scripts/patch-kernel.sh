#!/bin/sh
# A little script I whipped up to make it easy to
# patch source trees and have sane error handling
# -Erik
#
# (c) 2002 Erik Andersen <andersen@codepoet.org>

# Set directories from arguments, or use defaults.
targetdir=${1-.}
patchdir=${2-../kernel-patches}
patchpattern=${3-*}

if [ ! -d "${targetdir}" ] ; then
	echo "Aborting.  '${targetdir}' is not a directory."
	exit 1
fi
if [ ! -d "${patchdir}" ] ; then
	echo "Aborting.  '${patchdir}' is not a directory."
	exit 1
fi

# Sort files to apply patch
files_patches=$(cd "${patchdir}" && ls ${patchpattern} 2>/dev/null | grep -E '^[0-9]+(-[0-9]+)?' | sort -t '-' -k 1,1n -k 2,2n)

for patch_file in $files_patches; do
	i="${patchdir}/${patch_file}"

	case "$i" in
	*.gz)
	type="gzip"; uncomp="gunzip -dc"; ;;
	*.bz)
	type="bzip"; uncomp="bunzip -dc"; ;;
	*.bz2)
	type="bzip2"; uncomp="bunzip2 -dc"; ;;
	*.zip)
	type="zip"; uncomp="unzip -d"; ;;
	*.Z)
	type="compress"; uncomp="uncompress -c"; ;;
	*)
	type="plaintext"; uncomp="cat"; ;;
	esac
	[ -d "${i}" ] && echo "Ignoring subdirectory ${i}" && continue
	echo ""
	echo "Applying ${i} using ${type}: "
	${uncomp} ${i} | ${PATCH:-patch} -f -p1 -d ${targetdir}
	if [ $? != 0 ] ; then
		echo "Patch failed!  Please fix $i!"
		exit 1
	fi
done
