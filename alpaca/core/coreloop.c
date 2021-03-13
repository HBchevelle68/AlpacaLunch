#include <sys/poll.h>

#include <interfaces/comms_interface.h>
#include <core/codes.h>
#include <core/logging.h>

#define MILLI_ONE_SEC (1000) // one second in milliseconds


/* Process level comms connected to controller */
extern Alpaca_commsCtx_t *coreComms;

ALPACA_STATUS AlpacaCore_coreLoop(void){

	// /ALPACA_STATUS result = ALPACA_SUCCESS;
	int32_t ret = 0;
    ssize_t numRecvd = 0;
    struct pollfd pfd;
    uint8_t data[ALPACACOMMS_MAX_BUFF] = {0};
    ENTRY;

    /* 
     * We only care about inbound data from the 
     * controller. we also are concerned about
     * loss of connection  
     */
    pfd.fd = coreComms->fd;
    pfd.events = POLLIN;
    while(1) {
        pfd.revents = 0;
        /* TODO
         * Amount to memset will likely need change after payload
         * processing is decided upon
         */
        memset(data, 0, numRecvd);

        ret = poll(&pfd, 1, MILLI_ONE_SEC);
        if(ret > 0) {
            /* Data available for read */
            if(pfd.revents == POLLIN) {
                /* ** TO DO ********************************************************
                 * Since the header size is deterministic should read off header
                 * from there introspection into the header will provide 
                 * remaining length to readoff
                 */
                coreComms->read(coreComms->protoCtx, &data, ALPACACOMMS_MAX_BUFF, &numRecvd);

                // Maybe a ...._processPayload() call??
            }
            else if (pfd.revents & (POLLERR|POLLHUP)) {
                /*
                 * FIN or RST recieved
                 * This should never be expected 
                 * attempt to re-establish connection to controller
                 */
                AlpacaComms_destroyCtx(&coreComms);
                 /*
                 * Here depenging on configuration 
                 * would connect or listen
                 */ 
                
                //AlpacaComms_initCtx(&coreComms, );
                
               

            }
        }
        else if(ret < 0) {
            /* An error occured */ 

        }
        /*
         * Either a timeout occured or data was 
         * read off the socket. Either way, time to 
         * check for any outbound data
         */

        // TO DO - CHECK FOR OUTBOUND DATA AND SEND
    }
}