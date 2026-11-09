#!/usr/bin/env perl
#
# SPDX-License-Identifier: GPL-2.0-only
#
# Turns a BUILD_TIME_LOG file into a report.
#
# Usage: build-time-report.pl [-n <count>] [-s <stage>] <logfile>
#
# Wall share is the share of the build duration that a stage is responsible
# for. Each second of the build is divided between the stages that run in it,
# so the wall shares of all stages add up to the busy time. A stage that runs
# alone on an idle machine gets the whole second. Solo time counts only the
# seconds in which a stage is the one thing that runs, so it is the part of
# the build that a faster stage shortens directly.

use strict;
use warnings;

my $top = 20;
my $only_stage;

while (@ARGV && $ARGV[0] =~ /^-/) {
	my $opt = shift @ARGV;
	if ($opt eq '-n') { $top = shift @ARGV }
	elsif ($opt eq '-s') { $only_stage = shift @ARGV }
	else { die "Usage: $0 [-n <count>] [-s <stage>] <logfile>\n" }
}

my $file = shift @ARGV or die "Usage: $0 [-n <count>] [-s <stage>] <logfile>\n";

open(my $fh, '<', $file) or die "$0: cannot read $file: $!\n";

my (@iv, %pending);

while (my $line = <$fh>) {
	chomp $line;
	my ($time, $event, $stage, $name) = split(/\t/, $line, 4);
	next unless defined $name;
	next unless defined $time && $time =~ /^[0-9]+(\.[0-9]+)?$/;
	next if defined $only_stage && $stage ne $only_stage;

	my $key = "$stage\t$name";

	if ($event eq 'begin') {
		push @{$pending{$key}}, $time;
		next;
	}

	next unless $event eq 'end';
	my $start = $pending{$key} ? pop @{$pending{$key}} : undef;
	next unless defined $start;

	push @iv, {
		idx => scalar @iv,
		start => $start,
		end => $time,
		dur => $time - $start,
		stage => $stage,
		name => $name,
		share => 0,
		solo => 0,
	};
}

close($fh);

my $incomplete = 0;
$incomplete += scalar @{$pending{$_}} for keys %pending;

die "$0: no completed stages in $file\n" unless @iv;

my @events;
for my $iv (@iv) {
	push @events, [ $iv->{start}, 1, $iv ];
	push @events, [ $iv->{end}, 0, $iv ];
}

# An end event sorts before a begin event at the same instant. One stage that
# hands over to the next one must not look like parallel work.
@events = sort { $a->[0] <=> $b->[0] || $a->[1] <=> $b->[1] } @events;

my (%active, $idle, $busy);
$idle = 0;
$busy = 0;

my $prev = $events[0][0];

for my $ev (@events) {
	my ($t, $is_start, $iv) = @$ev;
	my $dt = $t - $prev;

	if ($dt > 0) {
		my @cur = values %active;
		my $n = scalar @cur;

		if (!$n) {
			$idle += $dt;
		}
		else {
			$busy += $dt;
			my $part = $dt / $n;
			$_->{share} += $part for @cur;
			$cur[0]->{solo} += $dt if $n == 1;
		}
	}

	if ($is_start) { $active{$iv->{idx}} = $iv }
	else { delete $active{$iv->{idx}} }

	$prev = $t;
}

my $total = $events[-1][0] - $events[0][0];
my $sum_dur = 0;
$sum_dur += $_->{dur} for @iv;

my (%st, %pkg);

for my $iv (@iv) {
	my $s = $st{$iv->{stage}} ||= { count => 0, dur => 0, share => 0, solo => 0 };
	$s->{count}++;
	$s->{dur} += $iv->{dur};
	$s->{share} += $iv->{share};
	$s->{solo} += $iv->{solo};

	my $key = sprintf("%s [%s]", $iv->{name}, $iv->{stage});
	my $p = $pkg{$key} ||= { dur => 0, share => 0, solo => 0 };
	$p->{dur} += $iv->{dur};
	$p->{share} += $iv->{share};
	$p->{solo} += $iv->{solo};
}

sub hms {
	my $s = shift;
	return sprintf("%d:%02d:%05.2f",
		int($s / 3600), int($s / 60) % 60, $s - 60 * int($s / 60));
}

sub pct {
	my ($a, $b) = @_;
	return $b > 0 ? 100 * $a / $b : 0;
}

printf "build window      %s\n", hms($total);
printf "busy              %s (%.1f%%)\n", hms($busy), pct($busy, $total);
printf "idle              %s (%.1f%%)\n", hms($idle), pct($idle, $total);
printf "stage time        %s\n", hms($sum_dur);
printf "mean parallelism  %.2f\n", $busy > 0 ? $sum_dur / $busy : 0;
printf "stages completed  %d\n", scalar @iv;
printf "stages unfinished %d\n", $incomplete if $incomplete;
print "\n";

printf "%-10s %6s %12s %12s %8s %12s %8s\n",
	'stage', 'count', 'sum', 'wall share', 'of wall', 'solo', 'of wall';

for my $name (sort { $st{$b}{share} <=> $st{$a}{share} } keys %st) {
	my $s = $st{$name};
	printf "%-10s %6d %12s %12s %7.1f%% %12s %7.1f%%\n",
		$name, $s->{count}, hms($s->{dur}), hms($s->{share}),
		pct($s->{share}, $total), hms($s->{solo}), pct($s->{solo}, $total);
}

sub table {
	my ($title, $key, $ref) = @_;

	print "\n$title\n";
	printf "%-56s %12s %12s %12s\n", 'stage', 'duration', 'wall share', 'solo';

	my @k = sort { $ref->{$b}{$key} <=> $ref->{$a}{$key} } keys %$ref;
	splice(@k, $top) if @k > $top;

	for my $name (@k) {
		my $e = $ref->{$name};
		printf "%-56s %12s %12s %12s\n",
			$name, hms($e->{dur}), hms($e->{share}), hms($e->{solo});
	}
}

table("top $top by wall share", 'share', \%pkg);
table("top $top by solo time", 'solo', \%pkg);
table("top $top by duration", 'dur', \%pkg);
