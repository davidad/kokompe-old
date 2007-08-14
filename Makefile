CC := g++
CFLAGS := -O2 -I..
LIBS :=  

image_obj := main.o
image_obj += vvolume.o
image_obj += ppm_image.o


image := image

compile: $(image_obj) 

all: $(image)


# debug build	
$(image): $(image_obj)
	$(CC) -g -o $@ $(image_obj) $(LIBS) -pg
%.o: %.cpp
	$(CC) -g -c -o $@ $< $(CFLAGS) -pg

%.cpp : %.h

clean: tidy
	rm -f $(image)

tidy:
	rm -f $(image_obj)

run:	all
	time ./image

