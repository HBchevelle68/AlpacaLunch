#ifndef SIGHANDLER_H
#define SIGHANDLER_H

// Internal
#include <core/codes.h>

void alpacacore_sig_handler(int signum);
ALPACA_STATUS alpacacore_install_sighandlers();

#endif