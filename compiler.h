

#ifndef __COMPILER_H__
#define __COMPILER_H__

#if defined(__MINGW32__)

#define MKDIR _mkdir

#elif defined(__EMX__) || defined(__DJGPP__)

#define MKDIR(a) (mkdir((a),0))

#elif defined(__NetBSD__) || defined(__FreeBSD__) || defined(_AIX) || defined(__sun__) || defined(__linux__) || defined(__osf__) || defined(__hpux) || defined(__BEOS__) || defined(__OpenBSD__)
#define MKDIR(a) (mkdir((a), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))

#elif defined(UNIX)
#define MKDIR(a) (__mkdir((a), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
#endif

#else

#define MKDIR mkdir

#endif
