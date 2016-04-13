#ifndef COMPUTE_H
#define COMPUTE_H

///
//Computes the factorial of a number
//
//Parameters:
//	n: An integer to find the factorial of
//
//Returns:
//	an integer containing n!
int Compute_Factorial(int n);

///
//Computes the binomial coefficient of n and k
//This is equal to N!/((N-K)! * K!)
//
//Parameters:
//	n: The number of items
//	k: the number of items to choose each combination
int Compute_BinomialCoefficient(int n, int k);

#endif
