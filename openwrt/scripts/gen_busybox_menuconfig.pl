#!/usr/bin/perl
use strict;
my $PATH = $ARGV[0];
($PATH and -d $PATH) or die 'invalid path';
my $DEFCONFIG = $ARGV[1];
($DEFCONFIG and -f $DEFCONFIG) or die 'invalid config file';

my %config;

open CONFIG, $DEFCONFIG or die 'cannot open config file';
while (<CONFIG>) {
	/^([\w_]+)=([ym])/ and $config{$1} = $2;
	/^([\w_]+)=(\d+)/ and $config{$1} = $2;
	/^([\w_]+)=(".+")/ and $config{$1} = $2;
}
close CONFIG;

open FIND, "find \"$PATH\" -name Config.in |";
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
		$line =~ s/default CONFIG_FEATURE_BUFFERS_USE_MALLOC/default CONFIG_FEATURE_BUFFERS_GO_ON_STACK/;
		$line =~ s/default BUSYBOX_CONFIG_FEATURE_SH_IS_NONE/default BUSYBOX_CONFIG_FEATURE_SH_IS_ASH/;

		if ($line =~ /^\s*config\s*([\w_]+)/) {
			$cur = $1;
			undef $default_set;
		}
		if ($line =~ /^\s*(menu|choice|end|source)/) {
			undef $cur;
			undef $default_set;
		}
		$line =~ s/^(\s*source\s+)/$1package\/busybox\/config\//;
		
		$line =~ s/(\s+)((CONFIG|FDISK|USING|CROSS|EXTRA|PREFIX|FEATURE|HAVE|BUSYBOX)[\w_]*)/$1BUSYBOX_$2/g;
		
		if ($cur) {
			($cur !~ /^CONFIG/ or $cur eq 'CONFIG_LFS') and do {
				$line =~ s/^(\s*(bool|tristate|string))\s*".+"$/$1/;
			};
			if ($line =~ /^\s*default/) {
				my $c;
				$default_set = 1;
				$c = $config{$cur} or $c = 'n';

				$line =~ s/^(\s*default\s*)(\w+|"[^"]*")(.*)/$1$c$3/;
			}
		}
		
		print OUTPUT $line;
	}
	close OUTPUT;
	close INPUT;
	
}
close FIND;
