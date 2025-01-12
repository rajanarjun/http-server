
all: server

server: request.c main.c 
	@gcc request.c main.c -o server

clean:
	@echo "Removing old binary"
	@rm server

