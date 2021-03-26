import socket
import wolfssl
import time

bind_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
bind_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
bind_socket.bind(("127.0.0.1", 54321))
bind_socket.listen(5)

context = wolfssl.SSLContext(wolfssl.PROTOCOL_TLSv1_2, server_side=True)
context.verify_mode = wolfssl.CERT_NONE

context.load_cert_chain("/home/ap/AlpacaLunch/.testcerts/cert.pem", "/home/ap/AlpacaLunch/.testcerts/private.pem")

while True:
    try:
        secure_socket = None

        new_socket, from_addr = bind_socket.accept()
        print("Connection received from", from_addr)

        # wrap socket
        secure_socket = context.wrap_socket(new_socket)
        secure_socket.write(b"I hear you fa shizzle!")
        time.sleep(5)
        #print("\n", secure_socket.read(), "\n")

    except Exception as e:
        print()
        break

    finally:
        if secure_socket:
            secure_socket.close()

bind_socket.close()