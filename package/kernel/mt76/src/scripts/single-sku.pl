#!/usr/bin/perl
use strict;
use 5.010;

my $indent = 0;

my %chip_types = (
	mt7615 => "mt7615",
	mt7622 => "mt7615",
	mt7663 => "mt7615"
);

my $chip = shift @ARGV;
my $chip_type;

$chip or usage();

sub usage() {
	print STDERR <<EOF;
Usage: $0 <chip> [options] <file> [[options] <file>...]

Options:
	country=XX:		Set country code
	regdomain=XX:		Set regdomain

Multiple country codes/regdomains per file are supported

EOF
	exit(1);
}

sub convert_array($) {
	my $array = shift;

	return unless $array;
	foreach my $i (0 .. $#{$array}) {
		$array->[$i] = int($array->[$i] * 2);
	}

	return $array;
}

sub parse_channel($$$) {
	my $band = shift;
	my $channels = $band->{channels};

	my $ch = shift;
	my $line = shift;

	my @data = split /\s+/, $line;
	my $data = join(" ", @data);

	my $channel = {
		chlist => [ ],
		data => $data,
		mcs => [],
	};

	$channels->{$ch} = $channel;

	$band->{type} eq '2' and do {
		$channel->{"rates-cck"} = convert_array([
			$data[0], $data[0],
			$data[1], $data[1]
		]);
		splice @data, 0, 2;
	};

	$channel->{"rates-ofdm"} = convert_array([
		$data[0], $data[0],
		$data[1], $data[1],
		$data[2], $data[2],
		$data[3], $data[4]
	]);
	splice @data, 0, 5;

	my @bw = ( "bw20", "bw40", "bw80", "bw160" );
	$band->{type} eq '2' and @bw = ( "bw20", "bw40" );

	foreach my $bw (@bw) {
		push @{$channel->{"rates-mcs"}}, convert_array([
			$data[0],
			$data[1], $data[1],
			$data[2], $data[2],
			$data[3], $data[3],
			$data[4],
			$data[5],
			$data[6],
		]);
		splice @data, 0, 7;
	};

	@data > 0 and do {
		$channel->{"txs-delta"} = convert_array([ reverse splice @data, 0, 3 ]);
		delete $channel->{"txs-delta"} if join("", @{$channel->{"txs-delta"}}) =~ /^0+$/;
	};
}

sub read_data($) {
	my $file = shift;
	my $band;
	my %bands;

	open FILE, "<", $file or die "Can't open file $file\n";
	while (<FILE>) {
		chomp;

		/^Band: (2.4|5)G / and do {
			$band = $1;
			$band eq '2.4' and $band = "2";
			$bands{$band} = {
				type => $band,
				channels => {},
			};
		};
		/^Ch(\d+)\s+(.+?)\s*$/ and parse_channel($bands{$band}, $1, $2);
	}
	close FILE;

	return \%bands;
}

sub find_matching_channel($$) {
	my $band = shift;
	my $channels = $band->{channels};

	my $ch_idx = shift;
	my $ch = $channels->{$ch_idx};

	foreach my $cur (sort { $a <=> $b } keys %$channels) {
		my $cur_ch;

		return undef if $cur >= $ch_idx;

		$cur_ch = $channels->{$cur};
		$cur_ch->{data} eq $ch->{data} and return $cur_ch;
	}
}

sub optimize_channels($) {
	my $band = shift;
	my $channels = $band->{channels};
	my $prev;
	my $prev_chlist;

	foreach my $ch_idx (sort { $a <=> $b } keys %$channels) {
		my $ch = $channels->{$ch_idx};

		$prev and ($ch->{data} eq $prev->{data}) and do {
			$prev_chlist->[1] = $ch_idx;
			delete $channels->{$ch_idx};
			next;
		};

		$prev = find_matching_channel($band, $ch_idx);
		if ($prev) {
			delete $channels->{$ch_idx};
		} else {
			$prev = $ch;
		}

		$prev_chlist = [ $ch_idx, $ch_idx ];
		push @{$prev->{chlist}}, $prev_chlist;
	}
}

sub add_line {
	my $line = shift;
	print "".("\t" x $indent).$line;
}

sub array_str($) {
	my $a = shift;

	return "<".join(" ", @$a).">";
}

sub string_array_str($) {
	my $a = shift;

	return join(", ", map { "\"$_\"" } @$a);
}

sub add_named_array($$) {
	my $ch = shift;
	my $type = shift;

	return unless $ch->{$type};
	add_line("$type = ".array_str($ch->{$type}).";\n");
}

sub add_named_string_array($$) {
	my $ch = shift;
	my $type = shift;

	return unless $ch->{$type} and @{$ch->{$type}} > 0;
	add_line("$type = ".string_array_str($ch->{$type}).";\n");
}

sub add_multi_array($$) {
	my $name = shift;
	my $a = shift;

	add_line("$name =");
	if (@$a > 1) {
		my $first = 1;

		print "\n";
		$indent++;
		foreach my $v (@$a) {
			$first or print ",\n";
			undef $first;

			add_line(array_str($v));
		}
		$indent--;
	} else {
		print " ".array_str($a->[0]);
	}
	print ";\n";
}

sub print_txpower($) {
	my $ch = shift;
	my @data;

	add_named_array($ch, "txs-delta");
	add_named_array($ch, "rates-cck");
	add_named_array($ch, "rates-ofdm");

	my $prev;
	foreach my $v (@{$ch->{"rates-mcs"}}) {
		my $val = [1, @{$v}];

		if ($prev and (array_str($v) eq array_str($prev))) {
			$data[$#data]->[0]++;
		} else {
			push @data, $val;
		}

		$prev = $v;
	}

	add_multi_array("rates-mcs", \@data);
};

sub print_channels($) {
	my $band = shift;
	my $channels = $band->{channels};
	my $r = 0;

	foreach my $ch_idx (sort { $a <=> $b } keys %$channels) {
		my $ch = $channels->{$ch_idx};
		my $first = 1;

		add_line("r$r {\n");
		$indent++;

		add_multi_array("channels", $ch->{chlist});
		print_txpower($ch);

		$indent--;
		add_line("};\n");

		$r++;
	}
}

sub print_bands($) {
	my $bands = shift;

	foreach my $band_idx (sort keys %$bands) {
		my $band = $bands->{$band_idx};

		optimize_channels($band);

		add_line("txpower-".$band->{type}."g {\n");
		$indent++;

		print_channels($band);

		$indent--;
		add_line("};\n");
	};
}

my @files;
my $cur = {};

$chip_type = $chip_types{$chip} or die "Unsupported chip type '$chip', supported chip types: ".join(", ",sort keys %chip_types)."\n";

while (@ARGV > 0) {
	my $arg = shift @ARGV;

	$cur or $cur = {
		country => [],
		regdomain => [],
	};

	if ($arg =~ /country=(\w+)$/) {
		push @{$cur->{country}}, $1;
	} elsif ($arg =~ /regdomain=(\w+)$/) {
		push @{$cur->{regdomain}}, $1;
	} else {
		$cur->{bands} = read_data($arg);
		push @files, $cur;
		$cur = undef;
	}
}


add_line("power-limits {\n");
$indent++;

my $count = 0;

foreach my $domain (@files) {
	my $name = "r$count";
	$count++;

	add_line("$name {\n");
	$indent++;

	add_named_string_array($domain, "country");
	add_named_string_array($domain, "regdomain");
	print_bands($domain->{bands});

	$indent--;
	add_line("};\n");
}

$indent--;
add_line("};\n");
