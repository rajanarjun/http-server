
all: server

server: validate.c response.c main.c 
	@gcc validate.c response.c main.c -o server

clean:
	@echo "Removing old binary"
	@rm server

