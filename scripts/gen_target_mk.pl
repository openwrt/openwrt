#!/usr/bin/perl
# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

use strict;

my @target;
my $target;

while (<>) {
	chomp;
	/^Target:\s*((.+)-(\d+\.\d+))\s*$/ and do {
		$target = {
			id => $1,
			board => $2,
			kernel => $3
		};
		push @target, $target;
	};
	/^Target-Name:\s*(.+)\s*$/ and $target->{name} = $1;
	/^Target-Path:\s*(.+)\s*$/ and $target->{path} = $1;
	/^Target-Arch:\s*(.+)\s*$/ and $target->{arch} = $1;
	/^Target-Features:\s*(.+)\s*$/ and do {
		my $f = [];
		$target->{features} = $f;
		@$f = split /\s+/, $1;
	};
	/^Target-Description:/ and do {
		my $desc;
		while (<>) {
			last if /^@@/;
			$desc .= $_;
		}
		$target->{desc} = $desc;
	};
	/^Linux-Version:\s*(.+)\s*$/ and $target->{version} = $1;
	/^Linux-Release:\s*(.+)\s*$/ and $target->{release} = $1;
	/^Linux-Kernel-Arch:\s*(.+)\s*$/ and $target->{karch} = $1;
}

@target = sort {
	$a->{id} cmp $b->{id}
} @target;

foreach $target (@target) {
	my $conf = uc $target->{kernel}.'_'.$target->{board};
	$conf =~ tr/\.-/__/;
	print <<EOF
ifeq (\$(CONFIG_LINUX_$conf),y)
  define Target
    KERNEL:=$target->{kernel}
    BOARD:=$target->{board}
    LINUX_VERSION:=$target->{version}
    LINUX_RELEASE:=$target->{release}
    LINUX_KARCH:=$target->{karch}
  endef
endif

EOF
}
print "\$(eval \$(call Target))\n";
