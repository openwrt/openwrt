#!/usr/bin/perl

open (bzimg, @ARGV[0]);
while (<bzimg>) { $i .= $_; }
$i .= pack "v", -(unpack "%v*", $i); 
print "CSYS";
print pack("V", length($i));
print pack("V", 0);
print "WRRM";
print pack("V", length($i));
print $i;
