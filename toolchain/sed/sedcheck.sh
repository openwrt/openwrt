#!/bin/sh

if [ -x /usr/bin/sed ]; then 
    SED="/usr/bin/sed";
else
    if [ -x /bin/sed ]; then 
	SED="/bin/sed";
    fi;
fi;

echo "HELLO" > .sedtest
$SED -i -e "s/HELLO/GOODBYE/" .sedtest >/dev/null 2>&1

case "$1" in
	download)
		if [ $? != 0 ] ; then
			echo download-sed-binary
		fi;
	;;
	*)
		if [ $? != 0 ] ; then
			echo build-sed-host-binary
		else
			echo use-sed-host-binary
		fi;
	;;
esac
rm -f .sedtest


