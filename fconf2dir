#!/usr/bin/perl
#
# $Id$
#
# This script parses HYSKY fileareas file and produces export directories to
# T-Mail-compatible dir.frq file and BLstBBS-format file with descriptions.
#
# (c) 2002 by Andrew Mamohin 2:5022/16
# Changes by Dmitry Pankov 2:5022/81
# Patched by Stas Degteff 2:5080/102

if( $#ARGV <0 ){
print <<USAGE;
This script parse HUSKY fileareas file and produces export directories to
T-Mail-compatible dir.frq file and BLstBBS-format file with descriptions.
The 'include' config directive not supported.

Usage:
	fconf2dir <FileArea file> [<Freq file>] [<BLstBBS file>]

Example:
	fconf2dir config.fareas dir.frq blstbbs.dir
	fconf2dir config.fareas dir.frq
	fconf2dir config.fareas
USAGE
exit;
}

$freqfile = "dir.frq";
$freqfile = $ARGV[1] if( $ARGV[1] );

$bbs = $ARGV[2] if( $ARGV[2] );

if( $ARGV[0] ){
  $fidoconfig = $ARGV[0];
}elsif{
  $fidoconfig = $ENV{fidoconfig} if( $ENV{fidoconfig} );
  $fidoconfig = $ENV{FIDOCONFIG} if( $ENV{FIDOCONFIG} );

}else{
  $fidoconfig = "/etc/fido/config";
}
if ( !(-f $fidoconfig) ) {
  die "$fidoconfig not exist!";
}

open(AREAS, "$fidoconfig") || die("Can't open $fidoconfig: $!");
open(FREQ, ">$freqfile") || die("Can't create $freqfile: $!");
if($bbs){
  open(BBS, ">$bbs") || die("Can't create $bbs: $!");
}

while (<AREAS>) {
  next if( /^Filearea\s+[^\s]+\s+passthrough\s/i );
  s/^(.*)\#.*$/$1/; # Strip comments, if CommentChar = #
  if (/^\s*FileArea\s+(\S+)\s+(\S+).*?\-d\s*?\"(.*?)\".*$/i) {
    print FREQ "$2\n";
    if($bbs){
      print BBS "Name $1 \($3\)\nPath $2\n\n"
    }
  }
  else {
    if (/^\s*FileArea\s+(\S+)\s+(\S+).*$/i) {
      print FREQ "$2\n";
      if($bbs){
        print BBS "Name $1\nPath $2\n"
      }
    }
  }
}
close FREQ;
close AREAS;
close BBS if($bbs);
