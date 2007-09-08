#!/usr/bin/perl
use strict;
my %preconfig;
my %package;
my %srcpackage;
my %category;
my %subdir;

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
		/^Target:\s*(.+)\s*$/ and do {
			my $conf = $1;
			$conf =~ tr#/\.\-/#___#;
			$target = {
				id => $1,
				conf => $conf,
				profiles => []
			};
			push @target, $target;
		};
		/^Target-Board:\s*(.+)\s*$/ and $target->{board} = $1;
		/^Target-Kernel:\s*(\d+\.\d+)\s*$/ and $target->{kernel} = $1;
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
	my $subdir;
	my $src;
	while (<>) {
		chomp;
		/^Source-Makefile: \s*((.+\/)([^\/]+)\/Makefile)\s*$/ and do {
			$makefile = $1;
			$subdir = $2;
			$src = $3;
			$subdir =~ s/^package\///;
			$subdir{$src} = $subdir;
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
			$pkg->{subdir} = $subdir;
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

sub gen_kconfig_overrides() {
	my %config;
	my %kconfig;
	my $package;
	my $pkginfo = shift @ARGV;
	my $cfgfile = shift @ARGV;

	# parameter 2: build system config
	open FILE, "<$cfgfile" or return;
	while (<FILE>) {
		/^(CONFIG_.+?)=(.+)$/ and $config{$1} = 1;
	}
	close FILE;

	# parameter 1: package metadata
	open FILE, "<$pkginfo" or return;
	while (<FILE>) {
		/^Package:\s*(.+?)\s*$/ and $package = $1;
		/^Kernel-Config:\s*(.+?)\s*$/ and do {
			my @config = split /\s+/, $1;
			foreach my $config (@config) {
				my $val = 'm';
				my $override;
				if ($config =~ /^(.+?)=(.+)$/) {
					$config = $1;
					$override = 1;
					$val = $2;
				}
				if ($config{"CONFIG_PACKAGE_$package"} and ($config ne 'n')) {
					$kconfig{$config} = $val;
				} elsif (!$override) {
					$kconfig{$config} or $kconfig{$config} = 'n';
				}
			}
		};
	};
	close FILE;

	foreach my $kconfig (sort keys %kconfig) {
		if ($kconfig{$kconfig} eq 'n') {
			print "# $kconfig is not set\n";
		} else {
			print "$kconfig=$kconfig{$kconfig}\n";
		}
	}
}

sub merge_package_lists($$) {
	my $list1 = shift;
	my $list2 = shift;
	my @l = ();
	my %pkgs;

	foreach my $pkg (@$list1, @$list2) {
		$pkgs{$pkg} = 1;
	}
	foreach my $pkg (keys %pkgs) {
		push @l, $pkg unless ($pkg =~ /^-/ or $pkgs{"-$pkg"});
	}
	return sort(@l);
}

sub target_config_features(@) {
	my $ret;

	while ($_ = shift @_) {
		/broken/ and $ret .= "\tdepends BROKEN\n";
		/pci/ and $ret .= "\tselect PCI_SUPPORT\n";
		/usb/ and $ret .= "\tselect USB_SUPPORT\n";
		/pcmcia/ and $ret .= "\tselect PCMCIA_SUPPORT\n";
		/squashfs/ and $ret .= "\tselect USES_SQUASHFS\n";
		/jffs2/ and $ret .= "\tselect USES_JFFS2\n";
		/ext2/ and $ret .= "\tselect USES_EXT2\n";
		/tgz/ and $ret .= "\tselect USES_TGZ\n";
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
	default TARGET_brcm_2_4
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
config TARGET_$target->{conf}
	bool "$target->{name}"
	select $target->{arch}
	select LINUX_$kernel
$features$help

EOF
	}

	print <<EOF;
endchoice

config TARGET_BOARD
	string
EOF
	foreach my $target (@target) {
		print "\t\tdefault \"".$target->{board}."\" if TARGET_".$target->{conf}."\n";
	}
	print <<EOF;

choice
	prompt "Target Profile"

EOF
	
	foreach my $target (@target) {
		my $profiles = $target->{profiles};
		
		foreach my $profile (@$profiles) {
			print <<EOF;
config TARGET_$target->{conf}_$profile->{id}
	bool "$profile->{name}"
	depends TARGET_$target->{conf}
$profile->{config}
EOF
			$profile->{kconfig} and print "\tselect PROFILE_KCONFIG\n";
			my @pkglist = merge_package_lists($target->{packages}, $profile->{packages});
			foreach my $pkg (@pkglist) {
				print "\tselect DEFAULT_$pkg\n";
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
			print "package-$config += $pkg->{subdir}$pkg->{src}\n";
			$pkg->{prereq} and print "prereq-$config += $pkg->{subdir}$pkg->{src}\n";
		}
	
		my $hasdeps = 0;
		my $depline = "";
		foreach my $dep (@{$pkg->{depends}}, @{$pkg->{builddepends}}) {
			next if $dep =~ /@/;
			$dep =~ s/\+//;
			my $idx;
			my $pkg_dep = $package{$dep};
			next if defined $pkg_dep->{vdepends};

			if (defined $pkg_dep->{src}) {
				($pkg->{src} ne $pkg_dep->{src}) and $idx = $pkg_dep->{subdir}.$pkg_dep->{src};
			} elsif (defined($srcpackage{$dep})) {
				$idx = $subdir{$dep}.$dep;
			}
			undef $idx if $idx =~ /^(kernel)|(base-files)$/;
			if ($idx) {
				next if $dep{$pkg->{src}."->".$idx};
				$depline .= " \$(curdir)/$idx/compile";
				$dep{$pkg->{src}."->".$idx} = 1;
			}
		}
		if ($depline) {
			$line .= "\$(curdir)/".$pkg->{subdir}."$pkg->{src}/compile += $depline\n";
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
  package/preconfig: \$(TARGET_DIR)/etc/uci-defaults/$preconfig
endif
EOF
	}
}


sub parse_command() {
	my $cmd = shift @ARGV;
	for ($cmd) {
		/^target_config$/ and return gen_target_config();
		/^package_mk$/ and return gen_package_mk();
		/^package_config$/ and return gen_package_config();
		/^kconfig/ and return gen_kconfig_overrides();
	}
	print <<EOF
Available Commands:
	$0 target_config [file] 	Target metadata in Kconfig format
	$0 package_mk [file]    	Package metadata in makefile format
	$0 package_config [file] 	Package metadata in Kconfig format
	$0 kconfig [file] [config]	Kernel config overrides

EOF
}

parse_command();
