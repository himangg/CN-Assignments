#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8082

void* connect_server(void* arg) {
    int clientSocket, ret;
    struct sockaddr_in serverAddr;
    char buff[1024];
    
    pthread_t thread_id = pthread_self();
    printf("Thread ID: %lu is connecting to server\n", thread_id);

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        printf("Error creating socket in thread %lu\n", thread_id);
        pthread_exit(NULL);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        printf("Error in connecting to server in thread %lu\n", thread_id);
        close(clientSocket);
        pthread_exit(NULL);
    }

    char* message = "GET_TOP_CPU_PROCESSES";
    send(clientSocket, message, strlen(message), 0);

    int bytes_received = recv(clientSocket, buff, 1024, 0);
    if (bytes_received < 0) {
        printf("Error in receiving data in thread %lu\n", thread_id);
    } else {
        buff[bytes_received] = '\0';
        printf("Thread ID: %lu - Response from server:\n%s\n", thread_id, buff);
    }

    close(clientSocket);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n_clients>\n", argv[0]);
        exit(1);
    }

    int n_clients = atoi(argv[1]);
    pthread_t* threads = (pthread_t*)malloc(n_clients * sizeof(pthread_t));

    for (int i = 0; i < n_clients; i++) {
        if (pthread_create(&threads[i], NULL, connect_server, NULL) != 0) {
            printf("Error in thread creation %d\n", i);
        }
    }

    for (int i = 0; i < n_clients; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    return 0;
}
