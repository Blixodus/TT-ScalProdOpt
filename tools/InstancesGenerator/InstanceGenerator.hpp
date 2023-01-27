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

string path = "../instances/";

void init_instance(int n ,int min, int max, string dir );
void init_multiple(int n, int min, int max, int amount, string dir);

int amount = 1; //nombre d'instance

int size = 6; //nombre de sommet

int m = 1; //poids minimum

int M = 9; //poids maximum

string dir = "test/instance"; //path par défaut

#endif