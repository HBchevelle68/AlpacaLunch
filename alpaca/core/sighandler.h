#ifndef SIGHANDLER_H
#define SIGHANDLER_H

// Internal
#include <core/codes.h>

void AlpacaCore_sigHandler(int signum);
ALPACA_STATUS AlpacaCore_installSigHandlers();

#endif