/*
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|   TASK : WRITE A CLIENT PROGRAM THAT MAKES REQUESTS WITH SERVER USING SOCKETS                                                                                                  |                                          |
|   The client establishes a connection with the server socket on the specified port. Then, it passes NLINEX, READX and INSERTX requests to the server.                          |                                                                                                                                    |
|       1. The client takes NLINEX, READX or INSERTX commands from the user, along with the required parameters.                                                                 |                |
|       2. It constructs the request and passes it to the server                                                                                                                 |
|       3. Fetches the output message from the server and displays it to the user.                                                                                               |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*/

/* --- Defining Header files which contains several function for accomplishing our task --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/* --- Defining a static port no for process communication --- */
#define port 9999

/* --- Function that will be used for printing error messages ---*/
void printerror(char *msg)
{
    printf("%s\n", msg);
    exit(1);
}
int main()
{
    int socketFd, n, conn;
    char buffer[1024]; // Defining a buffer that will store the client message
    
    //Initializing the socket with required parameters
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0)
    {
        printerror("Error in Opening the Socket");
    }

    //Initializing the server structure with port no, IP address etc.
    struct sockaddr_in server_addr;
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port);

    //Making connection with server using connect() function
    conn = connect(socketFd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (conn < 0)
    {
        printerror("Error in connecting to server");
    }


    printf("-----Welcome to Socket Programming World-----\n");
    printf("Insert following commands for server in the same form mentioned below:-\n");
    printf(" 1.'NLINEX' : Counting the number of lines in the server file\n");
    printf(" 2.'READX k' : Reading the kth line from the server file \n");
    printf(" 3.'INSERTX k message': Inserting a message in the kth line in the server file\n");
    printf(" 4.'EXIT' : For disconnecting the connection \n");

    //Communication between the server and client starts here
    while(1)
    {
        /*
           Here we perform three tasks:
              1. Initialize the buffer[]
              2. Taking the client request as input and store it in the buffer
              3. Transfer the buffer content to server of the specified port.
        */
        bzero(buffer,1024);
        printf("Enter the command:\n");
        fgets(buffer,1024,stdin);
        n=write(socketFd,buffer,strlen(buffer));
        if (n < 0)
        {
            printerror("Error occurs while writing");
        }
        bzero(buffer,1024);
        //Reading the message that has been sent by the server using read function
        n=read(socketFd,buffer,1024);
        if (n < 0)
        {
            printerror("Error occurs while reading");
        }
        //Printing the server message 
        printf("Message from Server: %s\n",buffer);
        int check;
        //Checking whether the server want to break the connection or not
        check=strncmp("Successfully Disconnect",buffer,strlen("Successfully Disconnect"));
        if(check==0)
        {
            break;
        }
    }
    //Closing the client socket using close() function
    close(socketFd);
    return 0;
}