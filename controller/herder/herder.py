
import cmd2
import argparse


class Herder(cmd2.Cmd):
    """Main Client to connect to a hutch to interact with Alpacas Class"""

    connect_parser = argparse.ArgumentParser()
    connect_parser.add_argument('-u', '--username',
                                help='Username to connect to an Alpaca Hutch')
    # connect_parser.add_argument('-p', '--password',
    #                            help='Password associated with the selected username')
    connect_parser.add_argument('-s', '--server', action='append',
                                help='Servers to connect to. Can be IPs or Domain Names')
    connect_parser.add_argument('-p', '--port', default=443,
                                help='Port to connect to. Defaults to 443.')

    @cmd2.with_argparser(connect_parser)
    def do_connect(self, args):
        """Connects to a hutch"""
        print('Testing Connection...')
        print(args)
