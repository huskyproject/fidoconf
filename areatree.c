/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * areatree.c : some functions to make search areas by names faster
 *
 * by Max Chernogor <mihz@mail.ru>, 2:464/108@fidonet
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

#include <string.h>
#include "areatree.h"

static tree* echoAreaTree = NULL;
static tree* fileAreaTree = NULL;

static ps_area fileAreaPtr = NULL;
static ps_area echoAreaPtr = NULL;



int fc_compareEntries(char *p_e1, char *p_e2)
{
    ps_area e1 = (ps_area)p_e1;
    ps_area e2 = (ps_area)p_e2;
    if(stricmp(e1->areaName,e2->areaName) < 0)
        return -1;
    else if(stricmp(e1->areaName,e2->areaName) > 0)
        return 1;
    return 0;
}

int fc_deleteEntry(char *p_e1) {
    return 1;
}


ps_area FindAreaInTree(char* areaName)
{
    static s_area areaSrc;
    if(echoAreaPtr && stricmp(echoAreaPtr->areaName,areaName) == 0)
        return echoAreaPtr;
    else
        areaSrc.areaName = areaName;
    echoAreaPtr = (ps_area)tree_srch(&echoAreaTree, fc_compareEntries, (char *)(&areaSrc));
    return echoAreaPtr;
}

ps_area FindFileAreaInTree(char* areaName)
{
    static s_area areaSrc;
    if(fileAreaPtr && stricmp(fileAreaPtr->areaName,areaName) == 0)
        return fileAreaPtr;
    else
        areaSrc.areaName = areaName;
    fileAreaPtr = (ps_area)tree_srch(&fileAreaTree, fc_compareEntries, (char *)(&areaSrc));
    return fileAreaPtr;
}


int    RebuildEchoAreaTree(ps_fidoconfig config)
{
    unsigned int i = 0;
    
    if (echoAreaTree)
        tree_mung(&echoAreaTree, fc_deleteEntry);
    tree_init(&echoAreaTree, 1);
    
    for (i=0; i < config->echoAreaCount; i++)
    {
        if ( tree_add(&echoAreaTree, fc_compareEntries, (char *)(&(config->echoAreas[i])), fc_deleteEntry ) == 0 )
        {
            fprintf(stderr, "\nArea [%s]  defined twice\n",config->echoAreas[i].areaName );
            return 0;
        }
    }
    for (i=0; i < config->localAreaCount; i++)
    {
        if ( tree_add(&echoAreaTree, fc_compareEntries, (char *)(&(config->localAreas[i])), fc_deleteEntry ) == 0 )
        {    
            fprintf(stderr, "\nArea [%s]  defined twice\n",config->localAreas[i].areaName );
            return 0;
        }
    }
    echoAreaPtr = NULL;
    return 1;
}


int    RebuildFileAreaTree(ps_fidoconfig config)
{
    unsigned int i = 0;
    
    if (fileAreaTree)
        tree_mung(&fileAreaTree, fc_deleteEntry);
    tree_init(&fileAreaTree, 1);
    
    for (i=0; i < config->fileAreaCount; i++)
    {
        if ( tree_add(&fileAreaTree, fc_compareEntries, (char *)(&(config->fileAreas[i])), fc_deleteEntry) == 0 )
        {
            fprintf(stderr, "\nFileArea [%s]  defined twice\n",config->fileAreas[i].areaName );
            return 0;
        }
    }
    fileAreaPtr = NULL;
    return 1;
}


void     FreeAreaTree()
{
    tree_mung(&echoAreaTree, fc_deleteEntry);
    tree_mung(&fileAreaTree, fc_deleteEntry);
}
