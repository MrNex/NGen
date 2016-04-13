#ifndef KBLADE_H
#define KBLADE_H

struct Multivector;	//Forward decl. of Multivector to resolve circular dependency

typedef struct KBlade
{
	unsigned int grade;		//The magnitude of the span of this KBlade
	unsigned int dimension;		//The amount of dimensions this KBlade lives within

	int numComponents;	//Equal to the binomial Coefficient(dimension, grade)
	float* components;	//The scalar values which are scaling the individual
				//unit KBlades in a sum making up a linear combination
				//which creates this KBlade

	char* basisKBlades;	//the respective unit K blades scaled by the components

} KBlade;

///
//Allocates a KBlade
//
//returns:
//	A pointer to a newly allocated uninitialized KBlade
KBlade* KBlade_Allocate(void);

///
//Allocates an array of KBlades
//
//Parameters:
//	size: The size of the array to allocate
//
//Returns:
//	A pointer to an array of KBlades of length size
KBlade* KBlade_AllocateArray(int size);

///
//Initializes a KBlade
//
//Parameters:
//	blade: A pointer to the blade to initialize
//	grade: The grade of the blade
//	dimension: The dimension in which this blade lives
void KBlade_Initialize(KBlade* blade, int grade, int dimension);

///
//Free memory allocated for a KBlade
//
//Parameters:
//	blade: A pointer to the blade being freed
void KBlade_Free(KBlade* blade);

///
//Free memory allocated for an array of KBlades
//
//Parameters:
//	blades: A pointer to the array of blades to free
//	size: The number of blades in the array
void KBlade_FreeArray(KBlade* blades, int size);

///
//Alters the grade of a KBlade and resizes the blade accordingly
//The blade will be left as the zero KBlade with that grade
//
//Parameters:
//	blade: A pointer to the KBlade to regrade
//	grade: The new grade of the KBlade
void KBlade_ReGrade(KBlade* blade, int grade);

///
//Increments one KBlade by another
//
//Parameters:
//	dest: A pointer to the KBlade to increment
//	inc: A pointer to the KBlade to increment it by
void KBlade_Increment(KBlade* dest, KBlade* inc);

///
//Computes the sum of two KBlades
//
//Parameters:
//	dest: destination pointer to store sum of a and b
//	a: Pointer to LHS operand
//	b: pointer to RHS operand
void KBlade_Add(KBlade* dest, KBlade* a, KBlade* b);

///
//Deccrements one KBlade by another
//
//Parameters:
//	dest: A pointer to the KBlade to decrement
//	dec: A pointer to the KBlade to decrement it by
void KBlade_Decrement(KBlade* dest, KBlade* dec);

///
//Computes the difference of a and b
//that is, a - b.
//
//Parameters:
//	dest: destination pointer to store the difference of a and b
//	a: Pointer to the LHS operand
//	b: Pointer to the RHS operand
void KBlade_Subtract(KBlade* dest, KBlade* a, KBlade* b);

///
//Performs the inner product on two KBlades
//
//Parameters:
//	dest: a pointer to a KBlade to store the product in
//	a: a pointer to the LHS KBlade
//	b: a pointer to the RHS KBlade
//void KBlade_InnerProduct(KBlade* dest, KBlade* a, KBlade* b);

///
//Gets the dual of a blades
// 
//Parameters:
//	dest: A pointer to a KBlade to store the dual
//	blade: A pointer to a KBlade to take a dual of
void KBlade_GetDual(KBlade* dest, KBlade* blade);

///
//Gets the geometric product of two KBlades
//
//Parameters:
//	dest: A pointer to the destination of the multivector to store the product
//	a: A pointer to the LHS operand
//	b: A pointer to the RHS operand
void KBlade_GetProduct(struct Multivector* dest, KBlade* a, KBlade* b);

///
//Prints the contents of a K-Blade to standard output
//
//Parameters:
//	blade: A pointer to the blade to print
void KBlade_Print(KBlade* blade);

#endif
