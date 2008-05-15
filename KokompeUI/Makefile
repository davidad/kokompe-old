
CC = g++ 
INC =  `wx-config --cppflags` `sdl-config --cflags`
LIBS = `wx-config --libs` `sdl-config --libs`

FILES = main.o MainFrame.o SDLPanel.o PrimitiveInterface.o

all: $(FILES) link

.cpp.o:
	g++ -c $(INC) $< -o $@

link:
	g++ $(FILES) $(LIBS) -o Kokompe 

clean:
	rm $(FILES) Kokompe
