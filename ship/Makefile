CC = gcc
CC_OPTS = -Wall -g -O2 -std=c99 -I../lib -D_XOPEN_SOURCE=500
LD_OPTS = -L../lib 

PROGRAM = ship2

all: $(PROGRAM)

clean:
	rm -f $(PROGRAM)
	rm -f $(PROGRAM).o

$(PROGRAM): $(PROGRAM).o 
	$(CC) -o $@ $< $(LD_OPTS)

$(PROGRAM).o: $(PROGRAM).c
	$(CC) -o $@ -c $< $(CC_OPTS)



