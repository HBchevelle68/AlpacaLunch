#include <stdio.h>
#include <string.h> 
#include <netinet/in.h> 
#include <unistd.h> 
#include <errno.h>

#include <server.h>
#include <logging.h>
#include <macros.h>
#include <nscodes.h>
#include <crypto.h>


NS_server_t *ns_serv = NULL;




void NS_server_clean(){

    if(ns_serv != NULL){
        if(ns_serv->sock > 0) {
            close(ns_serv->sock);
        }
        if(ns_serv->tls_ctx != NULL){
            wolfSSL_CTX_free(ns_serv->tls_ctx);
        }
        BUFFREE(ns_serv->serv_addr);
        BUFFREE(ns_serv);
    }
}

static 
void NS_conn_handler(uint32_t cli_sock, struct sockaddr_in* cliaddr){

    WOLFSSL* ssl;
    uint32_t n;
    char buff[1500];

    if((ssl = wolfSSL_new(ns_serv->tls_ctx)) == NULL) {
		   LOGERROR("CyaSSL_new error\n");
		   return;
	}
	wolfSSL_set_fd(ssl, cli_sock);

    LOGDEBUG("Reading from tls1.2 socket\n");
    memset(&buff, 0, 1500);
    while ((n = wolfSSL_read(ssl, buff, 1500)) > 0){
        if( n < 0 ){
            LOGERROR("wolfSSL_read error = %d\n", wolfSSL_get_error(ssl,n));
            break;
        }
        LOGDEBUG("FROM CONNECTION: %s\n", buff);
    }
    wolfSSL_free(ssl);

}



static
NS_STATUS NS_server_loop(){
    struct sockaddr_in	cliaddr = {0};
    uint32_t cli_sock = 0;
    socklen_t addrlen = sizeof(cliaddr);
    while(1){
        cli_sock = accept(ns_serv->sock, (struct sockaddr*)(&cliaddr), &addrlen);
        /*
         * Can be interuppted by sighandler
         * or simply an error
         */
        if(cli_sock != -1){
            LOGDEBUG("Client connected\n");
            NS_conn_handler(cli_sock, &cliaddr);
            close(cli_sock);
            memset(&cliaddr, 0, (size_t)addrlen);
        }
    }
    return NS_FAILURE;
}


NS_STATUS NS_server_run(uint16_t port){

	NS_STATUS ret_status = NS_SUCCESS;
    
    BUFALLOC(ns_serv, sizeof(NS_server_t));
    BUFALLOC(ns_serv->serv_addr, sizeof(struct sockaddr_in));

    FAIL_IF_PRINT(NS_init_TLS_1_2(ns_serv), "TLS1.2 INIT\n");
 
    if ((ns_serv->sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        LOGERROR("Socket creation failure\n");
        return NS_FAILURE;
    } 

    ns_serv->serv_addr->sin_family = AF_INET;          
    ns_serv->serv_addr->sin_addr.s_addr = INADDR_ANY;  
    ns_serv->serv_addr->sin_port = BEU16(port);        

    FAIL_IF(REUSEADDR(ns_serv->sock));
    FAIL_IF(BIND(ns_serv->sock, ns_serv->serv_addr));
    FAIL_IF(LISTEN(ns_serv->sock,20));
    
    LOGDEBUG(">>> Sock_FD: %d Bound to %s:%hu <<<\n", ns_serv->sock, 
                inet_ntoa(ns_serv->serv_addr->sin_addr), HU16(ns_serv->serv_addr->sin_port));

    ret_status = NS_server_loop();

    NS_server_clean();

    return ret_status;
}

