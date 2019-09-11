import unittest
import os
import subprocess
import time
import socket

HOST = 'localhost'
PORT = 12345

class BasicServerSuite(unittest.TestCase):

    def _run_kill(self):
        try:
            process = subprocess.Popen(["binaries/netplay-release"])
        except subprocess.CalledProcessError:
            return -1
        
        time.sleep(1)
        process.terminate()
        return 0

    def _run_connect_kill(self):
        result = 0
        try:
            process = subprocess.Popen(["binaries/netplay-release"])
        except subprocess.CalledProcessError:
            return -1
        time.sleep(1)
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            s.connect((HOST, PORT))
        except Exception:
            result = -1
            self.fail(">>>>> Could not connect to port %d <<<<<" % PORT)

        s.close()
        process.terminate()
        return result

    
    def test_basicServerSuite(self):

        # Sub-test 1
        # Check binary can load without crashing
        results = self._run_kill()
        self.assertNotEqual(results, -1)

        # Sub-test 2
        # Check that connection to server can be established
        # Kill first run then re-run to verify ip:port sockopt binding
        results = self._run_connect_kill()
        self.assertNotEqual(results, -1)
        results = self._run_connect_kill()
        self.assertNotEqual(results, -1)





if __name__ == "__main__":
    
    #unittest.main()
    pass