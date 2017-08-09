#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tags.h"

int *intervals = NULL;
int intervals_size = 6;
int default_intervals[] = {20, 5, 20, 5, 20, 15};
int current_interval = 0;

int get_remaining_sec(time_t *time_start, time_t now) {
    int remaining_sec = (intervals[current_interval] * 60) - (now - *time_start);
    if (remaining_sec <= 0) {
        remaining_sec = 0;
        current_interval = (current_interval + 1) % intervals_size;
        *time_start = now;
    }
    return remaining_sec;
}

char* get_config_path() {
    char *home = getenv("HOME");
    char *dot_pomidor = ".pomidor";
    char *filepath = malloc(strlen(home) + strlen(dot_pomidor) + 2);
    sprintf(filepath, "%s/%s", home, dot_pomidor);
    return filepath;
}

void set_default_intervals() {
    intervals = malloc(intervals_size * sizeof(int));
    if (!intervals_size) {
        fprintf(stderr, "Can't allocate memory for interval array\n");
        exit(0);
    }
    for (int i = 0; i < intervals_size; i++) {
        intervals[i] = default_intervals[i];
    }
}

void read_config() {
    char *filepath = get_config_path();
    FILE *config = fopen(filepath, "r");
    set_default_intervals();

    if (!config) {
        printf("Can't find config file, setting default intervals\n");
        return;
    }

    int i;
    for (i = 0;; i++) {
        int cur_interval;
        if (fscanf(config, "%d", &cur_interval) > 0) {
            int* new_intervals = realloc(intervals, (i + 1) * sizeof(int));
            if (new_intervals) {
                intervals = new_intervals;
            } else {
                fprintf(stderr, "Can't allocate memory for interval array\n");
                free(intervals);
                exit(0);
            }
            intervals[i] = cur_interval;
        } else {
            break;
        }
    }
    intervals_size = i;
}

void start_server(char* program_name) {
    int server_socket, client_socket;
    int n_bind;

    time_t time_start;
    time(&time_start);
    read_config();

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
                payload[0] = get_remaining_sec(&time_start, now);
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
