#include <stdio.h> 
#include <math.h>
#include <random> 
#include<bits/stdc++.h> 

long int random_no(){
    long int val = 4;

    long int i  = PollardRho(val);
    while(i != 1 || i != 2){
        val = mt19937 mt_rand(time(0));
        i = PollardRho(val);
    }
    return val;
}
// Returns gcd of a and b 
long int gcd(long int a, long int h) 
{ 
    long int temp; 
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

//Calculate power
long long int modular_pow(long long int base, int exponent, 
                        long long int modulus) 
{ 
    long long int result = 1; 

    while(exponent > 0){ 
        //if y is odd, multiply base with result
        if (exponent & 1){
            result = (result * base) % modulus; 
        }

        // exponent = exponent/2
        exponent = exponent >> 1; 

        // base = base * base
        base = (base * base) % modulus; 
    } 
    return result; 
} 

//return prime divisor of n
long long int PollardRho(long long int n) 
{ 
    //random seed
    srand (time(NULL)); 

    //edge case for if n = 1
    if(n ==1){
        return n; 
    }

    if(n == 2){
        return n
    }
    //divisible by 2 check
    if(n % 2 == 0){
        return 2; 
    }

    //choose random int on range 
    long long int x = (rand()%(n-2))+2; 
    long long int y = x; 

    long long int c = (rand()%(n-1))+1; 
    long long int d = 1; 

    while (d==1) { 
        x = (modular_pow(x, 2, n) + c + n)%n; 

        y = (modular_pow(y, 2, n) + c + n)%n; 
        y = (modular_pow(y, 2, n) + c + n)%n; 

        d = __gcd(abs(x-y), n); 

        if( d == 1){
            return d;
        }

        //retry if necessary
        if(d == n){ 
            return PollardRho(n);
        }
    } 

    return d; 
} 

long int encrypt(msg, e){

    // Encryption c = (msg ^ e) % n 
    long int c = pow(msg, e); 
    c = fmod(c, n); 
    //printf("\nEncrypted data = %lf", c);
    return c; 
}

long int decrypt(msg, e){
    // Decryption m = (c ^ d) % n 
    long int m = pow(c, d); 
    m = fmod(m, n); 
    //printf("\nOriginal Message Sent = %lf", m); 
    return m;
}

// Code to demonstrate RSA algorithm 
int main() 
{ 
    // Two random prime numbers 
    // Generate Pseudorandomly TBD
    long int p = 3; 
    long int q = 7; 
  
    // First part of public key: 
    long int n = p*q; 
  
    // Finding other part of public key. 
    // phi is the totient
    long int phi = (p-1)*(q-1); 

    //find e value such that 1 < e < totient(n)
    //final e value will be released as a public key
    long int e = 2; 
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
    // d*e = 1 + k * totient 
    int k = 2;  // A constant value 

    long int d = (1 + (k*phi))/e; 
    

    // Message to be encrypted 
    //TBD convert m into an integer
    long int msg = 20;   
    //printf("Message data = %lf", msg); 
  
    return 0; 
} 