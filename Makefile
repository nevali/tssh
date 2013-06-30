OUT = tssh
OBJ = main.o cli.o \
	cmd_help.o cmd_logout.o cmd_connect.o

CFLAGS = -W -Wall -O0 -g

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) -o $(OUT) $(OBJ) $(LIBS)

clean:
	rm -f $(OUT) $(OBJ)
