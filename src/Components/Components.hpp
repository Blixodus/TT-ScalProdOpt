#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP
#include <vector>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <chrono>
#include <dirent.h>
#include <sys/types.h>
#include <string>
#include <fstream>
#include <unordered_map>
#include <limits>
#include "../../tools/Display/Display.hpp"

using namespace std;

//Type to encode vertices
typedef int vertexID_t;
//Type to encode edges
typedef int edgeID_t;
//Type to encode weights
typedef int weight_t;

//Type for costs
typedef int cost_t;
//Type for network dimensions
typedef int dim_t;
//Type for vector of cost
typedef vector<cost_t> CostTab;
//Type for vector of integer
typedef vector<int> Tab;

//Type for edges
struct edge_t{
    //Type for edges
    std::pair<vertexID_t, vertexID_t> m_edge;
    weight_t m_weight;

    edge_t(vertexID_t v1, vertexID_t v2, weight_t weight=0) : m_edge(std::make_pair(v1, v2)), m_weight(weight) {}
    edge_t(std::pair<int, int> edge, weight_t weight=0) : m_edge(edge), m_weight(weight) {}
};

//TODO: make an algorithm function out of that
// template<class T>
// void execfile(T& solver, string file){
//     string path = "../instances/" + file;

//     solver.init(path);
//     if(solver.dmax > -1){
//         std::cout << "Delta : " << solver.dmax << '\n';
//     }
//     auto start = std::chrono::high_resolution_clock::now();
//     solver.best_cost = solver.call_solve();
//     auto end = std::chrono::high_resolution_clock::now();
//     solver.time = end-start;
//     std::cout << "Best cost : " << solver.best_cost << '\n';
//     if(!solver.best_order.empty()){
//         std::cout << "Best order : ";
//         solver.display_order();
//     }
//     std::cout << std::scientific << "Temps : " << solver.time.count()  << "s" << '\n';
//     std::cout << "--------------" << std::endl;
// }

// template<class T>
// void execdir(T solver, string dir){
//     string base = "../instances/" + dir + "/";
//     DIR* dp = NULL;
//     struct dirent *file = NULL;
//     dp = opendir(base.c_str());
//     if(dp == NULL){
//         cerr << "Could not open directory : " << base << '\n';
//         exit(-1);
//     }
//     file = readdir(dp);
    
//     while(file != NULL){
//         if(file->d_name[0] != '.'){
//             string path = base + file->d_name;
//             display(path);
//             execfile<T>(solver, path);
//         }
//         file = readdir(dp);
//     }
//     closedir(dp);
// }


#endif