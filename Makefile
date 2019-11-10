#
# Path variables
#
DIR := ${CURDIR}
BIN = $(DIR)/binaries
HASH = $(BIN)/hashes

#
# Source directories
#
SRCBASE = $(DIR)/alpaca
ALPACACORE =$(SRCBASE)/core

#
# Header Directories
#
COREINCLUDE= $(SRCBASE)/coreinc

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
CFLAGS= -Werror -Wall -s -O2 -I$(COREINCLUDE) -I$(CRYPTINC) -I$(SNOW)
DBGCFLAGS= -Werror -Wall -DTALKATIVELLAMA -O2 -I$(COREINCLUDE) -I$(CRYPTINC)
DBG= -g3 -DTALKATIVELLAMA
LFLAGS= -L$(CRYPTBASE)/lib -lm -lpthread
TEST= -DSNOW_ENABLED
STATIC= -static

#
# ALPACA-CORE
# Build out seperate objs for release, test, debug 
#
ALPACACORE_ROBJS=$(addprefix $(ALPACACORE)/, main.o server.o crypto.o sighandler.o allu.o)
ALPACACORE_TOBJS=$(addprefix $(ALPACACORE)/, main-test.o server-test.o crypto-test.o sighandler-test.o allu-test.o)
ALPACACORE_DOBJS=$(addprefix $(ALPACACORE)/, main-debug.o server-debug.o crypto-debug.o sighandler-debug.o allu-debug.o) 

.PHONY: clean

all: clean \
	 alpacalunch-release alpacalunch-release-test \
	 alpacalunch-debug \
	 misc \
	 scrub

release: clean alpacalunch-release scrub

test: clean alpacalunch-release-test runtest scrub

debug: clean alpacalunch-debug scrub


#
# RELEASE, RELEASE TEST, RELEASE STATIC(broken) builds
#
alpacalunch-release: $(ALPACACORE_ROBJS)
	$(CC) $(CFLAGS) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-release-test: $(ALPACACORE_TOBJS)
	$(CC) $(CFLAGS) $(TEST) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-release-static: $(ALPACACORE_ROBJS)
	$(CC) $(CFLAGS) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@

#
# DEBUG, DEBUG STATIC(broken) builds
#
alpacalunch-debug: $(ALPACACORE_DOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-debug-static: $(ALPACACORE_DOBJS)
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
misc:
	mkdir -p $(HASH)
	md5sum $(BIN)/alpaca* >> $(HASH)/MD5SUMS
	sha1sum $(BIN)/alpaca* >> $(HASH)/SHA1SUMS
	
scrub:
	rm -f $(ALPACACORE)/*.o $(TESTCOMPONENT)/*.pyc

clean:
	rm -fr $(BIN)/* $(ALPACACORE)/*.o $(TESTCOMPONENT)/*.pyc $(CONTROLLERTEST)/*.pyc $(CONTROLLERSRC)/*.pyc