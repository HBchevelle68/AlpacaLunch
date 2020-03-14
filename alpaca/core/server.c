#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdarg.h>
#include <netinet/in.h> 
#include <unistd.h> 
#include <errno.h>

// Needs interface
#include <interfaces/comms_interface.h>

#include <core/server.h>
#include <core/logging.h>
#include <core/macros.h>
#include <core/codes.h>
#include <core/crypto.h>



//ALLU_comms_ctx *allu_serv = NULL;






 
/*
static
ALPACA_STATUS alpacacore_server_loop(){
    struct sockaddr_in	cliaddr = {0};
    uint32_t cli_sock = 0;
    socklen_t addrlen = sizeof(cliaddr);
    while(1){   
        cli_sock = accept(allu_serv->sock, (struct sockaddr*)(&cliaddr), &addrlen);

        if(cli_sock != -1){
            LOGDEBUG("Client connected\n");
            AlpacaComms_connection_handler(allu_serv, cli_sock, &cliaddr);
            close(cli_sock);
            memset(&cliaddr, 0, (size_t)addrlen);
        }
    }
    return ALPACA_FAILURE;
}
*/

ALPACA_STATUS AlpacaCore_serverRunST(uint16_t port, uint32_t listen_count){

    ENTRY;
	ALPACA_STATUS ret_status = ALPACA_SUCCESS;
    


    AlpacaCore_serverCleanST();
    LEAVING;
    return ret_status;
}

void AlpacaCore_serverCleanST(){
    ENTRY;
    /*
     * For now this is just a rather useless wrapper
     * Leaving it for now in case of possible function exapansion
     */
    AlpacaComms_cleanServerComms();
    LEAVING;
}