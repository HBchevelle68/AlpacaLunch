
import cmd2
import getpass
import argparse
from controller.herder.herder_auth import HerderAuth

class Herder(cmd2.Cmd):
    """Main Client to connect to a hutch to interact with Alpacas Class"""

    def __init__(self, params):

        print('Welcome to Herder!')
        print(params.username)
        print(params.port)
        print(params.server)



        # username = input("Username: ")
        # password = getpass.getpass()



    #@cmd2.with_argparser(connect_parser)
    def do_connect(self, args):
        """Connects to a hutch"""
        print('Testing Connection...')
        print(args)
