#!/usr/bin/perl
use strict;

my $name;
my $src;
my $makefile;
my %pkg;
my %dep;

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
	$line =~ /^(Build-)?Depends: \s*(.+)\s*$/ and do {
		$pkg{$name}->{depends} ||= [];
		foreach my $v (split /\s+/, $2) {
			next if $v =~ /^@/;
			$v =~ s/^\+//;
			push @{$pkg{$name}->{depends}}, $v;
		}
	};
}

$line="";

foreach $name (sort {uc($a) cmp uc($b)} keys %pkg) {
	print "package-\$(CONFIG_PACKAGE_$name) += $pkg{$name}->{src}\n";

	my $hasdeps = 0;
	my $depline = "";
	foreach my $dep (@{$pkg{$name}->{depends}}) {
		my $idx;
	        if (defined $pkg{$dep}->{src} && $pkg{$name}->{src} ne $pkg{$dep}->{src}) {
			$idx = $pkg{$dep}->{src};
		} elsif (defined $pkg{$dep}) {
			$idx = $dep;
		}
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
