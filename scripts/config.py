import argparse
import sys
import os
import socket

class Config:
    HDR_MAGIC = b'WwXxYyZz'
    FTR_MAGIC = b'wWxXyYzZ'
    USABLE_CONF_SIZE = 19
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def __validate_ipv4_address(address):
    try:
        socket.inet_pton(socket.AF_INET, address)
    except AttributeError:  
        try:
            socket.inet_aton(address)
        except socket.error:
            return False
        return address.count('.') == 3
    except socket.error:
        return False

    return True


def __validate_port(port):

    if port > 65535 or port < 4096:
        raise ValueError(Config.FAIL+f'Port {port} is not between required (4096,65535)'+Config.ENDC)
    


def __validate_file(infile):
    if not os.path.exists(infile):
        raise FileNotFoundError(f'{infile} was not found')
    
    elif not os.path.isfile(infile):
        raise ValueError(f'{infile} not a regular file')
    

def __find_magic(infile):

    with open(infile, 'rb') as vanilla_bin:

        v_bytes = vanilla_bin.read()

        hdr = v_bytes.find(Config.HDR_MAGIC)
        if hdr == -1:
            print(Config.FAIL+"[-] Couldn't find magic header..."+Config.ENDC)
            return hdr

        print(Config.OKGREEN+f'[+] Found {Config.HDR_MAGIC} at {hdr}'+Config.ENDC)
        # Need to move forward by 8 bytes
        # 'WwXxYyZz' <-----Current
        #  ^
        # 'WwXxYyZz ' <-----Where we need it
        #          ^
        hdr = hdr+8
        
        ftr = v_bytes.find(Config.FTR_MAGIC)
        if ftr == -1:
            print(Config.FAIL+f"[-] Couldn't find magic footer..."+Config.ENDC)
            return ftr

        print(Config.OKGREEN+f'[+] Found {Config.FTR_MAGIC} at {ftr}'+Config.ENDC)
        
        found_conf_byte_size = ftr - hdr
        print(Config.OKBLUE+f'[*] Config size {found_conf_byte_size}'+Config.ENDC)

        if found_conf_byte_size != Config.USABLE_CONF_SIZE:
            print(Config.WARNING+
                 f'Scaned binary has config size of {found_conf_byte_size} '+
                 f'expected {Config.USABLE_CONF_SIZE}'+Config.ENDC)
            return -1

        return hdr

    

def _validate_args(args):
    print(Config.OKCYAN+f'Validating args...')
    __validate_file(args.infile)
    __validate_port(args.port)
    __validate_ipv4_address(args.addr)
        

    # More need to happen here


    return True
    


def configure(args):
    if not _validate_args(args):
        print(Config.FAIL+f'[-] Validation failed...'+Config.ENDC)
        return


    if __find_magic(args.infile) == -1:
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


