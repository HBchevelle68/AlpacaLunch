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
CFLAGS= -Werror -Wall -fvisibility=hidden -fno-builtin-memset -ffast-math -flto -s -O3 -I$(ALPACAINCLUDE) -I$(CRYPTINC) -I$(SNOW)
DBGCFLAGS= -Werror -Wall -DTALKATIVELLAMA  -I$(ALPACAINCLUDE) -I$(CRYPTINC) #-fanalyzer
DBG= -g2 -DTALKATIVELLAMA
LFLAGS= -L$(CRYPTBASE)/lib -lm -pthread -Wl,--gc-sections
TEST= -DSNOW_ENABLED
STATIC= -static
STATICBUILD-CFLAGS = -Werror -Wall -fvisibility=hidden -flto -s -O2 -fPIC -I$(ALPACAINCLUDE) -I$(CRYPTINC) -I$(SNOW)

#
# ALPACA-CORE object files
# Build out seperate objs for release, test, debug 
#
ALPACACORE_ROBJS=$(addprefix $(ALPACACORESRC)/, main.o crypto.o sighandler.o allu.o devtests.o)
ALPACACORE_TOBJS=$(addprefix $(ALPACACORESRC)/, main-test.o crypto-test.o sighandler-test.o allu-test.o devtests-test.o)
ALPACACORE_DOBJS=$(addprefix $(ALPACACORESRC)/, main-debug.o crypto-debug.o sighandler-debug.o allu-debug.o devtests-debug.o) 

#
# ALPACA-MULTITHREADSERVER object files
# Build out seperate objs for release, test, debug 
#
ALPACAMTHREADSERV_ROBJS=$(addprefix $(ALPACATHREADSRC)/, multithreadserver.o)
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
ALPACACOMMS_ROBJS=$(addprefix $(ALPACACOMMSSRC)/, comms.o sock.o wolf.o) 
ALPACACOMMS_TOBJS=$(addprefix $(ALPACACOMMSSRC)/, comms-test.o sock-test.o wolf-test.o)
ALPACACOMMS_DOBJS=$(addprefix $(ALPACACOMMSSRC)/, comms-debug.o sock-debug.o wolf-debug.o)

#
# ALPACA-UTILS object files
# Build out seperate objs for release, test, debug 
#
ALPACAUTILS_ROBJS=$(addprefix $(ALPACAUTILSSRC)/, file_utils.o gen_utils.o) 
ALPACAUTILS_TOBJS=$(addprefix $(ALPACAUTILSSRC)/, file_utils-test.o gen_utils-test.o)
ALPACAUTILS_DOBJS=$(addprefix $(ALPACAUTILSSRC)/, file_utils-debug.o gen_utils-debug.o) 

#
# ALPACA-MEMORY object files
# Build out seperate objs for release, test, debug 
#
ALPACAMEM_ROBJS=$(addprefix $(ALPACAMEMORYSRC)/, alpaca_memory.o alpaca_buffer.o) 
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
	 alpacalunch-release\
	 alpacalunch-debug \
	 scrub misc success

release: init-dirs alpacalunch-release scrub success

test: init-dirs alpacalunch-release-test runtest

debug: init-dirs alpacalunch-debug scrub success

#
# RELEASE, RELEASE TEST, RELEASE STATIC(broken) builds
#
alpacalunch-release: $(ALLROBJS)
	$(call PG,Linking Release Build)
	$(CC) $(CFLAGS) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@
	$(call PG,Done)

alpacalunch-release-test: $(ALLTOBJS)
	$(CC) $(CFLAGS) $(TEST) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-release-static: $(ALLROBJS)
	$(CC) $(CFLAGS) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@

#
# DEBUG, DEBUG STATIC(broken) builds
#
alpacalunch-debug: $(ALLDOBJS)
	$(call PG,Linking Debug Build)
	$(CC) $(DBGCFLAGS) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@
	$(call PG,Done)

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
	

clean:
	rm -fr $(BIN)/* $(ALLROBJS) $(ALLDOBJS) $(ALLTOBJS) $(CONTROLLERTEST)/*.pyc $(CONTROLLERSRC)/*.pyc

scrub:
	$(call PY,Cleaning *.o's)
	rm -f $(ALLROBJS) $(ALLDOBJS) $(ALLTOBJS) 

success:
	@echo "\033[38;5;084m[+] BUILD COMPLETE\033[0m"


#
# Functions
#

define PY
	@echo "\033[1;93m[*] $(1) \033[0m"
endef
define PG
	@echo "\033[38;5;084m[+] $(1) \033[0m"
endef



