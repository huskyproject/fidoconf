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

tree *groupTree = NULL;

int grp_compareEntries_Add(char *p_g1, char *p_g2)
{
    return 1; /* we build a list instead of btree. */
}

int grp_compareEntries_Search(char *areaName, char *grptree_item)
{
    grp_t *g = (grp_t *)grptree_item;
    char *pattern;
    char *plist_tmp;
    char *plist;
    int found;

    if (!areaName || !grptree_item) return 1;

    plist_tmp = sstrdup(g->patternList);
    plist = plist_tmp;

    pattern = strtok(plist, " \t,");
    if (pattern) {
        if ((found = patimat(areaName, pattern)) == 0)
            while((pattern = strtok(NULL, " \t,")))
                if (found = patimat(areaName, pattern))
                    break;
    }
    nfree(plist_tmp);

    return !found;
}

int grp_compareEntries_SearchByName(char *groupName, char *grptree_item)
{
    grp_t *g = (grp_t *)grptree_item;

    if (!groupName || !grptree_item) return 1;

    if (stricmp(groupName, g->name))
        return 1;
    else
        return 0;
}

int grp_deleteEntry(char *p_g) {
    grp_t *g = (grp_t *) p_g;
    if (!g) return 1;
    nfree(g->name);
    nfree(g->patternList);
    nfree(g->area);
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

/* adds patterns to new group (returns 1) or to existing group (returns 0) */
int addPatternToGrpTree(char *grpname, char *patternList)
{
    grp_t *g;

    if ((g = findGroupByName(grpname)) == NULL) {
        g = (grp_t *) scalloc(sizeof(grp_t), 1);
        g->name = (char *) sstrdup(grpname);
        g->patternList = (char *) sstrdup(patternList);
        g->area = scalloc(sizeof(s_area), 1);
        addGrpToTree(g);
        return 1;
    } else {
        xstrscat(&(g->patternList), " ", patternList, NULL);
        return 0;
    }
}

/*
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
        while((pattern = strtok(NULL, " \t,")))
            addPatternToGrpTree(grpname, pattern);
    }
    nfree(plist_tmp);
    return 1;
}
*/

void initGroupTree()
{
    tree_init(&groupTree, 0);
}

grp_t *findGroupForArea(char *areaName)
{
    if (!areaName) return 0;
    return (grp_t *) tree_srch(&groupTree, grp_compareEntries_Search, areaName);
}

grp_t *findGroupByName(char *groupName)
{
    if (!groupName) return 0;
    return (grp_t *) tree_srch(&groupTree, grp_compareEntries_SearchByName, groupName);
}

/* DEBUG
main(int argc, char **argv)
{
    grp_t *t;
    initGroupTree();
    addPatternToGrpTree("SU", "su.* xsu.*");
    addPatternToGrpTree("humor", "*.HumoR *.prikol humor.*");
    t = findGroupForArea(argv[1]);
    if (t)
        printf("%s\n", t->pattern);
    else
        printf("not found\n");
    freeGrpTree();
}
*/
