import unittest
import socket
import wolfssl
import subprocess
import time

HOST = 'localhost'
PORT = 12345

class TLSSuite(unittest.TestCase):


    def _run_tls_connect_send(self):

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

        try:
            secure_socket.connect(("localhost", 12345))
        except ConnectionRefusedError:
            secure_socket.close()
            process.terminate()
            #self.fail(">>>>> _run_tls_connect_send failed ConnectionRefusedError <<<<<")

        secure_socket.write("THIS IS A TEST?!123456789!@#$%^&*()_+~<>?:")
        secure_socket.close()
        process.terminate()
        return 0

    def test_tlsSuite(self):

        # Sub-test 1
        # Check binary can load without crashing
        results = self._run_tls_connect_send()
        self.assertNotEqual(results, -1)




if __name__ == "__main__":
    
    #unittest.main()
    pass