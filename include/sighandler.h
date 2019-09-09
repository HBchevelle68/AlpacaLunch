#ifndef SIGHANDLER_H
#define SIGHANDLER_H

// Internal
#include <nscodes.h>

void NS_sig_handler(int signum);
NS_STATUS NS_install_sighandlers();

#endif