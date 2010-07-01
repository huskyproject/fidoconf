#!/usr/bin/perl

# Convert SQUISH.CFG to echoareas's part of fidoconfig
# REad SQUISH.CFG and SQAFIX.CFG from current directory
# and prints into stdout fidoconfig directives "Echoarea"
# @ Igor Zakharoff 2:5030/2404

use strict;
my $defaults = "-b squish";

open SQUISH, '<', "SQUISH.CFG" || die "Can't open SQUISH.CFG: $!\n";
open SQAFIX, '<', "SQAFIX.CFG" || die "Can't open SQAFIX.CFG: $!\n";

while( <SQUISH> ) {
  if( /^EchoArea\s+(\S+)\s+(\S+)\s+-\$m(\S+)\s+-p(.*)/ ) {
    my $name = $1;
    my $path = $2;
    my $purge = $3;
    my $links = $4;
    my $def = "$defaults $path -\$m $purge -a $links";

    seek SQAFIX,0,0;
    while( <SQAFIX> ) {
      if( /^EchoArea\s+$name\s+(\S+)\s+(\".*\")/ ) {
        $def = "$defaults $path -g $1 -d $2 -\$m $purge -a $links";
        last;
      }
    }
    print "EchoArea $name $def\n";
  }
}

close SQUISH;
close SQAFIX;
