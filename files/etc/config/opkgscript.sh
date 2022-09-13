#! /bin/sh

# Write a list of packages currently installed or read that list,
# presumably after a firmware upgrade, in order to reinstall all packages
# on that list not currently installed
#
# (c) 2013 Malte Forkel <malte.forkel@berlin.de>
#
# Originally found on OpenWrt forums at:
#    https://forum.openwrt.org/viewtopic.php?pid=194478#p194478
# Thanks, too, to hnyman for important comments on this script
#
# Version history
#    0.2.2 - editorial tweaks to help text -richb-hanvover 
#    0.2.1 - fixed typo in awk script for dependency detection
#    0.2.0 - command interface
#    0.1.0 - Initial release

PCKGLIST=/etc/config/opkg.installed  # default package list
SCRIPTNAME=$(basename $0)            # name of this script
COMMAND=""                           # command to execute

INSTLIST=$(mktemp)                   # list of packages to install
PREQLIST=$(mktemp)                   # list of prerequisite packages

UPDATE=false                         # update the package database
OPKGOPT=""                           # options for opkg calls
VERBOSE=false                        # be verbose

cleanup () {
    rm -f $INSTLIST $PREQLIST
}

echo_usage () {
    echo \
"Usage: $(basename $0) [options...] command [packagelist]
Available commands:
    help                print this help text
    write               write a list of currently installed packages
    install             install packages on list not currently installed
    script              output a script to install missing packages
    
Options:
    -u                  update the package database
    -t                  test only, execute opkg commands with --noaction
    -v                  be verbose
$SCRIPTNAME can be used to re-install those packages that were installed
before a firmware upgrade but are not part of the new firmware image.
Before the firmware upgrade, execute
    $SCRIPTNAME [options...] write [packagelist]
    
to save the list of currently installed packages. Save the package list in a 
place that will not be wiped out by the firmware upgrade. The default package list
is '$PCKGLIST', which works well for normal sysupgrades. Or copy that file to 
another computer before the upgrade if you are not preserving the settings.
After the firmware upgrade, execute
    $SCRIPTNAME [options...] install [packagelist]
    
to re-install all packages that were not part of the firmware image. 
By default, the script will use the previously-created '$PCKGLIST'.
Alternatively, you can execute
    $SCRIPTNAME [options...] script [packagelist]
    
to output a shell script that will contain calls to opkg to install those
missing packages. This might be useful if you want to check which packages
would be installed of if you want to edit that list.
In order for this script to work after a firmware upgrade or reboot, the
opkg database must have been updated. You can use the option -u to do this.
You can specify the option -t to test what $SCRIPTNAME would do. All calls
to opkg will be made with the option --noaction. This does not influence
the call to opkg to write the list of installed packages, though. 
"
}

trap cleanup SIGHUP SIGINT SIGTERM EXIT

# parse command line options
while getopts "htuvw" OPTS; do
    case $OPTS in
        t )
            OPKGOPT="$OPKGOPT --noaction";;
        u )
            UPDATE=true;;
        v )
            VERBOSE=true;;
        [h\?*] )
            echo_usage
            exit 0;;
    esac
done
shift $(($OPTIND - 1))

# Set the command
COMMAND=$1

# Set name of the package list
if [ "x$2" != "x" ]; then
    PCKGLIST="$2"
fi

#
# Help
#

if [ "x$COMMAND" == "x" ]; then
    echo "No command specified."
    echo ""
    COMMAND="help"
fi

if [ $COMMAND == "help" ]; then
    echo_usage
    exit 0
fi

#
# Write
#

if [ $COMMAND = "write" ] ; then
    if $VERBOSE; then
        echo "Saving package list to $PCKGLIST"
    fi
    # NOTE: option --noaction not valid for list-installed
    opkg list-installed > "$PCKGLIST"
    exit 0
fi

#
# Update 
#

if $UPDATE; then
    opkg $OPKGOPT update
fi

#
# Check
#

if [ $COMMAND == "install" ] || [ $COMMAND == "script" ]; then
    # detect uninstalled packages
    if $VERBOSE && [ $COMMAND != "script" ]; then
        echo "Checking packages... "
    fi
    cat "$PCKGLIST" | while read PACKAGE SEP VERSION; do
        # opkg status is much faster than opkg info
        # it only returns status of installed packages
        #if ! opkg status $PACKAGE | grep -q "^Status:.* installed"; then
        if [ "x$(opkg status $PACKAGE)" == "x" ]; then
            # collect uninstalled packages
            echo $PACKAGE >> $INSTLIST
            # collect prerequisites
            opkg info "$PACKAGE" |
            awk "/^Depends: / {
                                sub(\"Depends: \", \"\");   \
                                gsub(\", \", \"\\n\");      \
                                print >> \"$PREQLIST\";      \
                              }"
        fi
    done
fi

#
# Install or script
#

if [ $COMMAND == "install" ]; then
    # install packages
    cat "$INSTLIST" | while read PACKAGE; do
        if grep -q "^$PACKAGE\$" "$PREQLIST"; then
            # prerequisite package, will be installed automatically
            if $VERBOSE; then
                echo "$PACKAGE installed automatically"
            fi
        else
            # install package
            opkg $OPKGOPT install $PACKAGE
        fi
    done
elif [ $COMMAND == "script" ]; then
    # output install script
    echo "#! /bin/sh"
    cat "$INSTLIST" | while read PACKAGE; do
        if ! grep -q "^$PACKAGE\$" "$PREQLIST"; then
            echo "opkg install $PACKAGE"
        fi
    done
else
    echo "Unknown command '$COMMAND'."
    echo ""
    echo_usage
    exit 1
fi

# clean up and exit
exit 0
