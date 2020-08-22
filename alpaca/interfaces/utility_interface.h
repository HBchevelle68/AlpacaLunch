#ifndef UTILITY_INTERFACE_H
#define UTILITY_INTERFACE_H


#include <stdint.h>

#include <sys/stat.h>
#include <sys/types.h>



off_t AlpacaFileUtils_fSize(const char *filename);
void  AlpacaUtilities_daemonize(void);
int   AlpacaUtilities_mSleep(long msec);



#endif