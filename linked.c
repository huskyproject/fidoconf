/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * linked.c : Show linked areas for link.
 *
 * Copyright (C) Husky developers team
 *
 *
 * This file is part of FIDOCONFIG library (part of the Husky FIDOnet
 * software project)
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * FIDOCONFIG library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FIDOCONFIG library; see the file COPYING.  If not, write
 * to the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA
 *
 * See also http://www.gnu.org
 *****************************************************************************
 * $Id$
 */

#include "fidoconf.h"
#include "arealist.h"

s_fidoconfig *cfg;

#ifdef __cplusplus
extern "C" {
#endif

int subscribeCheck(s_area area, s_link *link)
{
    int found = 0;
    if (isLinkOfArea(link, &area)) return 0;

    if (area.group) {
	if (cfg->numPublicGroup)
	    found = grpInArray(area.group,cfg->PublicGroup,cfg->numPublicGroup);
	if (!found && link->numAccessGrp)
	    found = grpInArray(area.group,link->AccessGrp,link->numAccessGrp);
    } else found = 1;

    if (!found){
      return 2;
    }
    if (area.levelwrite > link->level && area.levelread > link->level){
      return 2;
    }
    return 1;
}

char *linked(s_link *link) {
    unsigned int i, n, rc;
    char *report = NULL;

    xscatprintf(&report, "%s areas on %s\n\n",((link->Pause & EPAUSE) == EPAUSE) ? "Passive" : "Active", aka2str(link->hisAka));

    for (i=n=0; i<cfg->echoAreaCount; i++) {
	rc=subscribeCheck(cfg->echoAreas[i], link);
	if (rc==0) {
	    xscatprintf(&report, "  %s\n", cfg->echoAreas[i].areaName);
	    n++;
	}
    }
    xscatprintf(&report, "\n%u areas linked\n", n);
    return report;
}

int main(int argc, char **argv) {

    cfg = readConfig(NULL);
    if (argc !=2) {
	printf(" Usage: linked <Address>\n");
    } else {
	printf(linked(getLink(cfg, argv[1] )));
    };
}

#ifdef __cplusplus
};
#endif
