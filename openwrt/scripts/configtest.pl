#!/usr/bin/perl

my %change = (
	'ROOTFS' => 'make linux-dirclean'
);

foreach my $change (keys %change) {
	my $v1 = `grep '$change' .config.test`;
	my $v2 = `grep '$change' .config`;
	$v1 eq $v2 or system($change{$change});
}
