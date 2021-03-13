#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <string.h>

/*
 * Suppress vscode's inability to detect volatility
 */ 
#if !defined(TALKATIVELLAMA) || !defined(VERBOSELLAMA)
#define DEBUGENABLE 0
#else
#define DEBUGENABLE 1
#endif 

/*
 * Grab source file
 */ 
#define __FNAME__ (strrchr(__FILE__, '/') + 1)


/*
 * Logging  
 */
#ifdef TALKATIVELLAMA

#define LOGINFO(fmt, ...) \
    do { fprintf(stdout, "[  INFO  ] \033[1m%s:%d\033[0m:%s(): " fmt, __FNAME__, \
    __LINE__, __func__, ##__VA_ARGS__); fflush(stdout); } while(0)

#define LOGDEBUG(fmt, ...) \
    do { fprintf(stdout, "[  \033[0;33mDEBUG\033[0m ] \033[1m%s:%d\033[0m:%s(): " fmt, __FNAME__, \
    __LINE__, __func__, ##__VA_ARGS__); fflush(stdout); } while(0)

#define LOGERROR(fmt, ...) \
    do { fprintf(stderr, "[  \033[0;31mERROR\033[0m ] \033[1m%s:%d\033[0m:%s(): " fmt, __FNAME__, \
    __LINE__, __func__, ##__VA_ARGS__); fflush(stderr); } while(0)

/*
 * Warn that debug build is running
 */ 
#define DEBUGWARNING() \
do { if (DEBUGENABLE){ \
        printf("****************************************\n"); \
        printf("*************** DEBUG ON ***************\n"); \
        printf("****************************************\n"); \
      }                                                       \
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
#endif

#ifdef VERBOSE
#define ENTRY \
    do { fprintf(stdout, "<ENTRY> Entering %s:%s()\n", __FNAME__, __func__); } while(0)
#else
#define ENTRY
#endif


#ifdef VERBOSE
#define LEAVING \
    do { fprintf(stdout, "<LEAVING> Leaving %s:%s()\n", __FNAME__, __func__); } while(0)
#else
#define LEAVING
#endif



#endif