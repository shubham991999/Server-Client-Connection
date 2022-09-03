/*
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|   TASK : WRITE A SERVER PROGRAM WHICH SERVES THE CLIENT REQUESTS USING SOCKET                                                                                                                      |
|    The server creates a socket and listens on some specific port for client connection.After connecting with the client, it waits for client's requests. The server contains a |
|   default file.After receiving the request from client,the server should parse the string received from the client, which will contain either of NLINEX, READX or INSERTX      |
|   requests where                                                                                                                                                               |
|       1. NLINEX : This is a helper command for the client to know the no. of lines in the file.                                                                                |
|       2. READX K: : Reads the k-th line from the file, containing, say, N lines and returns it to the client.                                                                  |
|       3. INSERTX K message: Inserts the string specified in message in the k-th line to the file. After that server return a message of success or failure.                    |
|                                                                                                                                                                                |
|   Additionally Server program has to handle different kinds of errors like in "INSERTX K message" if k value is missing then server has to insert the message at               |
|   end of the file, if message is not present then server program tell about this to client etc.                                                                                |
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
#include<limits.h>

/* --- Defining a static port no for process communication --- */
#define port 9999

/* --- Function that will be used for printing error messages ---*/
void printerror(char *msg)
{
    printf("%s\n", msg);
    exit(1);
}

/*--- Function that will be used for converting string to integer representation ---*/
int conv_str_to_int(char *k, int start, int end)
{
    int lineno, i,t;
    lineno = 0;
    for (i = start; i < end; i++)
    {
        if (k[i] >= 48 && k[i] <= 57)
        {
            t = k[i] - '0';
            lineno = 10 * lineno + t;
        }
        else{
            return INT_MIN;
        }
    }
    return lineno;
}


int main()
{

    int socketFd, newsocketFd, client_len, n;
    char buffer[1024]; // Defining a buffer that will store the client message

    socketFd = socket(AF_INET, SOCK_STREAM, 0); //Initializing the socket with required parameters
    if (socketFd < 0)
    {
        printerror("Error in Opening the Socket");
    }

    //Initializing the server structure with port no, IP address etc.

    struct sockaddr_in server_addr, client_addr;
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    //Bind function that binds the socket to the address and port number specified in server_addr structure
    if (bind(socketFd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printerror("Error in Binding");
    }

    //listen function that put the server in passive maode and 5 represents the size of queue for pending requests
    int k=listen(socketFd, 5);
    if(k<0)
    {
        printerror("Error in listening");
    }

    /* 
       Extracting the first connection request from queue and creates a new connected socket.
       After that connection between the client and server has been established.
    */
    client_len = sizeof(client_addr);
    newsocketFd = accept(socketFd, (struct sockaddr *)&client_addr, &client_len);
    if (newsocketFd < 0)
    {
        printerror("Error in accept connection");
    }

    //Communication between the server and client starts here
    while (1)
    {
        /*
           Here we perform three tasks:
              1. Initialize the buffer[]
              2. Reading the client request and store it in the buffer
              2. Print the buffer contents
        */
        bzero(buffer, 1024);
        n = read(newsocketFd, buffer, 1024);
        if (n < 0)
        {
            printerror("Error occurs while reading");
        }
        printf("Message from Client: %s", buffer);
        
        
        //Parse the client command and checking whether it is a valid command or not
        char *t = strtok(buffer, " \n\t");
        if (strncmp("EXIT", t, 4) ==0 && strlen(t) <= 5)  //If command is EXIT then we break the connection.
        {
            strcpy(buffer, "Successfully Disconnect");
            n = write(newsocketFd, buffer, strlen(buffer));
            if (n < 0)
            {
                printerror("Error occurs while writing");
            }
            break;
        }
        else if (strncmp("NLINEX", t, 6) == 0) //If command is NLINEX then we open the "server_file" and count the number of lines
        {
            //checking whether NLINEX command is given in proper manner
            if (strlen(t) > 7)
            {
                strcpy(buffer, "Sorry,You entered a invalid command");
            }
            else
            {
                t = strtok(NULL, "");
                if (t == NULL)
                {
                    FILE *f;
                    f = fopen("server_file.txt", "r");
                    if (f == NULL)
                    {
                        strcpy(buffer, "Sorry, we cannot open the file");
                    }
                    else
                    {
                        int countLine = 0;
                        char aux_Buffer[1000];
                        while (fgets(aux_Buffer, 1000, f) != NULL)//Counting the number of lines
                        {
                            ++countLine;
                        }
                        sprintf(buffer, "The number of lines present in the file = %d", countLine);//Storing the result into the buffer
                    }
                    fclose(f);
                }
                else
                {
                    strcpy(buffer, "You have entered invalid texts after NLINEX Command");
                }
            }
        }
        else if (strncmp("READX", t, 5) == 0)// If the command is READX then we read the kth line from the "server_file"
        {
            //checking whether READX command is given in proper manner
            if (strlen(t) > 6)
            {
                strcpy(buffer, "Sorry,You entered a invalid command");
            }
            else
            {
                /*
                    Checking whether k value is missing or not.
                    If k value is missing 
                    then we just open the "server_file" and copy the first line from the file into the buffer
                */
                t = strtok(NULL, " \t");
                
                if (t == NULL)
                {
                    FILE *f;
                    f = fopen("server_file.txt", "r");
                    if (f == NULL)
                    {
                        strcpy(buffer, "Sorry, we cannot open file for you");
                    }
                    else
                    {
                        fgets(buffer, 1024, f);
                    }
                    fclose(f);
                }
                else
                {
                    char *temp = t;
                    t = strtok(NULL, "");
                    /*
                       Checking whether READX command contains extra things after k or not.
                       If no extra things present 
                            then we check whether k is valid or not.

                            if k is valid
                               then read the kth line from server_file and give it to the client.
                            else 
                               give error message to client.

                       If extra things present, then give error message to client. 
                    */
                    if (t == NULL)
                    {
                        int i, lineno, flag;
                        lineno = 0;
                        i=strlen(temp)-1;
                        flag = 0;
                        if (temp[0] == '-')
                        {
                        /* calling conv_str_to_int() function for converting string to integer */
                            lineno=conv_str_to_int(temp,1,i);
                            if (lineno==INT_MIN)
                            {
                                flag=1;
                            }
                            if (flag == 0)
                            {
                                lineno = 0 - lineno;
                            }
                        }
                        else
                        {
                        /*  calling conv_str_to_int() function for converting string to integer  */
                            lineno=conv_str_to_int(temp,0,i);
                            if (lineno==INT_MIN)
                            {
                                flag=1;
                            }
                        }
                        if (flag == 1)
                        {
                            strcpy(buffer, "You have entered an invalid k value");
                        }
                        else
                        {
                            FILE *f;
                            f = fopen("server_file.txt", "r");
                            if (f == NULL)
                            {
                                strcpy(buffer, "Sorry,we cannot open file for you");
                            }
                            else
                            {
                                int countLine = 0;
                                int range1, range2, count;
                                char aux_Buffer[1000];
                                while (fgets(aux_Buffer, 1000, f) != NULL)//Counting the number of lines
                                {
                                    ++countLine;
                                }
                                rewind(f);
                                range1 = 0 - countLine;
                                range2 = countLine;
                                if (lineno >= range1 && lineno < range2)
                                {
                                    if (lineno < 0)
                                    {
                                        lineno = lineno + range2;
                                    }
                                    count = 0;
                                    while (fgets(aux_Buffer, 1000, f) != NULL)
                                    {
                                        if (count == lineno)
                                        {
                                            strcpy(buffer, aux_Buffer);
                                            break;
                                        }
                                        ++count;
                                    }
                                }
                                else
                                {
                                    strcpy(buffer, "You have entered out of range value");
                                }
                            }
                            fclose(f);
                        }
                    }
                    else
                    {
                        strcpy(buffer, "You have entered extra information after k");
                    }
                }
            }
        }
        else if (strncmp("INSERTX", t, 7) == 0) //if command is INSERTX then we open the server_file and insert the message at kth line
        {
            //Checking whether INSERTX command is given in proper manner.
            if (strlen(t) > 8)
            {
                strcpy(buffer, "Sorry,You entered a invalid command");
            }
            else
            {
                char *k, *msg, aux_buffer[1000], temp_buffer[1000];
                bzero((char *)&aux_buffer, sizeof(aux_buffer));
                bzero((char *)&temp_buffer, sizeof(temp_buffer));
                /*
                  checking whether any thing k or message is present after the INSERTX Command or not. 
                  If they are not present then we give an error message to client through the buffer[].
                */
                t = strtok(NULL, " \t");
                if (t == NULL)
                {
                    strcpy(buffer, "You have not insert any thing after INSERTX command");
                }
                else
                {
                    k = t;
                    t = strtok(NULL, "\n");
                    int lineno, i, flag, pos;
                    if(t)
                    {
                        i=strlen(k);
                    }
                    else{
                        i=strlen(k)-1;
                    }
                    /*
                       Here we check following things-
                        1. If k value after INSERTX command is valid then 
                           we check whether any message is present after k or not.
                           --->if message is present then we simply go to kth line of server_file
                               and insert that message in that position.

                           --->if message is not present then we simply give an error message to server 
                               through the buffer[].

                        2. If k is not valid means it is string so we simply take whatever is present after 
                           INSERTX command as message and append that message at last end of file.
                    */
                    flag = 0;
                    if (k[0] == '-')
                    {
                        /*calling conv_str_to_int() function for converting string to integer*/
                        lineno=conv_str_to_int(k,1,i);
                        if(lineno==INT_MIN)
                        {
                            flag=1;
                        }
                        if (flag == 0)
                        {
                            lineno = 0 - lineno;
                        }
                    }
                    else
                    {
                        /*calling conv_str_to_int() function for converting string to integer*/
                        lineno=conv_str_to_int(k,0,i);
                        if(lineno==INT_MIN)
                        {
                            flag=1;
                        } 
                    }
                    
                    if (flag == 1)
                    {
                        sprintf(buffer, "%s", k);
                        if (t)
                        {
                            strcat(buffer, " ");
                            strcat(buffer, t);
                        }
                        FILE *f1;
                        f1 = fopen("server_file.txt", "r");
                        int countline = 1;
                        if (f1 == NULL)
                        {
                            strcpy(buffer, "Failed to open the server file");
                        }
                        else
                        {
                            while (fgets(aux_buffer, 1000, f1) != NULL)
                            {
                                ++countline;
                            }
                        }
                        fclose(f1);
                        f1 = fopen("server_file.txt", "a");
                        if (f1 == NULL)
                        {
                            strcpy(buffer, "Failed to open the server file");
                        }
                        else
                        {
                            strcat(temp_buffer,"\n");
                            strcat(temp_buffer,buffer);
                            fputs(temp_buffer, f1);
                            fclose(f1);
                            stpcpy(buffer, "Your message has beed added to server file successfully");
                        }
                    }
                    else
                    {
                        if (t == NULL)
                        {
                            strcpy(buffer, "You have not entered any message after k");
                        }
                        else
                        {
                            FILE *f;
                            int range1, range2, countline;
                            f = fopen("server_file.txt", "r");
                            if (f == NULL)
                            {
                                strcpy(buffer, "Failed to open the server file");
                            }
                            else
                            {
                                countline = 0;
                                while (fgets(aux_buffer, 1000, f) != NULL)//Counting the number of lines
                                {
                                    ++countline;
                                }
                                rewind(f);
                                range1 = 0 - countline;
                                range2 = countline;
                                if (lineno >= range1 && lineno < range2)
                                {
                                    if (lineno < 0)
                                    {
                                        lineno = lineno + range2;
                                    }
                                    countline = 0;
                                    /*
                                      Here we perform following things 
                                        1. We create a temporary file.
                                        2. copy the k number of lines from server_file to this temporary file.
                                        3. Now we insert the message given by client in this temporary file.
                                        4. Copy remaining contents of server_file after k to to this temporary file
                                        5. Finally we open the sever_file again and copy all contents of temporary file
                                           to this server_file.
                                    */

                                    FILE *tmp = tmpfile();
                                    if (tmp == NULL)
                                    {
                                        strcpy(buffer, "Unable to create a temporary file");
                                    }
                                    else
                                    {
                                        while (fgets(aux_buffer, 1000, f) != NULL)
                                        {
                                            if (countline == (lineno))
                                            {
                                                strcpy(buffer, t);
                                                strcpy(temp_buffer, buffer);
                                                strcat(temp_buffer, "\n");
                                                fputs(temp_buffer, tmp);
                                                break;
                                            }
                                            fputs(aux_buffer, tmp);
                                            ++countline;
                                        }
                                        fputs(aux_buffer, tmp);
                                        while (fgets(aux_buffer, 1000, f) != NULL)
                                        {
                                            fputs(aux_buffer, tmp);
                                        }
                                        fclose(f);
                                        rewind(tmp);
                                        f = fopen("server_file.txt", "w");

                                        if (f == NULL)
                                        {
                                            strcpy(buffer, "Failed to open the server file");
                                        }
                                        else
                                        {
                                            while (fgets(aux_buffer, 1000, tmp) != NULL)
                                            {
                                                fputs(aux_buffer, f);
                                            }
                                        }
                                        fclose(f);
                                        strcpy(buffer, "Your message has been successfully added to server file");
                                    }
                                }
                                else
                                {
                                    strcpy(buffer, "You tried to insert at a position which is out of bound");
                                }
                            }
                        }
                    }
                }
            }
        }
        else{
            /* If the command given by the Client is invalid then this block will be excuted and give the error message to client*/
            strcpy(buffer, "Sorry,You entered a invalid command");
        }
        //write() function transfers the contents of buffer to client side.
        n = write(newsocketFd, buffer, strlen(buffer));
        if (n < 0)
        {
            printerror("Error occurs while writing");
        }
    }
    //Closing the socket of server 
    close(newsocketFd);
    close(socketFd);
    return 0;
}
