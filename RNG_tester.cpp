#include "prime_generator.h"
#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
	
	prime_generator prime_gen;
	prime_gen.generate_prime(100000, 1000000);
	cout << prime_gen.get_prime() << endl;
	return 0;
}