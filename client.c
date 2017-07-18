#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "tags.h"

int32_t get_data(int32_t tag, int32_t data) {
    struct sockaddr_in sck_addr;
    int sck;

    memset(&sck_addr, 0, sizeof sck_addr);
    sck_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &sck_addr.sin_addr);
    sck_addr.sin_port = htons(SERVER_PORT);

    if ((sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        fprintf(stderr, "Can't create a socket.\n");
        exit(EXIT_FAILURE);
    }

    if (connect(sck, (struct sockaddr*) &sck_addr, sizeof sck_addr) < 0) {
        fprintf(stderr, "Can't connect to server\n");
        exit(EXIT_FAILURE);
    }

    int32_t payload[2] = {tag, data};

    write(sck, payload, BUF_SIZE);

    int32_t bufor[2];
    read(sck, bufor, BUF_SIZE);
    close(sck);
    return bufor[0];
}
