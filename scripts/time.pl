#!/usr/bin/env perl

use strict;
use warnings;
use Time::HiRes qw(gettimeofday);

if (@ARGV < 2) {
	die "Usage: $0 <prefix> <command...>\n";
}

my ($prefix, @cmd) = @ARGV;
my ($sec, $msec) = gettimeofday();
my $pid = fork();

if (!defined($pid)) {
	die "$0: Failure to fork(): $!\n";
}
elsif ($pid == 0) {
	exec(@cmd);
	die "$0: Failure to exec(): $!\n";
}
else {
	$SIG{'INT'} = 'IGNORE';
	$SIG{'QUIT'} = 'IGNORE';

	if (waitpid($pid, 0) == -1) {
		die "$0: Failure to waitpid(): $!\n";
	}

	my $exitcode = $? >> 8;
	my ($sec2, $msec2) = gettimeofday();
	my (undef, undef, $cuser, $csystem) = times();

	printf STDERR "%s#%.2f#%.2f#%.2f\n",
		$prefix, $cuser, $csystem,
		($sec2 - $sec) + ($msec2 - $msec) / 1000000;

	$SIG{'INT'} = 'DEFAULT';
	$SIG{'QUIT'} = 'DEFAULT';

	exit $exitcode;
}
