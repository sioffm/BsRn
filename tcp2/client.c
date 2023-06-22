#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Read message from remote machine and print it,
// then read the message from the local machine and
// send it to the remote one.
void message_loop(int connfd)
{
    char message[MAX];
    int n;
    // infinite loop for chat
    while (1)
    {
        // read the local message
        printf("> ");
        if (fgets(message, MAX, stdin) == NULL)
        {
            // the user introduced Ctrl-D
            break;
        }
        else
        {
            // send the message to the remote machine
            write(connfd, message, sizeof(message));
        }

        // clears the content of the message
        bzero(message, MAX);
        // read the message from remote machine
        read(connfd, message, sizeof(message));
        // print the incoming message
        printf("Received: %s\n", message);

        // clears the content of the message
        bzero(message, MAX);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: chat <host name>\n");
        exit(1);
    }

    const char *host = argv[1];

    struct hostent *hostinfo;
    // Get host information by name
    hostinfo = gethostbyname(host);
    if (hostinfo == NULL)
    {
        printf("Error getting host information\n");
        exit(1);
    }

    // get the first host address
    struct in_addr *server_address;
    struct in_addr **addr_list = (struct in_addr **)hostinfo->h_addr_list;
    for (int i = 0; addr_list[i] != NULL; i++)
    {
        server_address = addr_list[i];
        break;
    }
    if (server_address == NULL)
    {
        printf("Host found but contains 0 addresses");
        exit(1);
    }

    // connect to the server
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
    {
        printf("Socket successfully created..\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = server_address->s_addr;
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
    {
        printf("connected to the server..\n");
    }

    // function for chat
    message_loop(sockfd);

    // close the socket
    close(sockfd);
}