#ifndef ADCASE_H
#define ADCASE_H

/*  ATTENTION: The adaptcase routine builds an internal cache which never
 *  expires. If you ever add files to or remove files to a subdirectory and
 *  later want to have adaptcase map this particular file name properly,
 *  you must call adaptcase_refresh_dir() with the subdirectory path name
 *  as argument!
 */
#include "fidoconf.h"

void adaptcase_refresh_dir(char *directory);
FCONF_EXT void adaptcase(char *);

#endif
