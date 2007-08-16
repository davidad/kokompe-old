CC := g++
CFLAGS := -Wall -O3 -I..
LDFLAGS := -lxmlrpc -lxmlrpc_server -lxmlrpc_client -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok -lxmlrpc_server_abyss -lxmlrpc_abyss -lpthread -lwwwhttp -lwwwxml -largtable2

geomeval_obj:=geomeval.o octree.o expression.o interval.o space_interval.o vector.o trimesh.o tool_path.o expand.o
geomeval:=geomeval

xmlrpc_server_obj:=xmlrpc_server.o octree.o expression.o interval.o space_interval.o vector.o trimesh.o
xmlrpc_server:= xmlrpc_server

xmlrpc_client_obj:=xmlrpc_client.o
xmlrpc_client:=xmlrpc_client

infix_to_postfix_obj:=infix_to_postfix.o expression.o interval.o space_interval.o
infix_to_postfix:= infix_to_postfix

math_string_to_stl_obj:=math_string_to_stl.o octree.o expression.o interval.o space_interval.o vector.o trimesh.o
math_string_to_stl:= math_string_to_stl

stl_to_ppm_obj:=stl_to_ppm.o vvolume.o ppm_image.o
stl_to_ppm:= stl_to_ppm


compile: all

all:  $(geomeval) $(xmlrpc_server) $(xmlrpc_client) $(image_obj) $(infix_to_postfix) $(math_string_to_stl) $(stl_to_ppm)

$(geomeval): $(geomeval_obj)
	g++ -o $@ $(geomeval_obj) $(LDFLAGS) $(LIBS)

$(xmlrpc_server): $(xmlrpc_server_obj)
	g++ -o $@ $(xmlrpc_server_obj) $(LDFLAGS) $(LIBS)

$(xmlrpc_client): $(xmlrpc_client_obj)
	g++ -o $@ $(xmlrpc_client_obj) $(LDFLAGS) $(LIBS)

$(infix_to_postfix): $(infix_to_postfix_obj)
	g++ -o $@ $(infix_to_postfix_obj) $(LDFLAGS) $(LIBS)

$(math_string_to_stl): $(math_string_to_stl_obj)
	g++ -o $@ $(math_string_to_stl_obj) $(LDFLAGS) $(LIBS)

$(stl_to_ppm): $(stl_to_ppm_obj) quaternion.h
	g++ -o $@ $(stl_to_ppm_obj) $(LDFLAGS) $(LIBS)

# debug build	
%.o: %.c
	$(CC) -g -c -o $@ $< $(CFLAGS) -pg
%.o: %.cpp
	$(CC) -g -c -o $@ $< $(CFLAGS) -pg

%.cpp : %.h

clean: tidy
	rm -f $(geomeval) $(xmlrpc_server) $(xmlrpc_client) $(image)

tidy:
	rm -f $(geomeval_obj) $(xmlrpc_server_obj) $(xmlrpc_client_obj) $(image_obj)

ppm_run: stl_to_ppm
	./stl_to_ppm < in.stl > out.ppm
	display out.ppm

geom_run: geomeval
	./geomeval
