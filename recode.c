/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * recode.c : charsets translating routines
 *
 * Compiled from hpt/recode
 * by Stas Degteff <g@grumbler.org>, 2:5080/102@fidonet
 *
 * Code taken from ifmail written by Eugene G. Crosser <crosser@pccross.msk.su>
 * Ported to HPT by Dmitry Borowskoy <dim@bacup.ru>
 *
 * Portions copyright (C) Matthias Tichy
 *                        Fido:     2:2433/1245 2:2433/1247 2:2432/605.14
 *                        Internet: mtt@tichy.de
 * Portions copyright (C) Max Levenkov
 *                        Fido:     2:5000/117
 *                        Internet: sackett@mail.ru
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
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "recode.h"
#include "log.h"

CHAR *intab  = NULL;

CHAR *outtab = NULL;

VOID initCharsets(VOID)
{
	int i;
	intab	= (CHAR *) smalloc(sizeof(CHAR) * 256);
	outtab	= (CHAR *) smalloc(sizeof(CHAR) * 256);
	for (i = 0; i < 256; i++) intab[i] = outtab[i] = (CHAR) i;
}

VOID doneCharsets(VOID)
{
	nfree(intab);
	nfree(outtab);
}

VOID recodeToInternalCharset(CHAR *string)
{
INT c;

    if( !intab || !outtab ) initCharsets();

    if (string != NULL) {
	for( ; *string != '\000'; string++ )
	    {
	    c=((INT)*string)&0xFF;
        *string = intab[c];
        }
    }

}

VOID recodeToTransportCharset(CHAR *string)
{
INT c;

    if( !intab || !outtab ) initCharsets();

    if (string != NULL) {
	for( ; *string != '\000'; string++ )
	    {
	    c=((INT)*string)&0xFF;
        *string = outtab[c];
        }
    }

}


INT ctoi(CHAR *s)
{
	char *foo;
	unsigned long res = strtoul((char*)s, &foo, 0);
	if (*foo)	/* parse error */
		return 0;
	return (INT)res;
}

void getctab(CHAR *dest, UCHAR *charMapFileName )
{
	FILE *fp;
	UCHAR buf[512],*p,*q;
	int in,on,count;
	INT line;

	if( !intab || !outtab ) initCharsets();

	if ((fp=fopen((char *)charMapFileName,"r")) == NULL)
	 {
		fprintf(stderr,"getctab: cannot open mapchan file \"%s\"\n", charMapFileName);
		return ;
	 }

	count=0;	
	line = 0;
	while (fgets((char*)buf,sizeof(buf),fp))
	{
		line++;
		p=(unsigned char *)strtok((char*)buf," \t\n#");
		q=(unsigned char *)strtok(NULL," \t\n#");

		if (p && q)
		{
#if defined(__WATCOMC__) && defined(__DOS4G__)
			in = ctoi((signed char *)p);
#else
			in = ctoi((char *)p);
#endif
			if (in > 255) {
				fprintf(stderr, "getctab: %s: line %d: char val too big\n", charMapFileName, line);
				break;
			}
#if defined(__WATCOMC__) && defined(__DOS4G__)
			on=ctoi((signed char *)q);
#else
			on=ctoi((char *)q);
#endif
			if (in && on)
                        {
                                if( count++ < 256 ) dest[in]=(char)on;
                                else
                                {
                                        fprintf(stderr,"getctab: char map table \"%s\" is big\n",charMapFileName);
                                        break;
                                }
                        }
		}
	}
	fclose(fp);
	
	w_log('2',"read recoding table from %s", charMapFileName);
	return ;
}
