#
# Path variables
#
DIR := ${CURDIR}
BIN = $(DIR)/binaries
HASH = $(BIN)/hashes
SRC = $(DIR)/src
INCLUDE= $(DIR)/include
TESTBASE= $(DIR)/tests
TESTCOMPONENT = $(TESTBASE)/component

#
# WolfSSL
#
CRYPTBASE= $(DIR)/crypt
CRYPTINC= $(CRYPTBASE)/include
CRYPTSTATIC= $(CRYPTBASE)/lib/libwolfssl.a
#
# Unit Test 
#
SNOW= $(TESTBASE)/unit

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
CFLAGS= -Werror -Wall -s -O2 -I$(INCLUDE) -I$(CRYPTINC) -I$(SNOW)
DBGCFLAGS= -Werror -Wall -DTALKATIVELLAMA -O2 -I$(INCLUDE) -I$(CRYPTINC)
DBG= -g3 -DTALKATIVELLAMA
LFLAGS= -L$(CRYPTBASE)/lib -lm -lpthread
TEST= -DSNOW_ENABLED
STATIC= -static

#
# Build out seperate objs for release, test, debug 
#
ROBJS=$(addprefix $(SRC)/, main.o server.o crypto.o sighandler.o ns.o)
TOBJS=$(addprefix $(SRC)/, main-test.o server-test.o crypto-test.o sighandler-test.o ns-test.o)
DOBJS=$(addprefix $(SRC)/, main-debug.o server-debug.o crypto-debug.o sighandler-debug.o ns-debug.o) 

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
alpacalunch-release: $(ROBJS)
	$(CC) $(CFLAGS) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-release-test: $(TOBJS)
	$(CC) $(CFLAGS) $(TEST) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-release-static: $(ROBJS)
	$(CC) $(CFLAGS) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@

#
# DEBUG, DEBUG STATIC(broken) builds
#
alpacalunch-debug: $(DOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

alpacalunch-debug-static: $(DOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@



%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(RELEASE) $< -o $@

%-test.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(TEST) $(DBG) $(RELEASE) $< -o $@

%-debug.o: %.c $(DEPS)
	$(CC) -c $(DBG) $(CFLAGS) $< -o $@

runtest:
	$(BIN)/alpacalunch-release-test

misc:
	mkdir -p $(HASH)
	md5sum $(BIN)/alpaca* >> $(HASH)/MD5SUMS
	sha1sum $(BIN)/alpaca* >> $(HASH)/SHA1SUMS
	
scrub:
	rm -f $(SRC)/*.o $(TESTCOMPONENT)/*.pyc

clean:
	rm -fr $(BIN)/* $(SRC)/*.o $(TESTCOMPONENT)/*.pyc $(CONTROLLERTEST)/*.pyc $(CONTROLLERSRC)/*.pyc