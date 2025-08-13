CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic

SRCS = file_info.c mime_type.c response.c main.c

TARGET = server

all: $(TARGET)

$(TARGET): $(SRCS) response.h file_info.h mime_type.h
	$(CC) $(CFLAGS) -o $@ $(SRCS)

clean:
	@echo "Removing old binary"
	@rm -f $(TARGET)
