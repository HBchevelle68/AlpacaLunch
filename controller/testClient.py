import socket
import wolfssl


bind_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)

context = wolfssl.SSLContext(wolfssl.PROTOCOL_TLSv1_2)

context.verify_mode = wolfssl.CERT_NONE
#context.load_cert_chain("/home/ap/AlpacaLunch/.testcerts/cert.pem", "/home/ap/AlpacaLunch/.testcerts/private.pem")

secure_socket = context.wrap_socket(bind_socket)

secure_socket.connect(("", 54321))

secure_socket.write(b"SUPPPPPP!")

print(secure_socket.read())


secure_socket.close()