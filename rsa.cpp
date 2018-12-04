#include <stdio.h> 
#include <math.h>
#include <random>
#include <string>
#include <vector>
#include "rsa.h"

using namespace std;

RSA::RSA(long long int fp, long long int fq):p(fp), q(fq) {;}//decrypt
RSA::RSA(long long int fe, long long int fn,bool mode): e(fe), n(fn) {;}//encrypt
//math suite start///////////////////////////////////////////////////////////
//convert message to integer
long long int RSA::convert(char msg){
    int temp = 0;
    string temp1 = "";
    string final = "";
    
    temp = int(msg);
    temp1 = to_string(temp);

    long long int cmsg = stoll(temp1);

    return cmsg;
}
//function to return random values
long long int RSA::lcm(long long int num1, long long int num2){
   long long int maxValue = max(num1, num2);
   while(1)
   {
      //condition of LCM
      if((maxValue % num1 == 0) && (maxValue % num2 == 0))
      {
        return maxValue;
        break;
      }
      ++maxValue;
   }
   return 0;
}
// Returns gcd of a and b 
long long int RSA::gcd(long long int a, long long int h) 
{ 
    long long int temp; 
    while(1) 
    { 
        temp = a % h; 
        if (temp == 0){
            return h;  
        } 

        a = h; 
        h = temp; 
    } 
} 
// C function for extended Euclidean Algorithm 
long long int RSA::gcdExtended(long long int a, long long int b, long long int *x, long long int *y) 
{ 
    // Base Case 
    if (a == 0) 
    { 
        *x = 0, *y = 1; 
        return b; 
    } 
  
    long long int x1, y1; // To store results of recursive call 
    long long int gcd = gcdExtended(b%a, a, &x1, &y1); 
  
    // Update x and y using results of recursive 
    // call 
    *x = y1 - (b/a) * x1; 
    *y = x1; 
  
    return gcd; 
} 
long long int RSA::modInverse(long long int a, long long int m) 
{ 
    long long int x, y; 
    long long int g = gcdExtended(a, m, &x, &y);
    //if g!= 1 Inverse doesn't exist
     // m is added to handle negative x 
    long long int res = (x%m + m) % m; 
    return res;
}
/* Iterative Function to calculate (x^y)%p in O(log y) */
long long int RSA::power(long long int x, unsigned long long int y, long long int p) 
{ 
    long long int res = 1;      // Initialize result 
  
    x = x % p;  // Update x if it is more than or  
                // equal to p 
  
    while (y > 0) 
    { 
        // If y is odd, multiply x with result 
        if (y & 1) 
            res = (res*x) % p; 
  
        // y must be even now 
        y = y>>1; // y = y/2 
        x = (x*x) % p;   
    } 
    return res; 
} 

void RSA::calculate(){
    // Two random prime numbers 
    // Generate Pseudorandomly TBD - TO REMOVE
  
    // First part of public key: 
    n = p*q; 

    // Finding other part of public key. 
    // phi is the totient
    phi = lcm((p-1), (q-1)); 

    //find e value such that 1 < e < totient(n)
    //final e value will be released as a public key
    e = 2; 
    while (e < phi) 
    { 
        // e must be co-prime to phi and 
        // smaller than phi. 
        if (gcd(e, phi) == 1){
            break;
        }  
        else{
            e++; 
        }
    }
    
    // Private key d will be the private key exponent
    // choosing d such that it satisfies 
    d = modInverse(e, phi); 
}
/////////////////////////////////////////////////////////////////////////
//basic encrypt and decrypt suite
long long int RSA::enc(long long int msg, long long int e, long long int n){
    // Encryption c = (msg ^ e) % n 
    long long int c = power(msg, e, n); 
    //c = fmod(c, n); 
    return c; 
}
long long int RSA::dec(int c, long long int n, long long int d){
    // Decryption m = (c ^ d) % n 
    long long int m = power(c, d, n);
    return m;
}
////////////////////////////////////////////////////////////////////////
string RSA::Encrypt(string usr_msg){
    string temp1 = to_string(n);
    string e_msg_fin = "";
    string temp = "";
    long long int e_msg = 0;


    for(int i = 0; i < usr_msg.length(); i++){
        char curr = usr_msg[i];
        e_msg = convert(curr);
        long long int val = enc(e_msg, e, n);
        temp = to_string(val);
        
        //will need to scale with n val
        while(temp.length() < temp1.length()){
            temp = "0" + temp;
        }
        e_msg_fin += temp;

    }
    return e_msg_fin;
}
string RSA::Decrypt(string usr_msg){
    string p_text = "";
    int n_l = to_string(n).length();
    int vals = usr_msg.length()/n_l;

    int temp_arr [vals];
    int t = 0;
    for(int i = 0; i < usr_msg.length(); i += n_l){
        temp_arr[t] = stoi(usr_msg.substr(i,n_l));
        t++;
    }
    
    for( int i = 0; i < vals; i++){
        long long int pval = dec(temp_arr[i],n,d);
        p_text += char(pval);
    } 
    return p_text;
}

//test code for main
/*
int main(){

    //encrypt test
    string message = "hello world";
    long long int p = 61; 
    long long int q = 53;  
    RSA r_1(p,q);
    r_1.calculate();
    
    RSA r_2(r_1.e,r_1.n,true);

    string m1 = r_2.encrypt_message(message);
    cout << m1 << endl;
    string m2 = r_1.decrypt_message(m1);
    cout << m2 << endl;

}
*/

