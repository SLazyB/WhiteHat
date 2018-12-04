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
#include "HMAC.h"
#include "blum_gold.h"
#include "rsa.h"
#include "prime_generator.h"
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

int main(int argc, char** argv){
    if(argc != 1){
        perror("ERROR: Incorrect number of arguments");
        return EXIT_FAILURE;
    }
    prime_generator prime_gen;

    map<string, string> users;
    HMAC mac;
    string mc;
    string key = "";
    string enc_init[] = {"rsa","sdes","blum_gold"};
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
            memset(&buffer, 0, BUFFER_SIZE);
            n = send(newsd, tmp.c_str(), tmp.length(), 0);
        }
        if(tmp == "sdes"){
            //Initializing Diffie-Hellman (TBD: Needs better randomization)     
            int p = 97;
            int g = 7;
            int s1 = rand() % 9 + 1;
            int m1 = long(pow(g, s1)) % p;  //Needs to be sent
            int s2;     //Symmetric keys

            tmp = to_string(m1);
            n = recv(newsd, (char*)&buffer, BUFFER_SIZE, 0);
            n = send(newsd, tmp.c_str(), tmp.length(),0);

            tmp = string(buffer);
            s2 = diffie(p, s1, atoi(tmp.c_str()));
            memset(&buffer, 0, BUFFER_SIZE);

            bitset<10> k(s2);
            key = k.to_string();
            S_DES sec(key);

            //User authentication begins
            n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);
            tmp = string(buffer);
            mc = tmp.substr(tmp.length() - 40);
            tmp = tmp.substr(0,tmp.length() -40);
            string u_name = sec.Decrypt(tmp);
            string psd;
            memset(&buffer, 0, BUFFER_SIZE);
            map<string,string>::iterator fnd; 

            fnd = users.find(u_name);
            if(fnd == users.end()){
                tmp = "n";
                n = send(newsd, tmp.c_str(),tmp.length(),0);
                n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);
                tmp = string(buffer);
                mc = tmp.substr(tmp.length() - 40);
                tmp = tmp.substr(0,tmp.length() -40);
                tmp = sec.Decrypt(tmp);
                memset(&buffer, 0, BUFFER_SIZE);
                if(mc != mac.HMAC_SHA1(key,tmp)){
                    break;
                }
                users.insert(pair<string,string>(u_name, tmp));
                psd = tmp;
                tmp = "received";
                n = send(newsd, tmp.c_str(),tmp.length(),0);
            }
            else{
                tmp = "o";
                n = send(newsd, tmp.c_str(),tmp.length(),0);
                n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);
                tmp = string(buffer);
                mc = tmp.substr(tmp.length() - 40);
                tmp = tmp.substr(0,tmp.length() -40);
                tmp = sec.Decrypt(tmp);
                memset(&buffer, 0, BUFFER_SIZE);

                if(mc != mac.HMAC_SHA1(key,tmp)){
                    break;
                }
                if(users[u_name] != tmp){
                    close(newsd);
                    exit(EXIT_SUCCESS);
                }
                else{
                    tmp = "accept";
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
                    tmp = string(buffer);
                    mc = tmp.substr(tmp.length() - 40);
                    tmp = tmp.substr(0,tmp.length() -40);
                    tmp = sec.Decrypt(tmp);
                    memset(&buffer, 0, BUFFER_SIZE);

                    if(mc != mac.HMAC_SHA1(key,tmp)){
                        break;
                    }
                    printf("%s\n", tmp.c_str());
                    memset(&buffer, 0, BUFFER_SIZE);
                    fflush(stdout);
                }
            }
            while ( n > 0 );
            close( newsd );
        }
        else if (tmp == "rsa"){
            prime_gen.generate_prime(10000, 100000);
            long long int p = prime_gen.get_prime();
            long long int q = prime_gen.get_prime();
            while(p == q){
                q = prime_gen.get_prime();
            }
            
            RSA sec(p,q);
            sec.calculate();
            n = recv(newsd, (char*)&buffer, BUFFER_SIZE, 0);
            memset(&buffer, 0, BUFFER_SIZE);

            tmp = to_string(sec.e);
            n = send(newsd, tmp.c_str(), tmp.length(),0);
            n = recv(newsd, (char*)&buffer, BUFFER_SIZE, 0);
            memset(&buffer, 0, BUFFER_SIZE);

            tmp = to_string(sec.n);
            n = send(newsd, tmp.c_str(), tmp.length(),0);

            //User authentication begins
            n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);
            tmp = string(buffer);
            mc = tmp.substr(tmp.length() - 40);
            tmp = tmp.substr(0,tmp.length() -40);
            string u_name = sec.Decrypt(tmp);
            string psd;
            memset(&buffer, 0, BUFFER_SIZE);
            map<string,string>::iterator fnd; 

            fnd = users.find(u_name);
            if(fnd == users.end()){
                tmp = "n";
                n = send(newsd, tmp.c_str(),tmp.length(),0);
                n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);
                tmp = string(buffer);
                mc = tmp.substr(tmp.length() - 40);
                tmp = tmp.substr(0,tmp.length() -40);
                tmp = sec.Decrypt(tmp);
                memset(&buffer, 0, BUFFER_SIZE);
                if(mc != mac.HMAC_SHA1(key,tmp)){
                    cout << "here " << endl;
                    break;
                }
                users.insert(pair<string,string>(u_name, tmp));
                psd = tmp;
                tmp = "received";
                n = send(newsd, tmp.c_str(),tmp.length(),0);
            }
            else{
                tmp = "o";
                n = send(newsd, tmp.c_str(),tmp.length(),0);
                n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);
                tmp = string(buffer);
                mc = tmp.substr(tmp.length() - 40);
                tmp = tmp.substr(0,tmp.length() -40);
                tmp = sec.Decrypt(tmp);
                memset(&buffer, 0, BUFFER_SIZE);

                if(mc != mac.HMAC_SHA1(key,tmp)){
                    break;
                }
                if(users[u_name] != tmp){
                    close(newsd);
                    exit(EXIT_SUCCESS);
                }
                else{
                    tmp = "accept";
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
                    tmp = string(buffer);
                    mc = tmp.substr(tmp.length() - 40);
                    tmp = tmp.substr(0,tmp.length() -40);
                    tmp = sec.Decrypt(tmp);
                    memset(&buffer, 0, BUFFER_SIZE);

                    if(mc != mac.HMAC_SHA1(key,tmp)){
                        break;
                    }
                    printf("%s\n", tmp.c_str());
                    memset(&buffer, 0, BUFFER_SIZE);
                    fflush(stdout);
                }
            }
            while ( n > 0 );
            close( newsd );
        }
        else if(tmp == "blum_gold"){
            prime_gen.generate_prime(1000,1000000);
            int p1 = prime_gen.get_prime();
            while((p1 % 4) != 3){
                p1 = prime_gen.get_prime();
            }

            int q1 = prime_gen.get_prime();
            while((q1 % 4 != 3) || q1 == p1){
                q1 = prime_gen.get_prime();
            }
            long long int p = p1;
            long long int q = q1;

            Blum_Gold sec(p,q);

            n = recv(newsd, (char*)&buffer, BUFFER_SIZE, 0);
            memset(&buffer, 0, BUFFER_SIZE);

            tmp = to_string(sec.key);
            n = send(newsd, tmp.c_str(), tmp.length(),0);
            n = recv(newsd, (char*)&buffer, BUFFER_SIZE, 0);
            string test = string(buffer);
            memset(&buffer, 0, BUFFER_SIZE);
            
            n = send(newsd, "yo\n", 3, 0);
            n = recv(newsd, (char*)&buffer, BUFFER_SIZE, 0);
            long long int prev = atoi(string(buffer).c_str());
            memset(&buffer, 0, BUFFER_SIZE);
            tmp = sec.Decrypt1(test,prev);
            if(tmp != "hi"){
                close(newsd);
                break;
            }
            n = send(newsd, "yo\n", 3, 0);

             //User authentication begins
            n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);
            tmp = string(buffer);
            mc = tmp.substr(tmp.length() - 40);
            tmp = tmp.substr(0,tmp.length() -40);
            string u_name = sec.Decrypt(tmp);
            string psd;
            memset(&buffer, 0, BUFFER_SIZE);
            map<string,string>::iterator fnd; 

            fnd = users.find(u_name);
            if(fnd == users.end()){
                tmp = "n";
                n = send(newsd, tmp.c_str(),tmp.length(),0);
                n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);
                tmp = string(buffer);
                mc = tmp.substr(tmp.length() - 40);
                tmp = tmp.substr(0,tmp.length() -40);
                tmp = sec.Decrypt(tmp);
                memset(&buffer, 0, BUFFER_SIZE);
                if(mc != mac.HMAC_SHA1(key,tmp)){
                    cout << "here " << endl;
                    break;
                }
                users.insert(pair<string,string>(u_name, tmp));
                psd = tmp;
                tmp = "received";
                n = send(newsd, tmp.c_str(),tmp.length(),0);
            }
            else{
                tmp = "o";
                n = send(newsd, tmp.c_str(),tmp.length(),0);
                n = recv( newsd, (char*)&buffer, BUFFER_SIZE, 0);
                tmp = string(buffer);
                mc = tmp.substr(tmp.length() - 40);
                tmp = tmp.substr(0,tmp.length() -40);
                tmp = sec.Decrypt(tmp);
                memset(&buffer, 0, BUFFER_SIZE);

                if(mc != mac.HMAC_SHA1(key,tmp)){
                    break;
                }
                if(users[u_name] != tmp){
                    close(newsd);
                    exit(EXIT_SUCCESS);
                }
                else{
                    tmp = "accept";
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
                    tmp = string(buffer);
                    mc = tmp.substr(tmp.length() - 40);
                    tmp = tmp.substr(0,tmp.length() -40);
                    tmp = sec.Decrypt(tmp);
                    memset(&buffer, 0, BUFFER_SIZE);

                    if(mc != mac.HMAC_SHA1(key,tmp)){
                        break;
                    }
                    printf("%s\n", tmp.c_str());
                    memset(&buffer, 0, BUFFER_SIZE);
                    fflush(stdout);
                }
            }
            while ( n > 0 );
            close( newsd );
        }
        
    }
    return EXIT_SUCCESS;
}