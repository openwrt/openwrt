#!/usr/bin/env perl

use strict;
use warnings;
use Cwd;

my (%targets, %architectures, %kernels, %devices);

$ENV{'TOPDIR'} = Cwd::getcwd();


sub parse_targetinfo {
	my ($target_dir, $subtarget) = @_;

	if (open M, "make -C '$target_dir' --no-print-directory DUMP=1 TARGET_BUILD=1 SUBTARGET='$subtarget' |") {
		my ($target_name, $target_arch, $target_kernel, $target_testing_kernel, @target_features);
		while (defined(my $line = readline M)) {
			chomp $line;

			if ($line =~ /^Target: (.+)$/) {
				$target_name = $1;
			}
			elsif ($line =~ /^Target-Arch-Packages: (.+)$/) {
				$target_arch = $1;
			}
			elsif ($line =~ /^Linux-Version: (\d\.\d+)\.\d+$/) {
				$target_kernel = $1;
			}
			elsif ($line =~ /^Linux-Testing-Version: (\d\.\d+)\.\d+$/) {
				$target_testing_kernel = $1;
			}
			elsif ($line =~ /^Target-Features: (.+)$/) {
				@target_features = split /\s+/, $1;
			}
			elsif ($line =~ /^@\@$/) {
				if ($target_name && $target_arch && $target_kernel &&
				    !grep { $_ eq 'broken' or $_ eq 'source-only' } @target_features) {
					$targets{$target_name} = $target_arch;
					$architectures{$target_arch} ||= [];
					push @{$architectures{$target_arch}}, $target_name;
					$kernels{$target_name} ||= [];
					push @{$kernels{$target_name}}, $target_kernel;
					if ($target_testing_kernel) {
						push @{$kernels{$target_name}}, $target_testing_kernel;
					}
				}

				undef $target_name;
				undef $target_arch;
				undef $target_kernel;
				undef $target_testing_kernel;
				@target_features = ();
			}
		}
		close M;
	}
}

sub parse_devices {
	my ($target_dir, $subtarget) = @_;

	if (open M, "make -C '$target_dir' --no-print-directory DUMP=1 TARGET_BUILD=1 SUBTARGET='$subtarget' V=s |") {
		my ($device_profile, $device_name, @device_alt_names, $device_is_alt);
		while (defined(my $line = readline M)) {
			chomp $line;

			if ($line =~ /^Target-Profile-Name: (.+)$/) {
				$device_name = $1;
			}
			elsif ($line =~ /^Target-Profile: DEVICE_(.+)$/) {
				$device_profile = $1;
			}
			# Logic behind this.
			# DUMP duplicate info for each alternative device name and
			# the alternative device name are printed first before the
			# primary device name
			# Alternative device titles always have the full list of
			# all the alternative device name.
			# The device name pattern for an alternative device name is
			# Target-Profile-Name: ALT_NAME (PRIMARY_NAME)
			# We compare the detected device name and check if it does
			# match the alternative device name pattern with one of
			# the alternative device name in Alternative device titles:
			# If an alternative device name is detected,
			# alternative device is skipped.
			elsif ($line =~ /^Alternative device titles:$/) {
				while (defined($line = readline M)) {
					if ($line =~ /^- (.+)$/) {
						if ($device_name =~ /^\Q$1\E \((.+)\)$/) {
							$device_is_alt = 1;
							last;
						}
						push @device_alt_names, $1;
					}
					else {
						last;
					}
				}
			}
			if ($line =~ /^@\@$/) {
				if ($device_name && $device_profile && ! $device_is_alt) {
					push @{$devices{$device_profile}}, $device_name;

					if (scalar @device_alt_names) {
						foreach my $device_alt_name (sort values @device_alt_names) {
							push @{$devices{$device_profile}}, $device_alt_name;
						}
					}
				}

				undef $device_name;
				undef $device_profile;
				undef $device_is_alt;
				@device_alt_names = ();
			}
		}
		close M;
	}
}

sub get_targetinfo {
	foreach my $target_makefile (glob "target/linux/*/Makefile") {
		my ($target_dir) = $target_makefile =~ m!^(.+)/Makefile$!;
		my @subtargets;

		if (open M, "make -C '$target_dir' --no-print-directory DUMP=1 TARGET_BUILD=1 val.FEATURES V=s 2>/dev/null |") {
			if (defined(my $line = readline M)) {
				chomp $line;
				if (grep { $_ eq 'broken' or $_ eq 'source-only' } split /\s+/, $line) {
					next;
				}
			}
		}

		if (open M, "make -C '$target_dir' --no-print-directory DUMP=1 TARGET_BUILD=1 val.SUBTARGETS V=s 2>/dev/null |") {
			if (defined(my $line = readline M)) {
				chomp $line;
				@subtargets = split /\s+/, $line;
			}
			close M;
		}

		push @subtargets, 'generic' if @subtargets == 0;

		foreach my $subtarget (@subtargets) {
			parse_targetinfo($target_dir, $subtarget);
		}
	}
}

sub get_devices {
	my ($target_subtarget) = @_;
	my ($target, $subtarget) = split /\//, $target_subtarget;

	my ($target_dir) = "target/linux/" . $target;

	parse_devices($target_dir, $subtarget)
}

if (@ARGV == 1 && $ARGV[0] eq 'targets') {
	get_targetinfo();
	foreach my $target_name (sort keys %targets) {
		printf "%s %s\n", $target_name, $targets{$target_name};
	}
}
elsif (@ARGV == 1 && $ARGV[0] eq 'architectures') {
	get_targetinfo();
	foreach my $target_arch (sort keys %architectures) {
		printf "%s %s\n", $target_arch, join ' ', @{$architectures{$target_arch}};
	}
}
elsif (@ARGV == 1 && $ARGV[0] eq 'kernels') {
	get_targetinfo();
	foreach my $target_name (sort keys %targets) {
		printf "%s %s\n", $target_name, join ' ', @{$kernels{$target_name}};
	}
}
elsif (@ARGV == 2 && $ARGV[0] eq 'devices') {
	get_devices($ARGV[1]);
	foreach my $device (sort keys %devices) {
		printf "%s \"%s\"\n", $device, join '" "', @{$devices{$device}};
	}
}
else {
	print "Usage: $0 targets\n";
	print "Usage: $0 architectures\n";
	print "Usage: $0 kernels\n";
	print "Usage: $0 devices <target/subtarget>\n";
}
