from controller.herder.herder import Herder
import sys
from cmd2 import Cmd2ArgumentParser

if __name__ == "__main__":

    print('Welcome to Herder')

    cli_parser = Cmd2ArgumentParser()
    cli_parser.add_argument('-u', '--username',
                            help='Username to connect to an Alpaca Hutch')
    cli_parser.add_argument('-s', '--server', default='localhost',
                            help='Servers to connect to. Can be IPs or Domain Names')
    cli_parser.add_argument('-p', '--port', default=443,
                            help='Port to connect to. Defaults to 443.')

    herder = Herder(cli_parser.parse_args())

    sys.exit(Herder.cmdloop())
