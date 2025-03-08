#include <string>
#ifndef INSTANCES_GENERATOR_HPP
#define INSTANCES_GENERATOR_HPP
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <string.h>
#include <time.h>
#include <chrono>

using namespace std;

//base path for generated files
string path = "../instances/";

void init_gaussian(int dim, int N, int Rmin, int Rmax, float Rnoise, string dir);
void init_instance(int dim ,int min, int max, string dir );
void init_multiple(int dim, int min, int max, int amount, string dir);

int amount = 1; //nombre d'instance

int dim = 3; //nombre de sommet

int m = 1; //poids minimum

int M = 9; //poids maximum

string dir = "test/instance"; //path par défaut

#endif