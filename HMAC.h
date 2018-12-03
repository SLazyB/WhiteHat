//HMAC.h

#ifndef HMAC_h_
#define HMAC_h_

#include "SHA1.h"
#include <memory.h>
#include <string>

#define BLOCKSIZE 64
#define DIGEST_LENGTH 20
typedef unsigned char BYTE;
using namespace std;

class HMAC
{
	public:
		HMAC() {}
		string HMAC_SHA1(string key, string text);
	private:
		char ipad[BLOCKSIZE];
		char opad[BLOCKSIZE];

		char SHA1_Key[BLOCKSIZE];
};

#endif

string string_to_hex(const string &input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}


//This outputs incorrectly and I don't know why because the math is correct
//Probably has something to do with memory but I can't figure out how to fix it
string HMAC::HMAC_SHA1(string key, string text)
{
	//Initialize values for the key, ipad, and opad
	memset(ipad, 0, BLOCKSIZE);
	memset(opad, 0, BLOCKSIZE);
	memset(SHA1_Key, 0, BLOCKSIZE);
	memset(ipad, 0x36, /*BLOCKSIZE);*/sizeof(ipad));
	//memset(opad, 0x5c, sizeof(opad));

	//Need this to call hash functions
	SHA1 checksum1;

	//Covers step 1-3 from the FIPS document
	if (key.length() > BLOCKSIZE)
	{
		checksum1.update(key);
		checksum1.final();
	}
	for (int i = 0; i < key.length(); ++i)
	{
		SHA1_Key[i] = key[i];
	}
	//cout << SHA1_Key << endl;
	//cout << string_to_hex(key) << endl;
	//cout << string_to_hex(ipad) << endl;
	
	//memset(opad, 0x5c, sizeof(opad));
	//cout << string_to_hex(opad) << endl;
	//Step 4 according to FIPS
	for (int i = 0; i < BLOCKSIZE; ++i)
	{
		ipad[i] ^= SHA1_Key[i];
	}

	//cout << sizeof(ipad) << endl;
	//cout << string_to_hex(ipad) << endl;
	//cout << string_to_hex(""+ipad[56]) << endl;

	//Steps 5-6 according to FIPS
	SHA1 checksum2;
	checksum2.update(ipad + text);
	//checksum2.update(text);
	
	//Step 7 according to FIPS
	memset(opad, 0x5c, BLOCKSIZE);
	//cout << string_to_hex(opad) << endl;
	for (int i = 0; i < BLOCKSIZE; ++i)
	{
		opad[i] ^= SHA1_Key[i];
	}

	//cout << sizeof(opad) << endl;
	//cout << string_to_hex(opad) << endl;

	//Steps 8-9 according to FIPS
	SHA1 checksum3;
	checksum3.update(opad + checksum2.final());
	//checksum3.update(checksum2.final());

	return checksum3.final();

}