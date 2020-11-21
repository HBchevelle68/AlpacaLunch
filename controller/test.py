import socket
import wolfssl

bind_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)

bind_socket.bind(("", 4433))
bind_socket.listen(5)

context = wolfssl.SSLContext(wolfssl.PROTOCOL_TLSv1_2, server_side=True)

context.load_cert_chain("certs/server-cert.pem", "certs/server-key.pem")

while True:
    try:
        secure_socket = None

        new_socket, from_addr = bind_socket.accept()

        secure_socket = context.wrap_socket(new_socket)

        print("Connection received from", from_addr)

        print("\n", secure_socket.read(), "\n")
        secure_socket.write(b"I hear you fa shizzle!")

    except KeyboardInterrupt:
        print()
        break

    finally:
        if secure_socket:
            secure_socket.close()

bind_socket.close()