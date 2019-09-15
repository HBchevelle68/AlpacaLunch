import unittest
import socket
import wolfssl
import subprocess
import time
import os

TSTFPATH = "/tmp/test.txt"
HOST = 'localhost'
PORT = 12345


def build_wolfssl_sock():
    # TLS wrapped Socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
    context = wolfssl.SSLContext(wolfssl.PROTOCOL_TLSv1_2)
    secure_socket = context.wrap_socket(sock)

    return sock, secure_socket

def clean_wolfssl_sock(sock, secure_socket):
    secure_socket.close()
    sock.close()

class TLSSuite(unittest.TestCase):
    
    def _run_tls_conn_send(self):

        # Deploy binary
        try:
            process = subprocess.Popen(["binaries/netplay-release"])
        except subprocess.CalledProcessError:
            return -1
        time.sleep(1)

        # TLS wrapped Socket
        sock, secure_socket = build_wolfssl_sock()

        try:
            secure_socket.connect(("localhost", 12345))
        except ConnectionRefusedError:
            secure_socket.close()
            process.terminate()
            #self.fail(">>>>> _run_tls_connect_send failed ConnectionRefusedError <<<<<")

        secure_socket.write("THIS IS A TEST?!123456789!@#$%^&*()_+~<>?:")

        # Clean
        clean_wolfssl_sock(sock, secure_socket)

        # Prevent race  
        time.sleep(1)
        process.terminate()
        return 0

    def _run_tls_2conn(self):

        # Deploy binary
        try:
            process = subprocess.Popen(["binaries/netplay-release"])
        except subprocess.CalledProcessError:
            return -1
        time.sleep(1)

        # TLS wrapped Socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
        context = wolfssl.SSLContext(wolfssl.PROTOCOL_TLSv1_2)
        secure_socket = context.wrap_socket(sock)

        # Purposefully connect and immediately close 
        try:
            secure_socket.connect(("localhost", 12345))
            secure_socket.close()
            sock.close()
        except ConnectionRefusedError:
            secure_socket.close()
            process.terminate()
            self.fail(">>>>> _run_tls_conn_err failed ConnectionRefusedError <<<<<")
        
        time.sleep(1)
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
        context = wolfssl.SSLContext(wolfssl.PROTOCOL_TLSv1_2)
        secure_socket = context.wrap_socket(sock)
        try:
            secure_socket.connect(("localhost", 12345))
        except ConnectionRefusedError:
            secure_socket.close()
            process.terminate()
            self.fail(">>>>> _run_tls_conn_err failed ConnectionRefusedError <<<<<")

        secure_socket.write("THIS IS A TEST?!123456789!@#$%^&*()_+~<>?:")
        secure_socket.close()
        time.sleep(1)
        process.terminate()
        return 0      

    
    def _run_verify_file_contents(self):
        
        if not os.path.isfile(TSTFPATH):
            self.fail(">>>>> _run_verify_file_contents failed FILE NOT FOUND <<<<<")
            return -1
        return 0

    def test_tlsSuite(self):

        # Sub-test 1
        # Binary can recieve tls 1.2 connection && decrypt data
        results = self._run_tls_conn_send()
        self.assertNotEqual(results, -1)

        # Sub-test 2
        # Binary receive conn+disconn, then valid conn and data
        results = self._run_tls_2conn()
        self.assertNotEqual(results, -1)


        # Final Sub-test 
        # Verify all data contents recieved
        results = self._run_verify_file_contents()
        self.assertNotEqual(results, -1)

if __name__ == "__main__":
    
    #unittest.main()
    pass