#ifndef __FINDTOK_H
#define __FINDTOK_H

typedef struct
{
    const char * token;
    int          id;
} token_t;
typedef struct
{
    const token_t * tokens;
    int             ntokens;
    int             bsearchable;
} token_list_t;
void make_token_list(token_list_t * dst, token_t * src);
int find_token(token_list_t *, const char *);

#endif
