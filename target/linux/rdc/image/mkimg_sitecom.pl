#!/usr/bin/perl

$bzimgsize = -s @ARGV[0];
$padding = 4 - ($bzimgsize + 2) % 4;
if ($padding == 4) {
	$padding = 0; }
open (bzimg, @ARGV[0]);
while (<bzimg>) { $i .= $_; }
$i .= "\0"x$padding;
$i .= pack "v", -(unpack "%v*", $i); 
print "CSYS";
print pack("V", length($i));
print pack("V", 0);
#-s @ARGV[1]
print "WRRM";
print pack("V", length($i));
print $i;
