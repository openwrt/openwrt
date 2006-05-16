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

$line="";

foreach $name (sort {uc($a) cmp uc($b)} keys %pkg) {
	print "package-\$(CONFIG_PACKAGE_$name) += $pkg{$name}->{src}\n";

	my $hasdeps = 0;
	my $depline = "";
	foreach my $dep (@{$pkg{$name}->{depends}}) {
	        if (defined $pkg{$dep}->{src} && $pkg{$name}->{src} ne $pkg{$dep}->{src}) {
			$depline .= " $pkg{$dep}->{src}-compile";
		}
	}
	if ($depline ne "") {
		$line .= "$pkg{$name}->{src}-compile: $depline\n";
	}
}

if ($line ne "") {
	print "\n$line";
}
