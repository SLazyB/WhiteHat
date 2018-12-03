#include <stdio.h> 
#include <math.h>
#include <random>
#include <string>
#include <vector>
#include<bits/stdc++.h> 

using namespace std;

//convert message to integer
long long int convert(char msg){
    int temp = 0;
    string temp1 = "";
    string final = "";
    
    temp = int(msg);
    temp1 = to_string(temp);

    long long int cmsg = stoll(temp1);

    return cmsg;
}
//function to return random values
long long int lcm(long long int num1, long long int num2){
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
long long int gcd(long long int a, long long int h) 
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
long long int gcdExtended(long long int a, long long int b, long long int *x, long long int *y) 
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

long long int modInverse(long long int a, long long int m) 
{ 
    long long int x, y; 
    long long int g = gcdExtended(a, m, &x, &y);
    //if g!= 1 Inverse doesn't exist
     // m is added to handle negative x 
    long long int res = (x%m + m) % m; 
    return res;
}
/* Iterative Function to calculate (x^y)%p in O(log y) */
long long int power(long long int x, unsigned long long int y, long long int p) 
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
    long long int encrypt(long long int msg, long long int e, long long int n){
    //printf("msg = %lld e= %lld n= %lld \n", msg, e, n);
    // Encryption c = (msg ^ e) % n 
    long long int c = power(msg, e, n); 
    //printf("c = %lld \n", c);
    //c = fmod(c, n); 
    return c; 
}

long long int decrypt(long long int c, long long int n, long long int d){
    // Decryption m = (c ^ d) % n 
    //printf("c = %lld n= %lld d= %lld \n", c, n, d);
    long long int m = power(c, d, n);
    //printf("m = %lld \n", m);
    //m = fmod(m, n); 
    //printf("\nOriginal Message Sent = %lu", m); 
    return m;
}

// Code to demonstrate RSA algorithm 
int main() 
{ 
    // Two random prime numbers 
    // Generate Pseudorandomly TBD
    long long int p = 61; 
    long long int q = 53;  
  
    // First part of public key: 
    long long int n = p*q; 

    // Finding other part of public key. 
    // phi is the totient
    long long int phi = lcm((p-1), (q-1)); 
    printf("phi = %lld \n", phi);

    //find e value such that 1 < e < totient(n)
    //final e value will be released as a public key
    long long int e = 2; 
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
    
    printf("e = %lld \n", e);
    // Private key d will be the private key exponent
    // choosing d such that it satisfies 
    // d*e = 1 + k * totient 
    long long int k = 2;  // A constant value 
    long long int d = modInverse(e, phi); 
    printf("d = %lld \n", d);


    // Message to be encrypted 
    //TBD convert m long long into an long long integer
    //long long int msg = convert("a b");
    string usr_msg = "PERMUTATIONS of SCIENCE";
    cout << usr_msg << endl;
    
    vector <long long int> msg_arr;
    long long int e_msg = 0;

    for(int i = 0; i < usr_msg.length(); i++){
        char curr = usr_msg[i];
        e_msg = convert(curr);
        long long int val = encrypt(e_msg, e, n);
        printf("Encrypted data = %lld \n", val);
        msg_arr.push_back(val);
    }

    //decryption
    long long int d_msg = 0;
    string p_text = "";
    for( int i = 0; i < msg_arr.size(); i++){
        long long int pval = decrypt(msg_arr[i],n,d);
        //printf("Original Message Sent = %c \n", char(pval));
        p_text += char(pval);
    } 

    cout << p_text << endl;
    return 0; 
} 