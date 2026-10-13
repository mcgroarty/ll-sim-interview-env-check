#!/usr/bin/env python3
"""check.py - verifies the env_check program end to end.

Connects to an already running env_check, sends one line, and confirms the
same line comes back. Starts nothing itself.

Usage: python3 scripts/check.py [port]
"""
import socket
import sys

DEFAULT_PORT = 7777            # must match kDefaultPort in src/env_check.cpp
HOST = "127.0.0.1"
TIMEOUT_SECONDS = 3            # max wait for connect and for the echo
RECV_CHUNK_SIZE = 1024         # bytes requested per recv() call
PROBE_LINE = b"hello from check.py\r\n"


def main():
    port = int(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_PORT
    try:
        sock = socket.create_connection((HOST, port), timeout=TIMEOUT_SECONDS)
    except OSError as e:
        sys.exit("could not connect to {}:{} ({}). Is env_check running?".format(HOST, port, e))
    with sock:
        sock.sendall(PROBE_LINE)
        # Read until the full line is back or the program closes the connection.
        received = b""
        while not received.endswith(b"\n"):
            chunk = sock.recv(RECV_CHUNK_SIZE)
            if not chunk:
                break
            received += chunk
    if received != PROBE_LINE:
        sys.exit("echo mismatch: sent {!r}, got {!r}".format(PROBE_LINE, received))
    print("environment check passed")
    print("Your build tools and network setup are ready for the interview.")
    print("Please check the README for any other requirements, like your editor,")
    print("AI coding tool, and screen sharing.")


if __name__ == "__main__":
    main()
