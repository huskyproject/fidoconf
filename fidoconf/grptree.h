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
#ifndef _GRPTREE_H
#define _GRPTREE_H

#include "fidoconf.h"
#include <huskylib/huskylib.h>
/* if returns 1 - All Ok                                           */
/* if returns 0 - we have duplicate definition for area in config  */
typedef struct
{
    char *   name;
    char *   patternList;
    s_area * area;
} grp_t;
HUSKYEXT tree * groupTree;
HUSKYEXT void initGroupTree();
HUSKYEXT int addPatternToGrpTree(char * grpname, char * patternList);
HUSKYEXT grp_t * findGroupForArea(char * areaName);
HUSKYEXT grp_t * findGroupByName(char * groupName);
HUSKYEXT void freeGrpTree();


#endif
