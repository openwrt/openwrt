use strict;

my @fields = (
	[ "_n", "UINT", " - Number of samples", 4 ],
	[ "_s", "UINT", " - Sum of samples", 8 ],
	[ "_ss", "UINT", " - Sum of squared samples", 8 ],
);

my $file = $ARGV[0] or die "Syntax: $0 <file> <start>\n";
-f $file or die "File not found\n";
my $start = $ARGV[1];
$start =~ /^\d+$/ or die "Invalid start number";
open FILE, "<$file" or die "Can't open file";
while (<FILE>) {
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
			my $nr = $start++;
			my $n = $f->[0];
			my $N = uc $n;
			my $ftype = $f->[1];
			my $fdesc = $f->[2];
			my $size = $f->[3];
			print "$nr, IPFIX_FT_WPROBE_$rfield$N, $size, IPFIX_CODING_$ftype, \"$nfield$n\", \"$descr$fdesc\"\n";
		}
	};
}
close FILE;

