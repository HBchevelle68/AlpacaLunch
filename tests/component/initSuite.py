import unittest
import os
import subprocess
import sys



class InitSuite(unittest.TestCase):

    def test_initSuite(self):
        # Move to the proper directory
        if os.getcwd().split('/')[-1] != "netSandbox":
            toplvl = os.path.join(os.path.dirname(__file__), "../../")
            os.chdir(toplvl)
            print(os.getcwd())

        # Wipe out any old binaries and rebuild fresh
        make_process = subprocess.Popen(["make", "release"], stdout=subprocess.PIPE)
        ret = make_process.wait()
        self.assertEqual(ret, 0)
        if sys.version_info[0] < 3:
            print("SCRIPT NOT RUNNING WITH PYTHON 3....this is gonna fail")
            self.fail(">>>>> RUN TESTS WITH PYTHON 3.5+ <<<<<")






if __name__ == "__main__":
    
    #unittest.main()
    pass