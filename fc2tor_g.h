/*--------------------------------------------------
    Victor 'mgl' Anikeev, mgl@pisem.net
    --------------------------------------------------*/

#ifndef __MGLGROUPS_H__
#define __MGLGROUPS_H__

#include "fidoconf.h"

/*--------------------------------------------------
 Returns number of groups in string. Groups must be
 separated with symbols, defined in delms string. The
 function doesn't check duplicates and skips
 empty groups.
 Expamples: 
 2 == ("windows,;,,,,linux", ",;"))
 3 == ("quake quake quake", " "));
    --------------------------------------------------*/
FCONF_EXT int getGroupSetSize(char *str, register char *delms);


/*--------------------------------------------------
 Returns 1 if one set of groups (sub) is 
 sub set of other set (groups). Groups must be
 separated with symbols, defined in delms string.
 Expamples:
 1 == ("quake doom", "doom wolf quake", " ")
 1 == ("quake doom", "quake doom"...
 1 == ("quake", "quake doom"...
 1 == ("", "quake doom"...
 0 == ("quake doom", "wolf quake"...
 0 == ("quake", "doom"...
 0 == ("quake", ""...
    --------------------------------------------------*/
FCONF_EXT int isSubSetOfGroups(char *sub, char *groups, register char *delms);


/*--------------------------------------------------
 Returns 1 if group sets grp1 and grp2 have common
 groups or both sets are empty. Groups must be
 separated with symbols, defined in delms string.
 Expamples:
 0 == ("quake", "doom")
 0 == ("quake", "")
 1 == ("quake", "quake")
 1 == ("quake doom", "doom wolf")
    --------------------------------------------------*/
FCONF_EXT int areCrossGroupSets(char *grp1, char *grp2, register char *delms);

/*--------------------------------------------------
 Returns 1 if two group sets are equal. Groups must be
 separated with symbols, defined in delms string.
    --------------------------------------------------*/
FCONF_EXT int areEqualGroupSets(char *grp1, char *grp2, register char *delms);

#endif
