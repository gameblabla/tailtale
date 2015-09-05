CC = clang-3.6
CXX = clang-3.6

CFLAGS = -I./src -I./src/gfx -g `sdl-config --cflags` $(DEFINES)
LDFLAGS = `sdl-config --libs` -lSDLmain -lSDL -lSDL_mixer -lm
DEFINES = -DNOTPSP -DFRAMELIMIT -DNODEBUG -DSDLGFX_SCALING -DNO_SNPRINTF
OUTPUT = tailtale

SOURCES = ./src/bootmain.c ./src/debug.c ./src/input.c ./src/sound.c  ./src/grp_texture.c ./src/grp_sprite.c ./src/gamemain.c ./src/puz_trial.c ./src/puz_base.c ./src/puz_disp.c ./src/gfx/SDL_rotozoom.c ./src/grp_screen.c $(SOURCES_CPLUS)
SOURCES_CPLUS = ./src/get_resolution.cpp
OBJS = ${SOURCES:.c=.o} ${SOURCES:.cpp=.o}

all: ${OUTPUT}
	
cplusplus:
	${CXX} -c $(SOURCES_CPLUS)

${OUTPUT}:${OBJS}
	${CC} -o ${OUTPUT} ${SOURCES} ${CFLAGS} ${LDFLAGS}

clean:
	rm ${OUTPUT}
	rm ./src/*.o
	rm ./src/gfx/*.o
