#ifndef UTILITY_INTERFACE_H
#define UTILITY_INTERFACE_H


#include <stdint.h>

#include <sys/stat.h>
#include <sys/types.h>




// Return min of two numbers
#ifndef MIN
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif


void  AlpacaUtilities_daemonize(void);
int   AlpacaUtilities_mSleep(long msec);

#endif