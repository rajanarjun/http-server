
all: server

server: validate.c main.c 
	@gcc validate.c main.c -o server

clean:
	@echo "Removing old binary"
	@rm server

