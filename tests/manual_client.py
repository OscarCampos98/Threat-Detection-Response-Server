#!/usr/bin/env python3

"""
manual_client.py

Purpose:
    Simple TCP test client for the Threat Detection & Response Server.

    This script connects to the local server, sends a sequence of
    test messages, and then closes the connection.

Usage:
    1. Start the C++ server:
        ./threat_server

    2. In another terminal, run:
        python3 tests/manual_client.py

Notes:
    This is not a unit test framework yet.
    It is a repeatable manual test client used to validate server behavior.
"""

import socket 
import time 

SERVER_HOST = "127.0.0.1"
SERVER_PORT = 8080

TEST_MESSAGES = [
    "HEARTBEAT",
    "STATUS OK",
    "ERROR TEMP_HIGH",
    "ERROR TEMP_HIGH",
    "ERROR TEMP_HIGH",
    "COMMAND INVALID",
    "BADMESSAGE",
    "STATUS",
    "ERROR",
]

def main():
    print(f"[TEST ClIENT] Connection to {SERVER_HOST}:{SERVER_PORT}")

    try: 
        with socket.create_connection((SERVER_HOST, SERVER_PORT), timeout=5) as client_socket:
            print("[TEST CLIENT] Connected.")

            for message in TEST_MESSAGES:
                print(f"[TEST CLIENT] Sending: {message}")
                client_socket.sendall((message+"\n").encode("utf-8"))
                time.sleep(0.5)
            
            print("[TEST CLIENT] Finished sending messages. ")
    except ConnectionRefusedError:
        print("[ERROR] Conection refused. Is the server running.")
    except TimeoutError:
        print("[ERROR] Conection time out.")
    except OSError as error:
        print("[ERROR] Socket error: {error}")
    
    print("[TEST CLIENT] Disconnected")

if __name__ == "__main__":
    main()
    