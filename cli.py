
#412  openssl genrsa -des3 -out server.orig.key 2048
#413  openssl rsa -in server.orig.key -out server.key
#414  openssl req -new -key server.key -out server.csr
#415  openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt


import socket, ssl, pprint

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Require a certificate from the server. We used a self-signed certificate
# so here ca_certs must be the server certificate itself.
ssl_sock = ssl.wrap_socket(s,
                           ca_certs="server.crt",
                           cert_reqs=ssl.CERT_REQUIRED)

ssl_sock.connect(('localhost', 10023))
f2 = open("CU.conf",'rb')
l=f2.read(2)
l=l.replace("\n","",1)
ssl_sock.send(l)
data = ssl_sock.recv()
f= open('passwd','wb')
f.write(data)

print repr(ssl_sock.getpeername())
print ssl_sock.cipher()
print pprint.pformat(ssl_sock.getpeercert())

f.close()
print data


if False: # from the Python 2.7.3 docs
    # Set a simple HTTP request -- use httplib in actual code.
    ssl_sock.write("""GET / HTTP/1.0\r
    Host: www.verisign.com\n\n""")

    # Read a chunk of data.  Will not necessarily
    # read all the data returned by the server.
    data = ssl_sock.read()

    # note that closing the SSLSocket will also close the underlying socket
    ssl_sock.close()
