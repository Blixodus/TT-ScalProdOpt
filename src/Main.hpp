#ifndef MAIN_HPP
#define MAIN_HPP
// #include "Matrice/MatrixSolver.hpp"
// #include "TriScoreNaive/TriScoreN.hpp"
#include "../tools/Argparser/Argparser.hpp"
#include "Components/Algorithm.hpp"
#include "AllSplits/AllSplits.hpp"
#include <signal.h>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <condition_variable>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std::chrono_literals;

//TODO: complete the list, + I will most likely need to use pointers to algorithm
static std::map<std::string, Algorithm> name_class_map{
    {"AllSplits", AllSplits()}//, {}, {}, {}, {}, {}, {}
    };

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