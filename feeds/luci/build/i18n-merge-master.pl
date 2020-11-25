#!/usr/bin/env perl

if (open F, '-|', 'find', $ARGV[0] || '.', '-type', 'f', '-name', '*.po') {
	while (defined(my $path = readline F)) {
		chomp $path;

		(my $ref = $path) =~ s/\.po$/\.master.po/;

		printf 'Updating %s ', $path;
		system("git show --format=\%B 'master:$path' > '$ref'");
		system('msgmerge', '-N', '-o', $path, $ref, $path);
		system('msgattrib', '--no-obsolete', '-o', $path, $path);
		unlink($ref);
	}

	close F;
}
