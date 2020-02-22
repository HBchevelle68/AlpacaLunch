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



void alpacacore_server_clean(){
    EPILOG;
    /*
     * For now this is just a rather useless wrapper
     * Leaving it for now in case of possible function exapansion
     */
    AlpacaComms_cleanComms();
    PROLOG;
}



/*
 * Currently single threaded 
 

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


ALPACA_STATUS alpacacore_server_run(uint16_t port, uint32_t listen_count){

    EPILOG;
	ALPACA_STATUS ret_status = ALPACA_SUCCESS;
    
    BUFALLOC(allu_serv, sizeof(ALLU_comms_ctx));
    BUFALLOC(allu_serv->serv_addr, sizeof(struct sockaddr_in));

    ret_status = AlpacaComms_create_listen_sock(allu_serv, port, listen_count);
    if(ret_status != ALPACA_SUCCESS){
        return ret_status;
    }

    ret_status = alpacacore_server_loop();

    alpacacore_server_clean();
    PROLOG;
    return ret_status;
}

*/