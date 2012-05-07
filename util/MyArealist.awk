#!/usr/bin/awk -f
# $Id$
# My Arealist: make arealist.NA from fidoconfig and echolist

# English:
# Parameters: file()s) with ''echoarea'' (or 'filearea'') lines from fidoconfig
# and echolist files in the comma-separated format (echo50.lst), in any order
#
# Russian (UTF-8 encoded):
# Параметры: файлы со строками echoarea или filearea от фидоконфига и файлы
# эхолистов в формате "значения разделённые запятыми" (как echo50.lst), порядок
# следования файлов - любой
#

BEGIN {
 if( ARGC < 2 ) {
   print "USAGE: myarealist fidoconfig/echoareas fileecho/xofcelist/echo50.lst [...]"
   exit 1
 }
}

func add_areadesc(areaname,desc) {
  areadesc[toupper(areaname)]=desc
}

func add_area(aname,   upperareaname) { # upperareaname - local variable
  upperareaname = toupper(aname)
  if( !(upperareaname in area) ) {
    area[length(area)+1]=upperareaname
  }
}


func parse_echolist_line() {
  if( /^[[:alnum:]]*,/ ) {
    FS = "," ; $0 = $0
    add_areadesc($2,$3)
  }
}

func parse_fidoconf_line() {
  if(/^[[:space:]]*([eE][cC][hH][oO]|[fF][iI][lL][eE])[Aa][rR][eE][aA][[:space:]]/) {
    FS = " " ; $0 = $0
    add_area($2)
  }
}

/^[[:alnum:]]*,/ {
  parse_echolist_line()
}

/^[[:space:]]*([eE][cC][hH][oO]|[fF][iI][lL][eE])[Aa][rR][eE][aA][[:space:]]/ {
  parse_fidoconf_line()
}

END {
  maxi=asort(area)
  for( i=1; i<=maxi;i++ ) {
    printf "%16-s %s\n", area[i], areadesc[area[i]]
  }
}

