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

using namespace std;

class Blum_Gold{
public:
    Blum_Gold(long long int fp, long long int fq);
    Blum_Gold(long long int fseed, long long int fkey, bool use);
    long long int modexp(long long int base, long long int exponent, long long int mod);
    pair<string, long long int> Encrypt1(string m);
    string Encrypt(string m);
    string Decrypt1(string c, long int l_x);
    string Decrypt(string c);

    long long int key;
private:
    long long int p;
    long long int q;
    long long int seed;
};