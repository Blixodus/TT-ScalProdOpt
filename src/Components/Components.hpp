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
#include <map>
#include "../../tools/Display/Display.hpp"

using namespace std;

//Enum and map to switch on the algorithms' name
enum algorithm_e {ALLSPLITS, ALLEDGEBYEDGE, CONVEXSPLITS, GREEDYEDGESORT,
ONESIDEDIMBYDIM, SHUFFLE, SPLITSDIMBYDIM};
static std::map<std::string, algorithm_e> ALGO_MAP {
    {"AllSplits", ALLSPLITS}, {"AllEdgeByEdge", ALLEDGEBYEDGE},{"ConvexSplits", CONVEXSPLITS},
    {"GreedyEdgeSort", GREEDYEDGESORT},{"OneSideDimByDim", ONESIDEDIMBYDIM},
    {"Shuffle", SHUFFLE},{"SplitsDimByDim", SPLITSDIMBYDIM}
};

//Type for network dimensions
using dim_t = int ;

//Type to encode vertices
using vertexID_t = dim_t;
//Type to encode edges
using edgeID_t = dim_t;

//Type to encode weights
using weight_t = int;
//Type for costs
using cost_t = int;

//Type for vector of weight
using matrix_weight_t = std::vector<weight_t> ;
using vector_weight_t = std::vector<weight_t> ;
//Type for vector of cost
using matrix_cost_t = std::vector<cost_t> ;
using vector_cost_t = std::vector<cost_t> ;
using CostTab = vector<cost_t> ;
//Type for vector of integer
using Tab = std::vector<int> ;
using vector_edgeID_t = std::vector<edgeID_t> ;
using vector_vertexID_t = std::vector<vertexID_t> ;

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