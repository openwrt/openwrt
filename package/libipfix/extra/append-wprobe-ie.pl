use strict;

my @fields = (
	[ "_n", "UINT", " - Number of samples", 4 ],
	[ "_s", "UINT", " - Sum of samples", 8 ],
	[ "_ss", "UINT", " - Sum of squared samples", 8 ],
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
			@f = [ "", "UINT", "", 4];
		} else {
			@f = @fields;
		}
		foreach my $f (@f) {
			my $nr = ++$last_ie;
			my $n = $f->[0];
			my $N = uc $n;
			my $ftype = $f->[1];
			my $fdesc = $f->[2];
			my $size = $f->[3];
			print "$nr, IPFIX_FT_WPROBE_$rfield$N, $size, IPFIX_CODING_$ftype, \"$nfield$n\", \"$descr$fdesc\"\n";
		}
	};
}

