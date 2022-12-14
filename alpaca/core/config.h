#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>

#include <core/codes.h>


#define HDR_FTR_SIZE 8

typedef struct __attribute__((packed)) AlpacaLunch_Configuration {

    char MAGIC_HEADER[HDR_FTR_SIZE]; // 8

    uint8_t  initial_protocol; // 1
    uint8_t  initial_behavior; // 1
    uint16_t port;             // 2
    uint8_t  addr[INET_ADDRSTRLEN]; // 16

    // Max retries
    // Max threads
    // Retry delay
    // What else????

    char MAGIC_FOOTER[HDR_FTR_SIZE]; // 8

} Alpaca_config_t;

#define ALPACA_CONFIG_FULL_SIZE (sizeof(struct AlpacaLunch_Configuration))
#define ALPACA_CONFIG_USEABLE_SIZE (sizeof(struct AlpacaLunch_Configuration)-(HDR_FTR_SIZE*2))

extern Alpaca_config_t alpaca_config __attribute__ ((section (".data")));


ALPACA_STATUS AlpacaConfig_init(void);


#endif