import unittest
import os
import sys

'''
    Import Test suites
'''
import initSuite
import basicServerSuite
import signalSuite
import tlsSuite



if __name__ == "__main__":
    if os.geteuid() != 0:
        print("You need to have root privileges to run this script.\n")
        exit(0)
        
    if sys.version_info[0] < 3:
            print("This script is designed to be run with Python 3.5 and up.\n")
            exit(0)
           
    # Initialize the test suite
    loader = unittest.TestLoader()
    suite  = unittest.TestSuite()

    # Add tests to the suite
    suite.addTests(loader.loadTestsFromModule(initSuite))
    suite.addTests(loader.loadTestsFromModule(basicServerSuite))
    suite.addTests(loader.loadTestsFromModule(signalSuite))
    suite.addTests(loader.loadTestsFromModule(tlsSuite))


    # Run Component tests
    result = unittest.TextTestRunner(verbosity=3).run(suite)
    