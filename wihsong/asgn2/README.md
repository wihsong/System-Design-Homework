# Assignment 2

This directory contains source code and relevant files for the HTTP Server project.

`httpServer.c` **Summary**  
`httpServer.c` is a C utility designed to provide a robust server that listens to HTTP requests and sends responses accordingly. The server processes incoming connections and the HTTP methods GET and PUT.

**HTTP Request Handling**  
Upon receiving an HTTP request:

- The program parses the request, extracts the method (GET/PUT), and processes it.
  
**"GET" Method**  
Upon recognizing the GET method:
  
- The server checks for the requested resource (file).
- If the file is found:
  - Returns the content of the file.
- Otherwise:
  - Sends a 404 Not Found response.
  
**"PUT" Method**  
Upon recognizing the PUT method:

- The server reads the content of the request.
- Writes the content to the specified file.
- Sends an appropriate response based on success or failure.

**Error Handling**  
The server ensures robustness by handling:

- Malformed requests.
- File read/write errors.
- Connection timeouts.
- Missing resources.

**Efficiency & Constraints**  

- **Error Handling**: Manages scenarios like missing files, invalid requests, and connection errors.
- **Time Efficiency**: Uses optimized functions for efficient request processing.
- **Space Efficiency**: Manages memory allocations for optimal performance.
- **Request Parsing**: Efficiently handles incoming HTTP requests and formulates correct responses.
- **Clean Operation**: Ensures no memory leaks and operates without crashes.
- **Usage Restrictions**: Designed for scalability and avoids pitfalls of socket programming.


This directory contains source code and other files for Assignment 2.

Use this README document to store notes about design, testing, and
questions you have while developing your assignment.

