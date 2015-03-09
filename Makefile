SRC := ./src/
LIB := ./lib/
BIN := ./bin/
INCLUDE := ./include

CFLAGS := -g

$(LIB)lsoperate.o : $(SRC)lsoperate.c
	gcc $(CFLAGS) $(SRC)lsoperate.c -I$(INCLUDE) -c -o $(LIB)lsoperate.o
$(LIB)ls.o : $(LIB)lsoperate.o $(SRC)ls.c
	gcc $(CFLAGS) $(SRC)ls.c -I$(INCLUDE) -c -o $(LIB)ls.o
all : $(LIB)lsoperate.o $(LIB)ls.o
	gcc $(CFLAGS) $(LIB)ls.o $(LIB)lsoperate.o -o $(BIN)ls
clean:
	rm ./lib/*.o > /dev/null 2>&1
	rm ./bin/ls > /dev/null 2>&1
