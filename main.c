#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include "server.h"
#include "client.h"
#include "tags.h"


int main(int argc, char* argv[]) {
    // if start server
    if (argc == 1) {
        if(fork() == 0) {
            start_server(argv[0]);
        } else {
            return 0;
        }
    }

    int32_t response = 0;
    if (argc == 2) {
        char *flag = argv[1];
        if (strcmp(flag, "-g") == 0) {
            response = get_data(GET_TIME, 0);
            printf("%d\n", response);
            return 0;
        }
    }

    return 0;
}
