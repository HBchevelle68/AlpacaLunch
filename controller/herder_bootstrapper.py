from controller.herder.herder import Herder
import sys
from cmd2 import Cmd2ArgumentParser

if __name__ == "__main__":

    cli_parser = Cmd2ArgumentParser()
    cli_parser.add_argument('-u', '--username',
                            help='Username to connect to an Alpaca Hutch')
    cli_parser.add_argument('-s', '--server', default='localhost',
                            help='Servers to connect to. Can be IPs or Domain Names')
    cli_parser.add_argument('-p', '--port', default=6969, type=int,
                            help='Port to connect to. Defaults to 443.')
    args = cli_parser.parse_args()
    herder = Herder(args.username, args.server, args.port)

    sys.exit(herder.cmdloop())
