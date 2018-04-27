

OBJS = main.o timer.o engine.o paragraph.o editor.o line.o interpreter.o editor_scene.o scene.o end_scene.o menu_scene.o
BC_OBJS = main.bc timer.bc engine.bc paragraph.bc editor.bc line.bc interpreter.bc editor_scene.bc scene.bc end_scene.bc menu_scene.bc

LIBS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf

CFLAGS = -Wall -O1 -g `sdl2-config --cflags` -DDEBUG
CC = gcc
EM_PORTS = -s USE_LIBPNG=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 # -s SDL2_IMAGE_FORMATS=["png"]

main: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o main $(LIBS)

quizz.html: $(BC_OBJS) shell.html
	emcc  -O2 --shell-file shell.html $(BC_OBJS) -o quizz.html --preload-file data $(EM_PORTS)

$(BC_OBJS):	%.bc:	%.c 
	emcc  -O2 -c $<  -o $@ $(EM_PORTS)

$(OBJS):	%.o:	%.c 
	$(CC) -c $<  $(CFLAGS)
clean:
	rm -f *.o core main *~ quizz.* *.bc 
