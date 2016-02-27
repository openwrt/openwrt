#!/usr/bin/env perl
# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

use strict;
use warnings;
use File::Basename;
use File::Copy;

@ARGV > 2 or die "Syntax: $0 <target dir> <filename> <hash> <url filename> [<mirror> ...]\n";

my $url_filename;
my $target = shift @ARGV;
my $filename = shift @ARGV;
my $file_hash = shift @ARGV;
$url_filename = shift @ARGV unless $ARGV[0] =~ /:\/\//;
my $scriptdir = dirname($0);
my @mirrors;
my $ok;

$url_filename or $url_filename = $filename;

sub localmirrors {
	my @mlist;
	open LM, "$scriptdir/localmirrors" and do {
	    while (<LM>) {
			chomp $_;
			push @mlist, $_ if $_;
		}
		close LM;
	};
	open CONFIG, "<".$ENV{'TOPDIR'}."/.config" and do {
		while (<CONFIG>) {
			/^CONFIG_LOCALMIRROR="(.+)"/ and do {
				chomp;
				my @local_mirrors = split(/;/, $1);
				push @mlist, @local_mirrors;
			};
		}
		close CONFIG;
	};

	my $mirror = $ENV{'DOWNLOAD_MIRROR'};
	$mirror and push @mlist, split(/;/, $mirror);

	return @mlist;
}

sub which($) {
	my $prog = shift;
	my $res = `which $prog`;
	$res or return undef;
	$res =~ /^no / and return undef;
	$res =~ /not found/ and return undef;
	return $res;
}

sub hash_cmd() {
	my $len = length($file_hash);
	my $cmd;

	$len == 64 and return "openssl dgst -sha256 | sed -e 's,.*= ,,'";
	$len == 32 and do {
		my $cmd = which("md5sum") || which("md5") || die 'no md5 checksum program found, please install md5 or md5sum';
		chomp $cmd;
		return $cmd;
	};
	return undef;
}

my $hash_cmd = hash_cmd();

sub download
{
	my $mirror = shift;
	my $options = $ENV{WGET_OPTIONS} || "";

	$mirror =~ s!/$!!;

	if ($mirror =~ s!^file://!!) {
		if (! -d "$mirror") {
			print STDERR "Wrong local cache directory -$mirror-.\n";
			cleanup();
			return;
		}

		if (! -d "$target") {
			system("mkdir", "-p", "$target/");
		}

		if (! open TMPDLS, "find $mirror -follow -name $filename 2>/dev/null |") {
			print("Failed to search for $filename in $mirror\n");
			return;
		}

		my $link;

		while (defined(my $line = readline TMPDLS)) {
			chomp ($link = $line);
			if ($. > 1) {
				print("$. or more instances of $filename in $mirror found . Only one instance allowed.\n");
				return;
			}
		}

		close TMPDLS;

		if (! $link) {
			print("No instances of $filename found in $mirror.\n");
			return;
		}

		print("Copying $filename from $link\n");
		copy($link, "$target/$filename.dl");

		$hash_cmd and do {
			if (system("cat '$target/$filename.dl' | $hash_cmd > '$target/$filename.hash'")) {
				print("Failed to generate hash for $filename\n");
				return;
			}
		};
	} else {
		open WGET, "wget -t5 --timeout=20 --no-check-certificate $options -O- '$mirror/$url_filename' |" or die "Cannot launch wget.\n";
		$hash_cmd and do {
			open MD5SUM, "| $hash_cmd > '$target/$filename.hash'" or die "Cannot launch $hash_cmd.\n";
		};
		open OUTPUT, "> $target/$filename.dl" or die "Cannot create file $target/$filename.dl: $!\n";
		my $buffer;
		while (read WGET, $buffer, 1048576) {
			$hash_cmd and print MD5SUM $buffer;
			print OUTPUT $buffer;
		}
		$hash_cmd and close MD5SUM;
		close WGET;
		close OUTPUT;

		if ($? >> 8) {
			print STDERR "Download failed.\n";
			cleanup();
			return;
		}
	}

	$hash_cmd and do {
		my $sum = `cat "$target/$filename.hash"`;
		$sum =~ /^(\w+)\s*/ or die "Could not generate file hash\n";
		$sum = $1;

		if ($sum ne $file_hash) {
			print STDERR "MD5 sum of the downloaded file does not match (file: $sum, requested: $file_hash) - deleting download.\n";
			cleanup();
			return;
		}
	};

	unlink "$target/$filename";
	system("mv", "$target/$filename.dl", "$target/$filename");
	cleanup();
}

sub cleanup
{
	unlink "$target/$filename.dl";
	unlink "$target/$filename.hash";
}

@mirrors = localmirrors();

foreach my $mirror (@ARGV) {
	if ($mirror =~ /^\@SF\/(.+)$/) {
		# give sourceforge a few more tries, because it redirects to different mirrors
		for (1 .. 5) {
			push @mirrors, "http://downloads.sourceforge.net/$1";
		}
	} elsif ($mirror =~ /^\@APACHE\/(.+)$/) {
		push @mirrors, "http://ftp.tudelft.nl/apache/$1";
		push @mirrors, "http://apache.openmirror.de/$1";
		push @mirrors, "http://mirrors.ocf.berkeley.edu/apache/$1";
		push @mirrors, "http://mirror.cc.columbia.edu/pub/software/apache/$1";
		push @mirrors, "http://ftp.jaist.ac.jp/pub/apache/$1";
	} elsif ($mirror =~ /^\@GNU\/(.+)$/) {
		push @mirrors, "http://ftpmirror.gnu.org/$1";
		push @mirrors, "http://ftp.gnu.org/pub/gnu/$1";
		push @mirrors, "ftp://ftp.belnet.be/mirror/ftp.gnu.org/gnu/$1";
		push @mirrors, "ftp://ftp.mirror.nl/pub/mirror/gnu/$1";
		push @mirrors, "http://mirror.switch.ch/ftp/mirror/gnu/$1";
	} elsif ($mirror =~ /^\@SAVANNAH\/(.+)$/) {
		push @mirrors, "http://download.savannah.gnu.org/releases/$1";
		push @mirrors, "http://nongnu.uib.no/$1";
		push @mirrors, "http://ftp.igh.cnrs.fr/pub/nongnu/$1";
		push @mirrors, "http://download-mirror.savannah.gnu.org/releases/$1";
	} elsif ($mirror =~ /^\@KERNEL\/(.+)$/) {
		my @extra = ( $1 );
		if ($filename =~ /linux-\d+\.\d+(?:\.\d+)?-rc/) {
			push @extra, "$extra[0]/testing";
		} elsif ($filename =~ /linux-(\d+\.\d+(?:\.\d+)?)/) {
			push @extra, "$extra[0]/longterm/v$1";
		}		
		foreach my $dir (@extra) {
			push @mirrors, "https://kernel.org/pub/$dir";
			push @mirrors, "ftp://kernel.org/pub/$dir";
		}
    } elsif ($mirror =~ /^\@GNOME\/(.+)$/) {
		push @mirrors, "http://ftp.gnome.org/pub/GNOME/sources/$1";
		push @mirrors, "http://www.mirrorservice.org/sites/ftp.gnome.org/pub/GNOME/sources/$1";
		push @mirrors, "http://fr2.rpmfind.net/linux/gnome.org/sources/$1";
		push @mirrors, "http://ftp.acc.umu.se/pub/GNOME/sources/$1";
		push @mirrors, "http://ftp.belnet.be/ftp.gnome.org/sources/$1";
		push @mirrors, "ftp://ftp.cse.buffalo.edu/pub/Gnome/sources/$1";
		push @mirrors, "ftp://ftp.nara.wide.ad.jp/pub/X11/GNOME/sources/$1";
    }
    else {
		push @mirrors, $mirror;
	}
}

#push @mirrors, 'http://mirror1.openwrt.org';
push @mirrors, 'http://mirror2.openwrt.org/sources';
push @mirrors, 'http://downloads.openwrt.org/sources';

while (!$ok) {
	my $mirror = shift @mirrors;
	$mirror or die "No more mirrors to try - giving up.\n";

	download($mirror);
	-f "$target/$filename" and $ok = 1;
}

$SIG{INT} = \&cleanup;

