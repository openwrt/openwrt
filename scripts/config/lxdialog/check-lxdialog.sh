#!/usr/bin/env bash
# Check ncurses compatibility

# What library to link
ldflags()
{
	for ext in so dylib; do
		for dir in "" /usr/local/lib /opt/local/lib; do
			$cc ${dir:+-L$dir} -print-file-name=libncursesw.$ext | grep -q /
			if [ $? -eq 0 ]; then
				echo $dir '-lncursesw'
				exit
			fi
			$cc ${dir:+-L$dir} -print-file-name=libncurses.$ext | grep -q /
			if [ $? -eq 0 ]; then
				echo $dir '-lncurses'
				exit 
			fi
			$cc ${dir:+-L$dir} -print-file-name=libcurses.$ext | grep -q /
			if [ $? -eq 0 ]; then
				echo $dir '-lcurses'
				exit
			fi
		done
	done
	exit 1
}

# Where is ncurses.h?
ccflags()
{
	if [ -f /usr/include/ncurses/ncurses.h ]; then
		echo '-I/usr/include/ncurses -DCURSES_LOC="<ncurses.h>"'
	elif [ -f /usr/include/ncurses/curses.h ]; then
		echo '-I/usr/include/ncurses -DCURSES_LOC="<ncurses/curses.h>"'
	elif [ -f /opt/local/include/ncurses/ncurses.h ]; then
		echo '-I/opt/local/include -I/opt/local/include/ncurses -DCURSES_LOC="<ncurses/ncurses.h>"'
	elif [ -f /usr/include/ncurses.h ]; then
		echo '-DCURSES_LOC="<ncurses.h>"'
	else
		echo '-DCURSES_LOC="<curses.h>"'
	fi
}

# Temp file, try to clean up after us
tmp=.lxdialog.tmp
trap "rm -f $tmp" 0 1 2 3 15

# Check if we can link to ncurses
check() {
	echo "main() {}" | $cc -xc - -o $tmp 2> /dev/null
	if [ $? != 0 ]; then
		echo " *** Unable to find the ncurses libraries."          1>&2
		echo " *** make menuconfig require the ncurses libraries"  1>&2
		echo " *** "                                               1>&2
		echo " *** Install ncurses (ncurses-devel) and try again"  1>&2
		echo " *** "                                               1>&2
		exit 1
	fi
}

usage() {
	printf "Usage: $0 [-check compiler options|-header|-library]\n"
}

if [ $# -eq 0 ]; then
	usage
	exit 1
fi

cc=""
case "$1" in
	"-check")
		shift
		cc="$@"
		check
		;;
	"-ccflags")
		ccflags
		;;
	"-ldflags")
		shift
		cc="$@"
		ldflags
		;;
	"*")
		usage
		exit 1
		;;
esac
