#ifndef MAIN_HPP
#define MAIN_HPP
#include "Matrice/MatrixSolver.hpp"
#include "TriScoreNaive/TriScoreN.hpp"
#include <signal.h>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <condition_variable>
#include <sys/types.h>
#include <sys/wait.h>

using namespace chrono_literals;

//csv file to send the results
ofstream result_file("../results/results.csv");
string separator(" "); //séparateur utilisé pour les csv
string instance_file; //fichier d'instance
vector<Algorithm*> algorithms; //liste des algos utilisés

void init_algos();
// void add_separator();

// template<class T>
// void export_header(T& solver);

// template<class T>
// void export_results(T& solver);

template<class T>
void launch_exec(T& solver, int delta = -1);

// void init_csv();

// void get_size();

// void export_display();

// void export_order(Tab order);

// void init_files();

void execfile_on_all();

#endif