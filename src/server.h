#ifndef SERVER_H
#define SERVER_H

/*

    server.h

    Purpose:
    Declares the server class responsible for 
    -creating a TCP socket 
    -binding it to a specific port
    -listening for incoming clients connections
    -accepting a basic client connection


*/

class Server {
    private:
        int port;   // The port number the server will listen on
        int server_fd; // File descriptor for the server socket
        bool running;   // track if server is running

    public:
    
    //constructors reciebes the port number
    explicit Server(int port);
    bool start();
    void stop();

};

#endif