#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

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

    // addr: 127.0.0.1:1234
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET; // AF_INET is for IPv4
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);  // 127.0.0.1

    int connect_error = connect(
        sock_fd,
        (struct sockaddr *)&addr,
        sizeof(addr)
    );
    if (connect_error != 0) {
        printf("Failed to connect to a remote socket.\n");
        return -2;
    }

    char message[] = "hello";
    ssize_t bytes_written = write(sock_fd, message, strlen(message));
    if (bytes_written < 0) {
        printf("Failed to write into a remote socket.");
        return -3;
    }

    char read_buffer[64] = {};
    ssize_t bytes_read = read(sock_fd, read_buffer, sizeof(read_buffer) - 1);
    if (bytes_read < 0) {
        printf("Failed to read from a remote socket.");
        return -4;
    }

    printf("server says: %s\n", read_buffer);
    close(sock_fd);

    return 0;
}