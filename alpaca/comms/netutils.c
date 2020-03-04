#define _GNU_SOURCE // Will freak out if this isn't here

// Implements interface 
#include <interfaces/comms_interface.h>
#include <core/logging.h>


#include <unistd.h>
#include <sys/utsname.h>
#include <sys/socket.h> 
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/types.h>



/*
 * @brief grabs uname output for current host system
 * @param allu_hinfo ALLU_hinfo pointer to struct to fill
 * @return ALPACA_STATUS of success or failure  
 */
ALPACA_STATUS AlpacaNetUtils_getUname(ALLU_hinfo* allu_hinfo){
 
   	if (uname(&(allu_hinfo->host_uname)) != 0) {
   		return ALPACA_FAILURE;
   	}
   return ALPACA_SUCCESS;
}

/*
 * @brief grabs host entry output for current host system
 * @param allu_hinfo ALLU_hinfo pointer to struct to fill
 * @return ALPACA_STATUS of success or failure  
 */
ALPACA_STATUS AlpacaNetUtils_getHostEntry(ALLU_hinfo* allu_hinfo){
 
    // To retrieve host information 
    if(!(allu_hinfo->host_entry = gethostbyname(allu_hinfo->host_uname.nodename))){
    	return ALPACA_FAILURE; 
    }
  	return ALPACA_SUCCESS; 
}


/*
 * @brief grabs all interfaces and respective ip addrs 
 *		  for current host system
 * @param allu_hinfo ALLU_hinfo pointer to struct to fill
 * @return ALPACA_STATUS of success or failure  
 */
ALPACA_STATUS AlpacaNetUtils_getIfAddrs(ALLU_hinfo** allu_hinfo){

    if (getifaddrs(&((*allu_hinfo)->interfaces)) == -1) {
        LOGERROR("getifaddrs");
        return ALPACA_FAILURE;
    }


	return ALPACA_SUCCESS;
}