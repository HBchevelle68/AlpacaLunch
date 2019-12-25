

// Internal
#include <core/allu.h>
#include <core/macros.h>
#include <core/server.h>
#include <core/logging.h>
#include <core/sighandler.h>


ALPACA_STATUS alpacacore_init(){
    ALPACA_STATUS result = ALPACA_SUCCESS;

    DEBUGWARNING();
    FAIL_IF_TRUE(getuid()); 
    FAIL_IF_TRUE(alpacacore_install_sighandlers());

    return result;
}


/*
 * Small for now. Called by sighandler
 */
void alpacacore_exit(){
    alpacacore_server_clean();
    exit(0);
}