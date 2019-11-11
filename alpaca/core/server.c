#include <stdio.h>
#include <string.h> 
#include <stdarg.h>
#include <netinet/in.h> 
#include <unistd.h> 
#include <errno.h>

#include <core/server.h>
#include <core/memory.h>
#include <core/logging.h>
#include <core/macros.h>
#include <core/codes.h>
#include <core/crypto.h>


#define BUFSIZE 1500
allu_server_t *allu_serv = NULL;




void alpacacore_server_clean(){

    if(allu_serv != NULL){
        if(allu_serv->sock > 0) {
            close(allu_serv->sock);
        }
        if(allu_serv->tls_ctx != NULL){
            wolfSSL_CTX_free(allu_serv->tls_ctx);
        }
        BUFFREE(allu_serv->serv_addr);
        BUFFREE(allu_serv);
    }
}

static 
void alpacacore_conn_handler(uint32_t cli_sock, struct sockaddr_in* cliaddr){

    WOLFSSL* l_tls;
    uint32_t n;
    char* buff;
    
    BUFALLOC(buff, BUFSIZE);
    ZEROBUFF(buff, BUFSIZE);

    /* Wrap socket */ 
    l_tls = alpacacore_wrap_sock(allu_serv, cli_sock);
    

    LOGDEBUG("Reading from tls1.2 socket\n");
    while ( (n = wolfSSL_read(l_tls, buff, BUFSIZE) ) > 0 ){
        
        if( n < 0 ){
            LOGERROR("wolfSSL_read error = %d\n", wolfSSL_get_error(l_tls,n));
            break;
        }
        
        LOGDEBUG("FROM CONNECTION: %s\n", buff);
        LOGTESTFILE(buff);

        ZEROBUFF(buff,BUFSIZE);
    }

    BUFFREE(buff);
    wolfSSL_free(l_tls);
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
            alpacacore_conn_handler(cli_sock, &cliaddr);
            close(cli_sock);
            memset(&cliaddr, 0, (size_t)addrlen);
        }
    }
    return ALPACA_FAILURE;
}


ALPACA_STATUS alpacacore_server_run(uint16_t port){

	ALPACA_STATUS ret_status = ALPACA_SUCCESS;
    
    BUFALLOC(allu_serv, sizeof(allu_server_t));
    BUFALLOC(allu_serv->serv_addr, sizeof(struct sockaddr_in));

    /* Init WolfSSL */
    FAIL_IF(alpacacore_init_TLS(allu_serv));
 
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

