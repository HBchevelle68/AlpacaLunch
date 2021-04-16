import socket
import wolfssl
import logging
import struct
import time


class Hutch:
    herder_socket = None
    herder_tls_context = None
    herder_tls_socket = None

    def __init__(self, addr='0.0.0.0', port=6969):
        """A server to facilitate interactions between herders and alpacas

        :param addr: Bind socket address for herder connections
        :param port: Bind socket port for herder connections
        """

        # Setup logging
        # TODO Setup init to handle different logging levels
        logging.basicConfig(level=logging.DEBUG)
        self.logger = logging.getLogger(__name__)
        self.logger.debug(f"Server console logging set to DEBUG level")

        # Setup the primary communications channel to hutch from herders
        # TODO Might think about using a configuration file to load server config

        self.herder_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.herder_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.herder_socket.bind((addr, port))
        self.herder_socket.listen(5)

        self.logger.debug(f"Server set to listen on {addr}:{port}")

        # TODO Should be able to handle any TLS Level - maybe use straight python sockets
        self.herder_tls_context = wolfssl.SSLContext(wolfssl.PROTOCOL_TLSv1_2, server_side=True)

        # TODO Should the server generate a cert if it doesn't have one or should it be an option
        self.herder_tls_context.verify_mode = wolfssl.CERT_NONE
        self.herder_tls_context.load_cert_chain("/home/chris/repos/AlpacaLunch/controller/hutch/hutch-cert-dungpile.pem", "/home/chris/repos/AlpacaLunch/controller/hutch/hutch-key-dungpile.pem")
        
        self.main()

    def main(self):
        """Main control loop for Hutch
        """
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

                new_socket, from_addr = self.herder_socket.accept()
                print("Connection received from", from_addr)

                # wrap socket
                secure_socket = self.herder_tls_context.wrap_socket(new_socket)
                for i in range(0,5,1):
                    print("sending payload...")
                    _tmp_payload = self.buildCustomPayload(5432122,1,0,17,9123,f"Its your number {i} boi...")
                    secure_socket.write(_tmp_payload)
                    #print("sleeping...")
                    #time.sleep(6)
                #print("\n", secure_socket.read(), "\n")

            except Exception as e:
                print()
                break

            finally:
                if secure_socket:
                    secure_socket.close()

        self.herder_socket.close()

    def buildCustomPayload(self, AlpacaID, flags, reserved, cmdNum, cmdID, body):
        _bodySize = len(body)
        _header_size = struct.calcsize("!IBBHHI")
        _payload_size = _header_size + _bodySize
        _payload_header = struct.pack("!IBBHHI", AlpacaID, flags, reserved, cmdNum, cmdID, _bodySize)
        self.logger.debug(f'HEADER SIZE: {_header_size}\nBODY SIZE: {_bodySize}\nPAYLOAD SIZE: {_payload_size}')
        self.logger.debug(f'Header: {_payload_header}')
        return _payload_header + body.encode()
