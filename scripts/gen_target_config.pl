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
my $profiles;
my $profile;

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
		my $conf = uc $3.'_'.$2;
		$conf =~ tr/\.-/__/;
		$target = {
			id => $1,
			conf => $conf,
			board => $2,
			kernel => $3
		};
		$target->{kernel} =~ tr/\./_/;
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
	/^Default-Packages:\s*(.+)\s*$/ and do {
		my @pkgs = split /\s+/, $1;
		$target->{defaultpkgs} = \@pkgs;
	};
	/^Target-Profile:\s*(.+)\s*$/ and do {
		$profiles = $target->{profiles} or $target->{profiles} = $profiles = [];
		$profile = {
			id => $1
		};
		push @$profiles, $profile;
	};
	/^Target-Profile-Name:\s*(.+)\s*$/ and $profile->{name} = $1;
	/^Target-Profile-Packages:\s*(.+)\s*$/ and do {
		my @pkgs = split /\s+/, $1;
		$profile->{pkgs} = \@pkgs;
	};
}

@target = sort {
	$a->{name} cmp $b->{name}
} @target;


print <<EOF;
choice
	prompt "Target System"
	default LINUX_2_4_BRCM
	
EOF

foreach $target (@target) {
	my $features = features(@{$target->{features}});
	my $help = $target->{desc};
	chomp $features;
	$features .= "\n";
	if ($help =~ /\w+/) {
		$help =~ s/^\s*/\t  /mg;
		$help = "\thelp\n$help";
	} else {
		undef $help;
	}

	print <<EOF
config LINUX_$target->{conf}
	bool "$target->{name}"
	select $target->{arch}
	select LINUX_$target->{kernel}
$features$help

EOF
}

print <<EOF;
if DEVEL

config LINUX_2_6_ARM
	bool "UNSUPPORTED little-endian arm platform"
	depends BROKEN
	select LINUX_2_6
	select arm

config LINUX_2_6_CRIS
	bool "UNSUPPORTED cris platform"
	depends BROKEN
	select LINUX_2_6
	select cris

config LINUX_2_6_M68K
	bool "UNSUPPORTED m68k platform"
	depends BROKEN
	select LINUX_2_6
	select m68k

config LINUX_2_6_SH3
	bool "UNSUPPORTED little-endian sh3 platform"
	depends BROKEN
	select LINUX_2_6
	select sh3

config LINUX_2_6_SH3EB
	bool "UNSUPPORTED big-endian sh3 platform"
	depends BROKEN
	select LINUX_2_6
	select sh3eb

config LINUX_2_6_SH4
	bool "UNSUPPORTED little-endian sh4 platform"
	depends BROKEN
	select LINUX_2_6
	select sh4

config LINUX_2_6_SH4EB
	bool "UNSUPPORTED big-endian sh4 platform"
	depends BROKEN
	select LINUX_2_6
	select sh4eb

config LINUX_2_6_SPARC
	bool "UNSUPPORTED sparc platform"
	depends BROKEN
	select LINUX_2_6
	select sparc

endif

endchoice

choice
	prompt "Target Profile"

EOF

foreach $target (@target) {
	my $profiles;
	
	$profiles = $target->{profiles} or $profiles = [
		{
			id => 'Default',
			name => 'Default',
			pkgs => []
		}
	];
	foreach my $profile (@$profiles) {
		print <<EOF;
config LINUX_$target->{conf}_$profile->{id}
	bool "$profile->{name}"
	depends LINUX_$target->{conf}
EOF
		foreach my $pkg (@{$target->{defaultpkgs}}, @{$profile->{pkgs}}) {
			print "\tselect DEFAULT_$pkg\n";
		}
		print "\n";
	}
}

print "endchoice\n";
