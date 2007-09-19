#!/usr/bin/env perl
# 
# Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

use warnings;
use strict;

my @arg = @ARGV;

sub load_config($) {
	my $file = shift;
	my %config;

	open FILE, "$file" or die "can't open file";
	while (<FILE>) {
		chomp;
		/^CONFIG_(.+?)=(.+)/ and do {
			$config{$1} = $2;
			next;
		};
		/^# CONFIG_(.+?) is not set/ and do {
			$config{$1} = "#undef";
			next;
		};
		/^#/ and next;
		/^(.+)$/ and print "WARNING: can't parse line: $1\n";
	}
	return \%config;
}


sub config_and($$) {
	my $cfg1 = shift;
	my $cfg2 = shift;
	my %config;

	foreach my $config (keys %$cfg1) {
		my $val1 = $cfg1->{$config};
		my $val2 = $cfg2->{$config};
		$val2 and ($val1 eq $val2) and do {
			$config{$config} = $val1;
		};
	}
	return \%config;
}


sub config_add($$$) {
	my $cfg1 = shift;
	my $cfg2 = shift;
	my $mod_plus = shift;
	my %config;
	
	for ($cfg1, $cfg2) {
		my %cfg = %$_;
		
		foreach my $config (keys %cfg) {
			next if $mod_plus and $config{$config} and $config{$config} eq "y";
			$config{$config} = $cfg{$config};
		}
	}
	return \%config;
}

sub config_diff($$) {
	my $cfg1 = shift;
	my $cfg2 = shift;
	my %config;
	
	foreach my $config (keys %$cfg2) {
		if (!defined($cfg1->{$config}) or $cfg1->{$config} ne $cfg2->{$config}) {
			$config{$config} = $cfg2->{$config};
		}
	}
	return \%config
}

sub config_sub($$) {
	my $cfg1 = shift;
	my $cfg2 = shift;
	my %config = %{$cfg1};
	
	foreach my $config (keys %$cfg2) {
		delete $config{$config};
	}
	return \%config;
}

sub print_cfgline($$) {
	my $name = shift;
	my $val = shift;
	if ($val eq '#undef') {
		print "# CONFIG_$name is not set\n";
	} else {
		print "CONFIG_$name=$val\n";
	}
}


sub dump_config($) {
	my $cfg = shift;
	die "argument error in dump_config" unless ($cfg);
	my %config = %$cfg;
	foreach my $config (sort keys %config) {
		print_cfgline($config, $config{$config});
	}
}

sub parse_expr($);

sub parse_expr($) {
	my $pos = shift;
	my $arg = $arg[$$pos++];
	
	die "Parse error" if (!$arg);
	
	if ($arg eq '&') {
		my $arg1 = parse_expr($pos);
		my $arg2 = parse_expr($pos);
		return config_and($arg1, $arg2);
	} elsif ($arg =~ /^\+/) {
		my $arg1 = parse_expr($pos);
		my $arg2 = parse_expr($pos);
		return config_add($arg1, $arg2, 0);
	} elsif ($arg =~ /^m\+/) {
		my $arg1 = parse_expr($pos);
		my $arg2 = parse_expr($pos);
		return config_add($arg1, $arg2, 1);
	} elsif ($arg eq '>') {
		my $arg1 = parse_expr($pos);
		my $arg2 = parse_expr($pos);
		return config_diff($arg1, $arg2);
	} elsif ($arg eq '-') {
		my $arg1 = parse_expr($pos);
		my $arg2 = parse_expr($pos);
		return config_sub($arg1, $arg2);
	} else {
		return load_config($arg);
	}
}

my $pos = 0;
dump_config(parse_expr(\$pos));
die "Parse error" if ($arg[$pos]);
