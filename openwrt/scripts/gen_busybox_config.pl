#!/usr/bin/perl
use strict;

my $line;
my $l1 = '';
my $l2 = '=y';
while (<>) {
	chomp;
	/^(# )BR2_LARGEFILE(.+)$/ and do {
		$l1 = $1;
		$l2 = $2;
	};
	/^(# )?BUSYBOX_(.+)/ and do {
		my $p1 = $1;
		my $p2 = $2;
		$p2 =~ /(CONFIG_LFS|FDISK_SUPPORT_LARGE_DISKS)/ and do {
			$p1 = $l1;
			$p2 = "$1$l2";
		};
		print "$p1$p2\n";
	}
}
