#!/usr/bin/perl
use strict;

my $name;
my $src;
my $makefile;
my %pkg;

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
	$line =~ /^Depends: \s*(.+)\s*$/ and do {
		my @dep = split /,\s*/, $1;
		$pkg{$name}->{depends} = \@dep;
	};
}

foreach $name (sort {uc($a) cmp uc($b)} keys %pkg) {
	my $hasdeps = 0;
	$line = "$pkg{$name}->{src}-compile:";
	foreach my $dep (@{$pkg{$name}->{depends}}) {
	        if (defined $pkg{$dep}->{src} && $pkg{$name}->{src} ne $pkg{$dep}->{src}) {
			$hasdeps = 1;
			$line .= " $pkg{$dep}->{src}-compile";
		}
	}
	if ($hasdeps) {
		print "$line\n";
	}
}
