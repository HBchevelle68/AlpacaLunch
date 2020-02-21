#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <string.h>

/*
 * Suppress vscode's inability to detect volatility
 */ 
#ifndef TALKATIVELLAMA
#define DEBUGENABLE 0
#else
#define DEBUGENABLE 1
#endif 

/*
 * Grab source file
 */ 
#define __FNAME__ (strrchr(__FILE__, '/') + 1)

/*
 * Warn that debug build is running
 */ 
#ifdef TALKATIVELLAMA
#define DEBUGWARNING() \
do { if (DEBUGENABLE){ \
        printf("****************************************\n"); \
        printf("*************** DEBUG ON ***************\n"); \
        printf("****************************************\n"); \
      }                                                       \
} while(0)
#else
#define DEBUGWARNING()
#endif


/*
 * Logging
 */
#ifdef TALKATIVELLAMA
#define LOGINFO(fmt, ...) \
    do { fprintf(stdout, "%s <INFO> %s:%d:%s(): " fmt, __TIME__, __FNAME__, \
    __LINE__, __func__, ##__VA_ARGS__); fflush(stdout); } while(0)
#else
#define LOGINFO(fmt, ...)
#endif


#ifdef TALKATIVELLAMA
#define LOGDEBUG(fmt, ...) \
    do { fprintf(stdout, "%s <DEBUG> %s:%d:%s(): " fmt, __TIME__, __FNAME__, \
    __LINE__, __func__, ##__VA_ARGS__); fflush(stdout); } while(0)
#else
#define LOGDEBUG(fmt, ...)
#endif


#ifdef TALKATIVELLAMA
#define LOGERROR(fmt, ...) \
    do { fprintf(stderr, "%s <ERROR> %s:%d:%s(): " fmt, __TIME__, __FNAME__, \
    __LINE__, __func__, ##__VA_ARGS__); fflush(stderr); } while(0)
#else
#define LOGERROR(fmt, ...)
#endif

#ifdef TALKATIVELLAMA
#define EPILOG \
    do { fprintf(stdout, "%s <EPILOG> Entering %s:%s()\n", __TIME__, __FNAME__, __func__); } while(0)
#else
#define EPILOG
#endif

#ifdef TALKATIVELLAMA
#define PROLOG \
    do { fprintf(stdout, "%s <PROLOG> Leaving %s:%s()\n", __TIME__, __FNAME__, __func__); } while(0)
#else
#define PROLOG
#endif

#define LOGTESTFILE(buff) \
        FILE* fp;                                 \
        fp = fopen("/tmp/test.txt", "a");         \
        fprintf(fp, "%s\n", buff);                \
        fclose(fp);




#endif