To build:

All (release and debug): make
Release: 				 make release
Debug:   				 make debug
Built + Test: 			 make test
Clean: 	 				 make clean


Certs generated with:

	openssl req -newkey rsa:4096 -nodes -sha512 -x509 -days 3650 -nodes -out /home/ap/.certs/cert.pem -keyout /home/ap/.certs/private.pem
