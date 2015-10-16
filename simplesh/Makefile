CC = gcc
CC_OPTS = -Wall -g -O2 -std=c99 -I../lib
LD_OPTS = -L../lib 

PROGRAM = simplesh

all: $(PROGRAM)

clean:
	rm -f $(PROGRAM)
	rm -f $(PROGRAM).o

$(PROGRAM): $(PROGRAM).o 
	$(CC) -o $@ $< $(LD_OPTS)

$(PROGRAM).o: $(PROGRAM).c
	$(CC) -o $@ -c $< $(CC_OPTS)



