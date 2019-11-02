import unittest
import os
import subprocess
import sys

TSTFPATH = "/tmp/test.txt"

class InitSuite(unittest.TestCase):

    def test_initSuite(self):
        # Move to the proper directory
        if os.getcwd().split('/')[-1] != "sassyllama":
            toplvl = os.path.join(os.path.dirname(__file__), "../../")
            os.chdir(toplvl)

        # Wipe out any old binaries and rebuild fresh
        make_process = subprocess.Popen(["make", "release"], stdout=subprocess.PIPE)
        ret = make_process.wait()
        self.assertEqual(ret, 0)
        

        if os.path.isfile(TSTFPATH):
            os.remove(TSTFPATH)

            
        return 0





if __name__ == "__main__":
    
    #unittest.main()
    pass