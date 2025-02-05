
all: program

program: response.c main.c 
	@gcc response.c main.c -o program

clean:
	@echo "Removing old binary"
	@rm program

