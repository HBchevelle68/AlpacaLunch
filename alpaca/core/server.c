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
#include <core/memory.h>
#include <core/logging.h>
#include <core/macros.h>
#include <core/codes.h>
#include <core/crypto.h>



ALLU_comms_ctx *allu_serv = NULL;



void alpacacore_server_clean(){
    /*
     * For now this is just a rather useless wrapper
     * Leaving it for now in case of possible function exapansion
     */
    AlpacaComms_clean_comms_ctx(allu_serv);
}



/*
 * Currently single threaded 
 */

static
ALPACA_STATUS alpacacore_server_loop(){
    struct sockaddr_in	cliaddr = {0};
    uint32_t cli_sock = 0;
    socklen_t addrlen = sizeof(cliaddr);
    while(1){   
        cli_sock = accept(allu_serv->sock, (struct sockaddr*)(&cliaddr), &addrlen);
        /*
         * Can be interuppted by sighandler
         * or simply an error
         */
        if(cli_sock != -1){
            LOGDEBUG("Client connected\n");
            AlpacaComms_connection_handler(allu_serv, cli_sock, &cliaddr);
            close(cli_sock);
            memset(&cliaddr, 0, (size_t)addrlen);
        }
    }
    return ALPACA_FAILURE;
}


ALPACA_STATUS alpacacore_server_run(uint16_t port){

	ALPACA_STATUS ret_status = ALPACA_SUCCESS;
    
    BUFALLOC(allu_serv, sizeof(ALLU_comms_ctx));
    BUFALLOC(allu_serv->serv_addr, sizeof(struct sockaddr_in));

    /* Init WolfSSL */
    FAIL_IF(AlpacaComms_init_TLS(allu_serv));
 
    if ((allu_serv->sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        LOGERROR("Socket creation failure\n");
        return ALPACA_FAILURE;
    } 

    allu_serv->serv_addr->sin_family = AF_INET;          
    allu_serv->serv_addr->sin_addr.s_addr = INADDR_ANY;  
    allu_serv->serv_addr->sin_port = BEU16(port);

    FAIL_IF(REUSEADDR(allu_serv->sock));
    FAIL_IF(BIND(allu_serv->sock, allu_serv->serv_addr));
    FAIL_IF(LISTEN(allu_serv->sock,20));
    
    LOGDEBUG(">>> Sock_FD: %d Bound to %s:%hu <<<\n", allu_serv->sock, 
                inet_ntoa(allu_serv->serv_addr->sin_addr), HU16(allu_serv->serv_addr->sin_port));

    ret_status = alpacacore_server_loop();

    alpacacore_server_clean();

    return ret_status;
}

