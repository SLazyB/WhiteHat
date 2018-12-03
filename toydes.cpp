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
#include "toydes.h"

using namespace std;
//Server side

S_DES::S_DES(string k): key(k) {sub_gen(k);}
string S_DES::Encrypt(string input){
    vector<bitset<8> > s_b;
    vector<string> conv;
    vector<string> end_crypt;
    for(int i = 0; i < input.size(); i++){
        s_b.push_back(bitset<8>(input.c_str()[i]));
    }
    for(int j = 0; j < s_b.size(); j++){
        conv.push_back(Permutation(s_b[j]));
    }
    
    //Main Encryption
    string tmp = "";
    string tmp1_l = "";
    string tmp1_r = "";
    string tmp2_l = "";
    string tmp2_r = "";
    string tmp3 = "";
    string fk1 = "";
    string fk2 = "";
    for(int a = 0; a < conv.size(); a++){   //Loops through vector and stores all encrypted text to end_crypt
        tmp = conv[a];
        
        //Splitting the original 8-bit set
        tmp1_l = tmp.substr(0,4);
        tmp1_r = tmp.substr(4,4);
        tmp2_l = tmp1_r;
        
        //First round
        fk1 = F(tmp1_r, key1);
        tmp2_r = (bitset<4>(tmp1_l) ^ bitset<4>(fk1)).to_string();
        
        //Second round
        fk2 = F(tmp2_r, key2);
        tmp3 = (bitset<4>(tmp2_l) ^ bitset<4>(fk2)).to_string();
        
        tmp3.append(tmp2_r);
        end_crypt.push_back(invPermutation(tmp3));
        
    }
    //Creates new encrypted string
    string output = "";
    for(int m = 0; m < end_crypt.size(); m++){
        output.push_back(char(bitset<8>(end_crypt[m]).to_ulong()));
    }
    
    return output;
}
string S_DES::Decrypt(string input){
    vector<bitset<8> > s_b;
    vector<string> conv;
    vector<string> end_crypt;
    for(int i = 0; i < input.size(); i++){
        s_b.push_back(bitset<8>(input.c_str()[i]));
    }
    for(int j = 0; j < s_b.size(); j++){
        conv.push_back(Permutation(s_b[j]));
    }
    
    //Main Decrypt
    string tmp = "";
    string tmp1_l = "";
    string tmp1_r = "";
    string tmp2_l = "";
    string tmp2_r = "";
    string tmp3 = "";
    string fk1 = "";
    string fk2 = "";
    
    for(int a = 0; a < conv.size(); a++){   //Loops through vector and stores all encrypted text to end_crypt
        tmp = conv[a];
        
        //Splitting the original 8-bit set
        tmp1_l = tmp.substr(0,4);
        tmp1_r = tmp.substr(4,4);
        tmp2_l = tmp1_r;
        
        //First Round
        fk1 = F(tmp1_r, key2);
        tmp2_r = (bitset<4>(tmp1_l) ^ bitset<4>(fk1)).to_string();
        
        //Second round
        fk2 = F(tmp2_r, key1);
        tmp3 = (bitset<4>(tmp2_l) ^ bitset<4>(fk2)).to_string();
        
        
        tmp3.append(tmp2_r);
        end_crypt.push_back(invPermutation(tmp3));
        
    }
    
    //Creating new decrypted string
    string output = "";
    for(int m = 0; m < end_crypt.size(); m++){
        output.push_back(char(bitset<8>(end_crypt[m]).to_ulong()));
    }
    
    return output;
}
    
string S_DES::Permutation(bitset<8> input){    //Initial permutation
    string tmp = input.to_string();
    string out;
    out.push_back(tmp[1]);
    out.push_back(tmp[5]);
    out.push_back(tmp[2]);
    out.push_back(tmp[0]);
    out.push_back(tmp[3]);
    out.push_back(tmp[7]);
    out.push_back(tmp[4]);
    out.push_back(tmp[6]);
    return out;
}
string S_DES::invPermutation(string input){    //Ending permutation
    string tmp = input;
    string out;
    out.push_back(tmp[3]);
    out.push_back(tmp[0]);
    out.push_back(tmp[2]);
    out.push_back(tmp[4]);
    out.push_back(tmp[6]);
    out.push_back(tmp[1]);
    out.push_back(tmp[7]);
    out.push_back(tmp[5]);
    return out;
}
string S_DES::P10(string key){     //10-bit Permutation
    string tmp = key;
    string out;
    out.push_back(tmp[2]);
    out.push_back(tmp[4]);
    out.push_back(tmp[1]);
    out.push_back(tmp[6]);
    out.push_back(tmp[3]);
    out.push_back(tmp[9]);
    out.push_back(tmp[0]);
    out.push_back(tmp[8]);
    out.push_back(tmp[7]);
    out.push_back(tmp[5]);
    return out;
    
}
string S_DES::P8(string kp){   //8-bit Permutation
    string tmp = kp;
    string out;
    out.push_back(tmp[5]);
    out.push_back(tmp[2]);
    out.push_back(tmp[6]);
    out.push_back(tmp[3]);
    out.push_back(tmp[7]);
    out.push_back(tmp[4]);
    out.push_back(tmp[9]);
    out.push_back(tmp[8]);
    return out;
    
}
string S_DES::P4(string kp){   //4-bit Permutation
    string out;
    out.push_back(kp[1]);
    out.push_back(kp[3]);
    out.push_back(kp[2]);
    out.push_back(kp[0]);
    return out;
}
string S_DES::lshift(string kp){   //Left rolling by 1-bit
    string kp_s = kp.substr(1,4);
    kp_s.push_back(kp[0]);
    return kp_s;
}
string S_DES::Expansion(string input){     //4-bit to 8-bit expansion
    string out;
    out.push_back(input[3]);
    out.push_back(input[0]);
    out.push_back(input[1]);
    out.push_back(input[2]);
    out.push_back(input[1]);
    out.push_back(input[2]);
    out.push_back(input[3]);
    out.push_back(input[0]);
    return out;
}
void S_DES::sub_gen(string key){   //Creates the two parts of the key
    string kp = P10(key);
    string k1_l = lshift(kp.substr(0,5));
    string k1_r = lshift(kp.substr(5,5));
    key1 = P8((k1_l + k1_r));
    
    string k2_l = lshift(k1_l);
    string k2_r = lshift(k1_r);
    key2 = P8((k2_l + k2_r));
}
string S_DES::F(string input, string kp){  //F function
    string s0[4][4] = { {"01","00","11","10"}, {"11","10","01","00"}, {"00","10","01","11"}, {"11","01","11","10"}};
    string s1[4][4] = { {"00","01","10","11"}, {"10","00","01","11"}, {"11","00","01","00"}, {"10","01","00","11"}};
    
    string i_e = Expansion(input);
    string x_o = (bitset<8>(i_e) ^ bitset<8>(kp)).to_string();
    string x_ol = x_o.substr(0,4);
    string x_or = x_o.substr(4,4);
    string sl_0;
    sl_0.push_back(x_ol[0]);
    sl_0.push_back(x_ol[3]);
    string sr_0;
    sr_0.push_back(x_ol[1]);
    sr_0.push_back(x_ol[2]);
    string sl_1;
    sl_1.push_back(x_or[0]);
    sl_1.push_back(x_or[3]);
    string sr_1;
    sr_1.push_back(x_or[1]);
    sr_1.push_back(x_or[2]);
    
    string s_o0 = s0[int(bitset<2>(sl_0).to_ulong())][int(bitset<2>(sr_0).to_ulong())];
    string s_o1 = s1[int(bitset<2>(sl_1).to_ulong())][int(bitset<2>(sr_1).to_ulong())];
    string out = P4(s_o0.append(s_o1));
    return out;
}