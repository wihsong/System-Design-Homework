#Assignment 1 directory

This directory contains source code and other files for Assignment 1.

# memory.c Summary

`memory.c` is a C utility designed to provide get/set memory abstraction for files 
within a Linux directory. The program processes commands from standard input.

## Command Input
The program begins by reading a command from stdin. Valid commands include:
- `get`
- `set`

### "get" Command
Upon receiving the `get` command:
- It expects a subsequent filename (referred to as 'location').
- If the file is present within the current directory:
  - Outputs its content to stdout.
  - Otherwise, returns an "Invalid Command" message to stderr.

### "set" Command
Upon receiving the `set` command:
- Expects a filename (location), followed by content length and the actual content.
- Either creates or overwrites the specified file.
  - Reads content from stdin and writes to the file.
  - If successful, returns "OK" to stdout. 
  - Otherwise, returns "Operation Failed" to stderr.

### Invalid Commands
The program returns "Invalid Command" to stderr if:
- Command isn't `get` or `set`.
- Missing filename post-command.
- Filename is invalid or has non-ASCII characters/spaces.
- Extraneous content post a valid `get` command.

## Efficiency & Constraints
- **Error Handling**: Handles scenarios like missing files, invalid commands, I/O errors.
- **Time Efficiency**: Uses buffering for efficient I/O operations.
- **Space Efficiency**: Restricts memory usage to a capped amount.
- **Content Handling**: Writes either less or truncated content based on the `set` command details.
- **Clean Operation**: Ensures no file descriptor or memory leaks, and operates without crashes.
- **Usage Restrictions**: Avoids specific `C stdio.h` functions and doesn't execute external programs.

