/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * Copyright (C) 1998-1999
 *  
 * Matthias Tichy
 *
 * Fido:     2:2433/1245 2:2433/1247 2:2432/605.14
 * Internet: mtt@tichy.de
 *
 * Grimmestr. 12         Buchholzer Weg 4
 * 33098 Paderborn       40472 Duesseldorf
 * Germany               Germany
 *
 * This file is part of FIDOCONFIG.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
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
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#if !defined(MSDOS) || defined(__DJGPP__)
#include "fidoconfig.h"
#else
#include "fidoconf.h"
#endif

int syntax()
{
    printf("dumpfcfg - dump fidoconfig to file or stdout\n");
    printf("Syntax: dumpfcfg [OutFile]\n");
    return 1;
}

int main(int argc, char *argv[])
{
    FILE *outf;
    s_fidoconfig *config;

    if (argc > 1)
    {
	if (strcmp(argv[1], "-h") == 0) return syntax();
	if (strcmp(argv[1], "--help") == 0) return syntax();

	outf = fopen(argv[1], "w");
	if (outf == NULL)
	{
	    printf("could not open '%s' for writing!", argv[1]);
	    return 5;
	}
    }
    else
    {
	outf = stdout;
    }

    config = readConfig();
    dumpConfig(config, outf);

    if (outf != stdout) fclose(outf);

    return 0;
}

