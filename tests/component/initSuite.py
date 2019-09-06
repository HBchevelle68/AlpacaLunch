import unittest
import os
import subprocess



class InitSuite(unittest.TestCase):

    def test_initSuite(self):
        if os.getcwd().split('/')[-1] != "netSandbox":
            toplvl = os.path.join(os.path.dirname(__file__), "../../")
            os.chdir(toplvl)
            print(os.getcwd())

        if not os.path.isfile("binaries/netsand-release"):
            make_process = subprocess.Popen(["make", "clean", "netplay-release"], stdout=subprocess.PIPE)
            ret = make_process.wait()
            self.assertEqual(ret, 0)






if __name__ == "__main__":
    
    #unittest.main()
    pass