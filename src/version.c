/* $Id$ */
/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * Copyright (C) 1998-2002
 *
 * Husky Delopment Team
 *
 * Internet: http://husky.sourceforge.net
 *
 * This file is part of FIDOCONFIG.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library/Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; see file COPYING. If not, write to the Free
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * See also http://www.gnu.org
 *****************************************************************************
 */
#include <string.h>
#include <huskylib/huskylib.h>
/* export functions from DLL */
#define DLLEXPORT
#include <huskylib/huskyext.h>

#include "common.h"

#define __VERSION__C__

#include "version.h"
/* Check version of fidoconfig library
 * return zero if test failed; non-zero if passed
 * test cvs need for DLL version only, using #include <fidoconf/cvsdate.h>
 */
HUSKYEXT int CheckFidoconfigVersion(int need_major,
                                    int need_minor,
                                    int need_patch,
                                    branch_t need_branch,
                                    const char * cvs)
{
    /* We don't need check pathlevel: see huskybse/develop-docs/ */
    static
#include "../cvsdate.h"   /* char cvs_date[]=datestring; */

    if(need_major == FC_VER_MAJOR && need_minor == FC_VER_MINOR)
    {
        if(need_branch == BRANCH_CURRENT)
        {
            if(need_patch)
            {
                fprintf(stderr, "Fidoconfig: strange, current patch level can't be non-zero\n");
            }

            return (FC_VER_BRANCH == BRANCH_CURRENT) && !(cvs && strcmp(cvs, cvs_date));
        }
        else
        {
            return FC_VER_BRANCH != BRANCH_CURRENT;
        }
    }

    return 0;
}

#ifdef TEST_VERSION_C

#include <stdio.h>
#include <stdlib.h>
#include "../cvsdate.h"

int main()
{
    char * versionStr;

    versionStr = GenVersionStr("fidoconfig", 1, 3, 0, BRANCH_RELEASE, cvs_date);
    printf("RELEASE: %s\n\n", versionStr);
    nfree(versionStr);
    versionStr = GenVersionStr("fidoconfig", 1, 2, 1, BRANCH_RELEASE, cvs_date);
    printf("RELEASE: %s\n\n", versionStr);
    nfree(versionStr);
    versionStr = GenVersionStr("fidoconfig", 1, 2, 6, BRANCH_CURRENT, cvs_date);
    printf("CURRENT: %s\n\n", versionStr);
    nfree(versionStr);
    versionStr = GenVersionStr("fidoconfig", 1, 3, 0, BRANCH_CURRENT, cvs_date);
    printf("CURRENT: %s\n\n", versionStr);
    nfree(versionStr);
    versionStr = GenVersionStr("fidoconfig", 1, 3, 4, BRANCH_STABLE, cvs_date);
    printf("STABLE: %s\n\n", versionStr);
    nfree(versionStr);
    versionStr = GenVersionStr("fidoconfig", 1, 4, 0, BRANCH_STABLE, cvs_date);
    printf("STABLE: %s\n\n", versionStr);
    nfree(versionStr);
    return 0;
}

#endif /* ifdef TEST_VERSION_C */
