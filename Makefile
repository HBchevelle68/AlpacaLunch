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
ALPACATHREADSRC = $(SRCBASE)/multithreadserver
ALPACATPOOLSRC  = $(SRCBASE)/threadpool


#
# Header Directories
#
ALPACAINCLUDE= $(SRCBASE)
#ALPACAINTERFACE = $(SRCBASE)/interfaces

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
CC= gcc
CFLAGS= -Werror -Wall -s -O2 -I$(ALPACAINCLUDE) -I$(CRYPTINC) -I$(SNOW)
DBGCFLAGS= -Werror -Wall -DTALKATIVELLAMA -O2 -I$(ALPACAINCLUDE) -I$(CRYPTINC)
DBG= -g3 -DTALKATIVELLAMA
LFLAGS= -L$(CRYPTBASE)/lib -lm -lpthread
TEST= -DSNOW_ENABLED
STATIC= -static
STATICBUILD-CFLAGS = -Werror -Wall -s -O2 -fPIC -I$(ALPACAINCLUDE) -I$(CRYPTINC) -I$(SNOW)

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
alpacalunch-release: $(ALPACACORE_ROBJS) $(ALPACAMTHREADSERV_ROBJS) $(ALPACATPOOL_ROBJS)
	$(CC) $(CFLAGS) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-release-test: $(ALPACACORE_TOBJS) $(ALPACAMTHREADSERV_TOBJS) $(ALPACATPOOL_TOBJS)
	$(CC) $(CFLAGS) $(TEST) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-release-static: $(ALPACACORE_ROBJS) $(ALPACAMTHREADSERV_ROBJS) $(ALPACATPOOL_ROBJS)
	$(CC) $(CFLAGS) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@

#
# DEBUG, DEBUG STATIC(broken) builds
#
alpacalunch-debug: $(ALPACACORE_DOBJS) $(ALPACAMTHREADSERV_DOBJS) $(ALPACATPOOL_DOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-debug-static: $(ALPACACORE_DOBJS) $(ALPACAMTHREADSERV_DOBJS) $(ALPACATPOOL_DOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@



%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(RELEASE) $< -o $@

%-test.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(TEST) $(DBG) $(RELEASE) $< -o $@

%-debug.o: %.c $(DEPS)
	$(CC) -c $(DBG) $(CFLAGS) $< -o $@



runtest:
	sudo python3 $(COMPONENTALL) 
	

init-dirs:
	mkdir -p $(HASH)
	mkdir -p $(BIN)
	mkdir -p $(BUILD)
misc:
	mkdir -p $(HASH)
	md5sum $(BIN)/alpaca* >> $(HASH)/MD5SUMS
	sha1sum $(BIN)/alpaca* >> $(HASH)/SHA1SUMS
	
scrub:
	rm -f $(ALPACACORESRC)/*.o $(ALPACATHREADSRC)/*.o  $(ALPACATPOOLSRC)/*.o $(TESTCOMPONENT)/*.pyc

clean:
	rm -fr $(BIN)/* $(ALPACACORESRC)/*.o $(ALPACATHREADSRC)/*.o  $(ALPACATPOOLSRC)/*.o \
	$(TESTCOMPONENT)/*.pyc $(CONTROLLERTEST)/*.pyc $(CONTROLLERSRC)/*.pyc
