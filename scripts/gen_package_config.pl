#!/usr/bin/perl
# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

use strict;

my $src;
my $makefile;
my $pkg;
my %package;
my %category;


sub find_dep($$) {
	my $pkg = shift;
	my $name = shift;
	my $deps = ($pkg->{vdepends} or $pkg->{depends});

	return 0 unless defined $deps;
	foreach my $dep (@{$deps}) {
		return 1 if $dep eq $name;
		return 1 if ($package{$dep} and (find_dep($package{$dep},$name) == 1));
	}
	return 0;
}

sub depends($$) {
	my $a = shift;
	my $b = shift;
	my $ret;

	return 0 if ($a->{submenu} ne $b->{submenu});
	if (find_dep($a, $b->{name}) == 1) {
		$ret = 1;
	} elsif (find_dep($b, $a->{name}) == 1) {
		$ret = -1;
	} else {
		return 0;
	}
#	print STDERR "depends($a->{name}, $b->{name}) == $ret\n";
	return $ret;
}


sub print_category($) {
	my $cat = shift;
	my %menus;
	my %menu_dep;
	
	return unless $category{$cat};
	
	print "menu \"$cat\"\n\n";
	my %spkg = %{$category{$cat}};
	
	foreach my $spkg (sort {uc($a) cmp uc($b)} keys %spkg) {
		foreach my $pkg (@{$spkg{$spkg}}) {
			my $menu = $pkg->{submenu};
			if ($menu) {
				$menu_dep{$menu} or $menu_dep{$menu} = $pkg->{submenudep};
			} else {
				$menu = 'undef';
			}
			$menus{$menu} or $menus{$menu} = [];
			push @{$menus{$menu}}, $pkg;
		}
	}
	my @menus = sort {
		($a eq 'undef' ?  1 : 0) or
		($b eq 'undef' ? -1 : 0) or
		($a cmp $b)
	} keys %menus;

	foreach my $menu (@menus) {
		my @pkgs = sort {
			depends($a, $b) or
			($a->{name} cmp $b->{name})
		} @{$menus{$menu}};
		if ($menu ne 'undef') {
			$menu_dep{$menu} and print "if $menu_dep{$menu}\n";
			print "menu \"$menu\"\n";
		}
		foreach my $pkg (@pkgs) {
			my $title = $pkg->{name};
			my $c = (72 - length($pkg->{name}) - length($pkg->{title}));
			if ($c > 0) {
				$title .= ("." x $c). " ". $pkg->{title};
			}
			print "\tconfig DEFAULT_".$pkg->{name}."\n";
			print "\t\tbool\n\n";
			print "\t";
			$pkg->{menu} and print "menu";
			print "config PACKAGE_".$pkg->{name}."\n";
			print "\t\ttristate \"$title\"\n";
			print "\t\tdefault y if DEFAULT_".$pkg->{name}."\n";
			foreach my $default (split /\s*,\s*/, $pkg->{default}) {
				print "\t\tdefault $default\n";
			}
			foreach my $depend (@{$pkg->{depends}}) {
				my $m = "depends";
				$depend =~ s/^([@\+]+)//;
				my $flags = $1;
				my $vdep;
				
				if ($vdep = $package{$depend}->{vdepends}) {
					$depend = join("||", map { "PACKAGE_".$_ } @$vdep);
				} else {
					$flags =~ /@/ or $depend = "PACKAGE_$depend";
					$flags =~ /\+/ and $m = "select";
				}
				print "\t\t$m $depend\n";
			}
			print "\t\thelp\n";
			print $pkg->{description};
			print "\n";

			$pkg->{config} and print $pkg->{config}."\n";
		}
		if ($menu ne 'undef') {
			print "endmenu\n";
			$menu_dep{$menu} and print "endif\n";
		}
	}
	print "endmenu\n\n";
	
	undef $category{$cat};
}

my $line;
while ($line = <>) {
	chomp $line;
	$line =~ /^Source-Makefile: \s*(.+\/([^\/]+)\/Makefile)\s*$/ and do {
		$makefile = $1;
		$src = $2;
		undef $pkg;
	};
	$line =~ /^Package: \s*(.+)\s*$/ and do {
		$pkg = {};
		$pkg->{src} = $src;
		$pkg->{makefile} = $makefile;
		$pkg->{name} = $1;
		$pkg->{default} = "m if ALL";
		$package{$1} = $pkg;
	};
	$line =~ /^Version: \s*(.+)\s*$/ and $pkg->{version} = $1;
	$line =~ /^Title: \s*(.+)\s*$/ and $pkg->{title} = $1;
	$line =~ /^Menu: \s*(.+)\s*$/ and $pkg->{menu} = $1;
	$line =~ /^Submenu: \s*(.+)\s*$/ and $pkg->{submenu} = $1;
	$line =~ /^Submenu-Depends: \s*(.+)\s*$/ and $pkg->{submenudep} = $1;
	$line =~ /^Default: \s*(.+)\s*$/ and $pkg->{default} = $1;
	$line =~ /^Provides: \s*(.+)\s*$/ and do {
		my @vpkg = split /\s+/, $1;
		foreach my $vpkg (@vpkg) {
			$package{$vpkg} or $package{$vpkg} = { vdepends => [] };
			push @{$package{$vpkg}->{vdepends}}, $pkg->{name};
		}
	};
	$line =~ /^Depends: \s*(.+)\s*$/ and do {
		my @dep = split /\s+/, $1;
		$pkg->{depends} = \@dep;
	};
	$line =~ /^Category: \s*(.+)\s*$/ and do {
		$pkg->{category} = $1;
		defined $category{$1} or $category{$1} = {};
		defined $category{$1}->{$src} or $category{$1}->{$src} = [];
		push @{$category{$1}->{$src}}, $pkg;
	};
	$line =~ /^Description: \s*(.*)\s*$/ and do {
		my $desc = "\t\t$1\n\n";
		my $line;
		while ($line = <>) {
			last if $line =~ /^@@/;
			$desc .= "\t\t$line";
		}
		$pkg->{description} = $desc;
	};
	$line =~ /^Config: \s*(.*)\s*$/ and do {
		my $conf = "$1\n";
		my $line;
		while ($line = <>) {
			last if $line =~ /^@@/;
			$conf .= "$line";
		}
		$pkg->{config} = $conf;
	}
}

print_category 'Base system';
foreach my $cat (keys %category) {
	print_category $cat;
}
