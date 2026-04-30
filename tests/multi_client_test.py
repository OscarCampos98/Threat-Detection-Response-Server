#!/usr/bin/env python3

"""
multi_client_test.py

Purpose:
    Simulates multiple TCP clients connecting to the Threat Detection
    & Response Server at the same time.

    This validates:
    - concurrent client connections
    - thread-per-client handling
    - parser pipeline stability
    - threat classification under multiple clients
    - response decision generation
    - file-based logging under concurrent activity

Usage:
    1. Start the C++ server:
        ./threat_server

    2. In another terminal, run:
        python3 tests/multi_client_test.py
"""

import socket
import threading
import time 

SERVER_HOST = "127.0.0.1"
SERVER_PORT = 8080

CLIENT_SCENARIOS = {
    "sensor_01": [
        '{"client_id":"sensor_01","timestamp":"2026-04-30T09:40:00Z","event_type":"HEARTBEAT","status":"OK","request_id":"s1-001"}',
        '{"client_id":"sensor_01","timestamp":"2026-04-30T09:40:05Z","event_type":"STATUS","status":"OK","request_id":"s1-002"}',
        '{"client_id":"sensor_01","timestamp":"2026-04-30T09:40:10Z","event_type":"AUTH_ATTEMPT","status":"SUCCESS","request_id":"s1-003"}',
    ],
    "sensor_02": [
        '{"client_id":"sensor_02","timestamp":"2026-04-30T09:40:00Z","event_type":"HEARTBEAT","status":"OK","request_id":"s2-001"}',
        '{"client_id":"sensor_02","timestamp":"2026-04-30T09:40:05Z","event_type":"ERROR","status":"TEMP_HIGH","request_id":"s2-002"}',
        '{"client_id":"sensor_02","timestamp":"2026-04-30T09:40:10Z","event_type":"ERROR","status":"TEMP_HIGH","request_id":"s2-003"}',
    ],
    "sensor_03": [
        '{"client_id":"sensor_03","timestamp":"2026-04-30T09:40:00Z","event_type":"COMMAND","status":"INVALID","request_id":"s3-001"}',
        '{"client_id":"sensor_03","timestamp":"2026-04-30T09:40:05Z","event_type":"AUTH_ATTEMPT","status":"FAILED","request_id":"s3-002"}',
        '{"client_id":"sensor_03","event_type":"AUTH_ATTEMPT","status":"FAILED"',
    ],
}


def run_client(client_name, messages):
    """
    Connects to the server and sends a predefined message sequence.
    Each client runs in its own Python thread.
    """
    print(f"[{client_name}] Connecting to {SERVER_HOST}:{SERVER_PORT}")

    try:
        with socket.create_connection((SERVER_HOST, SERVER_PORT), timeout=5) as client_socket:
            print(f"[{client_name}] Connected")

            for message in messages:
                print(f"[{client_name}] Sending: {message}")
                client_socket.sendall((message + "\n").encode("utf-8"))
                time.sleep(0.5)

            print(f"[{client_name}] Finished sending messages")

    except ConnectionRefusedError:
        print(f"[{client_name}] ERROR: Connection refused. Is the server running?")
    except TimeoutError:
        print(f"[{client_name}] ERROR: Connection timed out.")
    except OSError as error:
        print(f"[{client_name}] ERROR: Socket error: {error}")

    print(f"[{client_name}] Disconnected")


def main():
    print("[MULTI-CLIENT TEST] Starting concurrent client simulation")

    threads = []

    for client_name, messages in CLIENT_SCENARIOS.items():
        thread = threading.Thread(target=run_client, args=(client_name, messages))
        threads.append(thread)
        thread.start()

        # Small delay so connections overlap but output remains readable.
        time.sleep(0.2)

    for thread in threads:
        thread.join()

    print("[MULTI-CLIENT TEST] Completed")


if __name__ == "__main__":
    main()