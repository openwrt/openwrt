#!/usr/bin/env perl
# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

use strict;

# The comparisons below are strict and the stamp file is scanned first, so a
# file that shares a timestamp with the stamp counts as older and the caller
# skips the work. Perl truncates the mtime of core stat to whole seconds, which
# makes that tie reachable whenever two touches fall in the same second. Ask
# Time::HiRes for the fractional mtime, and keep core stat where the module is
# missing.
BEGIN { eval { require Time::HiRes; Time::HiRes->import('stat') } }

sub get_ts($$) {
	my $path = shift;
	my $options = shift;
	my $ts = 0;
	my $fn = "";
	$path .= "/" if( -d $path);
	open FIND, "find $path -type f -and -not -path \\*/.svn\\* -and -not -path \\*CVS\\* $options 2>/dev/null |";
	while (<FIND>) {
		chomp;
		my $file = $_;
		next if -l $file;
		my $mt = (stat $file)[9];
		if ($mt > $ts) {
			$ts = $mt;
			$fn = $file;
		}
	}
	close FIND;
	return ($ts, $fn);
}

sub any_newer($$$) {
	my ($stamp, $paths, $options) = @_;
	my $list = join(" ", map { -d $_ ? "$_/" : $_ } @$paths);

	open FIND, "find $list -type f -and -not -path \\*/.svn\\* -and -not -path \\*CVS\\* $options -newer $stamp -print -quit 2>/dev/null |";
	my $hit = <FIND>;
	close FIND;

	return defined $hit;
}

(@ARGV > 0) or push @ARGV, ".";
my $ts = 0;
my $n = ".";
my %options;
my @paths;
while (@ARGV > 0) {
	my $path = shift @ARGV;
	if ($path =~ /^-x/) {
		my $str = shift @ARGV;
		$options{"findopts"} .= " -and -not -path '".$str."'"
	} elsif ($path =~ /^-f/) {
		$options{"findopts"} .= " -follow";
	} elsif ($path =~ /^-n/) {
		my $arg = $ARGV[0];
		$options{$path} = $arg;
	} elsif ($path =~ /^-/) {
		$options{$path} = 1;
	} else {
		push @paths, $path;
	}
}

my $findopts = $options{"findopts"} || "";

if (defined $options{"-n"} && !$options{"-p"} && !$options{"-t"} &&
    !$options{"-F"} && $findopts !~ /-follow/) {
	exit 1 unless -f $options{"-n"};
	exit(any_newer($options{"-n"}, \@paths, $findopts) ? 1 : 0);
}

for my $path (@paths) {
	my ($tmp, $fname) = get_ts($path, $findopts);
	if ($tmp > $ts) {
		if ($options{'-F'}) {
			$n = $fname;
		} else {
			$n = $path;
		}
		$ts = $tmp;
	}
}

if ($options{"-n"}) {
	exit ($n eq $options{"-n"} ? 0 : 1);
} elsif ($options{"-p"}) {
	print "$n\n";
} elsif ($options{"-t"}) {
	print "$ts\n";
} else {
	print "$n\t$ts\n";
}
