#include "utils.h"

int * establishConnectionsFromExternalProcesses() {
    static int client_socket[NUM_EXTERNALS];
    int socket_desc;
    struct sockaddr_in server_addr, client_addr;
    unsigned int client_size;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        printf("Error creating socket\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Couldn't bind to port\n");
        exit(1);
    }

    if (listen(socket_desc, NUM_EXTERNALS) < 0) {
        printf("Error listening\n");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    for (int i = 0; i < NUM_EXTERNALS; i++) {
        client_size = sizeof(client_addr);
        client_socket[i] = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);
        if (client_socket[i] < 0) {
            printf("Error accepting client %d\n", i);
            exit(1);
        }
        printf("External process %d connected from %s:%d\n",
               i + 1, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

    printf("All four clients connected.\n\n");
    return client_socket;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./server <initial_temp>\n");
        return -1;
    }

    float centralTemp = atof(argv[1]);
    float prevCentral = centralTemp;
    float externalTemps[NUM_EXTERNALS];
    float prevTemps[NUM_EXTERNALS] = {0};

    struct msg message;
    int *client_socket = establishConnectionsFromExternalProcesses();

    int stable = 0;
    int iteration = 0;

    while (!stable) {
        iteration++;

        // Receive temperatures from all 4 clients
        for (int i = 0; i < NUM_EXTERNALS; i++) {
            if (recv(client_socket[i], &message, sizeof(message), 0) < 0) {
                printf("Error receiving from client %d\n", i + 1);
                exit(1);
            }
            externalTemps[i] = message.T;
            printf("[Server] Received from client %d: %.3f\n", i + 1, externalTemps[i]);
        }

        // Compute new central temperature
        float newCentral = (2 * centralTemp +
                            (externalTemps[0] + externalTemps[1] +
                             externalTemps[2] + externalTemps[3])) / 6.0;

        printf("[Server] New central temp: %.4f\n", newCentral);

        // Check for convergence
        stable = 1;
        for (int i = 0; i < NUM_EXTERNALS; i++) {
            if (fabs(externalTemps[i] - prevTemps[i]) > EPS)
                stable = 0;
        }
        if (fabs(newCentral - prevCentral) > EPS)
            stable = 0;

        // Send updated central temperature to clients
        for (int i = 0; i < NUM_EXTERNALS; i++) {
            message.T = newCentral;
            message.done = stable;
            send(client_socket[i], &message, sizeof(message), 0);
        }

        // Update values
        for (int i = 0; i < NUM_EXTERNALS; i++)
            prevTemps[i] = externalTemps[i];
        prevCentral = centralTemp;
        centralTemp = newCentral;
    }

    printf("\n[Server] System stabilized after %d iterations. Final central temperature = %.4f\n",
           iteration, centralTemp);

    // Close all sockets
    for (int i = 0; i < NUM_EXTERNALS; i++)
        close(client_socket[i]);

    printf("[Server] All client connections closed. Exiting.\n");
    return 0;
}
