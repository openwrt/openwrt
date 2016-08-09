#!/usr/bin/env bash
#
# Copyright (C) 2016 Josua Mayer
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

usage() {
	echo "$0 <sectors> <outfile> <bootloader> [<type_partitionN> <sectors_partitionN> <img_partitionN>]?"
}

# always require first 3 arguments
# then in pairs up to 8 more for a total of up to 4 partitions
if [ $# -lt 3 ] || [ $# -gt 15 ] || [ $(($#%3)) -ne 0 ]; then
	usage
	exit 1
fi

set -e

# parameters
IMGSIZE=$1
OUTFILE="$2"
BOOTLOADER="$3"

# calculate number of partitions from argument list
NUMPARTS=$#
((NUMPARTS=(NUMPARTS-3)/3))

# find required applications
FDISK=$(env PATH="/usr/local/sbin:/usr/sbin:/sbin:$PATH" which fdisk)

# generate image file
printf "Creating $OUTFILE from /dev/zero: "
dd if=/dev/zero of="$OUTFILE" bs=512 count=1 >/dev/null
printf "Done\n"

# generate fdisk argument list
printf "Generating fdisk argument list: "
ARGSFILE=$(mktemp)

# empty partition table
printf "o\n" >> $ARGSFILE

# actual partitions
offset=2048
for i in $(seq 1 1 $NUMPARTS); do
	((n=3+3*i-2)); type=$(eval echo \${$n})
	((n=3+3*i-1)); size=$(eval echo \${$n})
	((end=offset+size-1))

	printf "n\np\n%i\n\n%i\n" $i $end >> $ARGSFILE

	# special case on first aprtition: fdisk wont ask which one
	if [ $i -eq 1 ]; then
		printf "t\n%s\n" $type >> $ARGSFILE
	else
		printf "t\n%i\n%s\n" $i $type >> $ARGSFILE
	fi

	# add this partitions size to offset for next partition
	((offset=end+1))
done

# write and exit
printf "w\n" >> $ARGSFILE

printf "Done\n"

# create real partition table using fdisk
printf "Creating partition table: "
cat $ARGSFILE | $FDISK "$OUTFILE" >/dev/null
printf "Done\n"

# remove temporary files
printf "Cleaning up: "
rm -f $ARGSFILE
printf "Done\n"

# install bootloader
printf "Writing bootloader: "
dd of="$OUTFILE" if="$BOOTLOADER" bs=512 seek=1 conv=notrunc 2>/dev/null
printf "Done\n"

# write partition data

# offset of first partition is 2048
offset=2048
for i in $(seq 1 1 $NUMPARTS); do
	((n=3+3*i-1)); size=$(eval echo \${$n})
	((n=3+3*i));   img="$(eval echo \${$n})"

	printf "Writing %s to partition %i: " "$img" $i
	dd if="$img" of="$OUTFILE" bs=512 seek=$offset conv=notrunc 2>/dev/null
	printf "Done\n"

	# add this partitions size to offset for next partition
	((offset=offset+size))
done
