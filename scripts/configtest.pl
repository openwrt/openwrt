#!/usr/bin/perl

my %change = (
	'BUSYBOX' => 'make -C package busybox-clean',
	'' => 'make target_clean'
);

foreach my $change (keys %change) {
	my $v1 = `grep '$change' .config.test`;
	my $v2 = `grep '$change' .config`;
	$v1 eq $v2 or system($change{$change});
}
