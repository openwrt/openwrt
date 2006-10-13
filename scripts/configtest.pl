#!/usr/bin/perl
# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

my %change = (
	'^BUSYBOX' => 'make package/busybox-clean',
	'OPENVPN_' => 'make package/openvpn-clean',
	'SYSCONF_' => 'make package/base-files-clean target_clean',
	'' => 'make target_clean'
);

my @configfiles = (
#	['package/linux/linux.config', 'build_mipsel/linux/.config' =>
#		'make -C package linux-clean']
);



foreach my $change (keys %change) {
	my $v1 = `grep '$change' .config.test`;
	my $v2 = `grep '$change' .config`;
	$v1 eq $v2 or system($change{$change});
}

foreach my $file (@configfiles) {
	if ((-f $file->[0]) and (-f $file->[1])) {
		my @s1 = stat $file->[0];
		my @s2 = stat $file->[1];
		$s1[9] > $s2[9] and system($file->[2]);
	}
}
