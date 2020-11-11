
import cmd2

class Controller(cmd2.Cmd):
    """Main Controller Class"""

if __name__ == "__main__":
    import sys
    c = Controller()
    sys.exit(c.cmdloop())