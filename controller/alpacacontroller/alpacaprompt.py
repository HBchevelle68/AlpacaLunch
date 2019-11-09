from cmd import Cmd
import socket
import re 

# Internal
import task
import network as snet


  

class Prompt(Cmd):

    prompt = 'Sall-controller> '
    netconnect = None


    def do_exit(self, inp):
        '''exit the application.'''
        print("Bye")
        return True

    def do_connect(self, inp):
        
        if self.netconnect is not None:
            print("[!] Connection already created, please use edit or discconect")
            return False

        user_ip = input("IP Address >")
        if not snet.valid_ip(user_ip):
            return False
        
        user_port = input("Port (default:12345)>")
        if user_port is not '':
            user_port = int(user_port)
            if not snet.valid_port(int(user_port)):
                return False
        else:
            user_port = None
        
        user_maxretries = input("Max Re-tries (default:0)>")
        if user_maxretries is not '':
            user_maxretries = int(user_maxretries)
            if int(user_maxretries) < 0:
                print("[!] Invalid max re-tries => %d" % int(user_maxretries))
                return False
        else:
            user_maxretries = None
        
        print(user_ip)
        self.netconnect = snet.NetworkConnect(user_ip, user_port, user_maxretries)
        
            