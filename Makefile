#
# Path variables
#
DIR := ${CURDIR}
BIN = $(DIR)/binaries
HASH = $(BIN)/hashes
INFO = $(BIN)/bininfo

#
# Source directories
#
SRCBASE = $(DIR)/alpaca
ALPACACORESRC   = $(SRCBASE)/core
ALPACACOMMSSRC	= $(SRCBASE)/comms
ALPACATPOOLSRC  = $(SRCBASE)/threadpool
ALPACAUTILSSRC	= $(SRCBASE)/utilities
ALPACAMEMORYSRC = $(SRCBASE)/memory
ALPACATASKSSRC  = $(SRCBASE)/tasks

#
# TEST DIRECTORIES
#
TESTBASE= $(DIR)/tests
UNITTESTBASE = $(TESTBASE)/unittests
UNITTESTSRC = $(UNITTESTBASE)
COMPONENTBASE = $(TESTBASE)/component
COMPONENTALL = $(COMPONENTBASE)/allTest.py

#
# WolfSSL
#
CRYPTBASE= $(SRCBASE)/cryptlibs
CRYPTINC= $(CRYPTBASE)/include
CRYPTSTATIC= $(CRYPTBASE)/lib/libwolfssl.a
CRYPTSRC= $(DIR)/ext/wolfSSL/wolfssl-4.7.0

#
# Header Directories
#
ALPACAINCLUDE= $(SRCBASE)
UNITINCLUDE =  $(TESTBASE)

#
# Controller
#
CONTROLLER= $(DIR)/controller
CONTROLLERSRC= $(CONTROLLER)/sassycontroller
CONTROLLERTEST=$(CONTROLLER)/tests


#
# Build Time Options
#
# Building wolfSSL 
WOLFDEBUG=0 

# Building Unit Tests
MEMCHECK=0
RUNUNIT=0
TALKBUGGYTOME=0

# Building Debug
WOLFLOG=0

ifeq ($(WOLFLOG),$(filter $(WOLFLOG),1 Yes yes YES True true TRUE))
	DBGWOLF= -DTALKATIVE_WOLF
endif
ifeq ($(TALKBUGGYTOME),$(filter $(TALKBUGGYTOME),1 Yes yes YES True true TRUE))
	UNITDBG= -DTALKATIVE_ALPACA
endif
ifeq ($(WOLFDEBUG),$(filter $(WOLFDEBUG),1 Yes yes YES True true TRUE))
	WOLFOPT= --enable-debug
else
	WOLFOPT= --disable-debug
endif

#
# Valgrind
#
VALGRIND= valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes

#
# Build variables
# RELEASE Compilation and Lining
CC= gcc -std=c11
CFLAGS= -Werror -Wall -fvisibility=hidden -fno-builtin-memset -ffast-math -flto -s -O3 -I$(ALPACAINCLUDE) -I$(CRYPTINC)
LFLAGS= -L$(CRYPTBASE)/lib -lm -pthread -Wl,--gc-sections

# DEBUG Compilation and Lining
DBG= -g2 -Og -DTALKATIVE_ALPACA $(DBGWOLF)
DBGCFLAGS= -Werror -Wall -DTALKATIVE_ALPACA $(DBGWOLF) -I$(ALPACAINCLUDE) -I$(CRYPTINC)

# UNIT Test Compilation and Lining
UNIT= -g -O2
UNITCFLAGS= -Werror -Wall $(UNITDBG) -I$(ALPACAINCLUDE) -I$(CRYPTINC) -I$(UNITINCLUDE)
UNITLFLAGS= -lcunit -L$(UNITTESTBASE)

# Static Compilation and Lining
# Not Working
STATIC= -static
STATICBUILD-CFLAGS= -Werror -Wall -fvisibility=hidden -flto -s -O2 -fPIC -I$(ALPACAINCLUDE) -I$(CRYPTINC) 

#
# ALPACA-CORE main object files
# Build out seperate objs for release and debug 
# Unit tests utilize the main.c in tests/unittests
#
ALPACAMAIN_ROBJ=$(addprefix $(SRCBASE)/, main.o)
ALPACAMAIN_DOBJ=$(addprefix $(SRCBASE)/, main-debug.o)

#
# ALPACA-CORE object files
# Build out seperate objs for release, test, debug 
#
ALPACACORE_ROBJS=$(addprefix $(ALPACACORESRC)/, coreloop.o config.o)
ALPACACORE_DOBJS=$(addprefix $(ALPACACORESRC)/, coreloop-debug.o config-debug.o) 
ALPACACORE_UOBJS=$(addprefix $(ALPACACORESRC)/, coreloop-unit.o config-unit.o)

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
ALPACACOMMS_ROBJS=$(addprefix $(ALPACACOMMSSRC)/, comms.o wolf.o) 
ALPACACOMMS_DOBJS=$(addprefix $(ALPACACOMMSSRC)/, comms-debug.o wolf-debug.o)
ALPACACOMMS_UOBJS=$(addprefix $(ALPACACOMMSSRC)/, comms-unit.o wolf-unit.o)

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
# ALPACA-MEMORY object files
# Build out seperate objs for release, test, debug 
#
ALPACATASK_ROBJS=$(addprefix $(ALPACATASKSSRC)/, ) 
ALPACATASK_DOBJS=$(addprefix $(ALPACATASKSSRC)/, ) 
ALPACATASK_UOBJS=$(addprefix $(ALPACATASKSSRC)/, ) 

#
# ALPACA-UNITTESTS object files
# Build out seperate objs for release, test, debug 
#
ALPACAUNIT_UOBJS=$(addprefix $(UNITTESTSRC)/, alpacaunit_main-unit.o alpacaunit_memory-unit.o alpacaunit_comms-unit.o)

#
# Combining all modules into single varible
#
ALLROBJS = $(ALPACAMAIN_ROBJ)		 \
		   $(ALPACACORE_ROBJS)       \
		   $(ALPACATPOOL_ROBJS) 	 \
		   $(ALPACACOMMS_ROBJS) 	 \
		   $(ALPACAUTILS_ROBJS) 	 \
		   $(ALPACATASK_ROBJS) 	 	 \
		   $(ALPACAMEM_ROBJS)

ALLDOBJS = $(ALPACAMAIN_DOBJ)		 \
		   $(ALPACACORE_DOBJS) 		 \
		   $(ALPACATPOOL_DOBJS)		 \
		   $(ALPACACOMMS_DOBJS)		 \
		   $(ALPACAUTILS_DOBJS)		 \
		   $(ALPACATASK_DOBJS)		 \
		   $(ALPACAMEM_DOBJS)		

ALLUOBJS = $(ALPACACORE_UOBJS) 		 \
		   $(ALPACATPOOL_UOBJS)		 \
		   $(ALPACACOMMS_UOBJS)		 \
		   $(ALPACAUTILS_UOBJS)		 \
		   $(ALPACATASK_UOBJS)		 \
		   $(ALPACAMEM_UOBJS) 		 \
		   $(ALPACAUNIT_UOBJS)

.PHONY: clean

all: clean init-dirs \
	 alpacalunch-release  \
	 alpacalunch-debug 	  \
	 alpacalunch-unittest \
	 rununittest 	 \
	 scrub misc success   \

release:  init-dirs prescrub alpacalunch-release scrub misc success

debug: 	  init-dirs prescrub alpacalunch-debug scrub misc success

unittest: init-dirs prescrub alpacalunch-unittest scrub misc success rununittest



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
	$(CC) $(DBGCFLAGS) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@
	$(call PG, $@ Done)

alpacalunch-debug-static: $(ALLDOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $(STATIC) $^ $(CRYPTSTATIC) -o $(BIN)/$@

#
# Unit tests
#
alpacalunch-unittest: $(ALLUOBJS)
	$(call PG, Linking Unit Test Build)
	$(CC) $(UNITCFLAGS) $(UNIT) $^ $(CRYPTSTATIC) $(UNITLFLAGS) $(LFLAGS) -o $(BIN)/$@
	$(call PG, $@ Done)
	
%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $< -o $@

%-debug.o: %.c $(DEPS)
	$(CC) -c $(DBG) $(DBGCFLAGS) $< -o $@

%-unit.o: %.c $(DEPS)
	$(CC) -c $(UNIT) $(UNITCFLAGS) $< -o $@

runcomponenttest:
	@echo "\n***** RUNNING COMPONENT TESTS *****\n"
	#@sudo python3 $(COMPONENTALL) 
	@echo "\n***** COMPONENT TESTS COMPLETE *****\n"
	
rununittest:
ifeq ($(MEMCHECK),$(filter $(MEMCHECK),1 Yes yes YES True true TRUE))
		$(call PT, Running unit tests in valgrind...)
		$(VALGRIND) $(BIN)/alpacalunch-unittest
		$(call PG, $@ Done)

else ifeq ($(RUNUNIT),$(filter $(RUNUNIT),1 Yes yes YES True true TRUE))
		$(call PT, Running unit tests...)
		$(BIN)/alpacalunch-unittest
		$(call PG, $@ Done)
endif

init-dirs:
	mkdir -p $(HASH)
	mkdir -p $(INFO)
	mkdir -p $(BIN)

BINLIST = $(shell ls -p $(BIN) | grep -v /)
FINDTREE= $(shell which tree)
misc: 
	md5sum $(BIN)/alpaca* >> $(HASH)/MD5SUMS
	sha1sum $(BIN)/alpaca* >> $(HASH)/SHA1SUMS
	$(call READELF_LOOP, $(BINLIST))


wolf:
	rm -rf $(CRYPTBASE)/*
	rm -rf $(CRYPTSRC)
	unzip -o -q $(CRYPTSRC).zip -d $(DIR)/ext/wolfSSL
	$(call PY, $(WOLFOPT) is set)
	cd $(CRYPTSRC) && ./configure \
		--prefix=$(CRYPTBASE) \
		--disable-oldtls \
		--disable-examples \
		--disable-crypttests \
		--disable-fips \
		--disable-aescbc \
		--disable-des3 \
		--disable-md5 \
		--disable-pkcs12 \
		$(WOLFOPT) \
		--enable-static \
		--enable-harden \
		--enable-fastmath \
		--enable-keygen \
		--enable-certgen \
		--enable-tls13 \
		--enable-aesctr \
		--enable-ssh \
		CFLAGS="-std=c11 -O2 -ffunction-sections -fdata-sections -fPIC -flto" \
		LDFLAGS="-Wl,--gc-sections"

	make -C $(CRYPTSRC)
	make install -C $(CRYPTSRC)
	rm -rf $(CRYPTSRC)
	$(call PG, WolfSSL sucessfully recompiled)
	
clean:
	$(call PY, Full clean...)
	rm -fr $(BIN)/* $(ALLROBJS) $(ALLDOBJS) $(ALLUOBJS) $(CONTROLLERTEST)/*.pyc $(CONTROLLERSRC)/*.pyc

prescrub scrub:
	$(call PY, Cleaning...)
	rm -f $(ALLROBJS) $(ALLDOBJS) $(ALLUOBJS)
	
success:
	$(call PG, BUILD COMPLETE) 
	@tree $(BIN)

# Functions
# Print Yellow
define PY
	@echo "\033[38;5;227m[*] $(1) \033[0m"
endef
# Print Yellow
define PT
	@echo "\033[38;5;081m[>] $(1) \033[0m"
endef
# Print Green
define PG
	@echo "\033[38;5;084m[+] $(1) \033[0m"
endef
# For fast builds
define SCRUB
	$(call PY, Cleaning...)
	rm -f $(1)
endef

define READELF_LOOP
	@for file in $(1) ; do \
	  out=$$file-readelf; \
	  readelf -a $(BIN)/$$file > $(INFO)/$$out; \
	done
endef
