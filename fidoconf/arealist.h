/* $Id$ */

#ifndef _AREALIST_H
#define _AREALIST_H

#include "fidoconf.h"

typedef struct arealisttiem {
	int active;
        int rescanable;
        int readonly;
        int writeonly;
        int fullaccess;
        int mandatory;
	char *tag;
        char *grp;
	char *desc;
} s_arealistitem, *ps_arealistitem;

typedef struct arealist {
	int count;
	int maxcount;
	ps_arealistitem areas;
} s_arealist, *ps_arealist;

HUSKYEXT ps_arealist newAreaList(s_fidoconfig *cfg);
HUSKYEXT void        freeAreaList(ps_arealist al);
HUSKYEXT int         addAreaListItem(ps_arealist al, int active, int rescanable, int import, int export, int mandatory, char *tag, char *desc, char *grp);
HUSKYEXT void        sortAreaList(ps_arealist al);
HUSKYEXT void        sortAreaListNoDupes(unsigned int halcnt, ps_arealist *hal, int nodupes);
HUSKYEXT char        *formatAreaList(ps_arealist al, int maxlen, char *activechars, int grps);

#endif
