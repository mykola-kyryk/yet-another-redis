#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>

void process(int connection_fd) {
    printf("Work is being done ;)\n");
    
    char read_buffer[64] = {};
    ssize_t bytes_read = read(
        connection_fd,
        read_buffer,
        sizeof(read_buffer) - 1
    );
    if (bytes_read < 0) {
        printf("Failed to read from incoming connection.");
        return;
    }
    printf("Client says: %s\n", read_buffer);

    char write_buffer[] = "world";
    ssize_t bytes_written = write(
        connection_fd,
        write_buffer,
        strlen(write_buffer)
    );
    if (bytes_written < 0) {
        printf("Failed to write into incoming connection.");
        return;
    }
}

int main() {
    int sock_fd = socket(
        // AF_INET is for IPv4
        AF_INET,
        // SOCK_STREAM is for TCP
        SOCK_STREAM, 
        // The  protocol specifies a particular protocol to be used with the socket.  Normally only a single protocol
        // exists to support a particular socket type within a given protocol family, in which case protocol  can  be
        // specified  as 0.  However, it is possible that many protocols may exist, in which case a particular proto‐
        // col must be specified in this manner.
        0
    );
    if (sock_fd == -1) {
        printf("Failed to acquire socket file descriptor.\n");
        return -1;
    }

    int val = 1;
    setsockopt(
        sock_fd,
        SOL_SOCKET, // level at which option is specified
        SO_REUSEADDR, // option name; Without this server won’t able to bind to the same address if restarted.
        &val, // option value; `0` means disable, any non-zero value (like `1`) means option is enabled
        sizeof(val)
    );


    // we’ll bind on the wildcard address 0.0.0.0:1234
    struct sockaddr_in addr = { };
    addr.sin_family = AF_INET; // AF_INET is for IPv4
    // The ntohs() function converts the unsigned short integer netshort from network byte order to host byte order.
    addr.sin_port = ntohs(1234); // port `1234`
    // The ntohl() function converts the unsigned integer netlong from network byte order to host byte order
    addr.sin_addr.s_addr = ntohl(0); // IP is set to wildcard address 0.0.0.0
        
    int bind_error = bind(
        sock_fd,
        (struct sockaddr *)&addr,
        sizeof(addr)
    );
    if (bind_error != 0) {
        printf("Failed to bind socket to address 0.0.0.0:1234.\n");
        return -2;
    }

    // listen
    int listen_error = listen(
        sock_fd,
        SOMAXCONN // maximum size to which the queue of pending connections for socket may grow
    );
    if (listen_error != 0) {
        printf("Failed to start listening on socket at address 0.0.0.0:1234.\n");
        return -3;
    }

    // accept connections    
    while (true) {
        struct sockaddr_in client_sock_addr = {};
        socklen_t client_sock_len = sizeof(client_sock_addr);

        int connection_fd = accept(
            sock_fd,
            (struct sockaddr *)&client_sock_addr,
            &client_sock_len
        );
        if (connection_fd < 0) {
            printf("Failed to accept an incoming connection.\n");
            continue;
        }

        process(connection_fd);
        close(connection_fd);
    }

    return 0;
}