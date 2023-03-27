#!/bin/bash
g++ -Wall -Wextra -O2 -g src/server.c -o bin/server
g++ -Wall -Wextra -O2 -g src/client.c -o bin/client