#include <flint/fmpz.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "KibichoSmallPrimes.h"
#define SMALL_PRIME_COUNT 5259
//clear && gcc main.c -o m.o -lm -lgmp -lmpfr -lflint && ./m.o 
void FindExponent(fmpz_t scalarExponent, int primeBound)
{
	assert(primeBound > 2);
	fmpz_t *tree = malloc(SMALL_PRIME_COUNT * sizeof(fmpz_t));
	fmpz_t largestPrimeExponentPlus1;fmpz_init(largestPrimeExponentPlus1);
	int primeInteger = 2;int primeIndex   = 0;
	while(primeInteger < primeBound)
	{
		fmpz_init(tree[primeIndex]);
		fmpz_set_ui(tree[primeIndex], primeInteger);
		fmpz_mul_ui(largestPrimeExponentPlus1, tree[primeIndex], primeInteger);
		//Find largest prime exponent
		while(fmpz_cmp_ui(largestPrimeExponentPlus1, primeBound) < 0){fmpz_set(tree[primeIndex], largestPrimeExponentPlus1);fmpz_mul_ui(largestPrimeExponentPlus1, tree[primeIndex], primeInteger);}
		primeIndex += 1;
		if(primeIndex >= SMALL_PRIME_COUNT){break;}
		primeInteger = first5259_small_primes[primeIndex];
	}
	int treeIndex = primeIndex - 1;
	while(treeIndex > 0)
	{
		for(int i = 0; i <= treeIndex; i += 2)
		{
			if(i < treeIndex){fmpz_lcm(tree[i/2], tree[i], tree[i+1]);}
			else{fmpz_set(tree[i/2], tree[i]);}
		}
		for(int i = (treeIndex >> 1); i < treeIndex - 1; i++){fmpz_set_ui(tree[i+1], 1);	}
		treeIndex /= 2;
	}
	fmpz_set(scalarExponent, tree[0]);
	for(int i = 0; i < primeIndex; i++){fmpz_clear(tree[i]);}fmpz_clear(largestPrimeExponentPlus1);free(tree);
}

void TestFindSmallExponent()
{
	fmpz_t scalarExponent, temp0;
	fmpz_init(scalarExponent);fmpz_init(temp0);
	int primeBound = 1000;
	FindExponent(scalarExponent, primeBound);
	size_t sizeBits = fmpz_sizeinbase(scalarExponent, 2);
	printf("S(%ld bits) :", sizeBits); fmpz_print(scalarExponent);printf("\n");
	for(int i = 1; i <= primeBound; i++)
	{
		int mod = fmpz_mod_ui(temp0, scalarExponent, i);
		assert(mod == 0);
	}
	fmpz_clear(scalarExponent);fmpz_clear(temp0);
}

bool FindSpecialFactor_Pollard(fmpz_t foundFactor, fmpz_t composite, fmpz_t scalarExponent)
{
	bool result = false;
	
	fmpz_t g, two;
	fmpz_init(g);fmpz_init(two);
	fmpz_set_ui(two, 2);
	//g = 2^S mod N
	fmpz_powm(g, two, scalarExponent, composite);
	fmpz_sub_ui(g,g,1);
	fmpz_gcd(foundFactor, composite, g);
	//fmpz_print(foundFactor);
	result = (fmpz_cmp_ui(foundFactor, 1) > 0 && fmpz_cmp(foundFactor, composite) < 0);
	fmpz_clear(g);fmpz_clear(two);
	return result;
}

void TestPollardPMinusOne()
{
	fmpz_t composite, scalarExponent,factor;
	fmpz_init(composite);fmpz_init(scalarExponent);fmpz_init(factor);
	fmpz_set_ui(composite, 299);
	int primeBound = 5;
	FindExponent(scalarExponent, primeBound);
	if(FindSpecialFactor_Pollard(factor, composite, scalarExponent))
	{
		flint_printf("Factor found: ");
		fmpz_print(factor);
		flint_printf("\n");
	}
	else
	{
		flint_printf("No factor found\n");
	}
	fmpz_clear(composite);fmpz_clear(scalarExponent);fmpz_clear(factor);
}

int main()
{
	//TestFindSmallExponent();
	TestPollardPMinusOne();
	flint_cleanup();
	return 0;
}
