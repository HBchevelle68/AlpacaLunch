import getpass

class HerderAuth():
    """ Autenticates a Herder (client) to the Hutch (Server) """

    def get_login_info(self):
        username = input("Username: ")
        password = getpass.getpass()
    # Code here...
