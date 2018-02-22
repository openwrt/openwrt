#!/usr/bin/perl
# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

use strict;
my $PATH = $ARGV[0];
($PATH and -d $PATH) or die 'invalid path';
my $DEFCONFIG = $ARGV[1];
($DEFCONFIG and -f $DEFCONFIG) or die 'invalid config file';

my %config;

open CONFIG, $DEFCONFIG or die 'cannot open config file';
while (<CONFIG>) {
	/^CONFIG_([\w_]+)=([ym])/ and $config{$1} = $2;
	/^CONFIG_([\w_]+)=(\d+)/ and $config{$1} = $2;
	/^CONFIG_([\w_]+)=(".+")/ and $config{$1} = $2;
}
close CONFIG;

open FIND, "find \"$PATH\" -name Kconfig |";
while (<FIND>) {
	chomp;
	my $input = $_;
	s/^$PATH\///g;
	s/sysdeps\/linux\///g;
	my $output = $_;
	print STDERR "$input => $output\n";
	$output =~ /^(.+)\/[^\/]+$/ and system("mkdir -p $1");
	
	open INPUT, $input;
	open OUTPUT, ">$output";
	my ($cur, $default_set, $line);
	while ($line = <INPUT>) {
		next if $line =~ /^\s*mainmenu/;

		# FIXME: make this dynamic
		$line =~ s/default FEATURE_BUFFERS_USE_MALLOC/default FEATURE_BUFFERS_GO_ON_STACK/;
		$line =~ s/default FEATURE_SH_IS_NONE/default FEATURE_SH_IS_ASH/;

		if ($line =~ /^\s*config\s*([\w_]+)/) {
			$cur = $1;
			undef $default_set;
		}
		if ($line =~ /^\s*(menu|choice|end|source)/) {
			undef $cur;
			undef $default_set;
		}
		$line =~ s/^(\s*source\s+)/$1package\/feeds\/ifx_feeds_uboot\/open_uboot\/config\//;
		
		$line =~ s/^(\s*(prompt "[^"]+" if|config|depends|depends on|select|default|default \w if)\s+\!?)([A-Z_])/$1UBOOT_CONFIG_$3/g;
		$line =~ s/(( \|\| | \&\& | \( )!?)([A-Z_])/$1UBOOT_CONFIG_$3/g;
		#$line =~ s/(\( ?!?)([A-Z_]+ (\|\||&&))/$1UBOOT_CONFIG_$2/g;
		
		if ($cur) {
			($cur eq 'LFS') and do {
				$line =~ s/^(\s*(bool|tristate|string))\s*".+"$/$1/;
			};
			if ($line =~ /^\s*default/) {
				my $c;
				$default_set = 1;
				$c = $config{$cur} or $c = 'n';

				#$line =~ s/^(\s*default\s*)(\w+|"[^"]*")(.*)/$1$c$3/;
			}
		}
		
		print OUTPUT $line;
	}
	close OUTPUT;
	close INPUT;
	
}
close FIND;
