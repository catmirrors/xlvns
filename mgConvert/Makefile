CFLAGS=-I/usr/local/lib/mgl2 -Wall -g -DDEBUG
#LDFLAGS=-lmgl2 -lm

OBJS= pakconv.o lfg.o lf2.o sdt.o leafpack.o pakwriter.o image.o lzcompress.o

all: pakconv

pakconv: $(OBJS)
	$(CC) -g -o pakconv $(OBJS) $(LDFLAGS)

clean:
	rm -f pakconv core $(OBJS) KNJ_ALL.KNJ *~
