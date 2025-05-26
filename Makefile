EXEC = a.out

CC = gcc
CFLAGS = -Wall
SRC = proiect.c 

all:	$(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

clean:
	rm -f $(EXEC)