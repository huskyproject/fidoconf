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
#include <string.h>

#include "fidoconf.h"

int syntax(void)
{
    printf("dumpfcfg - dump fidoconfig to file or stdout\n");
    printf("Syntax: dumpfcfg [OutFile]\n");
    return 1;
}

int main(int argc, char *argv[])
{
    FILE *outf = NULL;
    s_fidoconfig *config;
    int i;

    for (i=1; i<argc; i++)
    {
       if (argv[i][0]=='-' && argv[i][1]=='D') {
           char *p=strchr(argv[i], '=');
           if (p) {
               *p='\0';
               setvar(argv[i]+2, p+1);
               *p='=';
           } else {
               setvar(argv[i]+2, "");
           }
       }
       else if (strcmp(argv[i], "-h") == 0) return syntax();
       else if (strcmp(argv[i], "--help") == 0) return syntax();
       else if (outf == NULL) {
	   outf = fopen(argv[i], "w");
	   if (outf == NULL)
	   {
	       printf("could not open '%s' for writing!", argv[i]);
	       return 5;
	   }
       } else {
	   return syntax();
       }
    }

    if (outf == NULL)
	outf = stdout;

    config = readConfig(NULL);
    dumpConfig(config, outf);

    if (outf != stdout) fclose(outf);

    return 0;
}

