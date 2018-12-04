#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <bitset>
#include <math.h>
#include <unistd.h>
#include "blum_gold.h"

using namespace std;

Blum_Gold::Blum_Gold(long long int fp, long long int fq): p(fp), q(fq) {key = p * q;}
Blum_Gold::Blum_Gold(long long fseed, long long fkey, bool use): seed(fseed), key(fkey) {;}

long long int Blum_Gold::modexp(long long int base, long long int exponent, long long int mod)
{
    long long int x = 1;
    long long int y = base;
    while (exponent > 0)
    {
        if (exponent % 2 == 1)
        x = (x * y) % mod;
        y = (y * y) % mod;
        exponent = exponent / 2;
    }
    return x % mod;
}

//Bob encrypting a message for Alice
pair<string, long long int> Blum_Gold::Encrypt1(string m1){
    string m = "";
    for(int i = 0; i < m1.length(); i++){
        m += bitset<8>(m1.c_str()[i]).to_string();
    }
    //Calculate text size for each chunk of message
    int k = floor(log2(key));
    int h = floor(log2(k));
    
    //Number of chunks of text
    int num_text = ceil(m.length()/h);
    
    //Stores the x values
    long long int prev = seed;
    
    //Output string
    string out = "";
    
    string temp = "";

    for(int i = 0; i < num_text; i++){
        temp = m.substr(i*h, h);
        
        //Calculates x
        prev = pow(prev,2);
        prev = prev % key;
        
        //Obtains the least significant 4 bits
        bitset<4> tmp_bit(prev);
        
        //XOR
        tmp_bit ^= bitset<4>(temp);
        
        out += tmp_bit.to_string();
    }
    
    prev = pow(prev,2);
    prev = prev % key;
    
    return pair<string, long long int>(out,prev);
}

string Blum_Gold::Encrypt(string m1){
    string m = "";
    for(int i = 0; i < m1.length(); i++){
        m += bitset<8>(m1.c_str()[i]).to_string();
    }
    //Calculate text size for each chunk of message
    int k = floor(log2(key));
    int h = floor(log2(k));
    
    //Number of chunks of text
    int num_text = ceil(m.length()/h);
    
    //Stores the x values
    long long int prev = seed;
    
    //Output string
    string out = "";
    
    string temp = "";

    for(int i = 0; i < num_text; i++){
        temp = m.substr(i*h, h);
        
        //Calculates x
        prev = pow(prev,2);
        prev = prev % key;
        
        //Obtains the least significant 4 bits
        bitset<4> tmp_bit(prev);
        
        //XOR
        tmp_bit ^= bitset<4>(temp);
        
        out += tmp_bit.to_string();
    }
    
    return out;
}

//Alice decrypting a message from Bob
string Blum_Gold::Decrypt1(string c, long int l_x){

    int a = -57;
    int b = 52;
    //Calculate text size for each chunk of message
    int k = floor(log2(key));
    int h = floor(log2(k));
    
    //Number of chunks of text
    int num_text = ceil(c.length()/h);

    //rp, rq calculated from p and q
    int rp = modexp(((p+1)/4.0), num_text+1, p-1);
    int rq = modexp(((q+1)/4.0), num_text+1, q-1);
    
    //Calculate initial seed
    int u = modexp(l_x,rp,p);
    int v = modexp(l_x,rq,q);
    seed = (v*a*p + u*b*q) % key;
    long long int prev = seed;
    
    //Output string
    string out = "";
    
    string temp = "";
    for(int i = 0; i < num_text; i++){
        temp = c.substr(i*h, h);
        
        //Calculates x
        prev = pow(prev,2);
        prev = prev % key;
        
        //Obtains the least significant 4 bits
        bitset<4> tmp_bit(prev);
        
        //XOR
        tmp_bit ^= bitset<4>(temp);
        out += tmp_bit.to_string();
    }
    string out1 = "";
    for(int i = 0; i < out.length()/8; ++i ){
        out1 += char(bitset<8>(out.substr(i*8,8)).to_ulong());
    }
    return out1;
}

string Blum_Gold::Decrypt(string c){

    //Calculate text size for each chunk of message
    int k = floor(log2(key));
    int h = floor(log2(k));
    
    //Number of chunks of text
    int num_text = ceil(c.length()/h);

    long long int prev = seed;
    
    //Output string
    string out = "";
    
    string temp = "";
    for(int i = 0; i < num_text; i++){
        temp = c.substr(i*h, h);
        
        //Calculates x
        prev = pow(prev,2);
        prev = prev % key;
        
        //Obtains the least significant 4 bits
        bitset<4> tmp_bit(prev);
        
        //XOR
        tmp_bit ^= bitset<4>(temp);
        out += tmp_bit.to_string();
    }
    string out1 = "";
    for(int i = 0; i < out.length()/8; ++i ){
        out1 += char(bitset<8>(out.substr(i*8,8)).to_ulong());
    }
    return out1;
}
// int main(int argc, char *argv[]){
//     //Alice only values
//     long long int p = 499;
//     long long int q = 547;
//     Blum_Gold a = Blum_Gold(p,q);
    
//     //Bob only value
//     long int seed = 159201;
//     Blum_Gold b = Blum_Gold(seed,a.key,false);
    
//     //Plain text
//     string m = "helllow";
    
//     pair<string, long long int> k = b.encrypt(m);
//     cout << "Cipher Text: " << k.first << endl;
//     string plain = a.decrypt(k.first, k.second);
//     cout << "Plain Text: " << plain << endl;
    
// }