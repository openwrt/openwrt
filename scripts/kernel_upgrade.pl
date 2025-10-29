#!/usr/bin/env perl
#########################################################################
# SPDX-License-Identifier: GPL-3.0-or-later				#
#									#
# Copyright © 2024 Elliott Mitchell <ehem+openwrt@m5p.com>		#
#########################################################################

use warnings;
use strict;

#
# I'm not certain the technique originated here, but this comes from:
# <https://devblogs.microsoft.com/oldnewthing/20190919-00/?p=102904>
#
# Problem is copying a file in git causes the new file to be created
# without any history.  Files can move around without losing their
# history, but that only leaves the history on the new location.
#
# As such this can be solved with two commits.  The first commit moves
# files from their old name to their new name.  The second merges the
# original commit with the rename commit.  The merge commit then has
# files in both locations with the full history.
#
#
# Note, git handles discarded data by garbage collection.  When doing
# development on this script, beware this script is an excellent
# garbage generator.  Frequent use of `git gc` and `git prune` may be
# needed.
#


sub gethead()
{
	open(my $fd, '-|', 'git', 'rev-parse', 'HEAD');
	$_=<$fd>;
	chop if(defined($_));
	return $_;
}

sub getlistmatch($$@)
{
	my ($commit, $from, @target)=@_;
	my $ret=[];

	local $/="\0";
	open(my $fd, '-| :raw :bytes', 'git', 'ls-tree', '-trz', '--full-tree',
'--name-only', $commit.':', '--', @target)||die("failed to read git tree");

	while(<$fd>) {
		chop($_);
		push(@$ret, substr($_, 0, -length($from)))
if(index($_, $from, length($_)-length($from))>0);
	}

	check: foreach my $entr (@target) {
		my ($lo, $hi)=(0, scalar(@$ret));
		while($lo!=$hi) {
			$_=substr($ret->[($lo+$hi)>>1], 0, length($entr)) cmp $entr;
			if($_<0) {
				$lo=(($lo+$hi)>>1)+1;
			} elsif($_>0) {
				$hi=($lo+$hi)>>1;
			} else {
				next check;
			}
		}
		die("no files matching \"$from\" found in $entr");
	}

	@$ret=reverse(@$ret);

	return $ret;
}

{ # start of interface to git fast-import
package GitImporter;

# git fast-import's protocol uses *linefeeds*
local $/="\n";

sub new()
{
	my $class=shift;
	my $self={};
	my @child;
	pipe($child[0], $self->{out}) || die("pipe() failed");
	binmode($self->{out});

	local $^F=fileno($self->{out})+10;
	pipe($self->{in}, $child[1]) || die("pipe() failed");
	binmode($self->{in});

	$self->{pid}=fork();
	if($self->{pid}==0) {
		close($self->{out});
		close($self->{in});

		open(STDIN, '<&', $child[0]);
		close($child[0]);

		exec('git', 'fast-import', '--done', '--date-format=now', '--cat-blob-fd='.fileno($child[1]));
		die('exec() of git failed');
	} elsif(!$self->{pid}) {
		die('fork() failed');
	}
	close($child[0]);
	close($child[1]);
	$self->{out}->autoflush(1);

	return bless($self, $class);
}

sub send($)
{
	my $self=shift;
	my ($data)=@_;
	return print({$self->{out}} $data);
}

sub putcmd($)
{
	my $self=shift;
	my ($data)=@_;
	return $self->send("$data\n");
}

sub recv()
{
	my $self=shift;
	return $_=readline($self->{in});
}

sub getres()
{
	my $self=shift;
	$_=$self->recv();
	chomp;
	return $_;
}

sub namequote($)
{
	my $self=shift;
	my ($name)=@_;

	# Alas, `ls` uses quoted strings, hopefully the quoting doesn't change
	use feature 'state';
	state %charmap;
	unless(%charmap) {
		$charmap{chr($_)}=sprintf('\\x%02X', $_) foreach(1..255);
		$charmap{chr($_)}=chr($_) foreach(ord(' ')..ord('~'));
		$charmap{'"'}='\\"';
		$charmap{'\\'}='\\\\';
	}

	$_='';
	$_=$charmap{chop($name)}.$_ while($name);
	return $_;
}

sub ls($$)
{
	my $self=shift;
	my ($commit, $name)=@_;

	$name=$self->namequote($name);

	$commit.=' ' if($commit);
	$self->putcmd("ls $commit\"$name\"");
	$self->getres();

	if(/^([0-8]+)\s+[a-z]+\s+([0-9a-z]+)\s+.*$/) {
		return [$1, $2];
	} elsif($_ eq "missing $name" or $_ eq "missing \"$name\"") {
		return undef;
	} else {
		die("git ls failed data=\'ls $commit\"$name\"\' res=\'$_\'");
	}
}

sub commit($$;$@)
{
	my $self=shift;
	my ($mark, $message, $parent, @parents)=@_;

	use feature 'state';
	use Digest::SHA qw(sha1_hex);

	state $author;
	unless($author) {
		$author=['', ''];
		open(my $user, '-|', 'git', 'config', '--get', 'user.name');
		while(<$user>) {
			chomp;
			$author->[0].=$_;
		}
		$author->[0]=[split(/,/, [getpwuid($<)]->[6])]->[0]
unless($author->[0]);

		open(my $email, '-|', 'git', 'config', '--get', 'user.email');
		while(<$email>) {
			chomp;
			$author->[1].=$_;
		}
		$author->[1]='anonymous@example.com' unless($author->[1]);

		$author=$author->[0].' <'.$author->[1].'>';
	}

	$_=sha1_hex(time());
	$self->putcmd("commit $_");
	$self->putcmd("mark $mark");
	$self->putcmd("committer $author now");

	$_=length($message);
	$self->putcmd("data $_");
	$self->send($message);

	$self->putcmd("from $parent") if($parent);
	$self->putcmd("merge $_") foreach(@parents);
}

sub DESTROY()
{
	my $self=shift;

	$self->putcmd('done');
	close($self->{out});
	delete $self->{out};

	0 while(waitpid($self->{pid}, 0)!=$self->{pid});
	delete $self->{pid};
	close($self->{in});
	delete $self->{in};

	print(STDERR <<~"__GIT_STATUS__") if($?);
	WARNING: git returned error exit status: $?

	This likely means `git gc` needs to be run, but the return codes of
	`git fast-import` are undocumented.

	NOTICE: `git gc` should be run AFTER merging or creating the branch.
	Otherwise, the new commit is unreferenced and could be garbage-
	collected.

	__GIT_STATUS__
}
} # end of interface to git fast-import


die(<<"__USAGE__") if(@ARGV<2);
Usage: $0 <old-version> <new-version> [<board(s)...>]

Copies all kernel configuration files and patches from the old version
to the new version.  Git history is preserved on the copies by using a
move/merge strategy.  Must be run while somewhere inside the git
repository directory, but it does not matter where.

This can be used with partially updated repositories.  If some boards
have been updated, their configurations will be left untouched.  It is
suggested the versions should be of the form \"-X.Y\" to better ensure
extraneous files are left untouched.

Note, the two strings are non-optional, but completely free-form.
There are no limitations besides whether they can be used in a
file-name (\\0 is the only invalid character).

One or more boards can be specified to update a subset of boards.  This
can include "generic", this can also include a subdirectory for a device.

The end merge commit /can/ be amended to remove files which should not
be included during an update.
__USAGE__

my $from=shift(@ARGV);
my $to=shift(@ARGV);


my $target='target/linux/';

my $start=gethead();

die('git failed, likely not inside OpenWRT git repository?') unless($start||
($0=~m/^(.*)\// and chdir($1) and $start=gethead()));

my $list;
unless(@ARGV) {
	$list=getlistmatch($start, $from, $target);
} else {
	$_=$target.$_ foreach(@ARGV);
	$list=getlistmatch($start, $from, @ARGV);
}


my $git=GitImporter->new();

$git->commit(':1', <<"__TMP__", $start);
kernel: add configs and patches for $to

This is a special tool-generated commit.

Copy configuration and patches from $from to $to.

If you see this commit during a `git bisect` session, the recommended
course of action is to run `git bisect skip`.

If this is the last commit you see during `git log`, you likely want to
use `git log --follow`.
__TMP__

foreach my $name (@$list) {
	next if($git->ls($start, "$name$to"));
	my $new=$git->ls('', "$name$from");
	$_=$git->namequote($name);
	$git->putcmd("M $new->[0] $new->[1] \"$_$to\"");
	$git->putcmd("D \"$_$from\"");
}
$git->putcmd('');


$git->commit(':2', <<"__TMP__", ':1', $start);
kernel: finish update from $from to $to

This is a special tool-generated commit.

Merge the add commit into HEAD to create all files with full history.
__TMP__

foreach my $name (@$list) {
	next if($git->ls($start, "$name$to"));
	my $new=$git->ls($start, "$name$from");
	$_=$git->namequote($name);
	$git->putcmd("M $new->[0] $new->[1] \"$_$from\"");
}
$git->putcmd('');


$git->putcmd('get-mark :2');
my $result=$git->getres();

undef($git);

print(<<"__END__");
Result is commit $result.

Depending on the setup of your development environment, you now likely
want to run one of two commands:

	`git merge --ff-only $result`
or:
	`git branch linux$to $result`
__END__

exit(0);
