#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>


 pthread_t t;
/*
 * Grab source file
 */ 
#define __FNAME__ (strrchr(__FILE__, '/') + 1)


/*
 * Logging  
 */
#ifdef TALKATIVE_ALPACA

#define LOGINFO(fmt, ...) \
    do { fprintf(stdout, "[  INFO  ] ==%015lu== \033[1m%s:%d\033[0m:%s(): " fmt, pthread_self(), __FNAME__, \
    __LINE__, __func__, ##__VA_ARGS__); fflush(stdout); } while(0)

#define LOGDEBUG(fmt, ...) \
    do { fprintf(stdout, "[  \033[0;33mDEBUG\033[0m ] ==%015lu== \033[1m%s:%d\033[0m:%s(): " fmt, pthread_self(), __FNAME__, \
    __LINE__, __func__, ##__VA_ARGS__); fflush(stdout); } while(0)

#define LOGERROR(fmt, ...) \
    do { fprintf(stderr, "[  \033[0;31mERROR\033[0m ] ==%015lu== \033[1m%s:%d\033[0m:%s(): " fmt, pthread_self(), __FNAME__, \
    __LINE__, __func__, ##__VA_ARGS__); fflush(stderr); } while(0)

#ifdef TALKATIVE_WOLF

#define WOLFLOGGING wolfSSL_Debugging_ON();
#else
#define WOLFLOGGING wolfSSL_Debugging_OFF();
#endif

/*
 * Warn that debug build is running
 */ 
#define DEBUGWARNING() \
do {printf("****************************************\n"); \
    printf("*************** DEBUG ON ***************\n"); \
    printf("****************************************\n"); \
} while(0)


#define LOGTESTFILE(buff) \
        FILE* fp;                                 \
        fp = fopen("/tmp/test.txt", "a");         \
        fprintf(fp, "%s\n", buff);                \
        fclose(fp);
#else
#define LOGINFO(fmt, ...)
#define LOGDEBUG(fmt, ...)
#define LOGERROR(fmt, ...)
#define DEBUGWARNING()
#define WOLFLOGGING
#endif

#ifdef VERBOSE

#define ENTRY \
    do { fprintf(stdout, "<ENTRY> Entering %s:%s()\n", __FNAME__, __func__); } while(0)

#define LEAVING \
    do { fprintf(stdout, "<LEAVING> Leaving %s:%s()\n", __FNAME__, __func__); } while(0)

#else
#define ENTRY
#define LEAVING
#endif


#endif