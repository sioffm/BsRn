#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Read message from remote machine and print it,
// then read the message from the local machine and
// send it to the remote one.
void message_loop(int connfd) //connfd connection file desciptor
{
    char message[MAX];
    int n;
    // infinite loop for chat
    while (1)
    {

        // read the message from remote machine
        read(connfd, message, sizeof(message));
        // print the incoming message
        printf("Received: %s\n", message);

        // clears the content of the message
        bzero(message, MAX); // bzero clear the content of array

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
    }
}

// Driver function
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0)
    {
        printf("server accept failed...\n");
        exit(0);
    }

    printf("server waiting for messages\n");
    // Function for chatting between local and remote machine
    message_loop(connfd);

    // After chatting close the socket
    close(sockfd);
}
