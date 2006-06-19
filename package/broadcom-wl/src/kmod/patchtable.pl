#!/usr/bin/perl
use strict;

my $TABLE = pack("V", 0xbadc0ded);
my $TABLE_SIZE = 512;
my $SLT1 = "\x01\x00\x00\x00";
my $SLT2 = "\x02\x00\x00\x00";
my $ACKW = "\x03\x00\x00\x00";
my $PTABLE_END = "\xff\xff\xff\xff";

my $addr = "";
my $opcode = "";
my $function = "";

sub add_entry {
	my $key = shift;
	my $value = shift;
	my $default = shift;

	$TABLE .= $key;
	$TABLE .= pack("V", $value);
	$TABLE .= pack("V", $default);
}

while (<>) {
	$addr = $opcode = "";
	/^\w{8}\s*<(.*)>:$/ and $function = $1;
	/^\s*(\w+):\s*(\w{8})\s*/ and do {
		$addr = $1;
		$opcode = $2;
	};

	($function eq 'wlc_update_slot_timing') and do {
		# li	a2,9 	-- short slot time
		($opcode eq '24060009') and add_entry($SLT1, hex($addr), hex($opcode));
		# li	v0,519	-- 510 + short slot time
		($opcode eq '24020207') and add_entry($SLT2, hex($addr), hex($opcode));
		
		# li	a2,20	-- long slot time
		($opcode eq '24060014') and add_entry($SLT1, hex($addr), hex($opcode));
		# li	v0,530	-- 510 + long slot time
		($opcode eq '24020212') and add_entry($SLT2, hex($addr), hex($opcode));
	};
	($function eq 'wlc_d11hdrs') and do {
		# ori	s6,s6,0x1 -- ack flag (new)
		($opcode eq '36d60001') and add_entry($ACKW, hex($addr), hex($opcode));
		# ori	s3,s3,0x1 -- ack flag (old)
		($opcode eq '36730001') and add_entry($ACKW, hex($addr), hex($opcode));
	}
}

$TABLE .= $PTABLE_END;
$TABLE .= ("\x00" x ($TABLE_SIZE - length($TABLE)));
print $TABLE;
