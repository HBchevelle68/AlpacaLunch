#include <core/config.h>
#include <core/logging.h>
//#include "config.h"
//#include "logging.h"


volatile Alpaca_config_t alpaca_config = {
    .MAGIC_HEADER="WwXxYyZz",
    .MAGIC_FOOTER="wWxXyYzZ"
};




ALPACA_STATUS AlpacaConfig_init(void){
    ALPACA_STATUS result = ALPACA_SUCCESS;

    //LOGDEBUG("HEADER: %s", alpaca_config.MAGIC_HEADER);

    alpaca_config.initial_behavior = 1;

    return result;
}

// Mostly blank for now
