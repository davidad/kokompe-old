CC := g++
CFLAGS := -Wall -O3 -I..
LDFLAGS := -lxmlrpc -lxmlrpc_server -lxmlrpc_client -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok -lxmlrpc_server_abyss -lxmlrpc_abyss  -lpthread -lwwwhttp -lwwwxml

geomeval_obj:=geomeval.o octree.o expression.o interval.o space_interval.o vector.o trimesh.o
geomeval:=geomeval

xmlrpc_server_obj:=xmlrpc_server.o octree.o expression.o interval.o space_interval.o vector.o trimesh.o
xmlrpc_server:= xmlrpc_server

xmlrpc_client_obj:=xmlrpc_client.o
xmlrpc_client:=xmlrpc_client

all:  $(geomeval) $(xmlrpc_server) $(xmlrpc_client)

$(geomeval): $(geomeval_obj)
	g++ -o $@ $(geomeval_obj) $(LDFLAGS) $(LIBS)

$(xmlrpc_server): $(xmlrpc_server_obj)
	g++ -o $@ $(xmlrpc_server_obj) $(LDFLAGS) $(LIBS)

$(xmlrpc_client): $(xmlrpc_client_obj)
	g++ -o $@ $(xmlrpc_client_obj) $(LDFLAGS) $(LIBS)

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	g++ -c -o $@ $< $(CFLAGS)

clean: tidy
	rm -f $(geomeval) $(xmlrpc_server) $(xmlrpc_client)

tidy:
	rm -f $(geomeval_obj) $(xmlrpc_server_obj) $(xmlrpc_client_obj)
