#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#define SERVER_TCP_PORT 3000    /* well-known port */
#define BUFLEN      256          /* buffer length */
#define PACKET_SIZE = 100 //MAX packet size



int main(int argc, char **argv) {
    int sd, port;
    struct hostent *hp;
    struct sockaddr_in server;
    char *host;
    FILE *fp;
    char filename [BUFLEN];
    char rbuf[PACKET_SIZE];

    switch (argc) {
        case 2:
            host = argv[1];
            port = SERVER_TCP_PORT;
            break;
        case 3:
            host = argv[1];
            port = atoi(argv[2]);
            break;
        default:
            fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
            exit(1);
    }

    /* Create a stream socket */    
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Can't create a socket\n");
        exit(1);
    }

    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if ((hp = gethostbyname(host))) {
        bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
    } else if (inet_aton(host, (struct in_addr *)&server.sin_addr)) {
        fprintf(stderr, "Can't get server's address\n");
        exit(1);
    }

    /* Connecting to the server */
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        fprintf(stderr, "Can't connect\n");
        exit(1);
    }

    printf("Enter the filename you wish to retrieve");
    fgets(filename, BUFLEN, stdin); //Take user input for filename and store it to buffer
    filename[strcspn(filename, "\n")] = 0; 


    write(sd, filename, strlen(filename)); //send file name to server

    file = fopen("dl_file" , "wb"); //Binary write mode to write file onto dl_file
    if (!file){
        fprintf(stderr, "error opening file \n"); 
        exit(1);
    }

    while (1) {
    int n = read(sd, rbuf, PACKET_SIZE); //read data from socket into buffer 
    if (n <= 0) break; //if connection is closed then break out of TCP connection loop

    if (rbuf[0] == 'E') {  //check if error message has been received from server
    printf("Error: %s\n", rbuf + 1);  //print error msg after acknowledging it was sent from server
    break;
    } 
    else {
    fwrite(rbuf, 1, n, file); //download file if no errors occur (n bytes written one at a time)
    }
    }
    fclose(file);
    close(sd);
    return 0;
    }