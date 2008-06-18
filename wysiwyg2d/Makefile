CC := g++
CFLAGS := -O3 -g -I..
LIBS := -lglut 
# not currently used: -lpython2.5

obj = main.o
obj += application.o
obj += view.o
obj += design.o
obj += control_object.o
obj += ../engine/octree.o
obj += ../engine/expression.o
obj += ../engine/interval.o
obj += ../engine/space_interval.o
obj += ../engine/vector.o
obj += ../engine/math_string_gl_texture.o
obj += ../math/math_gl.o

bin := main

all: $(bin)

compile: $(obj) 
	
$(bin): $(obj)
	$(CC) -o $@ $(obj) $(LIBS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

%.cpp : %.h

clean: tidy
	rm -f $(bin)

tidy:
	rm -f $(obj)

run:	all
	./main
