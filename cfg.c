#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include "fidoconf.h"
#include "typesize.h"
#include "common.h"
#include "xstr.h"

#define setcond for (i=0, condition=1; i<=iflevel; condition=ifstack[i++].state && condition);

char *curconfname;
long curconfpos;
FILE *hcfg;
static short condition;
static int  iflevel, nvars, sp;
static int  maxnvars, maxsp, maxif;
static struct { short state, inelse, wastrue;
              } *ifstack;
static struct { char *var, *value;
              } *set;
static struct {
        FILE *farr;
        int  curline;
        char *confname;
      } *incstack;

static unsigned int cfgNamesCount;
static char **cfgNames;

int init_conf(char *conf_name)
{
  iflevel=-1;
  condition=1;
  sp=0;
  cfgNamesCount=0;
  hcfg=fopen(conf_name, "r");
  if (hcfg==NULL)
  {
    fprintf(stderr, "Can't open config file %s: %s!\n",
            conf_name, strerror(errno));
    wasError = 1;
    return -1;
  }
  curconfname=sstrdup(conf_name);
  actualLineNr=0;
#if defined(UNIX)
  setvar("OS", "UNIX");
#elif defined(OS2)
  setvar("OS", "OS/2");
#elif defined(NT) || defined(WINNT) || defined(__NT__)
  setvar("OS", "WIN");
#elif defined(MSDOS)
  setvar("OS", "MSDOS");
#endif
  setvar("[", "[");
  setvar("`", "`");
  return 0;
}

char *getvar(char *name)
{ int i;

  for (i=0; i<nvars; i++)
    if (stricmp(name, set[i].var)==0)
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
    if (stricmp(set[i].var, name)==0)
      break;
  if (i<nvars)
  { /* remove var */
    free(set[i].var);
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
  set[nvars].var=smalloc(strlen(name)+strlen(value)+2);
  strcpy(set[nvars].var, name);
  set[nvars].value=set[nvars].var+strlen(name)+1;
  strcpy(set[nvars].value, value);
  nvars++;
  return;
}

void close_conf(void)
{ int i;
  char *module;

  module = getvar("module");
  if (module) module = sstrdup(module);
  for(i=0; i<nvars; i++)
    nfree(set[i].var);
  maxnvars=nvars=0;
  nfree(set);
  if (module)
  { setvar("module", module);
    free(module);
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
  for (i=0; i<cfgNamesCount; i++) nfree(cfgNames[i]);
  nfree(cfgNames);
  cfgNamesCount=0;
}

static char *_configline(void)
{
  char *parsed, *src, *dest, *p, *p1, *newparsed, *line;
  int  curlen;
#if defined(UNIX) || (defined(OS2) && defined(__EMX__))
  FILE *f;
  int  i;
#endif

  curconfpos = ftell(hcfg);
  line = readLine(hcfg);
  if (line == NULL)
    return NULL;
  actualLineNr++;
  curlen = strlen(line)+1;
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
#if defined(UNIX) || (defined(OS2) && defined(__EMX__))
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
          if ((p1 = getvar(src)) == NULL)
            p1 = src;
          if (strlen(p1) > strlen(src)+2)
          {
            newparsed = srealloc(parsed, curlen += strlen(p1)-strlen(src)-2);
            dest = newparsed+(unsigned)(dest-parsed);
            parsed = newparsed;
          }
          strcpy(dest, p1);
          dest += strlen(p1);
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
  free(line);
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
  { if (*p1=='\"')
    { if (*(p1-1)=='\\')
        continue;
      inquote =! inquote;
      continue;
    }
    if (!inquote)
      if (strncmp(p1, "==", 2)==0 || strncmp(p1, "=~", 2)==0)
        break;
  }
  if (*p1==0)
  { fprintf(stderr, "Bad if expression in config %s, line %d: '%s'\n",
            curconfname, actualLineNr, str);
    wasError = 1;
    return ret;
  }
  relax=(p1[1]=='~');
  *p1=0;
  for (p2=p1-1; isspace(*p2); *p2--=0);
  for (p1+=2; isspace(*p1); p1++);
  for (p2=p1+strlen(p1)-1; isspace(*p2); *p2--=0);
  if (relax ? patimat(p, p1) : stricmp(p, p1))
    ret=!ret;
  return ret;
}

char *configline(void)
{ int  i;
  char *p, *p1, *p2, *str, *line;

  for (;;free(line)) {
      line=str=_configline();
      if (str==NULL) {
	  // save parsed config name
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
    if (strncasecmp(str, "if ", 3)==0)
    {
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
      for (p1=str+strlen(str)-1; isspace(*p1); *p1--='\0');
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
    if (strncasecmp(str, "elseif ", 7)==0)
    {
      if ((iflevel==-1) || ifstack[iflevel].inelse)
      { fprintf(stderr, "Misplaces elseif in config %s line %d ignored!\n",
                curconfname, actualLineNr);
        wasError = 1;
        continue;
      }
      if (ifstack[iflevel].wastrue)
        ifstack[iflevel].state=0;
      else
        ifstack[iflevel].state=ifstack[iflevel].wastrue=boolexpr(str+6);
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
      for (p=str+7; (*p==' ') || (*p=='\t'); p++);
      for (p1=p+strlen(p)-1; isspace(*p1); *p1--=0);
      for (i=0; i<sp; i++)
        if (strcmp(incstack[i].confname, p) == 0)
        { fprintf(stderr, "Line %d: WARNING: recursive include of file %s detected and fixed!\n", actualLineNr, p);
          continue;
        }
      if (sp==maxsp)
        incstack=srealloc(incstack, (maxsp+=10)*sizeof(*incstack));
      incstack[sp].farr=hcfg;
      hcfg=fopen(p, "r");
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
    return line;
  }
}

void checkIncludeLogic(ps_fidoconfig config)
{ 
    int i, j;

    for (j=0; j<config->linkCount; j++) {
	if (config->links[j].autoAreaCreateFile==NULL) continue;
	for (i=0; i<cfgNamesCount; i++) {
	    if (strcmp(cfgNames[i],config->links[j].autoAreaCreateFile)==0) break;
	}
	// if not found include file - return error
	if (i==cfgNamesCount) {
	    printf("AutoAreaCreateFile %s has never been included in config!\n",
		   config->links[j].autoAreaCreateFile);
	    exit(EX_CONFIG);
	}
    }

    for (j=0; j<config->linkCount; j++) {
	if (config->links[j].autoFileCreateFile==NULL) continue;
	for (i=0; i<cfgNamesCount; i++) {
	    if (strcmp(cfgNames[i],config->links[j].autoFileCreateFile)==0) break;
	}
	// if not found include file - return error
	if (i==cfgNamesCount) {
	    printf("AutoFileCreateFile %s has never been included in config!\n",
		   config->links[j].autoFileCreateFile);
	    exit(EX_CONFIG);
	}
    }
}
