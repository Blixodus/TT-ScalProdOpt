#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP
#include <vector>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <chrono>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <fstream>
#include <unordered_map>
#include <limits>
#include "../../tools/Sort/Sort.hpp"
#include "../../tools/Display/Display.hpp"

using namespace std;

//Type for costs
typedef int Cost;
//Type for network dimensions
typedef int Dim;
//Type for vector of cost
typedef vector<Cost> CostTab;
//Type for vector of integer
typedef vector<int> Tab;

template<class T>
void execfile(T& solver, string file){
    string path = "../instances/" + file;

    solver.init(path);
    if(solver.delta > -1){
        cout << "Delta : " << solver.delta << '\n';
    }
    auto start = std::chrono::high_resolution_clock::now();
    solver.bestCost = solver.call_solve();
    auto end = std::chrono::high_resolution_clock::now();
    solver.time = end-start;
    cout << "Best cost : " << solver.bestCost << '\n';
    if(!solver.bestOrder.empty()){
        cout << "Best order : ";
        solver.display_order();
    }
    std::cout << std::scientific << "Temps : " << solver.time.count()  << "s" << '\n';
    cout << "--------------" << endl;
}

template<class T>
void execdir(T solver, string dir){
    string base = "../instances/" + dir + "/";
    DIR* dp = NULL;
    struct dirent *file = NULL;
    dp = opendir(base.c_str());
    if(dp == NULL){
        cerr << "Could not open directory : " << base << '\n';
        exit(-1);
    }
    file = readdir(dp);
    
    while(file != NULL){
        if(file->d_name[0] != '.'){
            string path = base + file->d_name;
            display(path);
            execfile<T>(solver, path);
        }
        file = readdir(dp);
    }
    closedir(dp);
}


#endif