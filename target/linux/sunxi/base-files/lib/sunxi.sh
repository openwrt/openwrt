#!/bin/sh

# defaults
SUNXI_BOARD_NAME="generic sunxi"
SUNXI_BOARD_MODEL="generic sunxi"
SUNXI_ENV_DEV=/dev/mmcblk0

#Helper functions 
get_cmdline_opt() 
{
cat /proc/cmdline | awk -F$1= '{print $2}' | awk '{print $1}'
}

#Since fw_getenv doesn't work with blockdevs let's make a hack
uboot_getenv() 
{
    dd if=$SUNXI_ENV_DEV bs=1024 skip=544 count=128 2>dev/null |\strings|grep $1|cut -d"=" -f2
}

#Actual routines go below
sunxi_env_dev()
{
    local dev
    dev=`get_cmdline_opt root|cut -d"p" -f1`
    SUNXI_ENV_DEV=$dev
    echo "probing $dev for uboot env data"
}

sunxi_board_detect() {
    local board
    local model
    sunxi_env_dev
    [ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"
    board="`uboot_getenv wrt_board`"
    model="`uboot_getenv wrt_model`"
    if [ "$board" != "" ]; then
	SUNXI_BOARD_NAME="$board"
    fi
    if [ "$model" != "" ]; then
	SUNXI_BOARD_MODEL="$model"
    fi
    echo "$SUNXI_BOARD_NAME" > /tmp/sysinfo/board_name
    echo "$SUNXI_BOARD_MODEL" > /tmp/sysinfo/model
    echo "Detected $SUNXI_BOARD_NAME // $SUNXI_BOARD_MODEL"
}
