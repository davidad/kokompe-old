CC := g++
CFLAGS := -Wall -O3 -I..

geomeval_obj:=geomeval.o octree.o expression.o interval.o space_interval.o vector.o trimesh.o
geomeval:=geomeval


all:  $(geomeval)

$(geomeval): $(geomeval_obj)
	g++ -o $@ $(geomeval_obj) $(LDFLAGS) $(LIBS)

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	g++ -c -o $@ $< $(CFLAGS)

clean: tidy
	rm -f $(geomeval)

tidy:
	rm -f $(geomeval_obj)
