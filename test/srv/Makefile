simple_server_objects = ServerSocket.o Socket.o srv.o
simple_client_objects = ClientSocket.o Socket.o cln.o

all : simple_server simple_client

simple_server: $(simple_server_objects)
	g++ -o srv $(simple_server_objects)


simple_client: $(simple_client_objects)
	g++ -o cln $(simple_client_objects)


Socket: Socket.cpp
ServerSocket: ServerSocket.cpp SocketException.h ServerSocket.h
ClientSocket: ClientSocket.cpp SocketException.h ClientSocket.h
srv: srv.cpp
cln: cln.cpp


clean:
	rm -f *.o srv cln
bosh-client: bosh-client.cpp MyTime.cpp request2.c hires.c hires.h
	g++ -o bosh-client bosh-client.cpp MyTime.cpp request.c hires.c
pers-client: pers-client.cpp MyTime.cpp request2.c hires.c hires.h
	g++ -o pers-client pers-client.cpp MyTime.cpp request2.c hires.c
ss: ss.cpp
	g++ -o ss ss.cpp

