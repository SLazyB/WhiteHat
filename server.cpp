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
#include <vector>
#include <cstdlib>
#include <set>
#include <string>
#include <iostream>
#include <math.h>
#include "toydes.h"
#include <map>

#define BUFFER_SIZE 1024
using namespace std;


int diffie(int p, int a, int r){
    long long int calc = pow(r, a);
    int out = calc % p;
    return out;
}

int cmpfunc (const void * a, const void * b)
{
   const char **ia = (const char **) a;
   const char **ib = (const char **) b;
   return strcmp(*ia, *ib);
}

int getprime(){
    bool done = false;
    int test;
    int i = 0;
    int w = 0;
    while (!done) {
        test = rand();
        if(test == 2 || test == 3){
            done = true;
        }
        else if(test % 2 == 0 || test % 3 == 0){
            continue;
        }
        else{
            i = 5;
            w = 2;
            while (i * i <= test){
                if(test % i == 0){
                    break;
                }
                i += w;
                w = 6 - w;
            }
            if(i * i > test){
                done = true;
            }
        }
    }
    return test;
}

int main(int argc, char** argv){
    if(argc != 1){
        perror("ERROR: Incorrect number of arguments");
        return EXIT_FAILURE;
    }

    map<string, string> users;

    string enc_init[] = {"rsa","des","3des"};
    set<string> server_enc(enc_init, enc_init+3);
    printf("Started server\n");
    fflush(stdout);
      /* Create the listener socket as TCP socket */
    int sd = socket( AF_INET, SOCK_STREAM, 0 );
        /* here, the sd is a socket descriptor (part of the fd table) */

    if ( sd == -1 )
    {
        perror( "ERROR: SOCKET FAILED" );
        return EXIT_FAILURE;
    }


    /* socket structures */
    struct sockaddr_in server;
    server.sin_family = AF_INET;
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
    char buffer[ BUFFER_SIZE ];

    while ( 1 )
    {
        int newsd = accept( sd, (struct sockaddr *)&client, (socklen_t *)&fromlen );
        printf("Rcvd incoming TCP connection from: %s\n", inet_ntoa( (struct in_addr)client.sin_addr ));
        fflush(stdout);
    
        //Handshaking begins
        srand(time(NULL));
        set<string> client_enc;

        //Initializing Diffie-Hellman (TBD: Needs better randomization)     
        int p = 97;
        int g = 7;
        int s1 = rand() % 9 + 1;
        int m1 = long(pow(g, s1)) % p;  //Needs to be sent
        int s2;     //Symmetric keys

        /* recv() will block until we receive data (n > 0)
                or there's an error (n == -1)
                or the client closed the socket (n == 0) */
        n = recv(newsd, (char*)&buffer, BUFFER_SIZE, 0);
        string tmp = string(buffer);
        if(tmp != "start"){
            close(newsd);
            exit(EXIT_FAILURE);
        }
        memset(&buffer, 0, BUFFER_SIZE);
        n = send(newsd,"ACK\n",4,0);

        n = recv(newsd, (char*)&buffer, BUFFER_SIZE, 0);
        n = send(newsd,"ACK\n",4,0);
        tmp = string(buffer);
        memset(&buffer, 0, BUFFER_SIZE);
        
        if(tmp == "enc"){
            n = recv(newsd, (char*)&buffer, BUFFER_SIZE, 0);
            n = send(newsd,"Received first\n",15,0);
            tmp = string(buffer);
            memset(&buffer, 0, BUFFER_SIZE);

            //Receives all of clients encryption methods
            while(tmp.compare("done") != 0){
                client_enc.insert(tmp);
                n = recv(newsd, (char*)&buffer, BUFFER_SIZE, 0);
                n = send(newsd,"Receiving\n",10,0);
                tmp = string(buffer);
                memset(&buffer, 0, BUFFER_SIZE);
            }

            //Searches for matching encryption method
            bool done = false;
            for(set<string>::iterator ce = client_enc.begin(); ce != client_enc.end(); ++ce){
                for(set<string>::iterator se = server_enc.begin(); se != server_enc.end(); ++se){
                    if((*ce).compare(*se) == 0){
                        done = true;
                        tmp = *ce;
                        break;
                    }
                }
                if(done){
                    break;
                }
            }

            //Matching encryption found
            n = recv(newsd, (char*)&buffer, BUFFER_SIZE, 0);
            n = send(newsd, tmp.c_str(), tmp.length(), 0);

            
            tmp = to_string(m1);

            n = recv(newsd, (char*)&buffer, BUFFER_SIZE, 0);
            n = send(newsd, tmp.c_str(), tmp.length(),0);

            tmp = string(buffer);
            s2 = diffie(p, s1, atoi(tmp.c_str()));
            printf("%d\n",s2);
            fflush(stdout);
            memset(&buffer, 0, BUFFER_SIZE);

        }
        bitset<10> k(s2);
        S_DES sec(k.to_string());
        //Handshaking is complete

        //User authentication begins
        n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);
        string u_name = sec.Decrypt(string(buffer));
        string psd;
        memset(&buffer, 0, BUFFER_SIZE);
        map<string,string>::iterator fnd; 

        fnd = users.find(u_name);
        if(fnd == users.end()){
            tmp = sec.Encrypt("n");
            n = send(newsd, tmp.c_str(),tmp.length(),0);
            n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);
            tmp = sec.Decrypt(string(buffer));
            memset(&buffer, 0, BUFFER_SIZE);
            users.insert(pair<string,string>(u_name, tmp));
            psd = tmp;
            cout << u_name << endl;
            cout << tmp << endl;
            tmp = sec.Encrypt("received");
            n = send(newsd, tmp.c_str(),tmp.length(),0);
        }
        else{
            tmp = sec.Encrypt("o");
            n = send(newsd, tmp.c_str(),tmp.length(),0);
            n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);
            tmp = sec.Decrypt(string(buffer));
            memset(&buffer, 0, BUFFER_SIZE);
            if(users[u_name] != tmp){
                close(newsd);
                exit(EXIT_SUCCESS);
            }
            else{
                tmp = sec.Encrypt("accept");
                n = send(newsd, tmp.c_str(),tmp.length(),0);
            }
        }
        do
        {
            /* recv() will block until we receive data (n > 0)
                or there's an error (n == -1)
                or the client closed the socket (n == 0) */
            n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);

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
                tmp = sec.Decrypt(string(buffer));
                printf("%s\n", tmp.c_str());
                memset(&buffer, 0, BUFFER_SIZE);
                fflush(stdout);
            }
        }
        while ( n > 0 );
        close( newsd );
    }
    return EXIT_SUCCESS;
}