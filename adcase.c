/*
 *  ADCASE.C
 *  Provides case insensitive file name matching on UNIX filesystems. 
 *  Written 1999 by Tobias Ernst and released to the public domain. *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>


#include <smapi/compiler.h>

#ifdef HAS_UNISTD_H
#   include <unistd.h>
#endif

#ifdef HAS_IO_H
#   include <io.h>
#endif

#ifdef HAS_DIRENT_H
#  include <dirent.h>
#endif

#include "common.h"

#ifdef __UNIX__

#ifdef HAS_SYS_PARAMS_H
#   include <io.h>
#endif

#if 0
  #if (defined(__unix__) || defined(unix)) && !defined(USG)
  #include <sys/param.h>          /* used to differentiate BSD from SYSV  */
  #endif                                
#endif                                

/* The adaptcase routine behaves as follows: It assumes that pathname
   is a path name which may contain multiple dashes, and it assumes
   that you run on a case sensitive file system but want / must to
   match the path name insensitively. adaptcase takes every path
   element out of pathname and uses findfirst to check if it
   exists. If it exists, the path element is replaced by the exact
   spelling as used by the file system. If it does not exist, it is
   converted to lowercase.  This allows you to make you program deal
   with things like mounts of DOS file systems under unix

   Return value is 1 if the file exists and 0 if not.

   Attention: Do not ever try to understand this code. I had to do
   heavy caching and other optimizations in this routine in order to
   reduce that startup time of msged to a reasonable value. (The
   problem is that opendir / readdir is very slow ...). If you ever
   have to fix something in this routine, you'd better rewrite it from
   scratch.
 
 *  ATTENTION: The adaptcase routine builds an internal cache which never
 *  expires. If you ever add files to or remove files to a subdirectory and
 *  later want to have adaptcase map this particular file name properly,
 *  you must call adaptcase_refresh_dir() with the subdirectory path name
 *  as argument!

*/

/* the cache will take  about 30 * 4192 + 30 * 512 * 4 bytes in this
   configuration, i.e. 180K */

#define adaptcase_cachesize 30
#define rawcache_stepsize 4192
#define cacheindex_stepsize 512

struct adaptcase_cache_entry
{
    char *query;
    char *result;
    char *raw_cache;
    size_t *cache_index;
    size_t n;
};
static int adaptcase_cache_position = -1;
static struct adaptcase_cache_entry adaptcase_cache[adaptcase_cachesize];

static char *current_cache;
static int cache_sort_cmp(const void *a, const void *b)
{
    return strcasecmp(current_cache+(*((const size_t *)a)),
                   current_cache+(*((const size_t *)b)));
}

static int cache_find_cmp(const void *a, const void *b)
{
    return strcasecmp((const char *)a, current_cache+(*((const size_t *)b)));
}
  
/* #define TRACECACHE */

#ifdef BSD
#define DIRENTLEN(x) ((x)->d_namlen)
#else
#define DIRENTLEN(x) (strlen((x)->d_name))
#endif

void adaptcase_refresh_dir(char *directory)
{
    int k = strlen(directory), l;
    
    if (k && directory[k-1] == '/')
    {
        k--;
    }
    
    if (k != 0)
    {
        l = adaptcase_cache_position;
        do
        {
            if (adaptcase_cache[l].query != NULL)
            {
                if ((!memcmp(adaptcase_cache[l].query,directory,k)) &&
                    (adaptcase_cache[l].query[k] == '\0'))
                {
                    nfree(adaptcase_cache[l].query);
                    nfree(adaptcase_cache[l].result);
                    nfree(adaptcase_cache[l].raw_cache);
                }
            }

            l = (l == 0) ? adaptcase_cachesize - 1 : l - 1;
        } while (l != adaptcase_cache_position);
    }
}

void adaptcase(char *pathname)
{
    int i,j,k,l,n,nmax, found=1, addresult=0;
    size_t *m; size_t raw_high, rawmax;
    char buf[FILENAME_MAX + 1];
    DIR *dirp = NULL;
    struct dirent *dp;
    char c;

#ifdef TRACECACHE
    FILE *ftrc;
#endif
    
    if (!*pathname)
        return;
#ifdef TRACECACHE
    ftrc = fopen ("trace.log", "a");
    fprintf(ftrc, "--Query: %s\n", pathname);
#endif    
    
    if (adaptcase_cache_position == -1)
    {
        /* initialise the cache */
        memset(adaptcase_cache, 0, adaptcase_cachesize *
               sizeof(struct adaptcase_cache_entry));
        adaptcase_cache_position = 0;
    }
    
    k = strlen(pathname);

    if (k > 2)
    {
        for (k = k - 2; k>0 && pathname[k] != '/'; k--);
    }
    else
    {
        k = 0;
    }
    
    j = 0; i = 0;


start_over:

    if (k != 0)
    {
        l = adaptcase_cache_position;
        do
        {
            if (adaptcase_cache[l].query != NULL)
            {
                if ((!memcmp(adaptcase_cache[l].query,pathname,k)) &&
                    (adaptcase_cache[l].query[k] == '\0'))                                                  
                {
                    /* cache hit for the directory */
#ifdef TRACECACHE
                    fprintf (ftrc, "Cache hit for Dir: %s\n",
                             adaptcase_cache[l].result);
#endif                   
                    memcpy(buf, adaptcase_cache[l].result, k);
                    buf[k] = '/';
                    current_cache=adaptcase_cache[l].raw_cache;
                    m = bsearch(pathname + k + 1,
                                adaptcase_cache[l].cache_index,
                                adaptcase_cache[l].n,
                                sizeof(size_t),
                                cache_find_cmp);
                    if (m == 0)
                    {
#ifdef TRACECACHE
                        fprintf (ftrc, "Cache miss for file.\n");
#endif                   

                        /* file does not exist - convert to lower c. */
                        for (n = k + 1; pathname[n-1]; n++)
                        {
                            buf[n] = tolower(pathname[n]);
                        }
                        memcpy(pathname, buf, n-1);
#ifdef TRACECACHE
                        fprintf(ftrc, "Return: %s\n", pathname);
                        fclose(ftrc);
#endif    
                        return;
                    }
                    else
                    {
#ifdef TRACECACHE
                        fprintf (ftrc, "Cache hit for file: %s\n",
                                 adaptcase_cache[l].raw_cache+(*m));
#endif                   

                        /* file does exist = cache hit for the file */
                        for (n = k + 1; pathname[n-1]; n++)
                        {
                            buf[n] =
                                adaptcase_cache[l].raw_cache[(*m) + n - k - 1];
                        }
                        assert(buf[n-1] == '\0');
                        memcpy(pathname, buf, n-1);
#ifdef TRACECACHE
                        fprintf(ftrc, "Return: %s\n", pathname);
                        fclose(ftrc);
#endif
                        return;
                    }
                }
            }
            l = (l == 0) ? adaptcase_cachesize - 1 : l - 1;
        } while (l != adaptcase_cache_position);

#ifdef TRACECACHE
        fprintf (ftrc, "Cache miss for directory.\n");
#endif                   


        /* no hit for the directory */
        addresult = 1;
    }

               
    while (pathname[i])
    {
        if (pathname[i] == '/')
        {
            buf[i] = pathname[i]; 
            if (addresult && i == k)
            {
                goto add_to_cache;
            }
cache_failure:
            i++;
            buf[i]='\0';
            dirp = opendir(buf);
#ifdef TRACECACHE
            if (dirp == NULL)
            {
                fprintf (ftrc, "Error opening directory %s\n", buf);
            }
#endif            
        }
        else
        {
            assert(i==0);
            dirp = opendir("./");
#ifdef TRACECACHE
            if (dirp == NULL)
            {
                fprintf (ftrc, "Error opening directory ./\n");
            }
#endif            
        }

        j = i;
        for (; pathname[i] && pathname[i]!='/'; i++)
            buf[i] = pathname[i];
        buf[i] = '\0';
        found = 0;

        if (dirp != NULL)
        {
            while ((dp = readdir(dirp)) != NULL)
            {
                if (!strcasecmp(dp->d_name, buf + j))
                {
                    /* file exists, take over it's name */
            
                    assert(i - j == DIRENTLEN(dp));
                    memcpy(buf + j, dp->d_name, DIRENTLEN(dp) + 1);
                    closedir(dirp);
                    dirp = NULL;
                    found = 1;
                    break;
                }
            }
        }
        if (!found)
        {
            /* file does not exist - so the rest is brand new and
               should be converted to lower case */
            
            for (i = j; pathname[i]; i++)
                buf[i] = tolower(pathname[i]);
            buf[i] = '\0';
            if (dirp != NULL)
            {
                closedir(dirp);
            }
            dirp = NULL;
            break;
        }
    }
    assert(strlen(pathname) == strlen(buf));

add_to_cache:
    while (addresult)
    {
        l = adaptcase_cache_position;
        l = (l == adaptcase_cachesize - 1) ? 0 : l + 1;

        nfree(adaptcase_cache[l].query);
        nfree(adaptcase_cache[l].result);
        nfree(adaptcase_cache[l].raw_cache);

        if ( (adaptcase_cache[l].query = malloc(k + 1)) == NULL ||
             (adaptcase_cache[l].result = malloc(k + 1)) == NULL ||
             (adaptcase_cache[l].raw_cache =  malloc(rawmax = rawcache_stepsize)) == NULL ||
             (adaptcase_cache[l].cache_index = malloc((nmax = cacheindex_stepsize) * sizeof(size_t))) == NULL )
        {
            goto cache_error;
        }

        adaptcase_cache[l].n = 0;
        raw_high = 0;
        
        c = buf[k]; buf[k] = '\0';
        if ((dirp = opendir(buf)) == NULL)
        {
            buf[k] = c;
            goto cache_error;
        }
        buf[k] = c;

        while ((dp = readdir(dirp)) != NULL)
        {
            if (raw_high + DIRENTLEN(dp) + 1 > rawmax)
            {
                if ((adaptcase_cache[l].raw_cache =
                     realloc(adaptcase_cache[l].raw_cache,
                             rawmax+=rawcache_stepsize)) == NULL)
                {
                    goto cache_error;
                }
            }
           
            if (adaptcase_cache[l].n == nmax - 1)
            {
                if ((adaptcase_cache[l].cache_index =
                     realloc(adaptcase_cache[l].cache_index,
                             (nmax+=cacheindex_stepsize) *
                             sizeof(size_t))) == NULL)
                {
                    goto cache_error;
                }
            }

            memcpy (adaptcase_cache[l].raw_cache + raw_high,
                    dp->d_name, DIRENTLEN(dp) + 1);
            adaptcase_cache[l].cache_index[adaptcase_cache[l].n++] = raw_high;
            raw_high += DIRENTLEN(dp) + 1;
        }
        closedir(dirp);
        current_cache=adaptcase_cache[l].raw_cache;
        qsort(adaptcase_cache[l].cache_index, adaptcase_cache[l].n,
              sizeof(size_t), cache_sort_cmp);

        memcpy(adaptcase_cache[l].query, pathname, k);
        adaptcase_cache[l].query[k] = '\0';
        memcpy(adaptcase_cache[l].result, buf, k);
        adaptcase_cache[l].result[k] = '\0';
        
        adaptcase_cache_position = l;

#ifdef TRACECACHE
        fprintf  (ftrc, "Sucessfully added cache entry.\n");
#endif        
        goto start_over;

    cache_error:

        nfree(adaptcase_cache[l].query);
        nfree(adaptcase_cache[l].result);
        nfree(adaptcase_cache[l].raw_cache);
        nfree(adaptcase_cache[l].cache_index);
        
            if (dirp != NULL)
        {
            closedir(dirp);
        }
#ifdef TRACECACHE
        fprintf  (ftrc, "Error in building cache entry.\n");
#endif        
        addresult = 0;
        goto cache_failure;
    }
        
#ifdef TRACECACHE
    fprintf(ftrc, "Return: %s\n", pathname);
    fclose(ftrc);
#endif
    strcpy(pathname, buf);
    return;
}

 
#else

/* Not UNIX - Just convert the file to lower case, it will work because
   the FS is case insensitive */

#include "adcase.h"

void adaptcase (char *str)
{
    for (; *str; str++)
    {
        *str = (char)tolower(*str);
    }
}

#ifndef unused
#ifdef __GNUC__
#define unused(x)
#else
#define unused(x) (x)
#endif
#endif

void adaptcase_refresh_dir(char *directory)
{
    unused(directory);
}
#endif  

#if defined (TEST)
int main(int argc, char **argv)
{
    char cmdbuf[64];
    char fnbuf[128];

    do
    {
        printf ("(L)ookup, (I)nvalidate, (Q)uit? "); fflush(stdout);
        gets(cmdbuf);
        switch (*cmdbuf)
        {
        case 'q':
        case 'Q':
            return 0;
        case 'I':
        case 'i':
            gets(fnbuf);
            adaptcase_refresh_dir(fnbuf);
            break;
        case 'L':
        case 'l':
            gets(fnbuf);
            adaptcase(fnbuf);
            printf ("%s\n", fnbuf);
            break;
        }
    } while (1);
}
#endif


