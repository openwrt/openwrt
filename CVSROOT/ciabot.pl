#!/usr/bin/perl -w
#
# ciabot -- Mail a CVS log message to a given address, for the purposes of CIA
#
# Loosely based on cvslog by Russ Allbery <rra@stanford.edu>
# Copyright 1998  Board of Trustees, Leland Stanford Jr. University
#
# Copyright 2001, 2003, 2004  Petr Baudis <pasky@ucw.cz>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License version 2, as published by the
# Free Software Foundation.
#
# The master location of this file is
# http://pasky.or.cz/~pasky/dev/cvs/ciabot.pl.
#
# This program is designed to run from the loginfo CVS administration file. It
# takes a log message, massaging it and mailing it to the address given below.
#
# Its record in the loginfo file should look like:
#
#       ALL        $CVSROOT/CVSROOT/ciabot.pl %s $USER project from_email dest_email ignore_regexp
#
# Note that the last four parameters are optional, you can alternatively change
# the defaults below in the configuration section.
#
# If it does not work, try to disable $xml_rpc in the configuration section
# below.
#
# ciabot.pl,v 1.110 2004/01/09 17:40:13 pasky
# $Id$

use strict;
use vars qw ($project $from_email $dest_email $rpc_uri $sendmail $sync_delay
		$xml_rpc $ignore_regexp $alt_local_message_target);




### Configuration

# Project name (as known to CIA).
$project = 'ELinks';

# The from address in generated mails.
$from_email = 'pasky@ucw.cz';

# Mail all reports to this address.
$dest_email = 'cia@navi.cx';

# If using XML-RPC, connect to this URI.
$rpc_uri = 'http://cia.navi.cx/RPC2';

# Path to your USCD sendmail compatible binary (your mailer daemon created this
# program somewhere).
$sendmail = '/usr/sbin/sendmail';

# Number of seconds to wait for possible concurrent instances. CVS calls up
# this script for each involved directory separately and this is the sync
# delay. 5s looks as a safe value, but feel free to increase if you are running
# this on a slower (or overloaded) machine or if you have really a lot of
# directories.
$sync_delay = 5;

# This script can communicate with CIA either by mail or by an XML-RPC
# interface. The XML-RPC interface is faster and more efficient, however you
# need to have RPC::XML perl module installed, and some large CVS hosting sites
# (like Savannah or Sourceforge) might not allow outgoing HTTP connections
# while they allow outgoing mail. Also, this script will hang and eventually
# not deliver the event at all if CIA server happens to be down, which is
# unfortunately not an uncommon condition.
$xml_rpc = 0;

# You can make this bot to totally ignore events concerning the objects
# specified below. Each object is composed of <module>/<path>/<filename>,
# therefore file Manifest in root directory of module gentoo will be called
# "gentoo/Manifest", while file src/bfu/inphist.c of module elinks will be
# called "elinks/src/bfu/inphist.c". Easy, isn't it?
#
# This variable should contain regexp, against which will each object be
# checked, and if the regexp is matched, the file is ignored. Therefore ie.  to
# ignore all changes in the two files above and everything concerning module
# 'admin', use:
#
#$ignore_regexp = "^(gentoo/Manifest|elinks/src/bfu/inphist.c|admin/)";
$ignore_regexp = "/Manifest\$";

# It can be useful to also grab the generated XML message by some other
# programs and ie. autogenerate some content based on it. Here you can specify
# a file to which it will be appended.
$alt_local_message_target = "";




### The code itself

use vars qw ($user $module $tag @files $logmsg $message);

my @dir; # This array stores all the affected directories
my @dirfiles;  # This array is mapped to the @dir array and contains files
               # affected in each directory



### Input data loading


# These arguments are from %s; first the relative path in the repository
# and then the list of files modified.

@files = split (' ', ($ARGV[0] or ''));
$dir[0] = shift @files or die "$0: no directory specified\n";
$dirfiles[0] = "@files" or die "$0: no files specified\n";


# Guess module name.

$module = $dir[0]; $module =~ s#/.*##;


# Figure out who is doing the update.

$user = $ARGV[1];


# Use the optional parameters, if supplied.

$project = $ARGV[2] if $ARGV[2];
$from_email = $ARGV[3] if $ARGV[3];
$dest_email = $ARGV[4] if $ARGV[4];
$ignore_regexp = $ARGV[5] if $ARGV[5];


# Parse stdin (what's interesting is the tag and log message)

while (<STDIN>) {
  $tag = $1 if /^\s*Tag: ([a-zA-Z0-9_-]+)/;
  last if /^Log Message/;
}

while (<STDIN>) {
  next unless ($_ and $_ ne "\n" and $_ ne "\r\n");
  s/&/&amp;/g;
  s/</&lt;/g;
  s/>/&gt;/g;
  $logmsg .= $_;
}



### Remove to-be-ignored files

$dirfiles[0] = join (' ',
  grep {
    my $f = "$module/$dir[0]/$_";
    $f !~ m/$ignore_regexp/;
  } split (/\s+/, $dirfiles[0])
) if ($ignore_regexp);
exit unless $dirfiles[0];



### Sync between the multiple instances potentially being ran simultanously

my $sum; # _VERY_ simple hash of the log message. It is really weak, but I'm
         # lazy and it's really sorta exceptional to even get more commits
         # running simultanously anyway.
map { $sum += ord $_ } split(//, $logmsg);

my $syncfile; # Name of the file used for syncing
$syncfile = "/tmp/cvscia.$project.$module.$sum";


if (-f $syncfile and -w $syncfile) {
  # The synchronization file for this file already exists, so we are not the
  # first ones. So let's just dump what we know and exit.

  open(FF, ">>$syncfile") or die "aieee... can't log, can't log! $syncfile blocked!";
  print FF "$dirfiles[0]!@!$dir[0]\n";
  close(FF);
  exit;

} else {
  # We are the first one! Thus, we'll fork, exit the original instance, and
  # wait a bit with the new one. Then we'll grab what the others collected and
  # go on.

  # We don't need to care about permissions since all the instances of the one
  # commit will obviously live as the same user.

  # system("touch") in a different way
  open(FF, ">>$syncfile") or die "aieee... can't log, can't log! $syncfile blocked!";
  close(FF);

  exit if (fork);
  sleep($sync_delay);

  open(FF, $syncfile);
  my ($dirnum) = 1; # 0 is the one we got triggerred for
  while (<FF>) {
    chomp;
    ($dirfiles[$dirnum], $dir[$dirnum]) = split(/!@!/);
    $dirnum++;
  }
  close(FF);

  unlink($syncfile);
}



### Compose the mail message


my ($VERSION) = '$Revision$' =~ / (\d+\.\d+) /;
my $ts = time;

$message = <<EM
<message>
   <generator>
       <name>CIA Perl client for CVS</name>
       <version>$VERSION</version>
       <url>http://pasky.or.cz/~pasky/dev/cvs/ciabot.pl</url>
   </generator>
   <source>
       <project>$project</project>
       <module>$module</module>
EM
;
$message .= "       <branch>$tag</branch>" if ($tag);
$message .= <<EM
   </source>
   <timestamp>
       $ts
   </timestamp>
   <body>
       <commit>
           <author>$user</author>
           <files>
EM
;

for (my $dirnum = 0; $dirnum < @dir; $dirnum++) {
  map {
    $_ = $dir[$dirnum] . '/' . $_;
    s#^.*?/##; # weed out the module name
    s/&/&amp;/g;
    s/</&lt;/g;
    s/>/&gt;/g;
    $message .= "  <file>$_</file>\n";
  } split(/ /, $dirfiles[$dirnum]);
}

$message .= <<EM
           </files>
           <log>
$logmsg
           </log>
       </commit>
   </body>
</message>
EM
;



### Write the message to an alt-target

if ($alt_local_message_target and open (ALT, ">>$alt_local_message_target")) {
  print ALT $message;
  close ALT;
}



### Send out the XML-RPC message


if ($xml_rpc) {
  # We gotta be careful from now on. We silence all the warnings because
  # RPC::XML code is crappy and works with undefs etc.
  $^W = 0;
  $RPC::XML::ERROR if (0); # silence perl's compile-time warning

  require RPC::XML;
  require RPC::XML::Client;

  my $rpc_client = new RPC::XML::Client $rpc_uri;
  my $rpc_request = RPC::XML::request->new('hub.deliver', $message);
  my $rpc_response = $rpc_client->send_request($rpc_request);

  unless (ref $rpc_response) {
    die "XML-RPC Error: $RPC::XML::ERROR\n";
  }
  exit;
}



### Send out the mail


# Open our mail program

open (MAIL, "| $sendmail -t -oi -oem") or die "Cannot execute $sendmail : " . ($?>>8);


# The mail header

print MAIL <<EOM;
From: $from_email
To: $dest_email
Content-type: text/xml
Subject: DeliverXML

EOM

print MAIL $message;


# Close the mail

close MAIL;
die "$0: sendmail exit status " . ($? >> 8) . "\n" unless ($? == 0);

# vi: set sw=2:
