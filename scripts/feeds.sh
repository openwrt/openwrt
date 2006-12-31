#!/bin/sh
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Usage : $1 -> source feeds
# 	  $2 -> other options
#
# Note : we do not yet resolve package name conflicts
#
#
FEEDS_DIR=$TOPDIR/feeds
PACKAGE_DIR=$TOPDIR/package

cd $TOPDIR
# This directory will be structured this way : feeds/feed-name
[ -d $FEEDS_DIR ] || mkdir -p $FEEDS_DIR


# Some functions we might call several times a run
delete_symlinks() {
	find $PACKAGE_DIR -type l | xargs rm -f
}

setup_symlinks() {
	# We assume that feeds do reproduce the hierarchy : section/package
	for dir in $(ls $FEEDS_DIR/)
	do
		ln -s $FEEDS_DIR/$dir/*/* $PACKAGE_DIR/
	done
}

checkout_feed() {
	# We ensure the feed has not already been checkout, if so, just update the source feed
	if [ -d $FEEDS_DIR/$2 ]; then
		svn update $FEEDS_DIR/$2
		echo "Updated to revision $(LANG=C svn info $FEEDS_DIR/$2 | awk '/^Revision:/ { print $2 }' )";
	# Otherwise, we have to checkout in the 
	else
		svn co $1 $FEEDS_DIR/$2
		echo "Checked out revision $(LANG=C svn info $FEEDS_DIR/$2 | awk '/^Revision:/ { print $2 }' )";
	fi
}

extract_feed_name() {
	echo "$(echo $1 | awk -F/ '{ print $NF}')"
}

# We can delete symlinks every time we start this script, since modifications have been made anyway
delete_symlinks ""
# Now let's checkout feeds
for feed in $1
do
	name=$(extract_feed_name "$feed")
	checkout_feed "$feed" "$name"
done
# Finally setup symlinks
setup_symlinks ""
