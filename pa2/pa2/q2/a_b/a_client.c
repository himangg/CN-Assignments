#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <client_id>\n", argv[0]);
        exit(1);
    }

    int client_id = atoi(argv[1]);
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buff[1024];

    printf("Client ID: %d is connecting to server...\n", client_id);

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        printf("Error creating socket for client %d\n", client_id);
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);


    int ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        printf("Error in connecting to server for client %d. Server may be busy.\n", client_id);
        close(clientSocket);
        exit(1);
    }


    char* message = "GET_TOP_CPU_PROCESSES";
    send(clientSocket, message, strlen(message), 0);


    int bytes_received = recv(clientSocket, buff, sizeof(buff) - 1, 0);
    if (bytes_received < 0) {
        printf("Error in receiving data for client %d\n", client_id);
    } else {
        buff[bytes_received] = '\0';
        printf("Client ID: %d - Response from server:\n%s\n", client_id, buff);
    }

    close(clientSocket);
    return 0;
}
