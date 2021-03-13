import socket
import wolfssl


mysock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)

context = wolfssl.SSLContext(wolfssl.PROTOCOL_TLSv1_2)

#context.load_cert_chain("/home/ap/AlpacaLunch/.testcerts/cert.pem", "/home/ap/AlpacaLunch/.testcerts/private.pem")
context.verify_mode = wolfssl.CERT_NONE

secure_socket = context.wrap_socket(mysock)

secure_socket.connect(("", 54321))

secure_socket.write(b"SUPPPPPP!")

print(secure_socket.read())


secure_socket.close()