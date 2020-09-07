### TODO Please edit the following information in this README

- Name:Xinyu Dai
- How many hours did it take you to complete this assignment?
  - I spent 30 hours to complete this assignment.
- Did you collaborate with any other students/TAs/Professors? (Name them below)
  - None.
- Did you use any external resources? (Cite them below)
  - Beej's Guide to Networking Listed in Project Description.
- (Optional) What was your favorite part of the assignment?
  - The implementation of client and server communication.
- (Optional) How would you improve the assignment?
  - Extract all functions applied in the assignment for better reference.

The detail information of the assignment: https://course.ccs.neu.edu/cs5007su19-seattle/assignments/project.html

# Final Project: Client-Server

## Steps:
* ```Write The Client```:
    After started, the client should stay running until the user terminates it.
    Identify and implement a graceful way to with a server that is not accepting connections. This could be very simple or more complex, but your program should not crash. (It can terminate intentionally, with an informative message.)
    The client should make a new connection for every query, and close it properly.
    Limit query from user to 100 characters.
    Makefile has a target client which generates an executable queryclient (provided; if you need modifications that is fine)
    Client is started by calling queryclient [ipaddress] [port]
    Note: localhost is the same as IP 127.0.0.1
    If the wrong input is provided when launching the program, print out instructions to run it properly. 
* ```Write A Simple Server```:
    There is a target in the Makefile called "server", that builds the server. Modify it as/if necessary.
Create an executable called "queryserver".
Run the server like this: ./queryserver -f [datadir] -p [port], where the -f flag specifies which directory to index, and the -p flag specifies which port the server should be listening on for connections. We'll continue to use the data_tiny, data_small and data directories for data. Copy them into your project directory if you want, or modify the path to point at your data folders.
If improper arguments are provided when launching, handle it gracefully and provide a message specifying correct usage.

    Handling graceful could be either providing defaults or specifying the appropriate usage.
Run your server with valgrind to ensure you're handling memory properly. There should be no memory in use after the server is killed.
The server can be killed gracefully by hitting ctrl+C. There is a function in QueryServer.c (void sigint_handler(int sig)) that captures the ctrl+C event and does some cleanup. You might want to take advantage of this, but you don't need to worry about the details of it yet. 
* ```Write Multi Servers```:
    Makefile has a target multiserver which generates an executable multiserver
    Server is started by calling multiserver -f [dirname] -p [port]
    There is an optional flag -d for debugging. When the -d is passed when starting the multiserver, please ask the new process to sleep for 10 seconds before handling the query.
    If the wrong input is provided, print out instructions to run it properly.
    Run your code with valgrind to ensure you handled memory properly.
    You might want to introduce sleeps in your code to help test multiple connections.
    Commit your code! And push it to github, if you haven't done this yet.

