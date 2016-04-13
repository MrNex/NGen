#include "Compute.h"

#include <stdio.h>

///
//Computes the factorial of a number
//
//Parameters:
//	n: An integer to find the factorial of
//
//Returns:
//	an integer containing n!
int Compute_Factorial(int n)
{
	int x = 1, i = 1;
	while(++i < n) x *= i;
	return x;
}

///
//Computes the binomial coefficient of n and k
//This is equal to N!/((N-K)! * K!)
//
//Parameters:
//	n: The number of items
//	k: the number of items to choose each combination
int Compute_BinomialCoefficient(int n, int k)
{
	if(k != 0 && (n - k) >= 0)
	{
		if(k > (n / 2)) 
		{
			k = n - k;
			return Compute_BinomialCoefficient(n, k);
		}

		return (n * Compute_BinomialCoefficient(n - 1, k - 1))/ k;
	}
	else if( k == 0)
	{
		return 1;
	}
	else
	{
		printf("Compute_BinomialCoefficient Failed! k is larger than n! Returning 0!\n");
		return 0;
	}
}

