//This file will contain the SHA-1 implementation algorithm
//Basing implementation from this source: https://github.com/vog/sha1/blob/master/sha1.cpp
#ifndef SHA1_h_
#define SHA1_h_

#include <string>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

static const size_t BLOCK_INTS = 16; //Num 32 ints per SHA1 block
static const size_t BLOCK_BYTES = 64;

class SHA1
{
	public:
		SHA1();
		string final();
		string from_file(const string &filename);
		void update(const string &input);
		void update(istream &inputstream);
//		static void begin(uint32_t digest[], string &buffer, uint64_t &transforms);
		uint32_t digest[5];
		string buffer;
		uint64_t transforms;
};

#endif


//Initialize values required for SHA1
static void begin(uint32_t digest[], string &buffer, uint64_t &transforms)
{
	//Initialization values
	digest[0] = 0x67452301;
    digest[1] = 0xefcdab89;
    digest[2] = 0x98badcfe;
    digest[3] = 0x10325476;
	digest[4] = 0xc3d2e1f0;

	//Initialize/reset counter vars
	buffer = "";
	transforms = 0;
}

//Helper functions for bitwise and block operations
static uint32_t rotateLeft (const uint32_t val, const size_t bits)
{
	return (val << bits) | (val >> (32 - bits));
}

static uint32_t block (const uint32_t arr[BLOCK_INTS], const size_t i)
{
	return rotateLeft(arr[(i+13)&15] ^ arr[(i+8)&15] ^ arr[(i+2)&15] ^ arr[i], 1);
}



//R0, R1, R2, R3, R4 operations
static void R0(const uint32_t arr[BLOCK_INTS], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
	z+= ((w&(x^y))^y) + arr[i] + 0x5a827999 + rotateLeft(v, 5);
	w = rotateLeft(w, 30);
}

static void R1(uint32_t arr[BLOCK_INTS], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
	arr[i] = block(arr, i);
	z+= ((w&(x^y))^y) + arr[i] + 0x5a827999 + rotateLeft(v, 5);
	w = rotateLeft(w, 30);
}

static void R2(uint32_t arr[BLOCK_INTS], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
	arr[i] = block(arr, i);
	z+= (w^x^y) + arr[i] + 0x6ed9eba1 + rotateLeft(v, 5);
	w = rotateLeft(w, 30);
}

static void R3(uint32_t arr[BLOCK_INTS], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
	arr[i] = block(arr, i);
	z+= (((w|x)&y)|(w&x)) + arr[i] + 0x8f1bbcdc + rotateLeft(v, 5);
	w = rotateLeft(w, 30);
}

static void R4(uint32_t arr[BLOCK_INTS], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
	arr[i] = block(arr, i);
	z+= (w^x^y) + arr[i] + 0xca62c1d6 + rotateLeft(v, 5);
	w = rotateLeft(w, 30);
}


//Transform function
//Hashes a single 512 bit block
static void transform(uint32_t digest_vals[], uint32_t block[BLOCK_INTS], uint64_t &transforms)
{
		//I was hoping this would work but it doesn't and that makes me sad
	// uint32_t temp_vals[5] = {digest_vals[0], digest_vals[1], digest_vals[2], digest_vals[3], digest_vals[4]};
	// //Loop through the operations
	// for (int i = 0; i <= 15; ++i)
	// {
	// 	R0(block, temp_vals[i%5], temp_vals[(i+1)%5], temp_vals[(i+2)%5], temp_vals[(i+3)%5], temp_vals[(i+4)%5], i);
	// }
	// for (int i = 0; i <= 3; ++i)
	// {
	// 	R1(block, temp_vals[(i+4)%5], temp_vals[i%5], temp_vals[(i+1)%5], temp_vals[(i+2)%5], temp_vals[(i+3)%5], i);
	// }
	// for (int i = 4; i <= 15; ++i)
	// {
	// 	R2(block, temp_vals[i%5], temp_vals[(i+1)%5], temp_vals[(i+2)%5], temp_vals[(i+3)%5], temp_vals[(i+4)%5], i);
	// }
	// for (int i = 0; i <= 7; ++i)
	// {
	// 	R2(block, temp_vals[(i+3)%5], temp_vals[(i+4)%5], temp_vals[i%5], temp_vals[(i+1)%5], temp_vals[(i+2)%5], i);
	// }
	// for (int i = 8; i <= 15; ++i)
	// {
	// 	R3(block, temp_vals[i%5], temp_vals[(i+1)%5], temp_vals[(i+2)%5], temp_vals[(i+3)%5], temp_vals[(i+4)%5], i);
	// }
	// for (int i = 0; i <= 11; ++i)
	// {
	// 	R3(block, temp_vals[(i+2)%5], temp_vals[(i+3)%5], temp_vals[(i+4)%5], temp_vals[i%5], temp_vals[(i+1)%5], i);
	// }
	// for (int i = 12; i <= 15; ++i)
	// {
	// 	R4(block, temp_vals[i%5], temp_vals[(i+1)%5], temp_vals[(i+2)%5], temp_vals[(i+3)%5], temp_vals[(i+4)%5], i);
	// }
	// for (int i = 0; i <= 15; ++i)
	// {
	// 	R4(block, temp_vals[(i+1)%5], temp_vals[(i+2)%5], temp_vals[(i+3)%5], temp_vals[(i+4)%5], temp_vals[i%5], i);
	// }

	// for (int i = 0; i <= 4; ++i)
	// {
	// 	digest_vals[i] += temp_vals[i];
	// }

	uint32_t a = digest_vals[0];
	uint32_t b = digest_vals[1];
	uint32_t c = digest_vals[2];
	uint32_t d = digest_vals[3];
	uint32_t e = digest_vals[4];

	R0(block,a,b,c,d,e, 0); R0(block,e,a,b,c,d, 1); R0(block,d,e,a,b,c, 2); R0(block,c,d,e,a,b, 3);
	R0(block,b,c,d,e,a, 4); R0(block,a,b,c,d,e, 5); R0(block,e,a,b,c,d, 6); R0(block,d,e,a,b,c, 7);
	R0(block,c,d,e,a,b, 8); R0(block,b,c,d,e,a, 9); R0(block,a,b,c,d,e,10); R0(block,e,a,b,c,d,11);
	R0(block,d,e,a,b,c,12); R0(block,c,d,e,a,b,13); R0(block,b,c,d,e,a,14); R0(block,a,b,c,d,e,15);
	R1(block,e,a,b,c,d, 0); R1(block,d,e,a,b,c, 1); R1(block,c,d,e,a,b, 2); R1(block,b,c,d,e,a, 3);
	R2(block,a,b,c,d,e, 4); R2(block,e,a,b,c,d, 5); R2(block,d,e,a,b,c, 6); R2(block,c,d,e,a,b, 7);
	R2(block,b,c,d,e,a, 8); R2(block,a,b,c,d,e, 9); R2(block,e,a,b,c,d,10); R2(block,d,e,a,b,c,11);
	R2(block,c,d,e,a,b,12); R2(block,b,c,d,e,a,13); R2(block,a,b,c,d,e,14); R2(block,e,a,b,c,d,15);
	R2(block,d,e,a,b,c, 0); R2(block,c,d,e,a,b, 1); R2(block,b,c,d,e,a, 2); R2(block,a,b,c,d,e, 3);
	R2(block,e,a,b,c,d, 4); R2(block,d,e,a,b,c, 5); R2(block,c,d,e,a,b, 6); R2(block,b,c,d,e,a, 7);
	R3(block,a,b,c,d,e, 8); R3(block,e,a,b,c,d, 9); R3(block,d,e,a,b,c,10); R3(block,c,d,e,a,b,11);
	R3(block,b,c,d,e,a,12); R3(block,a,b,c,d,e,13); R3(block,e,a,b,c,d,14); R3(block,d,e,a,b,c,15);
	R3(block,c,d,e,a,b, 0); R3(block,b,c,d,e,a, 1); R3(block,a,b,c,d,e, 2); R3(block,e,a,b,c,d, 3);
	R3(block,d,e,a,b,c, 4); R3(block,c,d,e,a,b, 5); R3(block,b,c,d,e,a, 6); R3(block,a,b,c,d,e, 7);
	R3(block,e,a,b,c,d, 8); R3(block,d,e,a,b,c, 9); R3(block,c,d,e,a,b,10); R3(block,b,c,d,e,a,11);
	R4(block,a,b,c,d,e,12); R4(block,e,a,b,c,d,13); R4(block,d,e,a,b,c,14); R4(block,c,d,e,a,b,15);
	R4(block,b,c,d,e,a, 0); R4(block,a,b,c,d,e, 1); R4(block,e,a,b,c,d, 2); R4(block,d,e,a,b,c, 3);
	R4(block,c,d,e,a,b, 4); R4(block,b,c,d,e,a, 5); R4(block,a,b,c,d,e, 6); R4(block,e,a,b,c,d, 7);
	R4(block,d,e,a,b,c, 8); R4(block,c,d,e,a,b, 9); R4(block,b,c,d,e,a,10); R4(block,a,b,c,d,e,11);
	R4(block,e,a,b,c,d,12); R4(block,d,e,a,b,c,13); R4(block,c,d,e,a,b,14); R4(block,b,c,d,e,a,15);

	digest_vals[0] += a;
	digest_vals[1] += b;
	digest_vals[2] += c;
	digest_vals[3] += d;
	digest_vals[4] += e;

	transforms++;
}

//string to uint32_t array converter
static void string_to_block(const string &buffer, uint32_t block[BLOCK_INTS])
{
	for (size_t i = 0; i < BLOCK_INTS; ++i)
	{
		block[i] = (buffer[4*i+3] & 0xff)
				|  (buffer[4*i+2] & 0xff)<<8
				|  (buffer[4*i+1] & 0xff)<<16
				|  (buffer[4*i] & 0xff)<<24;
	}
}

SHA1::SHA1()
{
	begin(digest, buffer, transforms);
}

void SHA1::update(const string &input)
{
	istringstream is(input);
	update(is);
}

void SHA1::update(istream &inputstream)
{
	while (true)
	{
		char sbuf[BLOCK_BYTES];
		inputstream.read(sbuf, BLOCK_BYTES-buffer.size());
		buffer.append(sbuf, (size_t)inputstream.gcount());
		if (buffer.size() != BLOCK_BYTES)
		{
			return;
		}
		uint32_t block[BLOCK_INTS];
		string_to_block(buffer, block);
		transform(digest, block, transforms);
		buffer.clear();
	}
}

string SHA1::final()
{
	uint64_t total_bits = (transforms*BLOCK_BYTES + buffer.size())*8;

	//Add padding
	buffer += (char)0x80;
	size_t init_size = buffer.size();
	while (buffer.size() < BLOCK_BYTES)
	{
		buffer += (char)0x00;
	}

	uint32_t block[BLOCK_INTS];
	string_to_block(buffer, block);
	if (init_size > BLOCK_BYTES-8)
	{
		transform(digest, block, transforms);
		for (size_t i = 0; i < BLOCK_INTS -2; i++)
		{
			block[i] = 0;
		}
	}

	//Append total bits and split into 2 32 bit unsigned ints
	block[BLOCK_INTS-1] = (uint32_t) total_bits;
	block[BLOCK_INTS-2] = (uint32_t) (total_bits>>32);
	transform(digest, block, transforms);


	ostringstream result;
	for (size_t i = 0; i < (sizeof(digest)/sizeof(digest[0])); ++i)
	{
		result << hex << setfill('0') << setw(8);
		result << digest[i];
	}

	begin(digest, buffer, transforms);

	return result.str();
}

