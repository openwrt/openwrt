#!/usr/bin/perl
# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

use strict;

my $name;
my $src;
my $makefile;
my %conf;
my %pkg;
my %prereq;
my %dep;
my %options;
my $opt;

while ($opt = shift @ARGV) {
	$opt =~ /^-s/ and $options{SDK} = 1;
}

my $line;
while ($line = <>) {
	chomp $line;
	$line =~ /^Source-Makefile: \s*(.+\/([^\/]+)\/Makefile)\s*$/ and do {
		$makefile = $1;
		$src = $2;
	};
	$line =~ /^Package: \s*(.+)\s*$/ and do {
		$name = $1;
		defined $pkg{$name} or $pkg{$name} = {};
		$pkg{$name}->{src} = $src;
	};
	$line =~ /^Provides: \s*(.+)\s*$/ and do {
		foreach my $vpkg (split /\s+/, $1) {
			defined $pkg{$vpkg} or $pkg{$vpkg} = {};
			$pkg{$vpkg}->{virtual} = 1;
		}
	};
	$line =~ /^Prereq-Check:/ and !defined $prereq{$src} and do {
		$pkg{$name}->{prereq} = 1;
	};
	$line =~ /^(Build-)?Depends: \s*(.+)\s*$/ and do {
		$pkg{$name}->{depends} ||= [];
		foreach my $v (split /\s+/, $2) {
			next if $v =~ /^[\+]?@/;
			$v =~ s/^\+//;
			push @{$pkg{$name}->{depends}}, $v;
		}
	};
}

$line="";

foreach $name (sort {uc($a) cmp uc($b)} keys %pkg) {
	my $config;
	
	next if defined $pkg{$name}->{virtual};
	if ($options{SDK}) {
		$conf{$pkg{$name}->{src}} or do {
			$config = 'm';
			$conf{$pkg{$name}->{src}} = 1;
		};
	} else {
		$config = "\$(CONFIG_PACKAGE_$name)"
	}
	if ($config) {
		print "package-$config += $pkg{$name}->{src}\n";
		$pkg{$name}->{prereq} and print "prereq-$config += $pkg{$name}->{src}\n";
	}

	my $hasdeps = 0;
	my $depline = "";
	foreach my $dep (@{$pkg{$name}->{depends}}) {
		my $idx;
		next if defined $pkg{$dep}->{virtual};
		if (defined $pkg{$dep}->{src}) {
			($pkg{$name}->{src} ne $pkg{$dep}->{src}) and $idx = $pkg{$dep}->{src};
		} elsif (defined($pkg{$dep}) && !$options{SDK}) {
			$idx = $dep;
		}
		undef $idx if $idx =~ /^(kernel)|(base-files)$/;
		if ($idx) {
			next if $dep{$pkg{$name}->{src}."->".$idx};
			$depline .= " $idx\-compile";
			$dep{$pkg{$name}->{src}."->".$idx} = 1;
		}
	}
	if ($depline ne "") {
		$line .= "$pkg{$name}->{src}-compile: $depline\n";
	}
}

if ($line ne "") {
	print "\n$line";
}
