CC := g++
CFLAGS := -Wall -O3 -I..
LDFLAGS := -lpthread -largtable2
XMLRPC_LDFLAGS := -lwwwhttp -lwwwxml -lxmlrpc -lxmlrpc_server -lxmlrpc_client -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok -lxmlrpc_server_abyss -lxmlrpc_abyss 
LIBS := -lglut -lpython2.4

geomeval_obj:=geomeval.o octree.o expression.o interval.o space_interval.o vector.o trimesh.o tool_path.o expand.o
geomeval:=geomeval

kokompe_obj:=kokompe.o camera.o commands.o gui_console.o eval_geometry.o math/math_gl.o 
kokompe_obj+= octree.o expression.o interval.o space_interval.o vector.o trimesh.o
kokompe:=kokompe

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
math_string_slice_to_ppm_obj:=math_string_slice_to_ppm.o octree.o expression.o interval.o space_interval.o  ppm_image.o
math_string_slice_to_ppm:= math_string_slice_to_ppm

math_string_slice_to_toolpath_obj:=math_string_slice_to_toolpath.o octree.o expression.o interval.o space_interval.o  tool_path.o expand.o
math_string_slice_to_toolpath:= math_string_slice_to_toolpath


image := image


compile: all


all:  $(geomeval) $(xmlrpc_server) $(xmlrpc_client) $(image_obj) $(infix_to_postfix) $(math_string_to_stl) $(stl_to_ppm)  $(math_string_slice_to_ppm) $(math_string_slice_to_toolpath) $(kokompe)

$(kokompe): $(kokompe_obj)
	g++ -o $@ $(kokompe_obj) $(LDFLAGS) $(LIBS)
		
$(geomeval): $(geomeval_obj)
	g++ -o $@ $(geomeval_obj) $(LDFLAGS) $(LIBS)

$(xmlrpc_server): $(xmlrpc_server_obj)
	g++ -o $@ $(xmlrpc_server_obj) $(XMLRPC_LDFLAGS) $(LDFLAGS) $(LIBS)

$(xmlrpc_client): $(xmlrpc_client_obj)
	g++ -o $@ $(xmlrpc_client_obj) $(XMLRPC_LDFLAGS) $(LDFLAGS) $(LIBS)

$(infix_to_postfix): $(infix_to_postfix_obj)
	g++ -o $@ $(infix_to_postfix_obj) $(LDFLAGS) $(LIBS)

$(math_string_to_stl): $(math_string_to_stl_obj)
	g++ -o $@ $(math_string_to_stl_obj) $(LDFLAGS) $(LIBS)

$(stl_to_ppm): $(stl_to_ppm_obj) quaternion.h
	g++ -o $@ $(stl_to_ppm_obj) $(LDFLAGS) $(LIBS)

$(math_string_slice_to_ppm): $(math_string_slice_to_ppm_obj)
	g++ -o $@ $(math_string_slice_to_ppm_obj) $(LDFLAGS) $(LIBS)

$(math_string_slice_to_toolpath): $(math_string_slice_to_toolpath_obj)
	g++ -o $@ $(math_string_slice_to_toolpath_obj) $(LDFLAGS) $(LIBS)

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)


# debug build	
%.o: %.c
	$(CC) -g -c -o $@ $< $(CFLAGS) -pg
%.o: %.cpp
	$(CC) -g -c -o $@ $< $(CFLAGS) -pg

%.cpp : %.h

clean: tidy
	rm -f $(geomeval) $(xmlrpc_server) $(xmlrpc_client) $(infix_to_postfix) $(math_string_to_stl) $(stl_to_ppm) $(math_string_slice_to_ppm) $(math_string_slice_to_toolpath)

tidy:
	rm -f $(geomeval_obj) $(xmlrpc_server_obj) $(xmlrpc_client_obj) $(infix_to_postfix_obj) $(math_string_to_stl_obj) $(stl_to_ppm_obj) $(math_string_slice_to_ppm_obj) $(math_string_slice_to_toolpath_obj)

ppm_run: stl_to_ppm
	./stl_to_ppm < in.stl > out.ppm
	display out.ppm

geom_run: geomeval
	./geomeval
