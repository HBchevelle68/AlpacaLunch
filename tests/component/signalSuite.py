import unittest
import os
import subprocess
import time
import socket
import signal
import errno

HOST = 'localhost'
PORT = 12345

class SignalSuite(unittest.TestCase):

    '''
    def _check_PID(self,pid):
        try:
            os.kill(pid, 0)

        except OSError as err:
            print("HERE\n")
            if err.errno == errno.ESRCH:
                print("NO PROCESS: %d" % pid)
                return False
            elif err.errno == errno.EPERM:
                print("NO PROCESS1: %d" % pid)
                # EPERM clearly means there's a process to deny access to
                return True
            else:
                # According to "man 2 kill" possible error values are
                # (EINVAL, EPERM, ESRCH)
                print("NO PROCESS2: %d" % pid)
                raise
                #return False
        else:
            return True
    '''
    def _is_running(self, pid):
        if os.path.isdir('/proc/{}'.format(pid)):
            return True
        return False
    
    def _run_signal_SIGHUP(self):
        try:
            process = subprocess.Popen(["binaries/netplay-release"])
        except subprocess.CalledProcessError:
            return -1
          
        time.sleep(1)
        process.send_signal(signal.SIGHUP)
        
        if self._is_running(process.pid) == False:
            self.fail(">>>>> Signal handling of SIGHUP failed <<<<<")
            process.terminate()
            return -1
        
        process.terminate()
        return 0


    def _run_signal_SIGINT(self):
        try:
            process = subprocess.Popen(["binaries/netplay-release"])
        except subprocess.CalledProcessError:
            return -1
        
        time.sleep(1)

        process.send_signal(signal.SIGINT)

        if self._is_running(process.pid) is False:
            self.fail(">>>>> Signal handling of SIGINT failed <<<<<")
            process.terminate()
            return -1
        
        process.terminate()
        return 0


    def _run_signal_SIGQUIT(self):
        try:
            process = subprocess.Popen(["binaries/netplay-release"])
        except subprocess.CalledProcessError:
            return -1
        
        time.sleep(1)

        os.kill(process.pid, signal.SIGQUIT)

        if self._is_running(process.pid) is False:
            self.fail(">>>>> Signal handling of SIGQUIT failed <<<<<")
            process.terminate()
            return -1
        
        process.terminate()
        return 0


    def _run_signal_SIGALRM(self):
        try:
            process = subprocess.Popen(["binaries/netplay-release"])
        except subprocess.CalledProcessError:
            return -1
        
        time.sleep(1)

        process.send_signal(signal.SIGALRM)

        if self._is_running(process.pid) is False:
            self.fail(">>>>> Signal handling of SIGALRM failed <<<<<")
            process.terminate()
            return -1
        
        process.terminate()
        return 0


    def _run_signal_SIGTERM(self):
        try:
            process = subprocess.Popen(["binaries/netplay-release"])
        except subprocess.CalledProcessError:
            return -1
        
        time.sleep(1)
        process.send_signal(signal.SIGTERM)
        process.wait(timeout=5)

        if self._is_running(process.pid) is True:
            self.fail(">>>>> Signal handling of SIGTERM failed <<<<<")
            os.kill(process.pid, signal.SIGTERM)
            return -1
        
        return 0


    def test_signalSuite(self):

        # Sub-test 1
        # Check binary can handle signals properly
        results = self._run_signal_SIGHUP()
        self.assertNotEqual(results, -1)

        results = self._run_signal_SIGINT()
        self.assertNotEqual(results, -1)

        results = self._run_signal_SIGQUIT()
        self.assertNotEqual(results, -1)

        results = self._run_signal_SIGALRM()
        self.assertNotEqual(results, -1)

        results = self._run_signal_SIGTERM()
        self.assertNotEqual(results, -1)

if __name__ == "__main__":
    
    #unittest.main()
    pass