/* $Id$
 ******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * Copyright (C) 2000-2002
 *
 * Max Levenkov
 * Husky development team
 * http://husky.sourceforge.net/team.html
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
 *
 * See also http://www.gnu.org
 *****************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include <huskylib/huskylib.h>

#ifdef HAS_STRINGS_H
#   include <strings.h>
#endif /* HAS_STRINGS_H */

#if defined (__OS2__)
# define INCL_DOSFILEMGR
# include <os2.h>
#endif

/* export functions from DLL */
#define DLLEXPORT
#include <huskylib/huskyext.h>

#include "fidoconf.h"
#include "common.h"

#define setcond for (i=0, condition=1; i<=iflevel; condition=ifstack[i++].state && condition);

static char *curconfname=NULL;
static long curconfpos=0;
static FILE *hcfg=NULL;
static short condition;
static int  iflevel, nvars, sp;
static int  maxnvars, maxsp, maxif;
static struct { short state, inelse, wastrue;
              } *ifstack=NULL;
static struct { char *var, *value;
              } *set=NULL;
static struct {
        FILE *farr;
        int  curline;
        char *confname;
      } *incstack=NULL;

static unsigned int cfgNamesCount;
static char **cfgNames=NULL;

int init_conf(const char *conf_name)
{
  if( conf_name==NULL || conf_name[0]==0 )
  {
    w_log(LL_ERR, __FILE__ "::init_conf(): config name %s", conf_name?"has null length":"is NULL pointer");
    return -1;
  }
  iflevel=-1;
  condition=1;
  sp=0;
  cfgNamesCount=0;
  hcfg=fopen(conf_name, "rb");
  if (hcfg==NULL)
  {
    fprintf(stderr, "Can't open config file %s: %s!\n",
            conf_name, strerror(errno));
    wasError = 1;
    return -1;
  }
  curconfname=sstrdup(conf_name);
  actualLineNr=0;
#if defined(__UNIX__)
  setvar("OS", "UNIX");
#elif defined(__OS2__)
  setvar("OS", "OS/2");
#elif defined(__NT__)
  setvar("OS", "WIN");
#elif defined(__DOS__)
  setvar("OS", "MSDOS");
#endif
  setvar("[", "[");
  setvar("`", "`");
  /* Reinit CommentChar to the default value */
  CommentChar='#';
  return 0;
}

char *getvar(char *name)
{ int i;

  for (i=0; i<nvars; i++)
    if (sstricmp(name, set[i].var)==0)
    { if (set[i].value[0]==0)
        return NULL;
      return set[i].value;
    }
  return getenv(name);
}

void setvar(char *name, char *value)
{ int i, j;

  /* find var */
  for (i=0; i<nvars; i++)
    if (sstricmp(set[i].var, name)==0)
      break;
  if (i<nvars)
  { /* remove var */
    nfree(set[i].var);
    for (j=i; j<nvars-1; j++)
    { set[j].var=set[j+1].var;
      set[j].value=set[j+1].value;
    }
    nvars--;
  }
  if (value==NULL) value="";
  if (value[0]==0)
    if (getvar(value)==NULL)
      return;
  if (nvars==maxnvars)
    set = srealloc(set, (maxnvars+=10)*sizeof(*set));
  set[nvars].var=smalloc(sstrlen(name)+sstrlen(value)+2);
  sstrcpy(set[nvars].var, name);
  set[nvars].value=set[nvars].var+sstrlen(name)+1;
  sstrcpy(set[nvars].value, value);
  nvars++;
  return;
}

void free_vars(void)
{
  int i;
  for(i=0; i<nvars; i++)
    nfree(set[i].var);
  maxnvars=nvars=0;
  nfree(set);
}
void close_conf(void)
{
  int i;
  char *module;

  module = getvar("module");
  if (module) module = sstrdup(module);
  free_vars();
  if (module)
  { setvar("module", module);
    nfree(module);
  }
  nfree(ifstack);
  maxif=0;
  if (hcfg) fclose(hcfg);
  hcfg=NULL;
  for (i=0; i<sp; i++) {
    fclose(incstack[i].farr);
    nfree(incstack[i].confname);
  }
  nfree(curconfname);
  nfree(incstack);
  sp=maxsp=0;
  for (i=0; i<(int)cfgNamesCount; i++) nfree(cfgNames[i]);
  nfree(cfgNames);
  cfgNamesCount=0;
}

static char *_configline(void)
{
  char *line;

  curconfpos = ftell(hcfg);
  line = readLine(hcfg);
  if (line == NULL)
    return NULL;
  actualLineNr++;
  return line;
}

char *vars_expand(char *line)
{
  int  curlen;
  char *parsed, *src, *dest, *p, *p1, *newparsed;
#if defined(__UNIX__) || (defined(__OS2__) && defined(__EMX__))
  FILE *f;
  int  i;
#endif

#if defined(__UNIX__) || (defined(__OS2__) && defined(__EMX__))
  if (strpbrk(line, "[`")==NULL)
#else
  if (strchr(line, '[')==NULL)
#endif
     return line;
  curlen = sstrlen(line)+1;
  parsed = dest = smalloc(curlen);
  for (src = line; *src; src++)
  {
    if (dest-parsed >= curlen-2)
    {
      size_t offset = (size_t) (dest - parsed);
             /* we need this to fake around boundary checking */

      newparsed = srealloc(parsed, curlen+=80);
      dest = newparsed + offset;
      parsed = newparsed;
    }
    switch (*src)
    {
#if defined(__UNIX__) || (defined(__OS2__) && defined(__EMX__))
      case '`':
        p = strchr(src+1, '`');
        if (p == NULL)
        {
          *dest++ = *src;
          continue;
        }
        *p = '\0';
	src++;
	f = popen(src, "r");
        *p = '`';
        src = p;
        while ((i = fgetc(f)) != EOF)
        {
          if (dest-parsed >= curlen-2)
          {
            newparsed = srealloc(parsed, curlen+=80);
            dest = newparsed+(unsigned)(dest-parsed);
            parsed = newparsed;
          }
          if (i!='\n') *dest++ = (char)i;
        }
        pclose(f);
        continue;
#endif
      case '[':
        p = strchr(src, ']');
        if (p)
        {
          src++;
          *p = '\0';
          if ((p1 = getvar(src)))
          {
            if (sstrlen(p1) > sstrlen(src)+2)
            {
              newparsed = srealloc(parsed, curlen += sstrlen(p1)-sstrlen(src)-2);
              dest = newparsed+(unsigned)(dest-parsed);
              parsed = newparsed;
            }
            sstrcpy(dest, p1);
            dest += sstrlen(p1);
          }
          *p = ']';
          src = p;
          continue;
        }
      default:
        *dest++ = *src;
        continue;
    }
  }
  *dest++ = '\0';
  if (curlen != dest-parsed)
    parsed = srealloc(parsed, (unsigned)(dest-parsed));
  nfree(line);
  return parsed;
}

static short boolexpr(char *str)
{ char *p, *p1, *p2;
  short ret, inquote, relax;

  ret=1;
  for (p=str; isspace(*p); p++);
  if (strncasecmp(p, "not ", 4)==0)
  { ret=0;
    for (p+=4; isspace(*p); p++);
  }
  inquote=0;
  for (p1=p; *p1; p1++)
  {
    if (p1[0]=='\\' && (p1[1]=='\\' || p1[1]=='\"'))
    { p1++;
      continue;
    }
    if (*p1=='\"')
    { inquote = !inquote;
      continue;
    }
    if (!inquote)
      if ((p1[0] == '=' || p1[0] == '!') && (p1[1] == '=' || p1[1] == '~'))
        break;
  }
  if (*p1==0)
  { fprintf(stderr, "Bad if expression in config %s, line %d: '%s'\n",
            curconfname, actualLineNr, str);
    wasError = 1;
    return ret;
  }
  if (p1[0]=='!') ret=!ret;
  relax=(p1[1]=='~');
  *p1=0;
  for (p2=p1-1; isspace(*p2); *p2--=0);
  for (p1+=2; isspace(*p1); p1++);
  for (p2=p1+sstrlen(p1)-1; isspace(*p2); *p2--=0);
  if (relax ? patimat(p, p1) : sstricmp(p, p1))
    ret=!ret;
  return ret;
}

char *configline(void)
{ int  i;
  char *p, *p1, *p2, *str, *line=NULL;

  for (;;) {
    nfree(line);
    line=str=_configline();
    if (str==NULL) {
       /*  save parsed config name */
       cfgNames = srealloc(cfgNames, sizeof(char*)*(cfgNamesCount+1));
       cfgNames[cfgNamesCount] = NULL;
       xstrcat(&cfgNames[cfgNamesCount], curconfname);
       cfgNamesCount++;
       if (sp) {
          fclose(hcfg);
          nfree(curconfname);
          hcfg=incstack[--sp].farr;
          actualLineNr=incstack[sp].curline;
          curconfname=incstack[sp].confname;
          continue;
       }
       return NULL;
    }
    while (*str && isspace(*str)) str++;
    stripComment(str);
    if (strncasecmp(str, "if ", 3)==0)
    {
      p=vars_expand(line); str+=(p-line); line=p;
      iflevel++;
      if (iflevel==maxif)
        ifstack=srealloc(ifstack, (maxif+=10)*sizeof(*ifstack));
      ifstack[iflevel].inelse=0;
      ifstack[iflevel].state=ifstack[iflevel].wastrue=boolexpr(str+3);
      condition = condition && ifstack[iflevel].state;
      continue;
    }
    if ((strncasecmp(str, "ifdef ",  6)==0) ||
        (strncasecmp(str, "ifndef ", 7)==0))
    {
      p=vars_expand(line); str+=(p-line); line=p;
      for (p1=str+sstrlen(str)-1; isspace(*p1); *p1--='\0');
      for (p=str+6; isspace(*p); p++);
      if (*p=='\0')
      { fprintf(stderr, "Bad %s in config %s line %d!\n",
                str, curconfname, actualLineNr);
        wasError = 1;
        continue;
      }
      iflevel++;
      if (iflevel==maxif)
        ifstack=srealloc(ifstack, (maxif+=10)*sizeof(*ifstack));
      ifstack[iflevel].inelse=0;
      ifstack[iflevel].state=(getvar(p)!=NULL);
      if (tolower(str[2])=='n') /* ifndef */
        ifstack[iflevel].state=!ifstack[iflevel].state;
      ifstack[iflevel].wastrue=ifstack[iflevel].state;
      condition = condition && ifstack[iflevel].state;
      continue;
    }
    if (strncasecmp(str, "elseif ", 7)==0 || strncasecmp(str, "elif ", 5) == 0)
    {
      if ((iflevel==-1) || ifstack[iflevel].inelse)
      { fprintf(stderr, "Misplaces elseif in config %s line %d ignored!\n",
                curconfname, actualLineNr);
        wasError = 1;
        continue;
      }
      p=vars_expand(line); str+=(p-line); line=p;
      if (ifstack[iflevel].wastrue)
        ifstack[iflevel].state=0;
      else
        ifstack[iflevel].state=ifstack[iflevel].wastrue=boolexpr(strchr(str, ' '));
      setcond;
      continue;
    }
    if (strncasecmp(str, "else", 4)==0)
    {
      if ((iflevel==-1) || ifstack[iflevel].inelse)
      { fprintf(stderr, "Misplaces else in config %s line %d ignored!\n",
                curconfname, actualLineNr);
        wasError = 1;
        continue;
      }
      ifstack[iflevel].inelse=1;
      ifstack[iflevel].state=!ifstack[iflevel].wastrue;
      setcond;
      continue;
    }
    if (strncasecmp(str, "endif", 5)==0)
    {
      if (iflevel==-1)
      { fprintf(stderr, "Misplaced endif in config %s line %d ignored!\n",
                curconfname, actualLineNr);
        wasError = 1;
        continue;
      }
      iflevel--;
      setcond;
      continue;
    }
    if (!condition)
      continue;
    if (strncasecmp(str, "set ", 4)==0)
    {
      p=vars_expand(line); str+=(p-line); line=p;
      p=strchr(str, '\n');
      if (p) *p=0;
      p1=strchr(str+4, '=');
      if (p1==NULL)
      { fprintf(stderr, "Incorrect set in config %s line %d!\n",
                curconfname, actualLineNr);
        wasError = 1;
        continue;
      }
      *p1=0;
      for (p=p1-1; isspace(*p); *p--='\0');
      for (p=str+4; isspace(*p); p++);
      /* now p - name of var */
      for (p1++; isspace(*p1); p1++);
      if (*p1=='\"')
      { /* remove quote chars */
        for (p2=p1; (p2=strchr(p2+1, '\"'))!=NULL;)
          if (*(p2-1)!='\\')
            *p2--='\0';
        p1++;
      }
      setvar(p, p1);
      continue;
    }
    if (strncasecmp(str, "include", 7)==0)
    {
      p=vars_expand(line); str+=(p-line); line=p;
      for (p=str+7; (*p==' ') || (*p=='\t'); p++);
      for (p1=p+sstrlen(p)-1; isspace(*p1); *p1--=0);
      for (i=0; i<sp; i++)
        if (sstrcmp(incstack[i].confname, p) == 0)
        { fprintf(stderr, "Line %d: WARNING: recursive include of file %s detected and fixed!\n", actualLineNr, p);
          continue;
        }
      if (sp==maxsp)
        incstack=srealloc(incstack, (maxsp+=10)*sizeof(*incstack));
      incstack[sp].farr=hcfg;
      hcfg=fopen(p, "rb");
      if (hcfg==NULL)
      { fprintf(stderr, "Can't open include file %s: %s!\n", p, strerror(errno));
        hcfg=incstack[sp].farr;
        wasError = 1;
        continue;
      }
      incstack[sp].confname=curconfname;
      incstack[sp].curline=actualLineNr;
      sp++;
      curconfname=sstrdup(p);
      actualLineNr=0;
      continue;
    }
    if ((strncasecmp(str, "commentchar", 11) == 0) && isspace(str[11]))
    {
      for (p=str+11; isspace(*p); p++);
      if (!*p)
      { printf("\"%s\", line %d: There is a comment character missing after CommentChar!\n", curconfname, actualLineNr);
        continue;
      }
      if (!strchr(TRUE_COMMENT, *p))
      { printf("\"%s\", line %d: CommentChar - '%c' is not valid comment characters!\n", curconfname, actualLineNr, *p);
      } else
      { CommentChar = *p;
      }
      continue;
    }
    return line;
  }
}

void checkIncludeLogic(ps_fidoconfig config)
{
    UINT i, j;

    for (j=0; j<config->linkCount; j++) {
        if (config->links[j]->areafix.autoCreateFile==NULL) continue;
        for (i=0; i<cfgNamesCount; i++) {
            if (cmpfnames(cfgNames[i],config->links[j]->areafix.autoCreateFile)==0)
                break;
        }
        /*  if not found include file - return error */
        if (i==cfgNamesCount) {
            printf("areafix autoCreateFile %s has never been included in config!\n",
                config->links[j]->areafix.autoCreateFile);
            exit(EX_CONFIG);
        }
    }

    for (j=0; j<config->linkCount; j++) {
        if (config->links[j]->filefix.autoCreateFile==NULL) continue;
        for (i=0; i<cfgNamesCount; i++) {
            if (cmpfnames(cfgNames[i],config->links[j]->filefix.autoCreateFile)==0) break;
        }
        /*  if not found include file - return error */
        if (i==cfgNamesCount) {
            printf("filefix autoCreateFile %s has never been included in config!\n",
                config->links[j]->filefix.autoCreateFile);
            exit(EX_CONFIG);
        }
    }
    /* check for duplicate includes */
    for( i = 0; i < cfgNamesCount - 1; i++ )
        for ( j = i+1; j < cfgNamesCount;  j++ )
            if (cmpfnames(cfgNames[i],cfgNames[j])==0)
            {
                printf("File %s is included in config more then one time!\n",cfgNames[i]);
                exit(EX_CONFIG);
            }

}

const char* getCurConfName()
{
    return curconfname;
}

long getCurConfPos()
{
    return curconfpos;
}

long get_hcfgPos()
{
    return ftell(hcfg);
}

FILE *get_hcfg()
{
    return hcfg;
}
