#!/usr/bin/perl
#
# This script parses HPT echoareas file and produces export areatags and
# descriptions to FIDONET.NA-compatible file.
#
# (c) 2002 by Dmitry Pankov 2:5022/81

if ($#ARGV > 1) {
    print("Wrong command line arguments number\n");
    exit;
}
 
if ($#ARGV > 0) {
  open(AREAS, "<" . $ARGV[0]) || die("file $ARGV[0] not found");
  open(ELIST, ">" . $ARGV[1]);
  print "Parsing config...\n";
  while (<AREAS>) {
    s/^(.*)\#.*$/$1/; # Strip comments, if CommentChar = #
    if (/^\s*EchoArea\s+(\S+)\s+(\S+).*?\-d\s*?\"(.*?)\".*$/i) { 
       printf ELIST "%-25s %-s\n", $1, $3;
    }
    else {
      if (/^\s*EchoArea\s+(\S+)\s+(\S+).*?.*$/i) { 
         printf ELIST "%s\n", $1;
      }
    }
  }
  close ELIST; close AREAS;
  print "Done!\n";
}
else { 
  print "Usage\: fconf2na\.pl <EchoArea file> <EchoList file>\n";
  print "Example\: fconf2na\.pl config.areas echolist.na\n";
}
