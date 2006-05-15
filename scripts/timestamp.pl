#!/usr/bin/perl
use strict;
use File::stat;

sub crawl($$) {
	my $path = shift;
	my $options = shift;
	my @results = $path;
	opendir(DIR,$path);
	foreach my $file (readdir(DIR)) {
		if ($file !~m/^(\.(svn|\.?)|CVS$options)$/) {
			push @results, crawl("$path/$file",$options);
		}
	}
	closedir(DIR);
	return @results; 
}

sub get_ts($$) {
	my $path = shift;
	my $options = shift;
	my $ts = 0;
	my $fn = "";
	my @search = crawl($path,$options);
	while (@search) {
		my $file = shift @search;
		my $mtime = stat($file)->mtime;
		if ($mtime > $ts) {
			$ts = $mtime;
			$fn = $file;
		}
	}
	return ($ts, $fn);
}

(@ARGV > 0) or push @ARGV, ".";
my $ts = 0;
my $n = ".";
my %options;
while (@ARGV > 0) {
	my $path = shift @ARGV;
	if ($path =~ /^-x/) {
		my $str = shift @ARGV;
		$options{"-x"} .= "|".$str;
	} elsif ($path =~ /^-/) {
		$options{$path} = 1;
	} else {
		my ($tmp, $fname) = get_ts($path, $options{"-x"});
		if ($tmp > $ts) {
			if ($options{'-f'}) {
				$n = $fname;
			} else {
				$n = $path;
			}
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
