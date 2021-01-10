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
CFLAGS= -Werror -Wall -fvisibility=hidden -fno-builtin-memset -ffast-math -flto -s -O3 -I$(ALPACAINCLUDE) -I$(CRYPTINC)
DBGCFLAGS= -Werror -Wall -DTALKATIVELLAMA  -I$(ALPACAINCLUDE) -I$(CRYPTINC)
DBG= -g2 -Og -DTALKATIVELLAMA
LFLAGS= -L$(CRYPTBASE)/lib -lm -pthread -Wl,--gc-sections
UNITTEST= -lcunit
STATIC= -static
STATICBUILD-CFLAGS = -Werror -Wall -fvisibility=hidden -flto -s -O2 -fPIC -I$(ALPACAINCLUDE) -I$(CRYPTINC) 

#
# ALPACA-CORE object files
# Build out seperate objs for release, test, debug 
#
ALPACACORE_ROBJS=$(addprefix $(ALPACACORESRC)/, main.o crypto.o sighandler.o allu.o devtests.o)
ALPACACORE_DOBJS=$(addprefix $(ALPACACORESRC)/, main-debug.o crypto-debug.o sighandler-debug.o allu-debug.o devtests-debug.o) 
ALPACACORE_UOBJS=$(addprefix $(ALPACACORESRC)/, main-unit.o crypto-unit.o sighandler-unit.o allu-unit.o devtests-unit.o)
#
# ALPACA-MULTITHREADSERVER object files
# Build out seperate objs for release, test, debug 
#
ALPACAMTHREADSERV_ROBJS=$(addprefix $(ALPACATHREADSRC)/, multithreadserver.o)
ALPACAMTHREADSERV_DOBJS=$(addprefix $(ALPACATHREADSRC)/, multithreadserver-debug.o) 

#
# ALPACA-THREADPOOL object files
# Build out seperate objs for release, test, debug 
#
ALPACATPOOL_ROBJS=$(addprefix $(ALPACATPOOLSRC)/, threadpool.o alpacaqueue.o) 
ALPACATPOOL_DOBJS=$(addprefix $(ALPACATPOOLSRC)/, threadpool-debug.o alpacaqueue-debug.o) 
ALPACATPOOL_UOBJS=$(addprefix $(ALPACATPOOLSRC)/, threadpool-unit.o alpacaqueue-unit.o) 

#
# ALPACA-COMMS object files
# Build out seperate objs for release, test, debug 
#
ALPACACOMMS_ROBJS=$(addprefix $(ALPACACOMMSSRC)/, comms.o sock.o wolf.o) 
ALPACACOMMS_DOBJS=$(addprefix $(ALPACACOMMSSRC)/, comms-debug.o sock-debug.o wolf-debug.o)
ALPACACOMMS_UOBJS=$(addprefix $(ALPACACOMMSSRC)/, comms-unit.o sock-unit.o wolf-unit.o)

#
# ALPACA-UTILS object files
# Build out seperate objs for release, test, debug 
#
ALPACAUTILS_ROBJS=$(addprefix $(ALPACAUTILSSRC)/, file_utils.o gen_utils.o) 
ALPACAUTILS_DOBJS=$(addprefix $(ALPACAUTILSSRC)/, file_utils-debug.o gen_utils-debug.o) 
ALPACAUTILS_UOBJS=$(addprefix $(ALPACAUTILSSRC)/, file_utils-unit.o gen_utils-unit.o) 

#
# ALPACA-MEMORY object files
# Build out seperate objs for release, test, debug 
#
ALPACAMEM_ROBJS=$(addprefix $(ALPACAMEMORYSRC)/, alpaca_memory.o alpaca_buffer.o) 
ALPACAMEM_DOBJS=$(addprefix $(ALPACAMEMORYSRC)/, alpaca_memory-debug.o alpaca_buffer-debug.o) 
ALPACAMEM_UOBJS=$(addprefix $(ALPACAMEMORYSRC)/, alpaca_memory-unit.o alpaca_buffer-unit.o) 

#
# Combining all modules into single varible
#
ALLROBJS = $(ALPACACORE_ROBJS)       \
		   $(ALPACAMTHREADSERV_ROBJS)\
		   $(ALPACATPOOL_ROBJS) 	 \
		   $(ALPACACOMMS_ROBJS) 	 \
		   $(ALPACAUTILS_ROBJS) 	 \
		   $(ALPACAMEM_ROBJS)

ALLDOBJS = $(ALPACACORE_DOBJS) 		 \
		   $(ALPACAMTHREADSERV_DOBJS)\
		   $(ALPACATPOOL_DOBJS)		 \
		   $(ALPACACOMMS_DOBJS)		 \
		   $(ALPACAUTILS_DOBJS)		 \
		   $(ALPACAMEM_DOBJS)		

ALLUOBJS = $(ALPACACORE_UOBJS) 		 \
		   $(ALPACAMTHREADSERV_UOBJS)\
		   $(ALPACATPOOL_UOBJS)		 \
		   $(ALPACACOMMS_UOBJS)		 \
		   $(ALPACAUTILS_UOBJS)		 \
		   $(ALPACAMEM_UOBJS) 

.PHONY: release debug unittest clean scrub

all: clean init-dirs \
	 alpacalunch-release\
	 alpacalunch-debug \
	 alpacalunch-unittest \
	 scrub misc success

release: init-dirs scrub alpacalunch-release scrub success

debug: init-dirs scrub alpacalunch-debug scrub success

unittest: init-dirs scrub alpacalunch-unittest scrub success

#
# RELEASE, RELEASE STATIC(broken) builds
#
alpacalunch-release: $(ALLROBJS)
	$(call PG, Linking Release Build)
	$(CC) $(CFLAGS) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@
	$(call PG, $@ Done)

alpacalunch-release-static: $(ALLROBJS)
	$(CC) $(CFLAGS) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@

#
# DEBUG, DEBUG STATIC(broken) builds
#
alpacalunch-debug: $(ALLDOBJS)
	$(call PG, Linking Debug Build)
	$(CC) $(DBGCFLAGS) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) $(UNITTEST) -o $(BIN)/$@
	$(call PG, $@ Done)

alpacalunch-debug-static: $(ALLDOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@

#
# DEBUG, DEBUG STATIC(broken) builds
#
alpacalunch-unittest: $(ALLUOBJS)
	$(call PG, Linking Unit Test Build)
	$(CC) $(DBGCFLAGS) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) $(UNITTEST) -o $(BIN)/$@
	$(call PG, $@ Done)

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(RELEASE) $< -o $@

%-debug.o: %.c $(DEPS)
	$(CC) -c $(DBG) $(DBGCFLAGS) $< -o $@

%-unit.o: %.c $(DEPS)
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
	rm -fr $(BIN)/* $(ALLROBJS) $(ALLDOBJS) $(ALLUOBJS) $(CONTROLLERTEST)/*.pyc $(CONTROLLERSRC)/*.pyc

scrub:
	$(call PY, Cleaning...)
	rm -f $(ALLROBJS) $(ALLDOBJS) $(ALLUOBJS)
	
success:
	$(call PG, BUILD COMPLETE) 

#
# Functions
#

# Print Yellow
define PY
	@echo "\033[1;93m[*] $(1) \033[0m"
endef
# Print Green
define PG
	@echo "\033[38;5;084m[+] $(1) \033[0m"
endef





