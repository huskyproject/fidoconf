#ifndef _STAT_H
#define _STAT_H

#include "fidoconf.h"

#define ADV_STAT

/* stat record type */
typedef enum { stNORM, stBAD, stDUPE, stOUT } st_type;

HUSKYEXT void put_stat(s_area *echo, hs_addr *link, st_type type, INT32 len);
HUSKYEXT void upd_stat(char *file);

#endif
