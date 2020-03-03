#
# Path variables
#
DIR := ${CURDIR}
BIN = $(DIR)/binaries
HASH = $(BIN)/hashes
BUILD = $(DIR)/build

#
# Source directories
#
SRCBASE = $(DIR)/alpaca
ALPACACORESRC   = $(SRCBASE)/core
ALPACACOMMSSRC	= $(SRCBASE)/comms
ALPACATHREADSRC = $(SRCBASE)/multithreadserver
ALPACATPOOLSRC  = $(SRCBASE)/threadpool
ALPACAUTILSSRC	= $(SRCBASE)/utilities
ALPACAMEMORYSRC = $(SRCBASE)/memory

#
# Header Directories
#
ALPACAINCLUDE= $(SRCBASE)

#
# TEST DIRECTORIES
#
TESTBASE= $(DIR)/tests
COMPONENTBASE = $(TESTBASE)/component
COMPONENTALL = $(COMPONENTBASE)/allTest.py
SNOW= $(TESTBASE)/unit

#
# WolfSSL
#
CRYPTBASE= $(SRCBASE)/cryptlibs
CRYPTINC= $(CRYPTBASE)/include
CRYPTSTATIC= $(CRYPTBASE)/lib/libwolfssl.a

#
# Controller
#
CONTROLLER= $(DIR)/controller
CONTROLLERSRC= $(CONTROLLER)/sassycontroller
CONTROLLERTEST=$(CONTROLLER)/tests

#
# Build variables
#
CC= gcc -std=c11
CFLAGS= -Werror -Wall -fvisibility=hidden -fno-builtin-memset -flto -s -O2 -I$(ALPACAINCLUDE) -I$(CRYPTINC) -I$(SNOW)
DBGCFLAGS= -Werror -Wall -DTALKATIVELLAMA -I$(ALPACAINCLUDE) -I$(CRYPTINC)
DBG= -g -DTALKATIVELLAMA
LFLAGS= -L$(CRYPTBASE)/lib -lm -pthread
TEST= -DSNOW_ENABLED
STATIC= -static
STATICBUILD-CFLAGS = -Werror -Wall -fvisibility=hidden -flto -s -O2 -fPIC -I$(ALPACAINCLUDE) -I$(CRYPTINC) -I$(SNOW)

#
# ALPACA-CORE object files
# Build out seperate objs for release, test, debug 
#
ALPACACORE_ROBJS=$(addprefix $(ALPACACORESRC)/, main.o server.o crypto.o sighandler.o allu.o)
ALPACACORE_TOBJS=$(addprefix $(ALPACACORESRC)/, main-test.o server-test.o crypto-test.o sighandler-test.o allu-test.o)
ALPACACORE_DOBJS=$(addprefix $(ALPACACORESRC)/, main-debug.o server-debug.o crypto-debug.o sighandler-debug.o allu-debug.o) 

#
# ALPACA-MULTITHREADSERVER object files
# Build out seperate objs for release, test, debug 
#
ALPACAMTHREADSERV_ROBJS=$(addprefix $(ALPACATHREADSRC)/, multithreadserver.o)
ALPACAMTHREADSERV_LOBJS=$(addprefix $(ALPACATHREADSRC)/, multithreadserver-PIC.o)
ALPACAMTHREADSERV_TOBJS=$(addprefix $(ALPACATHREADSRC)/, multithreadserver-test.o)
ALPACAMTHREADSERV_DOBJS=$(addprefix $(ALPACATHREADSRC)/, multithreadserver-debug.o) 

#
# ALPACA-THREADPOOL object files
# Build out seperate objs for release, test, debug 
#
ALPACATPOOL_ROBJS=$(addprefix $(ALPACATPOOLSRC)/, threadpool.o alpacaqueue.o) 
ALPACATPOOL_LOBJS=$(addprefix $(ALPACATPOOLSRC)/, threadpool-PIC.o alpacaqueue-PIC.o)
ALPACATPOOL_TOBJS=$(addprefix $(ALPACATPOOLSRC)/, threadpool-test.o alpacaqueue-test.o)
ALPACATPOOL_DOBJS=$(addprefix $(ALPACATPOOLSRC)/, threadpool-debug.o alpacaqueue-debug.o) 

#
# ALPACA-COMMS object files
# Build out seperate objs for release, test, debug 
#
ALPACACOMMS_ROBJS=$(addprefix $(ALPACACOMMSSRC)/, comms.o sock.o netutils.o) 
ALPACACOMMS_LOBJS=$(addprefix $(ALPACACOMMSSRC)/, comms-PIC.o sock-PIC.o netutils-PIC.o)
ALPACACOMMS_TOBJS=$(addprefix $(ALPACACOMMSSRC)/, comms-test.o sock-test.o netutils-test.o)
ALPACACOMMS_DOBJS=$(addprefix $(ALPACACOMMSSRC)/, comms-debug.o sock-debug.o netutils-debug.o)

#
# ALPACA-UTILS object files
# Build out seperate objs for release, test, debug 
#
ALPACAUTILS_ROBJS=$(addprefix $(ALPACAUTILSSRC)/, file_utils.o gen_utils.o) 
ALPACAUTILS_LOBJS=$(addprefix $(ALPACAUTILSSRC)/, file_utils-PIC.o gen_utils-PIC.o)
ALPACAUTILS_TOBJS=$(addprefix $(ALPACAUTILSSRC)/, file_utils-test.o gen_utils-test.o)
ALPACAUTILS_DOBJS=$(addprefix $(ALPACAUTILSSRC)/, file_utils-debug.o gen_utils-debug.o) 

#
# ALPACA-MEMORY object files
# Build out seperate objs for release, test, debug 
#
ALPACAMEM_ROBJS=$(addprefix $(ALPACAMEMORYSRC)/, alpaca_memory.o alpaca_buffer.o) 
ALPACAMEM_LOBJS=$(addprefix $(ALPACAMEMORYSRC)/, alpaca_memory-PIC.o alpaca_buffer-PIC.o)
ALPACAMEM_TOBJS=$(addprefix $(ALPACAMEMORYSRC)/, alpaca_memory-test.o alpaca_buffer-test.o)
ALPACAMEM_DOBJS=$(addprefix $(ALPACAMEMORYSRC)/, alpaca_memory-debug.o alpaca_buffer-debug.o) 

#
# Combining all modules into single varible
#
ALLROBJS = $(ALPACACORE_ROBJS)       \
		   $(ALPACAMTHREADSERV_ROBJS)\
		   $(ALPACATPOOL_ROBJS) 	 \
		   $(ALPACACOMMS_ROBJS) 	 \
		   $(ALPACAUTILS_ROBJS) 	 \
		   $(ALPACAMEM_ROBJS)

ALLTOBJS = $(ALPACACORE_TOBJS) 		 \
		   $(ALPACAMTHREADSERV_TOBJS)\
		   $(ALPACATPOOL_TOBJS)		 \
		   $(ALPACACOMMS_TOBJS)		 \
		   $(ALPACAUTILS_TOBJS)		 \
		   $(ALPACAMEM_TOBJS)

ALLDOBJS = $(ALPACACORE_DOBJS) 		 \
		   $(ALPACAMTHREADSERV_DOBJS)\
		   $(ALPACATPOOL_DOBJS)		 \
		   $(ALPACACOMMS_DOBJS)		 \
		   $(ALPACAUTILS_DOBJS)		 \
		   $(ALPACAMEM_DOBJS)		 

.PHONY: clean

all: clean init-dirs \
	 alpacalunch-release alpacalunch-release-test \
	 alpacalunch-debug \
	 misc \
	 scrub

release: clean init-dirs alpacalunch-release scrub

test: clean init-dirs alpacalunch-release-test runtest scrub

debug: clean init-dirs alpacalunch-debug scrub

#
# RELEASE, RELEASE TEST, RELEASE STATIC(broken) builds
#
alpacalunch-release: $(ALLROBJS)
	$(CC) $(CFLAGS) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-release-test: $(ALLTOBJS)
	$(CC) $(CFLAGS) $(TEST) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-release-static: $(ALLROBJS)
	$(CC) $(CFLAGS) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@

#
# DEBUG, DEBUG STATIC(broken) builds
#
alpacalunch-debug: $(ALLDOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-debug-static: $(ALLDOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(RELEASE) $< -o $@

%-test.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(TEST) $(DBG) $(RELEASE) $< -o $@

%-debug.o: %.c $(DEPS)
	$(CC) -c $(DBG) $(DBGCFLAGS) $< -o $@

runtest:
	@echo "\n***** RUNNING COMPONENT TESTS *****\n"
	@sudo python3 $(COMPONENTALL) 
	@echo "\n***** COMPONENT TESTS COMPLETE *****\n"
	
init-dirs:
	mkdir -p $(HASH)
	mkdir -p $(BIN)

misc:
	mkdir -p $(HASH)
	md5sum $(BIN)/alpaca* >> $(HASH)/MD5SUMS
	sha1sum $(BIN)/alpaca* >> $(HASH)/SHA1SUMS
	
scrub:
	rm -f $(ALPACACORESRC)/*.o $(ALPACATHREADSRC)/*.o  $(ALPACATPOOLSRC)/*.o $(TESTCOMPONENT)/*.pyc

clean:
	rm -fr $(BIN)/* $(ALPACACORESRC)/*.o $(ALPACATHREADSRC)/*.o  $(ALPACATPOOLSRC)/*.o \
	$(TESTCOMPONENT)/*.pyc $(CONTROLLERTEST)/*.pyc $(CONTROLLERSRC)/*.pyc
