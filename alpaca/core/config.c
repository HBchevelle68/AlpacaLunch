#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <core/config.h>
#include <core/logging.h>


Alpaca_config_t alpaca_config = {
    .MAGIC_HEADER="WwXxYyZz",
    .MAGIC_FOOTER="wWxXyYzZ"
};




ALPACA_STATUS AlpacaConfig_init(void){
    ALPACA_STATUS result = ALPACA_SUCCESS;

    LOGDEBUG("HEADER_MAGIC: %s\n", alpaca_config.MAGIC_HEADER);
    LOGDEBUG("initial_behavior: %u\n", alpaca_config.initial_behavior);
    LOGDEBUG("port: %u\n", alpaca_config.port);
    LOGDEBUG("IP: %s\n", alpaca_config.addr);
    LOGDEBUG("MAGIC_FOOTER: %s\n", alpaca_config.MAGIC_FOOTER);

    // TODO
    // Should probably do some verification here

    return result;
}

// Mostly blank for now
