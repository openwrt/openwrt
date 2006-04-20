#!/usr/bin/perl
use strict;

sub get_ts($) {
	my $path = shift;
	my $ts = 0;
	open FIND, "find $path -not -path \\*.svn\\* -and -not -path \\*CVS\\* 2>/dev/null |";
	while (<FIND>) {
		open FILE, "<$_";
		my @stat = stat FILE;
		close FILE;
		$ts = $stat[9] if ($stat[9] > $ts);
	}
	close FIND;
	return $ts;
}

(@ARGV > 0) or push @ARGV, ".";
my $ts = 0;
my $n = ".";
my %options;
foreach my $path (@ARGV) {
	if ($path =~ /^-/) {
		$options{$path} = 1;
	} else {
		my $tmp = get_ts($path);
		if ($tmp > $ts) {
			$n = $path;
			$ts = $tmp;
		}
	}
}

if ($options{"-p"}) {
	print "$n\n";
} elsif ($options{"-t"}) {
	print "$ts\n";
} else {
	print "$n\t$ts\n";
}
