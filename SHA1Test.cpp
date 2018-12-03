//SHA1Test
//#include "SHA1.h"
#include "HMAC.h"

using namespace std;

//I copied this function from the following link:
//https://stackoverflow.com/questions/3381614/c-convert-string-to-hexadecimal-and-vice-versa
string hex_to_string(const string& input)
{
    static const char* const lut = "0123456789abcdef";
    size_t len = input.length();
    if (len & 1) throw invalid_argument("odd length");

    string output;
    output.reserve(len / 2);
    for (size_t i = 0; i < len; i += 2)
    {
        char a = input[i];
        const char* p = lower_bound(lut, lut + 16, a);
        if (*p != a) throw invalid_argument("not a hex digit");

        char b = input[i + 1];
        const char* q = lower_bound(lut, lut + 16, b);
        if (*q != b) throw invalid_argument("not a hex digit");

        output.push_back(((p - lut) << 4) | (q - lut));
    }
    return output;
}

int main(int argc, char* argv[])
{
	SHA1 checksum;
	HMAC mac;
	checksum.update("");
	cout << "Empty string:	" << checksum.final() << endl;
	cout << "HMAC empty key: " << mac.HMAC_SHA1("", "") << endl;
	checksum.update("The quick brown fox jumps over the lazy dog");
	cout << "lazy dog:	" << checksum.final() << endl;
	HMAC mac2;
	cout << "HMAC lazy dog:	" << mac2.HMAC_SHA1("key", "The quick brown fox jumps over the lazy dog") << endl;
	checksum.update("The quick brown fox jumps over the lazy cog");
	cout << "lazy cog:	" << checksum.final() << endl;
	HMAC mac3;
	cout << "LARGE HMAC KEY:	" << mac3.HMAC_SHA1(hex_to_string("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f"),
													hex_to_string("53616d706c65206d65737361676520666f72206b65796c656e3d626c6f636b6c656e")) << endl;
}