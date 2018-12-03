#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <time.h>
#include <math.h>
#include <map>

using namespace std;

class S_DES{

public:
    S_DES(string k);
    string Encrypt(string input);
    string Decrypt(string input);

private:
    string Permutation(bitset<8> input);
    string invPermutation(string input);
    string P10(string key);
    string P8(string kp);
    string P4(string kp);
    string lshift(string kp);
    string Expansion(string input);
    void sub_gen(string key);
    string F(string input, string kp);
    string key;
    string key1;
    string key2;
};