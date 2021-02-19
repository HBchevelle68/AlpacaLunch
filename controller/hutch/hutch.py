import socket
import wolfssl
import logging


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
        logging.getLogger(__name__)

        # Setup the primary communications channel to hutch from herders
        # TODO Might think about using a configuration file to load server config

        _bind_addr = addr
        _bind_port = port

        self.herder_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.herder_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.herder_socket.bind((_bind_addr, _bind_port))
        self.herder_socket.listen(5)

        self.herder_tls_context = wolfssl.SSLContext(wolfssl.PROTOCOL_TLSv1_2, server_side=True)

        # TODO Should the server generate a cert if it doesn't have one or should it be an option
        self.herder_tls_context.verify_mode = wolfssl.CERT_NONE
        self.herder_tls_context.load_cert_chain("/home/chris/repos/AlpacaLunch/controller/hutch/hutch-cert-dungpile.pem", "/home/chris/repos/AlpacaLunch/controller/hutch/hutch-key-dungpile.pem")
        
        self.main()

    def main(self):
        """Main control loop for Hutch
        """
        while True:
            try:
                self.herder_tls_socket = None

                new_socket, from_addr = self.herder_socket.accept()

                print("New connection attempt from ", from_addr)

                self.herder_tls_socket = self.herder_tls_context.wrap_socket(new_socket)

                print("Encrypted connection established with ", from_addr)

                print("\n", self.herder_tls_socket.read(), "\n")
                self.herder_tls_socket.write(b"I hear you fa shizzle!")

            except KeyboardInterrupt:
                print()
                break

            finally:
                if self.herder_tls_socket:
                    self.herder_tls_socket.close()

        self.herder_socket.close()
