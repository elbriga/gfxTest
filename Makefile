CC=gcc
CFLAGS=-lX11 -lm -g
DEPS = 3d.h  doublebuffer_pixmap.h  gfx.h  gfx_ptBR.h  mapa.h  readMdl.h  readBsp.h  render.h
OBJ =  3d.o  doublebuffer_pixmap.o  gfx.o  gfx_ptBR.o  mapa.o  readMdl.o  readBsp.o  render.o  main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

QuakeViewer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o QuakeViewer

