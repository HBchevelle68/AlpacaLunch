#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <string.h>

/*
 * Suppress vscode's inability to detect volatility
 */ 
#ifndef NETDEBUG
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
#define DEBUGWARNING() \
do { if (DEBUGENABLE){ \
        printf("****************************************\n"); \
        printf("*************** DEBUG ON ***************\n"); \
        printf("****************************************\n"); \
      }                                                       \
   } while(0)



/*
 * Logging  
 * 
 * TO DO
 * Create levels of logging
 */
#define LOGINFO(fmt, ...) \
    do { if (DEBUGENABLE) printf((fmt), ##__VA_ARGS__); fflush(stdout); } while(0)

#define LOGDEBUG(fmt, ...) \
    do { if (DEBUGENABLE) fprintf(stdout, "%s <DEBUG> %s:%d:%s(): " fmt, __TIME__, __FNAME__, \
    __LINE__, __func__, ##__VA_ARGS__); fflush(stdout); } while(0)

#define LOGERROR(fmt, ...) \
    do { if (DEBUGENABLE) fprintf(stderr, "%s <ERROR> %s:%d:%s(): " fmt, __TIME__, __FNAME__, \
    __LINE__, __func__, ##__VA_ARGS__); fflush(stderr); } while(0)

#define LOGTESTFILE(buff) \
        FILE* fp;                                 \
        fp = fopen("/tmp/test.txt", "a");         \
        fprintf(fp, "%s\n", buff);                \
        fclose(fp);


#endif