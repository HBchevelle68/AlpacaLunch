#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <core/config.h>
#include <core/logging.h>
#include <interfaces/comms_interface.h>

#define CAPW 87
#define LILW 119 



Alpaca_config_t alpaca_config = {
    .MAGIC_HEADER="WwXxYyZ\0",
    .MAGIC_FOOTER="wWxXyYz\0"
};




ALPACA_STATUS AlpacaConfig_init(void){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;

    LOGDEBUG("HEADER_MAGIC: %s\n", alpaca_config.MAGIC_HEADER);
    LOGDEBUG("initial_protocol: %u\n", alpaca_config.initial_protocol);
    LOGDEBUG("initial_behavior: %u\n", alpaca_config.initial_behavior);
    LOGDEBUG("port: %u\n", alpaca_config.port);
    LOGDEBUG("IP: %s\n", alpaca_config.addr);
    LOGDEBUG("MAGIC_FOOTER: %s\n", alpaca_config.MAGIC_FOOTER);


    if(CAPW != alpaca_config.MAGIC_HEADER[0] || 0 != alpaca_config.MAGIC_HEADER[7]){
        LOGERROR("Configuration MAGIC_HEADER corrupted\n");
        result = ALPACA_ERROR_CONFCORRUPT;
        goto exit;
    }

    if(ALPACACOMMS_PROTO_TLS12 != alpaca_config.initial_protocol &&
       ALPACACOMMS_PROTO_TLS13 != alpaca_config.initial_protocol &&
       ALPACACOMMS_PROTO_UDP != alpaca_config.initial_protocol &&
       ALPACACOMMS_PROTO_SSH != alpaca_config.initial_protocol)
    {
        LOGERROR("Configuration unknown initial protocol [%u]\n", alpaca_config.initial_protocol);
        result = ALPACA_ERROR_CONFINITPROTO;
        goto exit;
    }

    if(ALPACACOMMS_INIT_CALLBACK != alpaca_config.initial_behavior &&
       ALPACACOMMS_INIT_LISTEN != alpaca_config.initial_behavior)
    {
        LOGERROR("Configuration unknown initial behavior [%u]\n", alpaca_config.initial_behavior);
        result = ALPACA_ERROR_CONFINITPROTO;
        goto exit;
    }

    if(4096 > alpaca_config.port) {
        LOGERROR("Configuration bad port [%u]\n", alpaca_config.port);
        result = ALPACA_ERROR_CONFINITPROTO;
        goto exit;
    }

    if(LILW != alpaca_config.MAGIC_FOOTER[0] || 0 != alpaca_config.MAGIC_FOOTER[7]){
        LOGERROR("Configuration MAGIC_FOOTER corrupted\n");
        result = ALPACA_ERROR_CONFCORRUPT;
        goto exit;
    }
    
exit:
    LEAVING
    return result;
}

// Mostly blank for now
