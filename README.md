## HTTP server
A minimal HTTP/1.1 server written in C using POSIX sockets.
Currently supports serving static files (HTML, PNG, JPEG, etc.) in response to **GET** requests.

## Features
- Handles **HTTP GET** requests
- Serves static files from the server root directory
- Custom error pages for:
  - 400 — Bad Request
  - 404 — Not Found
  - 405 — Method Not Allowed
  - 501 — Not Implemented
  - 505 — HTTP Version Not Supported
- Serves HTML pages for above error responses

## Dependencies
- Written in **C17**
- Built using **CMake**

## Example Responses

### 200 OK
The welcome page served at `/`:

![200 OK](assets/response_200_index.png)

### 400 Bad Request
Invalid path example (`////adcj`):

![400 Bad Request](assets/response_400_invalid_path.png)

### 🕳️ 404 Not Found
Request for non-existent file (`word.exe`):

![404 Not Found](assets/response_404_not_found.png)

## Build and Run
To build and run the server:

```bash
mkdir build && cd build
cmake ..
cmake --build .
./server [PORT_NO]
```

**Example:**
```bash
./server 8080
```
Then open your browser and navigate to:
```
http://localhost:8080/
```

## Planned Improvements
- More GET responses apart from `index.html`
- Support for multiple concurrent clients
- Implement `HEAD` and `POST` methods
- Unit tests for request parsing

## License
MIT License — see `LICENSE` file.
