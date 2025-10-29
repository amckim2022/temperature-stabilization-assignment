#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include "utils.h"



int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./client <id> <initial_temp>\n");
        return -1;
    }

    int id = atoi(argv[1]);
    float externalTemp = atof(argv[2]);
    struct msg message;

    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("Unable to create socket\n");
        return -1;
    }

    // Server connection details
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("[Client %d] Unable to connect to server\n", id);
        close(client_socket);
        return -1;
    }

    printf("[Client %d] Connected to server. Initial temperature = %.2f\n", id, externalTemp);

    int done = 0;
    int iteration = 0;

    while (!done) {
        iteration++;

        // Send current temperature to the server
        message.T = externalTemp;
        message.done = 0;
        send(client_socket, &message, sizeof(message), 0);

        // Receive the server's new central temperature and "done" flag
        if (recv(client_socket, &message, sizeof(message), 0) <= 0) {
            printf("[Client %d] Error receiving data from server\n", id);
            break;
        }

        done = message.done;
        if (done) {
            printf("[Client %d] System stabilized. Final external temperature = %.4f\n",
                   id, externalTemp);
            break;
        }

        // Update external temperature using formula
        externalTemp = (3 * externalTemp + 2 * message.T) / 5.0;

        printf("[Client %d] Iteration %d: Updated external temperature = %.4f\n",
               id, iteration, externalTemp);
    }

    close(client_socket);
    printf("[Client %d] Connection closed.\n", id);
    return 0;
}
