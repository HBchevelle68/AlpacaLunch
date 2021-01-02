#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

// Internal
#include <core/logging.h>


// Implements Interface 
#include <interfaces/threadpool_interface.h>


// For Atomics
static const uint8_t FLAGON  = 1;
static const uint8_t FLAGOFF = 0;


