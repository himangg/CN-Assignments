#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080

void* conn_serv(void* arg) {
    int client_socket, ret;
    struct sockaddr_in serverAddr;
    char buff[1024];
    
    pthread_t thread_id = pthread_self();
    printf("Thread ID: %lu is connecting to server\n", thread_id);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("Error creating socket in thread %lu\n", thread_id);
        pthread_exit(NULL);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    ret = connect(client_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        printf("Error in connecting to server in thread %lu\n", thread_id);
        close(client_socket);
        pthread_exit(NULL);
    }

    char* message = "GET_TOP_CPU_PROCESSES";
    send(client_socket, message, strlen(message), 0);

    int rec = recv(client_socket, buff, 1024, 0);
    if (rec < 0) {
        printf("Error in receiving data in thread %lu\n", thread_id);
    } else {
        buff[rec] = '\0';
        printf("Thread ID: %lu - Response from server:\n%s\n", thread_id, buff);
    }

    close(client_socket);
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
        if (pthread_create(&threads[i], NULL, conn_serv, NULL) != 0) {
            printf("Error in thread creation %d\n", i);
        }
    }

    for (int i = 0; i < n_clients; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    return 0;
}
