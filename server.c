#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tags.h"

void start_server(char* program_name) {
    int server_socket, client_socket;
    int n_bind;

    // initialize timer
    time_t time_start;
    time(&time_start);

    struct sockaddr_in addr, client_addr;


    memset(&addr, 0, sizeof(struct sockaddr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERVER_PORT);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        fprintf(stderr, "Can't create a socket.\n");
        exit(1);
    }

    n_bind = bind(server_socket, (struct sockaddr*)&addr, sizeof(struct sockaddr));
    if (n_bind < 0) {
        fprintf(stderr, "Can't bind a name to a socket.\n");
        exit(1);
    }

    listen(server_socket, 0);

    while(1) {
        socklen_t socket_len = sizeof(struct sockaddr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &socket_len);
        if (client_socket < 0) {
            fprintf(stderr, "%s: Can't create a connection's socket.\n", program_name);
            exit(1);
        }

        int32_t data[2];
        read(client_socket, data, BUF_SIZE);
        time_t now;
        time(&now);

        int32_t payload[2] = {};

        switch (data[0]) {
            case GET_TIME:
                payload[0] = now - time_start;
                break;
            default:
                payload[0] = -1;
                break;
        }

        write(client_socket, payload, BUF_SIZE);
        close(client_socket);

    }
    close(server_socket);
}
