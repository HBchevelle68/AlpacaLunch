#ifndef UTILITY_INTERFACE_H
#define UTILITY_INTERFACE_H


#include <sys/stat.h>
#include <sys/types.h>

off_t AlpacaFileUtils_fsize(const char *filename);
void  AlpacaUtilities_daemonize(void);

#endif