#!/usr/bin/perl
# $Id$
###############################################################################
# fconf config file to areas.bbs converter
# Done Dez. 1999 by Grischa Brockhaus, brockhaus@grischa.de, 2:2411/601.1
# This source is public domain, do what you want ! :-)
#
# Added JAM support by Dmitry Pankov 2:5022/81

# Please change these variables, if needed, or (better) change the
# environment variable FIDOCONFIG:
my $fconf	= "/etc/fido/config";
my $areasbbs	= "AREAS.BBS";

# =============================================================================
# Main Routine
header();
if( $#ARGV <0 ) {

print <<USAGE;
Usage:
	fconf2areasbbs <FidoConfigFileName> <AreasBbsFileName>
	fconf2areasbbs <AreasBbsFileName>

Example:
	fconf2areasbbs config.areas areas.bbs
USAGE

exit;

}elsif( $#ARGV == 0 ) { # one parameter

  $fconf = $ENV{fidoconfig} if( $ENV{fidoconfig} );
  $fconf = $ENV{FIDOCONFIG} if( $ENV{FIDOCONFIG} );
  $areasbbs = $ARGV[0] if( $ARGV[0] );

}else{ # two parameters

  $fconf = $ARGV[0];
  $areasbbs = $ARGV[1];

}

if( $fconf && -f $fconf ){
  readConfig();
  saveAreasBBS();
}else{
  die "Undefined fidoconfig or fidoconfig not found!";
}
exit;

# =============================================================================
# Subroutinen, lokale Variablen

my @echoarea;

my ($cfgname,$cfgwert);

###############################################################################
# readConfig()
# Autor: gb
# Datum: 19991209
# Descr: Es wird die fconf Konfiguration eingelesen und Prgrammvariablen
#	mit der	Konfiguration geladen.
sub readConfig(){
  print ("reading $fconf\n");
  if (open(CONFIG, $fconf)) {
    while(<CONFIG>) {
        parseCfgLine($_);
	if ($cfgname =~ /echoarea/i) { saveEchoArea($cfgwert); }
        if ($cfgname =~ /include/i) {
            print "To process include file please call $0 $cfgwert $areasbbs\n";
        }
    }
    close(CONFIG);
  } else {
    print "Can't open $fconf: $!\n";
  }
}

###############################################################################
# parseCfgLine()
# Autor: gb
# Datum: 19991211
# Descr: Übergebener String wird in Schlüsselwort und Wert getrennt und
#	in die entsprechende globale Variable abgelegt ($bezeichner,$wert)
sub parseCfgLine(){
    my @werte;
    ($cfgname,@werte) = split (/[\s]+/, $_[0]);
    $cfgwert = "";
    foreach (@werte) {
	$cfgwert .=$_." ";
    }
    $cfgwert =~ s/\s$//g;
}

###############################################################################
# saveEchoArea()
# Autor: gb
# Datum: 19991211
# Descr: Zeile hinter dem EchoArea Tag wird zerlegt und relevante Teile in
#	in eine globae Liste abgespeichert.
#
# Parameter examples:
# DELPHI passthrough -a 2:5080/102 -g J -lw 2 -d "About DELPHI" 2:5080/68 -def 2:5080/102.84
# LINUX /fido/msgs/linux -b jam -a 2:5080/102 -g F -lw 9 -p 21 -d "International echo: Linux" 2:5020/1159 -def
#
sub saveEchoArea(){
    my @echoparse = split (/[\s]+/, $_[0]);
    my $echotype ="";
    my $oline = "";
    my $i;

    # areatag & file
    if ($echoparse[1] =~ /passthrough/i ){
      $oline = sprintf "P     %-25s   ", @echoparse[0] ;
    }else{
      if ($_[0] =~ /-b[\s]*squish/i ) {$echotype ="\$";}
      elsif ($_[0] =~ /-b[\s]*jam/i ) {$echotype ="\!";}
      elsif ($_[0] =~ /-b[\s]*msg/i ) {$echotype ="";}
      $oline = sprintf "%-15s %-25s   ",$echotype.@echoparse[1], @echoparse[0] ;
    }
    # add links
    for( $i=2; $i<=$#echoparse; $i++ ){
       $echoparse[$i] =~ s/^"(.*)"$/$1/; # quoted parameter: dequote.
       if( $echoparse[$i] =~ /^"/ ){             # quote open,
         while( $echoparse[$i] !~ /"$/ ){ $i++;} # skip to quote close
       }
       if( ($echoparse[$i] =~ m|^\d+:\d+/\d+(\.\d+)?$|) && ($echoparse[$i-1] !~ /-a/i )){ # link
         $oline .= " $echoparse[$i]";
       }
    }
    push @echoarea, $oline;

}

###############################################################################
# readConfig()
# Autor: gb
# Datum: 19991209
# Descr: Ausgabe des Infokopfes
sub header(){
    printf("fconf2areasbbs\n");
    printf("--------------\n");
}

###############################################################################
# saveAreasBBS()
# Autor: gb
# Datum: 19991211
# Descr: Es wird aus den ausgelesenen Daten eine Areas.BBS erzeugt.
sub saveAreasBBS(){
    print ("saving $areasbbs\n");
    open(AREAS, ">".$areasbbs);
    printf(AREAS "; AREAS.BBS file created by fconf2areasbbs, part of the husky FIDOnet software project\n");

    printf(AREAS ";\n");
    foreach(@echoarea) {
        printf(AREAS "%s\n",$_);
    }
    close(AREAS);
}
