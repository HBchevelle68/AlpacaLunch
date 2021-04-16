import socket
import wolfssl
import time
import struct

bind_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
bind_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
bind_socket.bind(("127.0.0.1", 54321))
bind_socket.listen(5)

context = wolfssl.SSLContext(wolfssl.PROTOCOL_TLSv1_2, server_side=True)
context.verify_mode = wolfssl.CERT_NONE

context.load_cert_chain("/home/chris/repos/AlpacaLunch/controller/hutch/hutch-cert-dungpile.pem", "/home/chris/repos/AlpacaLunch/controller/hutch/hutch-key-dungpile.pem")

#typedef struct __attribute__((packed)) AlpacaLunch_Protocol_Header {
#	uint32_t alpacaID;
#	uint8_t  flags;
#	uint8_t  reserved; 
#	uint16_t cmdNum;
#	uint16_t cmdID;
#	uint32_t bodySize;
#} Alpaca_protoHdr_t;

# Some dummy values
AlpacaID = 5432122
flags = 1
reserved = 0
cmdNum = 17
cmdID = 9123


# create header + payload
body = "This is a test_"*200
bodySize = len(body)
header_size = struct.calcsize("!IBBHHI")
paylaod_size = header_size + bodySize
payload_header = struct.pack("!IBBHHI", AlpacaID,flags,reserved,cmdNum,cmdID,bodySize)
payload = payload_header + body.encode()

# Some debug prints
print(f'HEADER SIZE: {header_size}\nBODY SIZE: {bodySize}\nPAYLOAD SIZE: {paylaod_size}')
print(f'Header: {payload_header}')

while True:
    try:
        secure_socket = None

        new_socket, from_addr = bind_socket.accept()
        print("Connection received from", from_addr)

        # wrap socket
        secure_socket = context.wrap_socket(new_socket)
        for i in range(0,5,1):
            print("sending payload...")
            secure_socket.write(payload)
            #print("sleeping...")
            #time.sleep(6)
        #print("\n", secure_socket.read(), "\n")

    except Exception as e:
        print()
        break

    finally:
        if secure_socket:
            secure_socket.close()

bind_socket.close()