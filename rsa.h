#include <stdio.h> 
#include <math.h>
#include <random>
#include <string>
#include <vector>
#include<bits/stdc++.h>

using namespace std;

class RSA{

public:
	RSA(long long int e, long long int n, bool mode); //encryption
	RSA(long long int p, long long int q); //decryption
	long long int n;
	long long int e;
	long long int p;
	long long int q;
	string encrypt_message(string usr_msg);
	string decrypt_message(string usr_msg);
	long long int encrypt(long long int msg, long long int e, long long int n);
	long long int decrypt(int c, long long int n, long long int d);
	void calculate();

private:
	long long int d;
	long long int phi;
	bool mode;
	long long int convert(char msg);
	long long int lcm(long long int num1, long long int num2);
	long long int gcd(long long int a, long long int h);
	long long int gcdExtended(long long int a, long long int b, long long int *x, long long int *y);
	long long int modInverse(long long int a, long long int m);
	long long int power(long long int x, unsigned long long int y, long long int p);
};