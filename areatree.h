/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * areatree.h : some functions to make search areas by names faster
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
 * or visit http://www.gnu.org
 *****************************************************************************
 * $Id$
 */
#ifndef	_AREA_TREE_FLAG
#define	_AREA_TREE_FLAG

#include "tree.h"
#include "fidoconf.h"

/* if returns 1 - All Ok                                           */
/* if returns 0 - we have duplicate definition for area in config  */
FCONF_EXT   int      RebuildEchoAreaTree(ps_fidoconfig config);
            void     FreeAreaTree(ps_fidoconfig config);
            ps_area  FindAreaInTree(char* areaName);

#endif 
