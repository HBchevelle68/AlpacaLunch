#include <stdio.h>
#include <string.h> 
#include <netinet/in.h> 
#include <unistd.h> 

#include <server.h>
#include <logging.h>
#include <macros.h>
#include <nscodes.h>



NS_server_t *ns_serv;


static
NS_STATUS NS_server_loop(){
    printf("here");
    uint32_t conn_sock = 0;
    while(1){
        conn_sock = accept(ns_serv->sock, (struct sockaddr *)&(ns_serv->serv_addr), (socklen_t*)sizeof(struct sockaddr_in));
        close(conn_sock); 
    }
    return NS_FAILURE;
}



void NS_server_clean(){

    if(ns_serv->sock > 0) {
        LOGDEBUG("Closing server Socket\n");
        close(ns_serv->sock);
    }
    BUFFREE(ns_serv->serv_addr);
	BUFFREE(ns_serv);

}

NS_STATUS NS_server_run(uint16_t port){

	NS_STATUS ret_status = NS_SUCCESS;
    
    BUFALLOC(ns_serv, sizeof(NS_server_t));
    BUFALLOC(ns_serv->serv_addr, sizeof(struct sockaddr_in));
 
    if ((ns_serv->sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        LOGERROR("Socket creation failure");
        return NS_FAILURE;
    } 
       
    ns_serv->serv_addr->sin_family = AF_INET; 
    ns_serv->serv_addr->sin_addr.s_addr = INADDR_ANY; 
    ns_serv->serv_addr->sin_port = BEU16(12345);
 
    SERVFAIL_IF(REUSEADDR(ns_serv->sock), "SETSOCKOPT");
    SERVFAIL_IF(BIND(ns_serv->sock, ns_serv->serv_addr), "BIND");
    SERVFAIL_IF(LISTEN(ns_serv->sock,20),"LISTEN");
    LOGINFO(">>> Bound to %s:%hu <<<", inet_ntoa(ns_serv->serv_addr->sin_addr), ns_serv->sock);

    ret_status = NS_server_loop();

    NS_server_clean();
    return ret_status;
}

