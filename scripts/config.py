import argparse
import sys
import os

def __validate_ipv4_address(address):
    try:
        socket.inet_pton(socket.AF_INET, address)
    except AttributeError:  # no inet_pton here, sorry
        try:
            socket.inet_aton(address)
        except socket.error:
            return False
        return address.count('.') == 3
    except socket.error:  # not a valid address
        return False

    return True

def __validate_files(infile, outfile):
    if not os.path.exists(infile):
        raise FileNotFoundError(f'{infile} was not found')
    
    elif not os.path.isfile(infile):
        raise ValueError(f'{infile} not a regular file')

    if not os.path.exists(outfile):
        raise FileNotFoundError(f'{outfile} was not found')

    elif not os.path.isfile(outfile):
        raise ValueError(f'{outfile} not a regular file')

def __find_magic(infile_bytes):
    pass
    

def _validate_args(args):
    
    __validate_files(args.infile, args.outfile)


def configure(args):
    print(f'{args}')
    if not _validate_args(args):
        return

if __name__ == "__main__":



    parser = argparse.ArgumentParser()
    parser.add_argument("infile", type=str, help="Vanilla Alpaca to configure")
    parser.add_argument("outfile", type=str, help="Output of configuration")
    parser.add_argument("--listen", dest="listen", action='store_true',  required=False, help="Set initial behavior to listen")
    parser.add_argument("--callback", dest="callback", action='store_true', required=False, help="Set initial behavior to callback")
    parser.add_argument("-p","--port", dest= "port", required=True, type=int, help="Callback port/Listen port")
    parser.add_argument("-a","--address", dest="addr", required=False, type=str)
    configure(parser.parse_args())


