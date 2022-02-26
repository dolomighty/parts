


PRE:=$(shell ./mk-headers.sh)
PRE:=$(shell ./mk-shell.sh)
SRC:=$(shell find -type f -name "*.c" -or -name "*.cpp" -or -name "*.asm")
HDR:=$(shell find -type f -name "*.h" -or -name "*.hpp")
RES:=$(shell find -type f -name "*.png")
DIR:=$(shell echo "-I ../dyn/.." && find -L -mindepth 1 -type d -not -wholename "*/.*" -printf " -I %P")




CC=gcc
LIBS    = `pkg-config --libs   sdl2` -lm   
CPPFLAGS= `pkg-config --cflags sdl2` -Werror $(DIR)

# optim
CPPFLAGS+=-O3


.PHONY : all
all : main


.PHONY : run
run : main
	./$^


OBS+=main.o
OBS+=parts.o
OBS+=frame.o
OBS+=loop.o



$(OBS) : Makefile $(SRC) $(HDR)

main : $(OBS)
	$(CC) $(CPPFLAGS) -o $@ $(OBS) $(LIBS)

#DYN+=draw_scene_gl.h
#draw_scene_gl.h : scenes/scene.obj obj2c.sh
#	./obj2c.sh scenes/scene.obj > $@

.PHONY : clean cl
clean cl :
	file * | awk '/ELF/ { gsub(/:.*/,"") ; print }' | xargs -r rm
	rm -fR dyn
	rm *~ .* 2> /dev/null

.PHONY : rebuild re
rebuild re : clean all




## 2016-09-08 12:08:27
## funziona, ma non capisco come:
## deps.inc non è un requisito di nessun target ... perche viene generato ?
## forse anche include si comporta un pò come un target
## cmq ... in caso aggiungere altre wildcard qui sotto.
#deps.inc : $(shell find . -type f -name "*.c")
#	gcc -M -MG $^ > deps.inc
#-include deps.inc


