#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#define SERVER_TCP_PORT 3000    /* well-known port */
#define BUFLEN      256          /* buffer length */
#define PACKET_SIZE = 100 //MAX packet size

void send_file(int, const char*);

int main(int argc, char **argv) {
    int sd, new_sd, client_len, port;
    struct sockaddr_in server, client;
    char filename[BUFLEN];

    switch (argc) {
        case 1:
            port = SERVER_TCP_PORT;
            break;
        case 2:
            port = atoi(argv[1]);
            break;
        default:
            fprintf(stderr, "Usage: %s [port]\n", argv[0]);
            exit(1);
    }

    /* Create a stream socket */    
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Can't create a socket\n");
        exit(1);
    }

    /* Bind an address to the socket */
    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        fprintf(stderr, "Can't bind name to socket\n");
        exit(1);
    }

    /* queue up to 5 connect requests */
    listen(sd, 5);

    while (1) {
        client_len = sizeof(client);
        new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
        if (new_sd < 0) {
            fprintf(stderr, "Can't accept client\n");
            exit(1);
        }
    int n = read(new_sd, filename, BUFLEN); //read from client for sd and filename
    if(n > 0){
        filename[n] = 0;
        send_file(new_sd, filename);
    }
    close(new_sd);
    }
    close(sd);
    return 0;

}

void send_file(int client_sd, const char *filename) { 
    FILE *file = fopen(filename, "rb");
    char buffer[PACKET_SIZE];
    
    if (!file) {
        // Send error message to the client
        snprintf(buffer, PACKET_SIZE, "Error: File not found\n");
        write(client_sd, buffer, strlen(buffer));
        return;
    }

    // Send file in chunks of PACKET_SIZE
    while (1) {
        int n = fread(buffer, 1, PACKET_SIZE, file); //read file at 100 byte packets into buffer
        if (n <= 0) break;  // End of file, then terminate the loop
        write(client_sd, buffer, n); //write file data from buffer to client_sd
    }

    fclose(file);
}