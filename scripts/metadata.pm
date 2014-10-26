package metadata;
use base 'Exporter';
use strict;
use warnings;
our @EXPORT = qw(%package %srcpackage %category %subdir %preconfig %features clear_packages parse_package_metadata get_multiline);

our %package;
our %preconfig;
our %srcpackage;
our %category;
our %subdir;
our %features;

sub get_multiline {
	my $fh = shift;
	my $prefix = shift;
	my $str;
	while (<$fh>) {
		last if /^@@/;
		$str .= (($_ and $prefix) ? $prefix . $_ : $_);
	}

	return $str ? $str : "";
}

sub clear_packages() {
	%subdir = ();
	%preconfig = ();
	%package = ();
	%srcpackage = ();
	%category = ();
	%features = ();
}

sub parse_package_metadata($) {
	my $file = shift;
	my $pkg;
	my $feature;
	my $makefile;
	my $preconfig;
	my $subdir;
	my $src;

	open FILE, "<$file" or do {
		warn "Cannot open '$file': $!\n";
		return undef;
	};
	while (<FILE>) {
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
		next unless $src;
		/^Package:\s*(.+?)\s*$/ and do {
			undef $feature;
			$pkg = {};
			$pkg->{src} = $src;
			$pkg->{makefile} = $makefile;
			$pkg->{name} = $1;
			$pkg->{title} = "";
			$pkg->{depends} = [];
			$pkg->{mdepends} = [];
			$pkg->{builddepends} = [];
			$pkg->{buildtypes} = [];
			$pkg->{subdir} = $subdir;
			$pkg->{tristate} = 1;
			$package{$1} = $pkg;
			push @{$srcpackage{$src}}, $pkg;
		};
		/^Feature:\s*(.+?)\s*$/ and do {
			undef $pkg;
			$feature = {};
			$feature->{name} = $1;
			$feature->{priority} = 0;
		};
		$feature and do {
			/^Target-Name:\s*(.+?)\s*$/ and do {
				$features{$1} or $features{$1} = [];
				push @{$features{$1}}, $feature;
			};
			/^Target-Title:\s*(.+?)\s*$/ and $feature->{target_title} = $1;
			/^Feature-Priority:\s*(\d+)\s*$/ and $feature->{priority} = $1;
			/^Feature-Name:\s*(.+?)\s*$/ and $feature->{title} = $1;
			/^Feature-Description:/ and $feature->{description} = get_multiline(\*FILE, "\t\t\t");
			next;
		};
		next unless $pkg;
		/^Version: \s*(.+)\s*$/ and $pkg->{version} = $1;
		/^Title: \s*(.+)\s*$/ and $pkg->{title} = $1;
		/^Menu: \s*(.+)\s*$/ and $pkg->{menu} = $1;
		/^Submenu: \s*(.+)\s*$/ and $pkg->{submenu} = $1;
		/^Submenu-Depends: \s*(.+)\s*$/ and $pkg->{submenudep} = $1;
		/^Source: \s*(.+)\s*$/ and $pkg->{source} = $1;
		/^License: \s*(.+)\s*$/ and $pkg->{license} = $1;
		/^LicenseFiles: \s*(.+)\s*$/ and $pkg->{licensefiles} = $1;
		/^Default: \s*(.+)\s*$/ and $pkg->{default} = $1;
		/^Provides: \s*(.+)\s*$/ and do {
			my @vpkg = split /\s+/, $1;
			foreach my $vpkg (@vpkg) {
				$package{$vpkg} or $package{$vpkg} = {
					name => $vpkg,
					vdepends => [],
					src => $src,
					subdir => $subdir,
					makefile => $makefile
				};
				push @{$package{$vpkg}->{vdepends}}, $pkg->{name};
			}
		};
		/^Menu-Depends: \s*(.+)\s*$/ and $pkg->{mdepends} = [ split /\s+/, $1 ];
		/^Depends: \s*(.+)\s*$/ and $pkg->{depends} = [ split /\s+/, $1 ];
		/^Conflicts: \s*(.+)\s*$/ and $pkg->{conflicts} = [ split /\s+/, $1 ];
		/^Hidden: \s*(.+)\s*$/ and $pkg->{hidden} = 1;
		/^Build-Variant: \s*([\w\-]+)\s*/ and $pkg->{variant} = $1;
		/^Default-Variant: .*/ and $pkg->{variant_default} = 1;
		/^Build-Only: \s*(.+)\s*$/ and $pkg->{buildonly} = 1;
		/^Build-Depends: \s*(.+)\s*$/ and $pkg->{builddepends} = [ split /\s+/, $1 ];
		/^Build-Depends\/(\w+): \s*(.+)\s*$/ and $pkg->{"builddepends/$1"} = [ split /\s+/, $2 ];
		/^Build-Types:\s*(.+)\s*$/ and $pkg->{buildtypes} = [ split /\s+/, $1 ];
		/^Feed:\s*(.+?)\s*$/ and $pkg->{feed} = $1;
		/^Category: \s*(.+)\s*$/ and do {
			$pkg->{category} = $1;
			defined $category{$1} or $category{$1} = {};
			defined $category{$1}->{$src} or $category{$1}->{$src} = [];
			push @{$category{$1}->{$src}}, $pkg;
		};
		/^Description: \s*(.*)\s*$/ and $pkg->{description} = "\t\t $1\n". get_multiline(*FILE, "\t\t ");
		/^Type: \s*(.+)\s*$/ and do {
			$pkg->{type} = [ split /\s+/, $1 ];
			undef $pkg->{tristate};
			foreach my $type (@{$pkg->{type}}) {
				$type =~ /ipkg/ and $pkg->{tristate} = 1;
			}
		};
		/^Config:\s*(.*)\s*$/ and $pkg->{config} = "$1\n".get_multiline(*FILE, "\t");
		/^Prereq-Check:/ and $pkg->{prereq} = 1;
		/^Preconfig:\s*(.+)\s*$/ and do {
			my $pkgname = $pkg->{name};
			$preconfig{$pkgname} or $preconfig{$pkgname} = {};
			if (exists $preconfig{$pkgname}->{$1}) {
				$preconfig = $preconfig{$pkgname}->{$1};
			} else {
				$preconfig = {
					id => $1
				};
				$preconfig{$pkgname}->{$1} = $preconfig;
			}
		};
		/^Preconfig-Type:\s*(.*?)\s*$/ and $preconfig->{type} = $1;
		/^Preconfig-Label:\s*(.*?)\s*$/ and $preconfig->{label} = $1;
		/^Preconfig-Default:\s*(.*?)\s*$/ and $preconfig->{default} = $1;
	}
	close FILE;
	return 1;
}

1;
