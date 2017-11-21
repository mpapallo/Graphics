OBJECTS= main.o draw.o display.o
CFLAGS= -Wall
CC= gcc

all: $(OBJECTS)
	$(CC) -o main $(OBJECTS)

main.o: main.c display.h draw.h ml6.h
	$(CC) -c main.c

draw.o: draw.c draw.h display.h ml6.h
	$(CC) $(CFLAGS) -c draw.c

display.o: display.c display.h ml6.h
	$(CC) $(CFLAGS) -c display.c

clean:
	rm *.o *~

run:
	./main
