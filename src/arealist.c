/*****************************************************************************
 * AreaFix for HPT (FTN NetMail/EchoMail Tosser)
 *****************************************************************************
 * Copyright (C) 2000
 *
 * Lev Serebryakov
 *
 * Fido:     2:5030/661
 * Internet: lev@serebryakov.spb.ru
 * St.Petersburg, Russia
 *
 * This file is part of HPT.
 *
 * HPT is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * HPT is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPT; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************************
 * $Id$
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>


#include <huskylib/compiler.h>

#ifdef HAS_UNISTD_H
#   include <unistd.h>
#endif
/* export functions from DLL */
#define DLLEXPORT
#include <huskylib/huskyext.h>

#include "arealist.h"
#include "common.h"

#define LIST_PAGE_SIZE 256

static s_fidoconfig * config;  /*  extern? see fidoconf.c */
ps_arealist newAreaList(s_fidoconfig * cfg)
{
    ps_arealist al;

    config = cfg;              /* This may cause problem if somewhere release memory... */

    if(NULL == (al = malloc(sizeof(s_arealist))))
    {
        return NULL;
    }

    al->areas    = NULL;
    al->count    = 0;
    al->maxcount = LIST_PAGE_SIZE;

    if(NULL == (al->areas = malloc(al->maxcount * sizeof(s_arealistitem))))
    {
        nfree(al);
        return NULL;
    }

    return al;
}

void freeAreaList(ps_arealist al)
{
    int i;

    if(al)
    {
        if(al->areas && al->maxcount)
        {
            for(i = 0; i < al->count; i++)
            {
                nfree(al->areas[i].tag);
                nfree(al->areas[i].desc);
                nfree(al->areas[i].grp);
            }
            nfree(al->areas);
        }

        nfree(al);
    }

    return;
}

int addAreaListItem(ps_arealist al,
                    int active,
                    int rescanable,
                    int import,
                    int aexport,
                    int mandatory,
                    char * tag,
                    char * desc,
                    char * grp)
{
    ps_arealistitem areas;
    int l;

    if(al->count == al->maxcount)
    {
        if(NULL ==
           (areas = realloc(al->areas, (al->maxcount + LIST_PAGE_SIZE) * sizeof(s_arealistitem))))
        {
            return 1;
        }

        al->areas     = areas;
        al->maxcount += LIST_PAGE_SIZE;
    }

    al->areas[al->count].active     = active;
    al->areas[al->count].rescanable = rescanable ? 2 : 0;
    al->areas[al->count].readonly   = import ? 0 : 3;
    al->areas[al->count].writeonly  = aexport ? 0 : 4;
    al->areas[al->count].fullaccess = (aexport && import) ? 5 : 0;
    al->areas[al->count].mandatory  = mandatory ? 6 : 0;
    al->areas[al->count].tag        = sstrdup(tag);
    al->areas[al->count].grp        = sstrdup(grp ? grp : "");

    if(desc)
    {
        l = (int)strlen(desc);
        al->areas[al->count].desc = smalloc(l + 3);

        if('"' == desc[0] && '"' == desc[l - 1])
        {
            strcpy(al->areas[al->count].desc, desc);
        }
        else
        {
            al->areas[al->count].desc[0] = '"';
            strcpy(&al->areas[al->count].desc[1], desc);
            al->areas[al->count].desc[l + 1] = '"';
            al->areas[al->count].desc[l + 2] = '\x00';
        }
    }
    else
    {
        al->areas[al->count].desc = NULL;
    }

    al->count++;
    return 0;
} /* addAreaListItem */

static int compare_bytag(const void * a, const void * b)
{
    return sstricmp(((ps_arealistitem)a)->tag, ((ps_arealistitem)b)->tag);
}

static int compare_bygrp(const void * a, const void * b)
{
    return strcmp(((ps_arealistitem)a)->grp, ((ps_arealistitem)b)->grp);
}

static int compare_bygrptag(const void * a, const void * b)
{
    register int r = strcmp(((ps_arealistitem)a)->grp, ((ps_arealistitem)b)->grp);

    return r ? r : sstricmp(((ps_arealistitem)a)->tag, ((ps_arealistitem)b)->tag);
}

HUSKYEXT void sortAreaList(ps_arealist al)
{
    if(config && al && al->count && al->areas)
    {
        switch(config->listEcho)
        {
            case lemGroupName:
                qsort(al->areas, al->count, sizeof(s_arealistitem), compare_bygrptag);
                break;

            case lemGroup:
                qsort(al->areas, al->count, sizeof(s_arealistitem), compare_bygrp);
                break;

            case lemUnsorted:
                break;

            case lemName:
            default:
                qsort(al->areas, al->count, sizeof(s_arealistitem), compare_bytag);
                break;
        }
    }
}

static int compare_arealistitems_and_desc(const void * a, const void * b)
{
    register int r;

    /* compare areatags */
    r = sstricmp(((ps_arealistitem)a)->tag, ((ps_arealistitem)b)->tag);

    if(r != 0)
    {
        return r;
    }

    /* comapre descriptions: if both presents it's eq; if both absence then eq also;
       else NULL'ed is little */
    r = (((ps_arealistitem)b)->desc != NULL) - (((ps_arealistitem)a)->desc != NULL);
    return r;
/* Don't compare descriptions text
   if ((r!=0)||(((ps_arealistitem)a)->desc == NULL))
    return r;

   return strcmp(((ps_arealistitem)a)->desc, ((ps_arealistitem)b)->desc);
 */
}

HUSKYEXT void sortAreaListNoDupes(unsigned int halcnt, ps_arealist * hal, int nodupes)
{
    int i, j;
    unsigned int k;
    char * prev;
    ps_arealist al;
    ps_arealistitem ali;

    if(!hal)
    {
        return;
    }

    al = hal[halcnt - 1];

    if(!(al && al->count && al->areas))
    {
        return;
    }

    if(!nodupes)
    {
        sortAreaList(al);
        return;
    }

    qsort(al->areas, al->count, sizeof(s_arealistitem), compare_arealistitems_and_desc);
    j    = 0;
    prev = NULL;

    for(i = 0; i < al->count; i++)
    {
        if(prev && (sstricmp(prev, al->areas[i].tag) == 0))
        {
            nfree(al->areas[i].tag);
            nfree(al->areas[i].desc);
            continue;
        }

        prev = al->areas[i].tag;
        ali  = NULL;

        for(k = 1; k < halcnt; k++)
        {
            ali =
                bsearch(&(al->areas[i]),
                        hal[k - 1]->areas,
                        hal[k - 1]->count,
                        sizeof(s_arealistitem),
                        compare_bytag);

            if(ali)
            {
                break;
            }
        }

        if(ali)
        {
            prev = NULL;
            nfree(al->areas[i].tag);
            nfree(al->areas[i].desc);
            continue;
        }

        if(i != j)
        {
            memcpy(&(al->areas[j]), &(al->areas[i]), sizeof(s_arealistitem));
        }

        j++;
    }

    if(j != (al->maxcount))
    {
        al->areas    = realloc(al->areas, j * sizeof(s_arealistitem));
        al->maxcount = j;
    }

    al->count = j;
} /* sortAreaListNoDupes */

static char * addline(char * text, char * line, int * pos, int * tlen)
{
    int ll;

    if(!text)
    {
        return NULL;
    }

    if(!line)
    {
        return text;
    }

    ll = (int)strlen(line);

    if(*pos + ll + 1 > *tlen)
    {
        *tlen += 1024;

        if(NULL == (text = realloc(text, *tlen)))
        {
            return NULL;
        }
    }

    strcpy(&text[*pos], line);
    *pos += ll;
    return text;
} /* addline */

static char * addchars(char * text, char c, int count, int * pos, int * tlen)
{
    int i;

    if(!text)
    {
        return NULL;
    }

    if(*pos + count + 1 > *tlen)
    {
        *tlen += count + 1024;

        if(NULL == (text = realloc(text, *tlen)))
        {
            return NULL;
        }
    }

    for(i = *pos; i < *pos + count; i++)
    {
        text[i] = c;
    }
    text[i] = '\x00';
    *pos   += count;
    return text;
} /* addchars */

static char * find_grpdesc(char * grp)
{
    register unsigned int i;
    char * ddef = NULL;

    if(*grp == 0)
    {
        return NULL;
    }

    if(config)
    {
        for(i = 0; i < config->groupCount; i++)
        {
            if(strcmp(grp, config->group[i].name) == 0)
            {
                return config->group[i].desc;
            }
            else if(*config->group[i].name == '*')
            {
                ddef = config->group[i].desc;
            }
        }
    }

    if(ddef)
    {
        return ddef;
    }
    else
    {
        return "*** Other areas" /* NULL*/;
    }
} /* find_grpdesc */

HUSKYEXT char * formatAreaList(ps_arealist al, int maxlen, char * activechars, int grps)
{
    char * text;
    char * p;
    char * cgrp = NULL, * cgrpdesc = NULL;
    int i;
    int clen, wlen;
    int tlen;
    int tpos = 0;

    if(!al || !al->count || !al->areas)
    {
        return NULL;
    }

    tlen = al->count * (maxlen + 5);

    if(NULL == (text = malloc(tlen)))
    {
        return NULL;
    }

    text[tpos] = '\x00';

    for(i = 0; i < al->count; i++)
    {
        clen = 0;

        if(tpos >= tlen)
        {
            tlen += (maxlen + 5) * 32;

            if(NULL == (text = realloc(text, tlen)))
            {
                return NULL;
            }
        }

        /* val: add group description */
        if(grps && (!cgrp || strcmp(cgrp, al->areas[i].grp) != 0))
        {
            char * dgrp = find_grpdesc(al->areas[i].grp);

            if(dgrp && dgrp != cgrpdesc)
            {
                if(cgrp)
                {
                    text[tpos++] = '\r';
                    text[tpos]   = 0;
                }

                if((text = addline(text, dgrp, &tpos, &tlen)) == NULL)
                {
                    return NULL;
                }

                text[tpos++] = '\r';
                text[tpos++] = '\r';
                text[tpos]   = 0;
                cgrpdesc     = dgrp;
            }

            cgrp = al->areas[i].grp;
        }

        if(activechars)
        {
            text[tpos++] = activechars[al->areas[i].active];
            text[tpos++] = activechars[al->areas[i].rescanable];

            if(al->areas[i].fullaccess)
            {
                text[tpos++] = activechars[al->areas[i].fullaccess];
            }
            else
            {
                if(al->areas[i].readonly)
                {
                    text[tpos++] = activechars[al->areas[i].readonly];
                }
                else
                {
                    text[tpos++] = activechars[al->areas[i].writeonly];
                }
            }

            text[tpos++] = activechars[al->areas[i].mandatory];
            clen++;
        }

        text[tpos++] = ' ';
        clen++;
        text[tpos] = '\x00';

        if(NULL == (text = addline(text, al->areas[i].tag, &tpos, &tlen)))
        {
            return NULL;
        }

        /* Not add description */
        if(!al->areas[i].desc)
        {
            text[tpos++] = '\r';
            text[tpos]   = '\x00';
            continue;
        }

        clen += (int)strlen(al->areas[i].tag);
        wlen  = (int)strlen(al->areas[i].desc);

        if(clen + 5 + wlen <= maxlen)
        {
            text[tpos++] = ' ';
            text[tpos]   = '\x00';

            if(NULL == (text = addchars(text, '.', maxlen - (clen + 4 + wlen), &tpos, &tlen)))
            {
                return NULL;
            }

            text[tpos++] = ' ';
            text[tpos]   = '\x00';

            if(NULL == (text = addline(text, al->areas[i].desc, &tpos, &tlen)))
            {
                return NULL;
            }
        }
        else
        {
            p = strchr(al->areas[i].desc, ' ');

            if(p && (p - al->areas[i].desc) + clen + 5 <= maxlen)
            {
                wlen         = (int)(p - al->areas[i].desc);
                *p           = '\x00';
                text[tpos++] = ' ';
                text[tpos]   = '\x00';

                if(NULL == (text = addchars(text, '.', maxlen - (clen + 4 + wlen), &tpos, &tlen)))
                {
                    *p = ' ';
                    return NULL;
                }

                text[tpos++] = ' ';
                text[tpos]   = '\x00';

                if(NULL == (text = addline(text, al->areas[i].desc, &tpos, &tlen)))
                {
                    *p = ' ';
                    return NULL;
                }

                wlen         = (int)strlen(p + 1);
                text[tpos++] = '\r';
                text[tpos]   = '\x00';

                if(NULL ==
                   (text =
                        addline(addchars(text, ' ', maxlen - wlen, &tpos, &tlen), p + 1, &tpos,
                                &tlen)))
                {
                    *p = ' ';
                    return NULL;
                }

                *p = ' ';
            }
            else
            {
                text[tpos++] = '\r';
                text[tpos]   = '\x00';

                if(NULL ==
                   (text =
                        addline(addchars(text, ' ', maxlen - wlen, &tpos, &tlen),
                                al->areas[i].desc, &tpos,
                                &tlen)))
                {
                    return NULL;
                }
            }
        }

        text[tpos++] = '\r';
        text[tpos]   = '\x00';
    }
    return text;
} /* formatAreaList */
