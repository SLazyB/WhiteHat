//Random prime generator
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>

using namespace std;

class prime_generator
{
	public:
		prime_generator() {srand(time(NULL));}
		void generate_prime(int min, int max);
		int get_prime() {return prime;}
	private:
		int prime;
};

bool check_prime(int p)
{
	if (p%2 == 0)
		return false;
	for (int i = 3; i <= sqrt(p); i+=2)
	{
		if (p%i==0)
			return false;
	}
	return true;
}

void prime_generator::generate_prime(int min, int max)
{
	int p; 
	p = (rand() % (max-min)) + min;
	//cout << p << endl;

	if (check_prime(p))
		this->prime = p;
	else
		generate_prime(min, max);
}