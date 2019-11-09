import socket
import wolfssl
import re

def valid_ip(ip):
    """
        validate an IP addess 
    """
    # for validating an Ip-address 
    regex = '''^(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\.( 
                25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\.( 
                25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\.( 
                25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)'''
    
    if not re.search(regex, ip):    
        print("\n[!] Invalid address ==> %s\n" % ip)
        return False

    return True

def valid_port(port):
    print("THE PORT %d" % port)
    if port < 0 or port > 65535:
        print("\n[!] Invalid port ==> %s\n" % port)
        return False
    return True

class NetworkConnect():
    ip = None
    port = 12345
    std_sock = None
    secure_sock = None
    max_retries = 0

    def __init__(self, ip, port=None, maxretries=None):

        if ip is None:
            print("[!] Attempt to initialize with empty ip!")
        else:
            self.ip = ip 
            if port is not None:
                self.port = port 
        
        print("[*] Connection set to %s:%d" % (self.ip, self.port) )

    def _build_wolfssl_sock(self):
        # TLS wrapped Socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
        context = wolfssl.SSLContext(wolfssl.PROTOCOL_TLSv1_2)
        self.secure_socket = context.wrap_socket(self.sock)

        

    def _clean_wolfssl_sock(self):
        """
            Graceful teardown and clean up
            object is not destroyed yet
        """
        self.secure_socket.close()
        self.secure_socket = None
        
        self.sock.close()
        self.sock = None

        self.ip = None
        self.max_retries = 0




    def connect_to_endpoint(self):
        # TLS wrapped Socket
        self.sock, self.secure_socket = self._build_wolfssl_sock()

        try:
            self.secure_socket.connect((self.ip, self.port))
        except Exception as e:
            print("[!] ERROR ==> %s" % str(e))
            self.secure_socket.close()
            return -1

        return 0


    def disconnect_from_endpoint(self):
        self._clean_wolfssl_sock()
