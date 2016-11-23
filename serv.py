import socket, ssl

bindsocket = socket.socket()
bindsocket.bind(('', 10023))
bindsocket.listen(5)

def do_something(connstream, data):
    print "do_something:", data
    return False

def deal_with_client(connstream):
	data=connstream.recv()
	print data
	f1 = open("passwd/"+data,'rb')
	l=f1.read(1024)
	f1.close()
	connstream.send(l)
	
	
    

while True:
    newsocket, fromaddr = bindsocket.accept()
    connstream = ssl.wrap_socket(newsocket,
                                 server_side=True,
                                 certfile="server.crt",
                                 keyfile="server.key")
    try:
        deal_with_client(connstream)
    finally:
        connstream.shutdown(socket.SHUT_RDWR)
        connstream.close()
