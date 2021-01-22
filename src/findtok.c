#include <stdlib.h>             /* bsearch           */
#include <stdio.h>              /* fprintf to stderr */
#include <string.h>             /* strcmp            */

#include "findtok.h"
/* This function fills in a structure of type token_list_t based on a token_t
   array. The token_list_t structure contains some internal control data used
   by find_token. The last element of the token_t array must have a NULL
   pointer in the "token" field.

   The main function of this function is to a) count the number of tokens and b)
   make sure that the array of tokens that the programmer supplies
   to us is sorted so that we can later use bsearch on it.

   If the array is not sorted, we simply print a warning to stderr.
   We do NOT resort the array in this case, because normally the
   order of magnitude of keywords in this array is in the same order
   of magnitude of keywords that will be in the config file to
   parse, so that sorting of the array of the program start would
   not yield any net performance gain.  The programmer should supply
   us with an already sorted list of keywords instead!
 */
void make_token_list(token_list_t * dst, token_t * src)
{
    int i;

    dst->tokens      = src;
    dst->ntokens     = 0;
    dst->bsearchable = 1;

    if(src[0].token == NULL)
    {
        return;                 /* nothing to do for an empty list */
    }

    for(i = 1; src[i].token != NULL; i++)
    {
        if(dst->bsearchable && strcmp(src[i - 1].token, src[i].token) >= 0)
        {
            fprintf(stderr,
                    "Warning: Token array is not bsearchable. This " "will result in a performance\npenalty. The offending " "token is: %s\n",
                    src[i].token);
            dst->bsearchable = 0;
        }
    }
    dst->ntokens = i;
}

static int token_compare(const void * a, const void * b)
{
    const token_t * pa = (const token_t *)a;
    const token_t * pb = (const token_t *)b;

    return strcmp(pa->token, pb->token);
}

/* This function returns the token ID based on a keyword, or -1 or
   the keyword is not found in the token list. */
int find_token(token_list_t * tokenlist, const char * key)
{
    int i;
    token_t keytoken, * result;

    /* fall back code when the array is not
       sorted */
    if(!tokenlist->bsearchable)
    {
        for(i = 0; i < tokenlist->ntokens; i++)
        {
            if(!strcmp(tokenlist->tokens[i].token, key))
            {
                return tokenlist->tokens[i].id;
            }
        }
        return -1;
    }

    /* bsearch code */
    keytoken.token = key;
    keytoken.id    = -1;
    result         = bsearch(&keytoken,
                             tokenlist->tokens,
                             tokenlist->ntokens,
                             sizeof(token_t),
                             token_compare);

    if(result == NULL)
    {
        return -1;
    }

    return result->id;
} /* find_token */

#if 0
/* This code is an example on how to use the routines from this file */

#include "findtok.h"

enum {ID_LINK = 0, ID_LOGFILE, ID_NODELIST, ID_SYSOP};

token_t mytokens[] =
{   {"link", ID_LINK}, {"logfile", ID_LOGFILE}, {"nodelist", ID_NODELIST}, {"sysop", ID_SYSOP},
    {NULL,   -1     }};
token_list_t mytokenlist;
int main(void)
{
    make_token_list(&mytokenlist, mytokens);

    switch(find_token(&mytokenlist, "logfile"))
    {
        case ID_LINK:
            printf("link\n");
            break;

        case ID_LOGFILE:
            printf("logfile\n");
            break;

        case ID_NODELIST:
            printf("nodelist\n");
            break;

        case ID_SYSOP:
            printf("sysop\n");
            break;

        default:
            printf("unknown keyword\n");
    }
    return 0;
} /* main */

#endif /* if 0 */
