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

if [ $? != 0 ] ; then
	echo build-sed-host-binary
else
	echo use-sed-host-binary
fi;
rm -f .sedtest


