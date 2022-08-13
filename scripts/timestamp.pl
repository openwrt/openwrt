#!/usr/bin/env perl
# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

use strict;
use POSIX;

sub get_ts($$) {
	my $path = shift;
	my $options = shift;
	my $ts = 0;
	my $fn = "";
	open FIND, "find -L $path '!' -path \\*/.svn\\* '!' -path \\*CVS\\* $options 2>/dev/null |";
	while (<FIND>) {
		chomp;
		my $file = $_;
		next if -l $file;
		my $mt = (stat $file)[9];
		if ($mt > $ts) {
			$ts = $mt;
			$fn = $file;
			last;
		}
	}
	close FIND;
	return ($ts, $fn);
}

sub get_fnames($$) {
	my $path = shift;
	my $options = shift;
	my $ts = 0;
	my @fns = ();
	open FIND, "find -L $path '!' -path \\*/.svn\\* '!' -path \\*CVS\\* $options 2>/dev/null |";
	while (<FIND>) {
		chomp;
		my $file = $_;
		next if -l $file;
		push(@fns, $file);
	}
	close FIND;
	return @fns;
}

(@ARGV > 0) or push @ARGV, ".";
my $n = ".";
my $t = "0";
my $ts = 0;
my $cs = 0;
my $namef = "%s\t";
my $timef = "%u\n";
my @fnames;
my %options;
my $path;
while (@ARGV > 0) {
	$path = shift @ARGV;
	if ($path =~ /^-x/) {
		my $arg = shift @ARGV;
		$options{-findopts} .= " '!' -path '" . $arg . "'"
	} elsif ($path =~ /^-a/) {
		my $arg = shift @ARGV;
		$options{-findopts} .= " " . $arg;
	} elsif ($path =~ /^-n/) {
		my $arg = $ARGV[0];
		$options{$path} = $arg;
	} elsif ($path =~ /^-cold.*/) {
		my $path = shift @ARGV;
		$cs = (stat $path)[9];
		$options{-c} = -1;
	} elsif ($path =~ /^-ceq.*/) {
		my $path = shift @ARGV;
		$cs = (stat $path)[9];
		$options{-c} = 0;
	} elsif ($path =~ /^-cnew.*/) {
		my $path = shift @ARGV;
		$cs = (stat $path)[9];
		$options{-c} = 1;
	} elsif ($path =~ /^-p/) {
		$namef = "%s\n";
		$timef = "";
	} elsif ($path =~ /^-t/) {
		$namef = "";
		$timef = "%s\n";
	} elsif ($path =~ /^-P/) {
		$options{$path} = 1;
	} elsif ($path =~ /^-T/) {
		$options{$path} = 1;
		$timef = "%s\n";
	} elsif ($path =~ /^-r/) {
		$options{$path} = 1;
	} elsif ($path =~ /^--/) {
		$options{-r} = 1;
		my $path = shift @ARGV;
		foreach my $paths(@ARGV) {
			$path .= " $paths";
		}
		@ARGV = ($path);
	} elsif ($path =~ /^-/) {
		my $path = <STDIN>;
		chomp($path);
		@ARGV = ($path);
	}
}

if ($options{-r}) {
	@fnames = get_fnames($path, $options{-findopts});
} else {
	my ($tmp, $fname) = get_ts($path, $options{-findopts});
	if ($tmp > $ts) {
		$n = $fname;
		($t, $ts) = ($tmp, $tmp);
	}
}

sub print_cs {
	$n =~ s{^.*/}{} if ($options{-P});
	$n .= "/" if (-d $n);
	$t = strftime("%Y-%m-%d %H:%M:%S %z", localtime($ts)) if ($options{-T});
	($ts <=> $cs) == $options{-c} and (printf $namef, $n and printf $timef, $t);
}

sub print_ts {
	$n =~ s{^.*/}{} if ($options{-P});
	$n .= "/" if (-d $n);
	$t = strftime("%Y-%m-%d %H:%M:%S %z", localtime($ts)) if ($options{-T});
	printf $namef, $n and printf $timef, $t;
}

if ($options{-n}) {
	exit ($n eq $options{-n} ? 0 : 1);
} elsif ($options{-r}) {
	foreach my $fname(@fnames) {
		$n = $fname;
		$ts = (stat $fname)[9];
		$t = $ts;
		if ($cs) {
			print_cs;
		} else {
			print_ts;
		}
	}
} elsif ($cs) {
	($ts <=> $cs) == $options{-c} and print_cs or exit(1);
} else {
	print_ts;
}
