
all: program

program: validate.c response.c main.c 
	@gcc validate.c response.c main.c -o program

clean:
	@echo "Removing old binary"
	@rm program

