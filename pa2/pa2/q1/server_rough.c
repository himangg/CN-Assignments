#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>

#define BUFF_SIZE 1024

typedef struct {
    char proc_name[256];
    int pid;
    unsigned long u_time;
    unsigned long k_time;
} ProcDetails;

int fetch_proc_info(const char* pid, ProcDetails* proc_info) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/%s/stat", pid);
    
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    char buff[BUFF_SIZE];
    read(fd, buff, sizeof(buff));
    close(fd);

    sscanf(buff, "%d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
           &proc_info->pid, proc_info->proc_name, &proc_info->u_time, &proc_info->k_time);

    memmove(proc_info->proc_name, proc_info->proc_name + 1, strlen(proc_info->proc_name) - 2);
    proc_info->proc_name[strlen(proc_info->proc_name) - 2] = '\0';

    return 0;
}

int comp_cpu_time(const void* proc1, const void* proc2) {
    return ((ProcDetails*)proc2)->u_time + ((ProcDetails*)proc2)->k_time -
           (((ProcDetails*)proc1)->u_time + ((ProcDetails*)proc1)->k_time);
}


void get_top_two_procs(char* res) {
    DIR* proc_dir = opendir("/proc");
    if (!proc_dir) {
        perror("Failed to open /proc directory");
        strcpy(res, "Error retrieving process information");
        return;
    }

    struct dirent* dir_entry;
    ProcDetails procs[1024];
    int proc_counter = 0;

    while ((dir_entry = readdir(proc_dir)) != NULL) {
        if (dir_entry->d_type == DT_DIR) {
            char* end_pointer;
            long pid = strtol(dir_entry->d_name, &end_pointer, 10);
            if (*end_pointer == '\0') {
                if (fetch_proc_info(dir_entry->d_name, &procs[proc_counter]) == 0) {
                    proc_counter++;
                }
            }
        }
    }
    closedir(proc_dir);

    qsort(procs, proc_counter, sizeof(ProcDetails), comp_cpu_time);

    snprintf(res, BUFF_SIZE, 
             "Process 1: %s (PID: %d), User Time: %lu, Kernel Time: %lu\n"
             "Process 2: %s (PID: %d), User Time: %lu, Kernel Time: %lu\n",
             procs[0].proc_name, procs[0].pid, procs[0].u_time, procs[0].k_time,
             procs[1].proc_name, procs[1].pid, procs[1].u_time, procs[1].k_time);
}

void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);
    
    pthread_t thread_id = pthread_self();
    printf("Handling client in thread: %lu\n", thread_id);

    char buff[BUFF_SIZE];
    ssize_t receive_stat = recv(client_socket, buff, BUFF_SIZE - 1, 0);
    if (receive_stat < 0) {
        perror("Receive failed");
        close(client_socket);
        return NULL;
    }

    buff[receive_stat] = '\0';
    printf("Client request: %s\n", buff);

    char res[BUFF_SIZE];
    get_top_two_procs(res);

    send(client_socket, res, strlen(res), 0);
    close(client_socket);
    return NULL;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Port number not provided!\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(port);

    int bind_res = bind(server_socket, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    if (bind_res < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(1);
    }

    printf("Server setup successful, listening port %d\n", port);

    while (1) {
        struct sockaddr_in clientaddr;
        socklen_t addr_len = sizeof(clientaddr);
        int client_socket = accept(server_socket, (struct sockaddr*)&clientaddr, &addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        int* client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, client_socket_ptr);
        pthread_detach(thread);
    }

    close(server_socket);
    return 0;
}
