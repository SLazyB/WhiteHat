#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h> 
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int cmpfunc (const void * a, const void * b)
{
   const char **ia = (const char **) a;
   const char **ib = (const char **) b;
   return strcmp(*ia, *ib);
}

int main(int argc, char** argv){
    if(argc != 1){
        perror("ERROR: Incorrect number of arguments");
        return EXIT_FAILURE;
    }
    printf("Started server\n");
    fflush(stdout);
      /* Create the listener socket as TCP socket */
    int sd = socket( PF_INET, SOCK_STREAM, 0 );
        /* here, the sd is a socket descriptor (part of the fd table) */

    if ( sd == -1 )
    {
        perror( "ERROR: SOCKET FAILED" );
        return EXIT_FAILURE;
    }


    /* socket structures */
    struct sockaddr_in server;
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = INADDR_ANY;  /* allow any IP address to connect */

    //unsigned short port = atoi(argv[1]);
    unsigned short port = 8080;

    /* htons() is host-to-network short for data marshalling */
    /* Internet is big endian; Intel is little endian */
    server.sin_port = htons( port );
    int len = sizeof( server );

    if ( bind( sd, (struct sockaddr *)&server, len ) == -1 )
    {
        perror( "ERROR: BIND FAILED" );
        return EXIT_FAILURE;
    }

    /* identify this port as a listener port */
    /* the value 5 here means that the OS will
        maintain a queue of at most 5
        pending connections for the accept() call */
    if ( listen( sd, 5 ) == -1 )
    {
        perror( "ERROR: LISTEN FAILED" );
        return EXIT_FAILURE;
    }

    printf( "Listening for TCP connections on port: %d\n", port );
    fflush(stdout);

    struct sockaddr_in client;
    int fromlen = sizeof( client );

    int n;
    pid_t pid;
    char buffer[ BUFFER_SIZE ];

    while ( 1 )
    {
        int rc = 0;
        int newsd = accept( sd, (struct sockaddr *)&client, (socklen_t *)&fromlen );
        printf("Rcvd incoming TCP connection from: %s\n", inet_ntoa( (struct in_addr)client.sin_addr ));
        fflush(stdout);
        /* Handle the accepted new client connection in a child process */
        pid = fork();

        if ( pid == -1 )
        {
            perror( "ERROR: FORK FAILED" );
            return EXIT_FAILURE;
        }

        if ( pid > 0 )
        {
        /* parent simply closes the new client socket
            because the child process will handle that connection */
            close( newsd );
        }
        else /* pid == 0 */
        {
            do
            {
                /* recv() will block until we receive data (n > 0)
                    or there's an error (n == -1)
                    or the client closed the socket (n == 0) */
                n = recv( newsd, buffer, BUFFER_SIZE, 0);

                if ( n == -1 )
                {
                    perror( "ERROR: RECV FAILED" );
                    return EXIT_FAILURE;
                }
                else if ( n == 0 )
                {
                    printf( "[child %d] Client disconnected\n",getpid() );
                    fflush(stdout);
                }
                else /* n > 0 */
                {
                    //buffer[n] = '\0';    /* assume this is text data */
                    int i = 0;
                    while(buffer[i] != ' ' && buffer[i] != '\n'){ i++;}
                    i++;
                    if(i < 5){
                        int fn; //Keeps track of index of filename array
                        char file_name[32];
                        for(fn = 0; i < n; fn++, i++){    //Checks that file name is alphanumeric
                            if(isalnum(buffer[i]) || buffer[i] == '.'){
                                file_name[fn] = buffer[i];
                            }
                            else{
                                file_name[fn] = '\0';
                                i++;
                                break;
                            }
                        }
                        char b_size[10];      //Stores the a 10 digit number
                        int not_d = 0;        //Determines whether all b_size are all digits
                        int k;
                        for(k = 0; i < n; k++, i++){      //Checks the file size is correct and valid
                            if(buffer[i] != '\n' && buffer[i] != ' '){
                                if(isdigit(buffer[i])){
                                    b_size[k] = buffer[i];
                                }
                                else{
                                    not_d = 1;
                                    break;
                                }
                            }
                            else{
                                b_size[k] = '\0';
                                i++;
                                break;
                            }
                        }
                        if(buffer[0] == 'P' && buffer[1] == 'U' && buffer[2] == 'T'){       //PUT Case
                            char instruct[i];
                            memcpy(instruct, &buffer[0], i-1);
                            instruct[i-1] = '\0'; 
                            printf( "[child %d] Received %s\n", getpid(), instruct );
                            fflush(stdout);
                            if(not_d == 0){
                                int file_size = atoi(b_size);
                                if(file_size < 1){    //Checks if file size is 0 or less
                                    n = send( newsd, "ERROR INVALID REQUEST\n", 22, 0);
                                    printf("[child %d] Sent ERROR INVALID REQUEST\n", getpid());
                                    fflush(stdout);
                                }
                                else{
                                    int fd = open( file_name, O_WRONLY | O_CREAT | O_EXCL, 0660 );
                                    if(fd == -1){
                                        n = send( newsd, "ERROR FILE EXISTS\n", 18, 0);
                                        printf("[child %d] Sent ERROR FILE EXISTS\n", getpid());
                                        fflush(stdout);
                                    }
                                    else{
                                        char temp[1024];
                                        int t;
                                        int wrote = 0;
                                        for(t = 0 ; i < n; t++, i++){
                                            temp[t] = buffer[i];
                                        }
                                        //temp[t] = '\0';
                                        rc = write(fd, temp, t);
                                        wrote += rc;
                                        while(wrote != file_size){        
                                            n = recv( newsd, buffer, BUFFER_SIZE, 0);
                                            for(t = 0; t < n; t++){
                                                temp[t] = buffer[t];
                                            }
                                            temp[t] = '\0';
                                            rc = write(fd, temp, t);
                                            wrote += rc;
                                        }
                                        close(fd);
                                        printf("[child %d] Stored file \"%s\" (%s bytes)\n", getpid(), file_name, b_size);
                                        fflush(stdout);
                                        /* send ACK message back to the client */
                                        n = send( newsd, "ACK\n", 4, 0 );
                                        printf("[child %d] Sent ACK\n", getpid());
                                        fflush(stdout);
                                
                                        if ( n != 4 )
                                        {
                                            perror( "ERROR: SEND FAILED" );
                                            return EXIT_FAILURE;
                                        }
                                    }
                                }
                            }
                            
                        }
                    }
                }
            }
        while ( n > 0 );
        close( newsd );
        exit( EXIT_SUCCESS );
        }
    }
    return EXIT_SUCCESS;
}