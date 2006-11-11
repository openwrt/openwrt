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

sub features(@) {
	my $ret;

	while ($_ = shift @_) {
		/broken/ and $ret .= "\tdepends BROKEN\n";
		/pci/ and $ret .= "\tselect PCI_SUPPORT\n";
		/usb/ and $ret .= "\tselect USB_SUPPORT\n";
		/atm/ and $ret .= "\tselect ATM_SUPPORT\n";
		/pcmcia/ and $ret .= "\tselect PCMCIA_SUPPORT\n";
		/squashfs/ and $ret .= "\tselect USES_SQUASHFS\n";
		/jffs2/ and $ret .= "\tselect USES_JFFS2\n";
		/ext2/ and $ret .= "\tselect USES_EXT2\n";
	}
	return $ret;
}

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
	$a->{name} cmp $b->{name}
} @target;


foreach $target (@target) {
	my $conf = uc $target->{kernel}.'_'.$target->{board};
	my $features = features(@{$target->{features}});
	my $help = $target->{desc};
	chomp $features;
	$features .= "\n";
	$conf =~ tr/\.-/__/;
	if ($help =~ /\w+/) {
		$help =~ s/^\s*/\t  /mg;
		$help = "\thelp\n$help";
	} else {
		undef $help;
	}

	print <<EOF
config LINUX_$conf
	bool "$target->{name}"
	select $target->{arch}
$features$help

EOF
}

