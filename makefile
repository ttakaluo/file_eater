CC=gcc
CFLAGS= -Wall -I.
EXE_NAME = eatd
FILES = file_eater.o
all: $(FILES)
	$(CC) -o $(EXE_NAME) $(FILES) $(CFLAGS)

clean:
	rm -f *.o

run:
	./$(EXE_NAME)

rebuild: clean all run

