/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * grptree.c : functions to implement grouplists
 *
 * by Dmitry Sergienko <trooper@email.dp.ua>, 2:464/910@fidonet
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
#include "tree.h"
#include "common.h"
#include "grptree.h"

static tree* groupTree = NULL;

int grp_compareEntries_Add(char *p_g1, char *p_g2)
{
    return 1; /* we build a list instead of btree. */
}

int grp_compareEntries_Search(char *areaName, char *grptree_item)
{
    grp_t *g = (grp_t *)grptree_item;

    if (!areaName || !grptree_item) return 1;
    if (patimat(areaName, g->pattern))
        return 0;
    else
        return 1;
}

int grp_deleteEntry(char *p_g) {
    grp_t *g = (grp_t *) p_g;
    free(g->name);
    free(g->pattern);
    return 1;
}

int  addGrpToTree(grp_t *grp)
{
    return tree_add(&groupTree, grp_compareEntries_Add, (char *)grp, grp_deleteEntry);
}

void freeGrpTree()
{
    tree_mung(&groupTree, grp_deleteEntry);
}

void addPatternToGrpTree(char *grpname, char *pattern)
{
    grp_t *t;
    t = (grp_t *) scalloc(sizeof(grp_t), 1);
    t->name = (char *) sstrdup(grpname);
    t->pattern = (char *) sstrdup(pattern);
    fflush(stdout);
    addGrpToTree(t);
}

int addPatternListToGrpTree(char *grpname, char *plist)
{
    char *pattern;
    char *plist_tmp;

    if (!grpname || !plist) return 0;

    plist_tmp = sstrdup(plist);
    plist = plist_tmp;

    pattern = strtok(plist, " \t,");
    if (pattern) {
        addPatternToGrpTree(grpname, pattern);
        while(pattern = strtok(NULL, " \t,"))
            addPatternToGrpTree(grpname, pattern);
    }
    nfree(plist_tmp);
    return 1;
}

void initGroupTree()
{
    tree_init(&groupTree, 0);
}

grp_t *findGroupForArea(char *areaName)
{
    if (!areaName) return 0;
    return (grp_t *) tree_srch(&groupTree, grp_compareEntries_Search, areaName);
}

/* DEBUG
main(int argc, char **argv)
{
    grp_t *t;
    initGroupTree();
    addPatternListToGrpTree("SU", "su.* xsu.*");
    addPatternListToGrpTree("humor", "*.HumoR *.prikol humor.*");
    t = findGroupForArea(argv[1]);
    if (t)
        printf("%s\n", t->pattern);
    else
        printf("not found\n");
    freeGrpTree();
}
*/