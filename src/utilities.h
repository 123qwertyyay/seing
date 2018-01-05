#ifndef UTILITIES_H
#define UTILITIES_H

#include "atomicsystem.h"

double get_min_distance(double, double, double, bool);
double dot(double *, double*);   
double cutoff_func(double,double);
double *get_vector(AtomicSystem,int, int);
int Kronecker(int, int);

double cutoff_func_prime(double,double);

unsigned int factorial(unsigned int);
double get_binomial(int, int);
double calculate_norm (double,double,double);


#endif