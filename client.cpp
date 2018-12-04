//Client side code

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
#include <math.h>
#include <iostream>
#include "toydes.h"
#include "blum_gold.h"
#include "rsa.h"
#include "HMAC.h"
#include "prime_generator.h"

#define PORT 8080 
   
#define BUFFER_SIZE 1024
using namespace std;


int diffie(int p, int a, int r){
    long long int calc = pow(r, a);
    int out = calc % p;
    return out;
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

int main(int argc, char const *argv[]) 
{ 

    //Keeps track of allowed encryption algorithms
    string enc_init[] = {"rsa","sdes","blum_gold"};
    set<string> client_enc(enc_init, enc_init+3);

    srand(time(NULL));
    prime_generator prime_gen;
    HMAC mac;
    string mc = "";
    string key = "";
    long long int e;
    long long int n1;

    struct sockaddr_in address; 
    int sock = 0, n; 
    struct sockaddr_in serv_addr; 
    char buffer[1024]; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n");
        fflush(stdout); 
        return -1; 
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
    // Convert IPv4 and IPv6 addresses from text to binary form 
    // if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    // { 
    //     printf("\nInvalid address/ Address not supported \n"); 
    //     return -1; 
    // } 

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

    //Initializing contact (Message can be anything)
    send(sock , "start" , 6 , 0 ); 
    n = recv( sock , buffer, BUFFER_SIZE,0); 
    memset(&buffer, 0, BUFFER_SIZE);

    //Inform server that list of suite is about to begin
    n = send(sock,"enc",3,0);
    n = recv( sock , buffer, BUFFER_SIZE,0); 
    memset(&buffer, 0, BUFFER_SIZE);

    //Looping through set to send all encryption methods
    for(set<string>::iterator e = client_enc.begin(); e != client_enc.end(); ++e){
        n = send(sock, (*e).c_str(), (*e).length(),0);
        n = recv( sock , buffer, BUFFER_SIZE,0); 
        memset(&buffer, 0, BUFFER_SIZE);
    }

    //Ending encryption
    n = send(sock, "done",4,0);
    n = recv(sock, buffer, BUFFER_SIZE,0);
    memset(&buffer, 0, BUFFER_SIZE);

    //Receives encryption algorithm
    n = send(sock, "got",3,0);
    n = recv( sock , buffer, BUFFER_SIZE,0); 
    string data = string(buffer);
    memset(&buffer, 0, BUFFER_SIZE);

    
    if(data == "sdes"){
        //Beginning Diffie Hellman
        int p = 97;
        int g = 7;
        int s1 = rand() % 9 + 1;
        cout << "private " << s1 << endl;

        int m1 = long(pow(g, s1)) % p;
        data = to_string(m1);

        n = send(sock, data.c_str() , data.length(),0);
        n = recv( sock , buffer, BUFFER_SIZE,0); 
        data = string(buffer);
        int s2 = diffie(p,s1,atoi(data.c_str()));
        fflush(stdout);
        memset(&buffer, 0, BUFFER_SIZE);

        bitset<10> k(s2);
        key = k.to_string();
        S_DES sec(key);

        //User authentication begins
        string inp = "";
        printf("Please enter your username: \n");
        fflush(stdout);
        cin >> inp;
        
        data = sec.Encrypt(inp);
        data += mac.HMAC_SHA1(key, inp);
        n = send(sock, data.c_str() , data.length(),0);
        n = recv(sock, buffer, BUFFER_SIZE,0);
        data = string(buffer);
        memset(&buffer, 0, BUFFER_SIZE);

        memset(&buffer, 0, BUFFER_SIZE);
        if(data == "n"){
            printf("Please give a new password: \n");
            fflush(stdout);
            cin >> inp;
            data = sec.Encrypt(inp);
            data += mac.HMAC_SHA1(key, inp);

            n = send(sock, data.c_str() , data.length(),0);
            n = recv(sock, buffer, BUFFER_SIZE,0);
            data = string(buffer);
            memset(&buffer, 0, BUFFER_SIZE);
        }
        else{
            printf("Please enter your password: \n");
            fflush(stdout);
            cin >> inp;
            data = sec.Encrypt(inp);
            data += mac.HMAC_SHA1(key, inp);

            n = send(sock, data.c_str() , data.length(),0);
            n = recv(sock, buffer, BUFFER_SIZE,0);
            data = string(buffer);
            memset(&buffer, 0, BUFFER_SIZE);
            
            if(data != "accept"){
                return 0;
            }
        }
        while(inp != "q"){
            cin >> inp;
            data = sec.Encrypt(inp);
            data += mac.HMAC_SHA1(key, inp);
            n = send(sock, data.c_str() , data.length(),0);
        }
    }
    else if( data == "rsa"){
        n = send(sock, "go\n", 3,0);
        n = recv( sock , buffer, BUFFER_SIZE,0); 
        data = string(buffer);
        e = atoi(data.c_str());
        memset(&buffer, 0, BUFFER_SIZE);

        n = send(sock, "go\n", 3,0);
        n = recv( sock , buffer, BUFFER_SIZE,0); 
        data = string(buffer);
        n1 = atoi(data.c_str());
        memset(&buffer, 0, BUFFER_SIZE);

        RSA sec(e,n1,false);

        //User authentication begins
        string inp = "";
        printf("Please enter your username: \n");
        fflush(stdout);
        cin >> inp;
        
        data = sec.Encrypt(inp);
        data += mac.HMAC_SHA1(key, inp);
        n = send(sock, data.c_str() , data.length(),0);
        n = recv(sock, buffer, BUFFER_SIZE,0);
        data = string(buffer);
        memset(&buffer, 0, BUFFER_SIZE);

        if(data == "n"){
            printf("Please give a new password: \n");
            fflush(stdout);
            cin >> inp;
            data = sec.Encrypt(inp);
            data += mac.HMAC_SHA1(key, inp);

            n = send(sock, data.c_str() , data.length(),0);
            n = recv(sock, buffer, BUFFER_SIZE,0);
            data = string(buffer);
            memset(&buffer, 0, BUFFER_SIZE);
        
        }
        else{
            printf("Please enter your password: \n");
            fflush(stdout);
            cin >> inp;
            data = sec.Encrypt(inp);
            data += mac.HMAC_SHA1(key, inp);

            n = send(sock, data.c_str() , data.length(),0);
            n = recv(sock, buffer, BUFFER_SIZE,0);
            data = string(buffer);
            memset(&buffer, 0, BUFFER_SIZE);
            
            if(data != "accept"){
                return 0;
            }
        }
        while(inp != "q"){
            cin >> inp;
            data = sec.Encrypt(inp);
            data += mac.HMAC_SHA1(key, inp);
            n = send(sock, data.c_str() , data.length(),0);
        }
        
    }
    else if(data == "blum_gold"){
        n = send(sock, "go\n", 3,0);
        n = recv( sock , buffer, BUFFER_SIZE,0); 
        data = string(buffer);
        n1 = atoi(data.c_str());
        memset(&buffer, 0, BUFFER_SIZE);
        long int seed = 159201;
        Blum_Gold sec(seed, n1, false);
        pair<string, long long int> enc1 = sec.Encrypt1("hi");

        data = enc1.first;
        n = send(sock, data.c_str(), data.length(),0);
        n = recv(sock, (char*)&buffer, BUFFER_SIZE, 0);
        memset(&buffer, 0, BUFFER_SIZE);

        data = to_string(enc1.second);
        n = send(sock, data.c_str(), data.length(),0);
        n = recv(sock, (char*)&buffer, BUFFER_SIZE, 0);
        memset(&buffer, 0, BUFFER_SIZE);

        //User authentication begins
        string inp = "";
        printf("Please enter your username: \n");
        fflush(stdout);
        cin >> inp;
        
        data = sec.Encrypt(inp);
        data += mac.HMAC_SHA1(key, inp);
        n = send(sock, data.c_str() , data.length(),0);
        n = recv(sock, buffer, BUFFER_SIZE,0);
        data = string(buffer);
        memset(&buffer, 0, BUFFER_SIZE);

        if(data == "n"){
            printf("Please give a new password: \n");
            fflush(stdout);
            cin >> inp;
            data = sec.Encrypt(inp);
            data += mac.HMAC_SHA1(key, inp);

            n = send(sock, data.c_str() , data.length(),0);
            n = recv(sock, buffer, BUFFER_SIZE,0);
            data = string(buffer);
            memset(&buffer, 0, BUFFER_SIZE);
        }
        else{
            printf("Please enter your password: \n");
            fflush(stdout);
            cin >> inp;
            data = sec.Encrypt(inp);
            data += mac.HMAC_SHA1(key, inp);

            n = send(sock, data.c_str() , data.length(),0);
            n = recv(sock, buffer, BUFFER_SIZE,0);
            data = string(buffer);
            memset(&buffer, 0, BUFFER_SIZE);
            if(data != "accept"){
                return 0;
            }
        }
        while(inp != "q"){
            cin >> inp;
            data = sec.Encrypt(inp);
            data += mac.HMAC_SHA1(key, inp);
            n = send(sock, data.c_str() , data.length(),0);
        }
        
    }
    
    return 0; 
} 