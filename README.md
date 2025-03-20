# HTTP server in C
A lightweight HTTP server implemented in C. An interesting project to learn the C language and Hyper Text Transfer Protocol (HTTP). This project also gives an understanding of how HTML pages are shown on the web in accordance to RFC standards.

#### Current implementation:
- Supports HTTP/1.0, HTTP/1.1
- Handles GET requests
- Serves static files (HTML, .jpg, .png, etc)
- Custom error pages
- Handles dangerous path traversals

#### Future implementation to try:
- Basic logging
- Handling concurrent connections
- Multi-threaded request handling

## Prequisites
- GCC or Clang compiler
- Make
- Git
 
## Getting Started
##### 1. Clone the repository:
```sh
git clone https://github.com/rajanarjun/http-server.git
```
##### 2. Change into the project directory:
```sh
cd http-server
```
##### 3. Build the program:
```sh
make all
```
##### 4. Run the server:
```sh
./program 8080
```
You can enter other port numbers apart from 8080.

##### Cleaning build files:
```sh
make clean
```
