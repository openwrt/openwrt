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
	print "$name: ";
	foreach my $dep (@{$pkg{$name}->{depends}}) {
		print "$dep ";
	}
	print "\n\tmake -C ".$pkg{$name}->{src}."\n";
	print "\n";
}
