/* syslog portability layer for fidoconfig, written 2001 by tobias ernst */

#ifndef __SYSLOGP_H
#define __SYSLOGP_H

#include <huskylib/compiler.h>

/* ===== part 1: include system syslog header or provide our own prototypes */

#if defined (__UNIX__) && !defined(__BEOS__)

#define SYSLOG_NAMES

#ifdef HAS_SYSLOG_H
#   include <syslog.h>
#else
/* TODO: add HAS_SYS_SYSLOG_H to the platforms, which supported it */
#   include <sys/syslog.h>
#endif /* HAS_SYSLOG_H */

#ifndef HAVE_SYSLOG
#define HAVE_SYSLOG             /* indicate that this platform can do syslog */
#endif

#endif
                                /* we may add support for os/2 here in the
                                   future  */


/* ===== part 2: write some portability layer code for broken unix systems */

#ifdef HAVE_SYSLOG

/* ----- unix systems that have syslog, but not facilitynames */

#if defined(__sun__) || defined(__CYGWIN__) || defined(_AIX)

typedef struct _code {
        char    *c_name;
        int     c_val;
} CODE;

/* we only define ones which we think are present on all crazy unix systems
   and useful for fido apps. i.e. we leave out obscurities like "security" etc
*/

CODE facilitynames[] = {
        { "daemon",     LOG_DAEMON,     },
        { "mail",       LOG_MAIL,       },
        { "news",       LOG_NEWS,       },
        { "user",       LOG_USER,       },
        { "uucp",       LOG_UUCP,       },
        { "local0",     LOG_LOCAL0,     },
        { "local1",     LOG_LOCAL1,     },
        { "local2",     LOG_LOCAL2,     },
        { "local3",     LOG_LOCAL3,     },
        { "local4",     LOG_LOCAL4,     },
        { "local5",     LOG_LOCAL5,     },
        { "local6",     LOG_LOCAL6,     },
        { "local7",     LOG_LOCAL7,     }
};
#endif

#endif /* HAVE_SYSLOG */


#endif /* not defined __SYSLOGP_H */
