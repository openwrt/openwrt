#!/usr/bin/perl

use strict;
use warnings;
use IPC::Open2;
use POSIX;

$ENV{'LC_ALL'} = 'C';
POSIX::setlocale(POSIX::LC_ALL, 'C');

@ARGV >= 1 || die "Usage: $0 <source directory>\n";


my %keywords = (
	'.js' => [ '_:1', '_:1,2c', 'N_:2,3', 'N_:2,3,4c' ],
	'.lua' => [ '_:1', '_:1,2c', 'translate:1', 'translate:1,2c', 'translatef:1', 'N_:2,3', 'N_:2,3,4c', 'ntranslate:2,3', 'ntranslate:2,3,4c' ],
	'.htm' => [ '_:1', '_:1,2c', 'translate:1', 'translate:1,2c', 'translatef:1', 'N_:2,3', 'N_:2,3,4c', 'ntranslate:2,3', 'ntranslate:2,3,4c' ],
	'.json' => [ '_:1', '_:1,2c' ]
);

sub xgettext($@) {
	my $path = shift;
	my @keywords = @_;
	my ($ext) = $path =~ m!(\.\w+)$!;
	my @cmd = qw(xgettext --from-code=UTF-8 --no-wrap);

	if ($ext eq '.htm' || $ext eq '.lua') {
		push @cmd, '--language=Lua';
	}
	elsif ($ext eq '.js' || $ext eq '.json') {
		push @cmd, '--language=JavaScript';
	}

	push @cmd, map { "--keyword=$_" } (@{$keywords{$ext}}, @keywords);
	push @cmd, '-o', '-';

	return @cmd;
}

sub whitespace_collapse($) {
	my $s = shift;
	my %r = ('n' => ' ', 't' => ' ');

	# Translate \t and \n to plain spaces, leave all other escape
	# sequences alone. Finally replace all consecutive spaces by
	# single ones and trim leading and trailing space.
	$s =~ s/\\(.)/$r{$1} || "\\$1"/eg;
	$s =~ s/ {2,}/ /g;
	$s =~ s/^ //;
	$s =~ s/ $//;

	return $s;
}

sub postprocess_pot($$) {
	my ($path, $source) = @_;
	my (@res, $msgid);
	my $skip = 1;

	$source =~ s/^#: (.+?)\n/join("\n", map { "#: $path:$_" } $1 =~ m!:(\d+)!g) . "\n"/emg;

	my @lines = split /\n/, $source;

	# Remove all header lines up to the first location comment
	while (@lines > 0 && $lines[0] !~ m!^#: !) {
		shift @lines;
	}

	while (@lines > 0) {
		my $line = shift @lines;

		# Concat multiline msgids and collapse whitespaces
		if ($line =~ m!^(msg\w+) "(.*)"$!) {
			my $kw = $1;
			my $kv = $2;

			while (@lines > 0 && $lines[0] =~ m!^"(.*)"$!) {
				$kv .= ' '. $1;
				shift @lines;
			}

			$kv = whitespace_collapse($kv);

			# Filter invalid empty msgids by popping all lines in @res
			# leading to this point and skip all subsequent lines in
			# @lines belonging to this faulty id.
			if ($kw ne 'msgstr' && $kv eq '') {
				while (@res > 0 && $res[-1] !~ m!^$!) {
					pop @res;
				}

				while (@lines > 0 && $lines[0] =~ m!^(?:msg\w+ )?"(.*)"$!) {
					shift @lines;
				}

				next;
			}

			push @res, sprintf '%s "%s"', $kw, $kv;
		}

		# Ignore any flags added by xgettext
		elsif ($line =~ m!^#, !) {
			next;
		}

		# Pass through other lines unmodified
		else {
			push @res, $line;
		}
	}

	return @res ? join("\n", '', @res, '') : '';
}

sub uniq(@) {
	my %h = map { $_, 1 } @_;
	return sort keys %h;
}

sub preprocess_htm($$) {
	my ($path, $source) = @_;
	my $sub = {
		'=' => '(%s)',
		'_' => 'translate([==[%s]==])',
		':' => 'translate([==[%s]==])',
		'+' => 'include([==[%s]==)',
		'#' => '--[==[%s]==]',
		''  => '%s'
	};

	# Translate the .htm source into a valid Lua source using bracket quotes
	# to avoid the need for complex escaping.
	$source =~ s|<%-?([=_:+#]?)(.*?)-?%>|sprintf "]==]; $sub->{$1}; [==[", $2|sge;

	# Discover expressions like "lng.translate(...)" or "luci.i18n.translate(...)"
	# and return them as extra keyword so that xgettext recognizes such expressions
	# as translate(...) calls.
	my @extra_function_keywords =
		map { ("$_:1", "$_:1,2c") }
		uniq($source =~ m!((?:\w+\.)+translatef?)[ \t\n]*\(!g);

	return ("[==[$source]==]", @extra_function_keywords);
}

sub preprocess_lua($$) {
	my ($path, $source) = @_;

	# Discover expressions like "lng.translate(...)" or "luci.i18n.translate(...)"
	# and return them as extra keyword so that xgettext recognizes such expressions
	# as translate(...) calls.
	my @extra_function_keywords =
		map { ("$_:1", "$_:1,2c") }
		uniq($source =~ m!((?:\w+\.)+translatef?)[ \t\n]*\(!g);

	return ($source, @extra_function_keywords);
}

sub preprocess_json($$) {
	my ($path, $source) = @_;
	my ($file) = $path =~ m!([^/]+)$!;

	$source =~ s/("(?:title)")\s*:\s*("(?:[^"\\]|\\.)*")/$1: _($2)/sg;

	return ($source);
}


my ($msguniq_in, $msguniq_out);
my $msguniq_pid = open2($msguniq_out, $msguniq_in, 'msguniq', '-s');

print $msguniq_in "msgid \"\"\nmsgstr \"Content-Type: text/plain; charset=UTF-8\"\n";

if (open F, "find @ARGV -type f '(' -name '*.htm' -o -name '*.lua' -o -name '*.js' -o -path '*/menu.d/*.json' ')' |")
{
	while (defined( my $file = readline F))
	{
		chomp $file;

		if (open S, '<', $file)
		{
			local $/ = undef;
			my $source = <S>;
			my @extra_function_keywords;

			if ($file =~ m!\.htm$!)
			{
				($source, @extra_function_keywords) = preprocess_htm($file, $source);
			}
			elsif ($file =~ m!\.lua$!)
			{
				($source, @extra_function_keywords) = preprocess_lua($file, $source);
			}
			elsif ($file =~ m!\.json$!)
			{
				($source, @extra_function_keywords) = preprocess_json($file, $source);
			}

			my ($xgettext_in, $xgettext_out);
			my $pid = open2($xgettext_out, $xgettext_in, xgettext($file, @extra_function_keywords), '-');

			print $xgettext_in $source;
			close $xgettext_in;

			my $pot = readline $xgettext_out;
			close $xgettext_out;

			waitpid $pid, 0;

			print $msguniq_in postprocess_pot($file, $pot);
		}
	}

	close F;
}

close $msguniq_in;

my @pot = <$msguniq_out>;

close $msguniq_out;
waitpid $msguniq_pid, 0;

while (@pot > 0) {
	my $line = shift @pot;

	# Reorder the location comments in a detemrinistic way to
	# reduce SCM noise when frequently updating templates.
	if ($line =~ m!^#: !) {
		my @locs = ($line);

		while (@pot > 0 && $pot[0] =~ m!^#: !) {
			push @locs, shift @pot;
		}

		print
			map { join(':', @$_) . "\n" }
			sort { ($a->[0] cmp $b->[0]) || ($a->[1] <=> $b->[1]) }
			map { [ /^(.+):(\d+)$/ ] }
			@locs
		;

		next;
	}

	print $line;
}
