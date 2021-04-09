To build:

All (release, debug, and unittest): make
Release: 				 			make release
Debug:   				 			make debug
Unit Tests:							make unittest [options]
Clean: 	 				 			make clean

All build options are enabled using <BUILDOPTION>= (1, Yes, yes, YES, True, true, TRUE)

Unit Build Options:


RUNUNIT - Run unit test
MEMCHECK - Run unit test with valgrind
TALKBUGGYTOME - Add "-DTALKATIVE_ALPACA" to build


Certs generated with:

	openssl req -newkey rsa:4096 -nodes -sha512 -x509 -days 3650 -nodes -out /home/ap/.certs/cert.pem -keyout /home/ap/.certs/private.pem
