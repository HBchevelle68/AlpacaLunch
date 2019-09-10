import unittest
import os

import initSuite
import basicServer
import signalSuite




if __name__ == "__main__":
    if os.geteuid() != 0:
        print("You need to have root privileges to run this script.\n")
        exit(0)
    
    # Initialize the test suite
    loader = unittest.TestLoader()
    suite  = unittest.TestSuite()

    # Add tests to the suite
    suite.addTests(loader.loadTestsFromModule(initSuite))
    suite.addTests(loader.loadTestsFromModule(basicServer))
    suite.addTests(loader.loadTestsFromModule(signalSuite))

    # Run Component tests
    result = unittest.TextTestRunner(verbosity=3).run(suite)
    