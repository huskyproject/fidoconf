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

#include <stdio.h>
#include <huskylib/huskylib.h>
#include "fidoconf.h"
#include "arealist.h"
#include "common.h"

#ifndef VERSION_H
#define VERSION_H

#include "version.h"
#include "cvsdate.h"

#endif

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

int linked(s_link *link) {
    unsigned int i, n, rc;

    if (!link) return -1;

    for (i=n=0; i<cfg->echoAreaCount; i++) {
	rc=subscribeCheck(cfg->echoAreas[i], link);
	if (rc==0) {
	    if(!n)
              printf("\n%s areas on %s:\n\n",((link->Pause & ECHOAREA) == ECHOAREA) ? "Passive" : "Active", aka2str(link->hisAka));
	    printf("  %s\n", cfg->echoAreas[i].areaName);
	    n++;
	}
    }
    if( n ) printf("\n%u areas linked\n\n", n);
    else    printf("%s not linked to any area\n", aka2str(link->hisAka));

    return 0;
}

int main(int argc, char **argv) {

   { char *temp;
     printf("%s\n\n", temp=GenVersionStr( "linked", FC_VER_MAJOR, FC_VER_MINOR,
				FC_VER_PATCH, FC_VER_BRANCH, cvs_date ));
     nfree(temp);
   }

    cfg = readConfig(NULL);
    if (argc <2) {
	printf("\tShow linked areas for link\n\n");
	printf(" Usage: linked <Address> [<Address> ... ]\n");
    } else
	for(; --argc; ) {
          if(linked(getLink(cfg, argv[argc] ))) {
            hs_addr paddr;
            string2addr(argv[argc],&paddr);
            if( paddr.zone ) {
              printf("link %s not found in config file\n", argv[argc]);
            } else printf("illegal parameter no.%d (\"%s\"): not an FTN address\n", argc, argv[argc]);
          } else if ( argc>1 ) printf( "-------------------------------------\n" );
    };
    return 0;
}

#ifdef __cplusplus
};
#endif
