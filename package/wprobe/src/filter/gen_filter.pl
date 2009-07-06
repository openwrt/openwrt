#!/usr/bin/perl
use strict;

# helpers for custom packet format
# bytes 0-7 are used by a dummy radiotap header
my $WLAN_LEN = "radio[8:2]";
my $SNR = "radio[10:1]";
my $DEFAULT = undef;

my $MAGIC = "WPFF";
my $VERSION = 1; # filter binary format version
my $HDRLEN = 3; # assumed storage space for custom fields

my $output = "filter.bin";
my $config = {
	"packetsize" => [
		[ "small", "$WLAN_LEN < 250" ],
		[ "medium", "$WLAN_LEN < 800" ],
		[ "big", $DEFAULT ],
	],
	"snr" => [
		[ "low", "$SNR < 10" ],
		[ "medium", "$SNR < 20" ],
		[ "high", $DEFAULT ],
	],
	"type" => [
		[ "beacon", "type mgt subtype beacon" ],
		[ "data", "type data subtype data" ],
		[ "qosdata", "type data subtype qos-data" ],
		[ "other", "$DEFAULT" ]
	]
};

sub escape_q($) {
	my $str = shift;
	$str =~ s/'/'\\''/g;
	return $str;
}

my $GROUPS = scalar(keys %$config);
open OUTPUT, ">$output" or die "Cannot open output file: $!\n";
print OUTPUT pack("a4CCn", $MAGIC, $VERSION, $HDRLEN, $GROUPS);

foreach my $groupname (keys %$config) {
	my $default = 0;
	my $group = $config->{$groupname};
	print OUTPUT pack("a32N", $groupname, scalar(@$group));
	foreach my $filter (@$group) {
		if (!$filter->[1]) {
			$default > 0 and print "Cannot add more than one default filter per group: $groupname -> ".$filter->[0]."\n";
			print OUTPUT pack("a32N", $filter->[0], 0);
			$default++;
		} else {
			open FILTER, "./pfc '".escape_q($filter->[0])."' '".escape_q($filter->[1])."' |"
				or die "Failed to run filter command for '".$filter->[0]."': $!\n";
			while (<FILTER>) {
				print OUTPUT $_;
			}
			close FILTER;
			$? and die "Filter '".$filter->[0]."' did not compile.\n";
		}
	}
}
