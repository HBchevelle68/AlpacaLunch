import socket
import cmd2
import wolfssl
import wolfcrypt
import getpass
import sys
import argparse
from controller.herder.herder_auth import HerderAuth


class Herder(cmd2.Cmd):
    """Main Client to connect to a hutch to interact with Alpacas Class
    """

    def __init__(self, username, server='localhost', port=6969):
        """A command line tool to control your ever growing herd of Alpacas through various hutches.

        :param username: username for authentication to a hutch
        :param server: ip or hostname of the hutch to connect to, if not specified, defaults to localhost
        :param port: port to use when connecting ot the hutch
        """
        print('Welcome to Herder!')
        _password = getpass.getpass('Please enter your hutch password: ')

        self.hutch_connect(server, port, username, _password)

        # Initialize cmd2
        super().__init__(allow_cli_args=False)

    def hutch_connect(self, server, port, username, password, timeout=5):
        """A command line tool to control your ever growing herd of Alpacas through various hutches.

        :param server: ip or hostname of the hutch to connect to
        :param port: port to use when connecting to the hutch
        :param username: username for authentication to a hutch
        :param password: plaintext password for authentication to a hutch
        :param timeout: socket timeout for connection attempt, if not specified, defaults to 5 seconds
        """

        hutch_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        context = wolfssl.SSLContext(wolfssl.PROTOCOL_TLSv1_2)
        context.verify_mode = wolfssl.CERT_NONE
        #context.load_verify_locations(cafile="./hutch-cert-dungpile.crt")

        hutch_secure_socket = context.wrap_socket(hutch_socket)

        try:
            hutch_secure_socket.connect((server, port))

            print("Sucessfully Connected!")

            hutch_secure_socket.write("Testing 1 2 3...\n")

            print(hutch_secure_socket.read())

        except socket.error as error:
            print("Error: exception received...")
            print(error)



    # @cmd2.with_argparser(connect_parser)
    def do_connect(self, args):
        """Connects to a hutch"""
        print('Testing Connection...')
        print(args)
