
all: program

program: response.c main.c 
	@gcc -Wall -Wextra -Wpedantic mime_type.c get_file.c response.c main.c -o server

clean:
	@echo "Removing old binary"
	@rm server

