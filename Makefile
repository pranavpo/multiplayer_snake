CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lraylib -lm -ldl -lpthread  # Include Raylib and required libraries

# Final executable output
Final.exe: struct_data.o client.o server.o main.o
	$(CC) struct_data.o client.o server.o main.o -o Final $(LDFLAGS)

# Compiling object files
struct_data.o: struct_data.c struct_data.h
	$(CC) $(CFLAGS) -c struct_data.c -o struct_data.o

# game.o: game.c game.h struct_data.h
# 	$(CC) $(CFLAGS) -c game.c -o game.o

client.o: client.c client.h
	$(CC) $(CFLAGS) -c client.c -o client.o

server.o: server.c server.h
	$(CC) $(CFLAGS) -c server.c -o server.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm -f *.o Final

# run-scripts:
# 	@bash ./build_client.bash && bash ./build_server.bash && bash ./build_game.bash

