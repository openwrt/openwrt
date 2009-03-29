use strict;

my @fields = (
	[ "_avg", "FLOAT", " - Average" ],
	[ "_stdev", "FLOAT", " - Standard deviation" ],
	[ "_n", "UINT", " - Number of samples" ]
);

my $file = $ARGV[0] or die "Syntax: $0 <file>\n";
-f $file or die "File not found\n";
my $last_ie = 0;
my $line;
open IES, "<$file" or die "Can't open file";
while ($line = <IES>) {
	$line =~ /^(\d+)\s*,/ and $last_ie = $1;
}
close IES;
while (<STDIN>) {
	/^(%?)(\w+),\s*(\w+),\s*(.+)$/ and do {
		my $counter = $1;
		my $rfield = $2;
		my $nfield = $3;
		my $descr = $4;
		my @f;
		if ($counter) {
			@f = [ "", "UINT", "" ];
		} else {
			@f = @fields;
		}
		foreach my $f (@f) {
			my $nr = ++$last_ie;
			my $n = $f->[0];
			my $N = uc $n;
			my $ftype = $f->[1];
			my $fdesc = $f->[2];
			print "$nr, IPFIX_FT_WPROBE_$rfield$N, 4, IPFIX_CODING_$ftype, \"$nfield$n\", \"$descr$fdesc\"\n";
		}
	};
}

