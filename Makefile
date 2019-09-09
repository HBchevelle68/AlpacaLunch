#
# Path variables
#
DIR := ${CURDIR}
BIN = $(DIR)/binaries
SRC = $(DIR)/src
INCLUDE= $(DIR)/include
TESTBASE= $(DIR)/tests
TESTCOMPONENT = $(TESTBASE)/component
CRYPTBASE= $(DIR)/crypt
CRYPTINC= $(CRYPTBASE)/include
CRYPTSTATIC= $(CRYPTBASE)/lib/libwolfssl.a
SNOW= $(TESTBASE)/lib

#
# Build variables
#
CC= gcc
CFLAGS= -Werror -Wall -s -O2 -I$(INCLUDE) -I$(CRYPTINC) -I$(SNOW)
DBGCFLAGS= -Werror -Wall -DNETDEBUG -O2 -I$(INCLUDE) -I$(CRYPTINC)
DBG= -g3 -DNETDEBUG
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
	 netplay-release netplay-release-test \
	 netplay-debug \
	 misc \
	 scrub

release: clean netplay-release scrub

test: clean netplay-release-test runtest scrub

debug: clean netplay-debug scrub


#
# RELEASE, RELEASE TEST, RELEASE STATIC(broken) builds
#
netplay-release: $(ROBJS)
	$(CC) $(CFLAGS) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

netplay-release-test: $(TOBJS)
	$(CC) $(CFLAGS) $(TEST) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

netplay-release-static: $(ROBJS)
	$(CC) $(CFLAGS) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@

#
# DEBUG, DEBUG STATIC(broken) builds
#
netplay-debug: $(DOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

netplay-debug-static: $(DOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@



%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(RELEASE) $< -o $@

%-test.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(TEST) $(DBG) $(RELEASE) $< -o $@

%-debug.o: %.c $(DEPS)
	$(CC) -c $(DBG) $(CFLAGS) $< -o $@

runtest:
	$(BIN)/netplay-release-test

misc:
	md5sum $(BIN)/* >> $(BIN)/MD5SUMS
	sha1sum $(BIN)/* >> $(BIN)/SHA1SUMS
	
scrub:
	rm -f $(SRC)/*.o $(TESTCOMPONENT)/*.pyc

clean:
	rm -fr $(BIN)/* $(SRC)/*.o $(TESTCOMPONENT)/*.pyc