#include "server.h"
#include <iostream>

using namespace std;

/*
    main.cpp

    purpose:
    Entry point for the threat detection server application

      Current milestone:
    - create a Server object
    - start listening on port 8080

    Later:
    - initialize parser
    - initialize threat engine
    - initialize response engine
    - initialize logger
    - connect all modules together

*/

int main(){
    const int port = 8080;
    cout << "[INFO] Starting Threat Detection & Response server...\n";
    Server server(port);
    
    if (!server.start()) {
        cerr << "[ERROR] Failed to start server\n";
        return 1;
    }

    server.stop();

    return 0;
    
}
