#!/usr/bin/perl
use strict;
my %preconfig;
my %package;
my %srcpackage;
my %category;

sub get_multiline {
	my $prefix = shift;
	my $str;
	while (<>) {
		last if /^@@/;
		s/^\s*//g;
		$str .= (($_ and $prefix) ? $prefix . $_ : $_);
	}

	return $str;
}

sub parse_target_metadata() {
	my ($target, @target, $profile);	
	while (<>) {
		chomp;
		/^Target:\s*((.+)-(\d+\.\d+))\s*$/ and do {
			my $conf = uc $3.'_'.$2;
			$conf =~ tr/\.-/__/;
			$target = {
				id => $1,
				conf => $conf,
				board => $2,
				kernel => $3,
				profiles => []
			};
			push @target, $target;
		};
		/^Target-Name:\s*(.+)\s*$/ and $target->{name} = $1;
		/^Target-Path:\s*(.+)\s*$/ and $target->{path} = $1;
		/^Target-Arch:\s*(.+)\s*$/ and $target->{arch} = $1;
		/^Target-Features:\s*(.+)\s*$/ and $target->{features} = [ split(/\s+/, $1) ];
		/^Target-Description:/ and $target->{desc} = get_multiline();
		/^Linux-Version:\s*(.+)\s*$/ and $target->{version} = $1;
		/^Linux-Release:\s*(.+)\s*$/ and $target->{release} = $1;
		/^Linux-Kernel-Arch:\s*(.+)\s*$/ and $target->{karch} = $1;
		/^Default-Packages:\s*(.+)\s*$/ and $target->{packages} = [ split(/\s+/, $1) ];
		/^Target-Profile:\s*(.+)\s*$/ and do {
			$profile = {
				id => $1,
				name => $1,
				packages => []
			};
			push @{$target->{profiles}}, $profile;
		};
		/^Target-Profile-Name:\s*(.+)\s*$/ and $profile->{name} = $1;
		/^Target-Profile-Packages:\s*(.*)\s*$/ and $profile->{packages} = [ split(/\s+/, $1) ];
		/^Target-Profile-Description:\s*(.*)\s*/ and $profile->{desc} = get_multiline();
		/^Target-Profile-Config:/ and $profile->{config} = get_multiline("\t");
		/^Target-Profile-Kconfig:/ and $profile->{kconfig} = 1;
	}
	foreach my $target (@target) {
		@{$target->{profiles}} > 0 or $target->{profiles} = [
			{
				id => 'Default',
				name => 'Default',
				packages => []
			}
		];
	}
	return @target;
}

sub parse_package_metadata() {
	my $pkg;
	my $makefile;
	my $preconfig;
	my $src;
	while (<>) {
		chomp;
		/^Source-Makefile: \s*(.+\/([^\/]+)\/Makefile)\s*$/ and do {
			$makefile = $1;
			$src = $2;
			$srcpackage{$src} = [];
			undef $pkg;
		};
		/^Package:\s*(.+?)\s*$/ and do {
			$pkg = {};
			$pkg->{src} = $src;
			$pkg->{makefile} = $makefile;
			$pkg->{name} = $1;
			$pkg->{default} = "m if ALL";
			$pkg->{depends} = [];
			$pkg->{builddepends} = [];
			$package{$1} = $pkg;
			push @{$srcpackage{$src}}, $pkg;
		};
		/^Version: \s*(.+)\s*$/ and $pkg->{version} = $1;
		/^Title: \s*(.+)\s*$/ and $pkg->{title} = $1;
		/^Menu: \s*(.+)\s*$/ and $pkg->{menu} = $1;
		/^Submenu: \s*(.+)\s*$/ and $pkg->{submenu} = $1;
		/^Submenu-Depends: \s*(.+)\s*$/ and $pkg->{submenudep} = $1;
		/^Default: \s*(.+)\s*$/ and $pkg->{default} = $1;
		/^Provides: \s*(.+)\s*$/ and do {
			my @vpkg = split /\s+/, $1;
			foreach my $vpkg (@vpkg) {
				$package{$vpkg} or $package{$vpkg} = { vdepends => [] };
				push @{$package{$vpkg}->{vdepends}}, $pkg->{name};
			}
		};
		/^Depends: \s*(.+)\s*$/ and $pkg->{depends} = [ split /\s+/, $1 ];
		/^Build-Depends: \s*(.+)\s*$/ and $pkg->{builddepends} = [ split /\s+/, $1 ];
		/^Category: \s*(.+)\s*$/ and do {
			$pkg->{category} = $1;
			defined $category{$1} or $category{$1} = {};
			defined $category{$1}->{$src} or $category{$1}->{$src} = [];
			push @{$category{$1}->{$src}}, $pkg;
		};
		/^Description: \s*(.*)\s*$/ and $pkg->{description} = "\t\t $1\n". get_multiline("\t\t ");
		/^Config: \s*(.*)\s*$/ and $pkg->{config} = "$1\n".get_multiline();
		/^Prereq-Check:/ and $pkg->{prereq} = 1;
		/^Preconfig:\s*(.+)\s*$/ and do {
			my $pkgname = $pkg->{name};
			$preconfig{$pkgname} or $preconfig{$pkgname} = [];
			$preconfig = {
				id => $1
			};
			push @{$preconfig{$pkgname}}, $preconfig;
		};
		/^Preconfig-Type:\s*(.*?)\s*$/ and $preconfig->{type} = $1;
		/^Preconfig-Label:\s*(.*?)\s*$/ and $preconfig->{label} = $1;
		/^Preconfig-Default:\s*(.*?)\s*$/ and $preconfig->{default} = $1;
	}
	return %category;
}


sub gen_target_mk() {
	my @target = parse_target_metadata();
	
	@target = sort {
		$a->{id} cmp $b->{id}
	} @target;
	
	foreach my $target (@target) {
		my ($profiles_def, $profiles_eval);
		my $conf = uc $target->{kernel}.'_'.$target->{board};
		$conf =~ tr/\.-/__/;
		
		foreach my $profile (@{$target->{profiles}}) {
			$profiles_def .= "
  define Profile/$conf\_$profile->{id}
    ID:=$profile->{id}
    NAME:=$profile->{name}
    PACKAGES:=".join(" ", @{$profile->{packages}})."\n";
			$profile->{kconfig} and $profiles_def .= "    KCONFIG:=1\n";
			$profiles_def .= "  endef";
			$profiles_eval .= "
\$(eval \$(call AddProfile,$conf\_$profile->{id}))"
		}
		print "
ifeq (\$(CONFIG_LINUX_$conf),y)
  define Target
    KERNEL:=$target->{kernel}
    BOARD:=$target->{board}
    BOARDNAME:=$target->{name}
    LINUX_VERSION:=$target->{version}
    LINUX_RELEASE:=$target->{release}
    LINUX_KARCH:=$target->{karch}
    DEFAULT_PACKAGES:=".join(" ", @{$target->{packages}})."
  endef$profiles_def
endif$profiles_eval

"
	}
	print "\$(eval \$(call Target))\n";
}

sub target_config_features(@) {
	my $ret;

	while ($_ = shift @_) {
		/broken/ and $ret .= "\tdepends BROKEN\n";
		/pci/ and $ret .= "\tselect PCI_SUPPORT\n";
		/usb/ and $ret .= "\tselect USB_SUPPORT\n";
		/atm/ and $ret .= "\tselect ATM_SUPPORT\n";
		/pcmcia/ and $ret .= "\tselect PCMCIA_SUPPORT\n";
		/video/ and $ret .= "\tselect VIDEO_SUPPORT\n";
		/squashfs/ and $ret .= "\tselect USES_SQUASHFS\n";
		/jffs2/ and $ret .= "\tselect USES_JFFS2\n";
		/ext2/ and $ret .= "\tselect USES_EXT2\n";
	}
	return $ret;
}


sub gen_target_config() {
	my @target = parse_target_metadata();

	@target = sort {
		$a->{name} cmp $b->{name}
	} @target;
	
	
	print <<EOF;
choice
	prompt "Target System"
	default LINUX_2_4_BRCM
	reset if !DEVEL
	
EOF

	foreach my $target (@target) {
		my $features = target_config_features(@{$target->{features}});
		my $help = $target->{desc};
		my $kernel = $target->{kernel};
		$kernel =~ tr/./_/;

		chomp $features;
		$features .= "\n";
		if ($help =~ /\w+/) {
			$help =~ s/^\s*/\t  /mg;
			$help = "\thelp\n$help";
		} else {
			undef $help;
		}
	
		print <<EOF
config LINUX_$target->{conf}
	bool "$target->{name}"
	select $target->{arch}
	select LINUX_$kernel
$features$help

EOF
	}

	print <<EOF;
if DEVEL

config LINUX_2_6_ARM
	bool "UNSUPPORTED little-endian arm platform"
	depends BROKEN
	select LINUX_2_6
	select arm

config LINUX_2_6_CRIS
	bool "UNSUPPORTED cris platform"
	depends BROKEN
	select LINUX_2_6
	select cris

config LINUX_2_6_M68K
	bool "UNSUPPORTED m68k platform"
	depends BROKEN
	select LINUX_2_6
	select m68k

config LINUX_2_6_SH3
	bool "UNSUPPORTED little-endian sh3 platform"
	depends BROKEN
	select LINUX_2_6
	select sh3

config LINUX_2_6_SH3EB
	bool "UNSUPPORTED big-endian sh3 platform"
	depends BROKEN
	select LINUX_2_6
	select sh3eb

config LINUX_2_6_SH4
	bool "UNSUPPORTED little-endian sh4 platform"
	depends BROKEN
	select LINUX_2_6
	select sh4

config LINUX_2_6_SH4EB
	bool "UNSUPPORTED big-endian sh4 platform"
	depends BROKEN
	select LINUX_2_6
	select sh4eb

config LINUX_2_6_SPARC
	bool "UNSUPPORTED sparc platform"
	depends BROKEN
	select LINUX_2_6
	select sparc

endif

endchoice

choice
	prompt "Target Profile"

EOF
	
	foreach my $target (@target) {
		my $profiles = $target->{profiles};
		
		foreach my $profile (@$profiles) {
			print <<EOF;
config LINUX_$target->{conf}_$profile->{id}
	bool "$profile->{name}"
	depends LINUX_$target->{conf}
$profile->{config}
EOF
			$profile->{kconfig} and print "\tselect PROFILE_KCONFIG\n";
			my %pkgs;
			foreach my $pkg (@{$target->{packages}}, @{$profile->{packages}}) {
				$pkgs{$pkg} = 1;
			}
			foreach my $pkg (keys %pkgs) {
				print "\tselect DEFAULT_$pkg\n" unless ($pkg =~ /^-/ or $pkgs{"-$pkg"});
			}
			print "\n";
		}
	}

	print "endchoice\n";
}

sub find_package_dep($$) {
	my $pkg = shift;
	my $name = shift;
	my $deps = ($pkg->{vdepends} or $pkg->{depends});

	return 0 unless defined $deps;
	foreach my $dep (@{$deps}) {
		return 1 if $dep eq $name;
		return 1 if ($package{$dep} and (find_package_dep($package{$dep},$name) == 1));
	}
	return 0;
}

sub package_depends($$) {
	my $a = shift;
	my $b = shift;
	my $ret;

	return 0 if ($a->{submenu} ne $b->{submenu});
	if (find_package_dep($a, $b->{name}) == 1) {
		$ret = 1;
	} elsif (find_package_dep($b, $a->{name}) == 1) {
		$ret = -1;
	} else {
		return 0;
	}
	return $ret;
}

sub mconf_depends($$) {
	my $depends = shift;
	my $only_dep = shift;
	my $res;

	$depends or return;
	my @depends = @$depends;
	foreach my $depend (@depends) {
		my $m = "depends";
		$depend =~ s/^([@\+]+)//;
		my $flags = $1;
		my $vdep;
	
		if ($vdep = $package{$depend}->{vdepends}) {
			$depend = join("||", map { "PACKAGE_".$_ } @$vdep);
		} else {
			$flags =~ /\+/ and do {
				next if $only_dep;
				$m = "select";

				# Menuconfig will not treat 'select FOO' as a real dependency
				# thus if FOO depends on other config options, these dependencies
				# will not be checked. To fix this, we simply emit all of FOO's
				# depends here as well.
				$package{$depend} and $res .= mconf_depends($package{$depend}->{depends}, 1);
			};
			$flags =~ /@/ or $depend = "PACKAGE_$depend";
		}
		$res .= "\t\t$m $depend\n";
	}
	return $res;
}

sub print_package_config_category($) {
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
			print "\tconfig DEFAULT_".$pkg->{name}."\n";
			print "\t\tbool\n\n";
		}
	}
	my @menus = sort {
		($a eq 'undef' ?  1 : 0) or
		($b eq 'undef' ? -1 : 0) or
		($a cmp $b)
	} keys %menus;

	foreach my $menu (@menus) {
		my @pkgs = sort {
			package_depends($a, $b) or
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
			print "\t";
			$pkg->{menu} and print "menu";
			print "config PACKAGE_".$pkg->{name}."\n";
			print "\t\ttristate \"$title\"\n";
			print "\t\tdefault y if DEFAULT_".$pkg->{name}."\n";
			foreach my $default (split /\s*,\s*/, $pkg->{default}) {
				print "\t\tdefault $default\n";
			}
			print mconf_depends($pkg->{depends}, 0);
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

sub gen_package_config() {
	parse_package_metadata();
	print "menuconfig UCI_PRECONFIG\n\tbool \"Image configuration\"\n";
	foreach my $preconfig (keys %preconfig) {
		foreach my $cfg (@{$preconfig{$preconfig}}) {
			my $conf = $cfg->{id};
			$conf =~ tr/\.-/__/;
			print <<EOF
	config UCI_PRECONFIG_$conf
		string "$cfg->{label}" if UCI_PRECONFIG
		depends PACKAGE_$preconfig
		default "$cfg->{default}"

EOF
		}
	}
	print_package_config_category 'Base system';
	foreach my $cat (keys %category) {
		print_package_config_category $cat;
	}
}

sub gen_package_mk() {
	my %conf;
	my %dep;
	my $line;

	parse_package_metadata();
	foreach my $name (sort {uc($a) cmp uc($b)} keys %package) {
		my $config;
		my $pkg = $package{$name};
		
		next if defined $pkg->{vdepends};
		if ($ENV{SDK}) {
			$conf{$pkg->{src}} or do {
				$config = 'm';
				$conf{$pkg->{src}} = 1;
			};
		} else {
			$config = "\$(CONFIG_PACKAGE_$name)"
		}
		if ($config) {
			print "package-$config += $pkg->{src}\n";
			$pkg->{prereq} and print "prereq-$config += $pkg->{src}\n";
		}
	
		my $hasdeps = 0;
		my $depline = "";
		foreach my $dep (@{$pkg->{depends}}, @{$pkg->{builddepends}}) {
			next if $dep =~ /@/;
			$dep =~ s/\+//;
			my $idx;
			my $pkg_dep = $package{$dep};
			$pkg_dep or $pkg_dep = $srcpackage{$dep}->[0];
			next unless defined $pkg_dep;
			next if defined $pkg_dep->{vdepends};

			if (defined $pkg_dep->{src}) {
				($pkg->{src} ne $pkg_dep->{src}) and $idx = $pkg_dep->{src};
			} elsif (defined($pkg_dep) && !defined($ENV{SDK})) {
				$idx = $dep;
			}
			undef $idx if $idx =~ /^(kernel)|(base-files)$/;
			if ($idx) {
				next if $dep{$pkg->{src}."->".$idx};
				$depline .= " $idx\-compile";
				$dep{$pkg->{src}."->".$idx} = 1;
			}
		}
		if ($depline) {
			$line .= "$pkg->{src}-compile: $depline\n";
		}
	}
	
	if ($line ne "") {
		print "\n$line";
	}
	foreach my $preconfig (keys %preconfig) {
		my $cmds;
		foreach my $cfg (@{$preconfig{$preconfig}}) {
			my $conf = $cfg->{id};
			$conf =~ tr/\.-/__/;
			$cmds .= "\techo \"uci set '$cfg->{id}=\$(subst \",,\$(CONFIG_UCI_PRECONFIG_$conf))'\"; \\\n";
		}
		next unless $cmds;
		print <<EOF

\$(TARGET_DIR)/etc/uci-defaults/$preconfig: FORCE
	( \\
$cmds \\
	) > \$@
	
ifneq (\$(UCI_PRECONFIG)\$(CONFIG_UCI_PRECONFIG),)
  preconfig: \$(TARGET_DIR)/etc/uci-defaults/$preconfig
endif
EOF
	}
}


sub parse_command() {
	my $cmd = shift @ARGV;
	for ($cmd) {
		/^target_mk$/ and return gen_target_mk();
		/^target_config$/ and return gen_target_config();
		/^package_mk$/ and return gen_package_mk();
		/^package_config$/ and return gen_package_config();
	}
	print <<EOF
Available Commands:
	$0 target_mk [file] 		Target metadata in makefile format
	$0 target_config [file] 	Target metadata in Kconfig format
	$0 package_mk [file] 		Package metadata in makefile format
	$0 package_config [file] 	Package metadata in Kconfig format
EOF
}

parse_command();
