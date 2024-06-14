#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP
#include <vector>
#include <deque>
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
ONESIDEDONEDIM, SHUFFLE, SPLITSDIMBYDIM, ONESIDEDDELTADIM, TWOSIDEDDELTADIM, COTENGRAOPTIMALWRAPPER};
static std::map<std::string, algorithm_e> ALGO_MAP {
    {"AllSplits", ALLSPLITS}, {"AllEdgeByEdge", ALLEDGEBYEDGE},{"ConvexSplits", CONVEXSPLITS},
    {"GreedyEdgeSort", GREEDYEDGESORT},{"OneSidedOneDim", ONESIDEDONEDIM},
    {"Shuffle", SHUFFLE},{"SplitsDimByDim", SPLITSDIMBYDIM}, {"OneSidedDeltaDim", ONESIDEDDELTADIM},
    {"TwoSidedDeltaDim", TWOSIDEDDELTADIM},
    {"CotengraOptimalWrapper", COTENGRAOPTIMALWRAPPER}
};

//Type for network dimensions
using dim_t = int;

//Type to encode vertices
using vertexID_t = dim_t;
//Type to encode edges
using edgeID_t = dim_t;

//Type to encode weights
using weight_t = int64_t;
//Type for costs
using cost_t = int64_t;

//Type for vector of weight
using matrix_weight_t = std::vector<weight_t> ;
using vector_weight_t = std::vector<weight_t> ;
//Type for vector of cost
using matrix_cost_t = std::vector<cost_t> ;
using vector_cost_t = std::vector<cost_t> ;
// using CostTab = vector<cost_t> ;
//Type for vector of integer
// using Tab = std::vector<int> ;
using vector_edgeID_t = std::vector<edgeID_t> ;
using vector_vertexID_t = std::vector<vertexID_t> ;
using deque_vertexID_t = std::deque<vertexID_t> ;

//Type for edges
struct edge_t{
    //Type for edges
    std::pair<vertexID_t, vertexID_t> m_edge;
    weight_t m_weight;

    edge_t(vertexID_t v1, vertexID_t v2, weight_t weight=0) : m_edge(std::make_pair(v1, v2)), m_weight(weight) {}
    edge_t(std::pair<int, int> edge, weight_t weight=0) : m_edge(edge), m_weight(weight) {}
};

// Direction for the start of the contraction
enum direction_e {LEFT_TO_RIGHT, RIGHT_TO_LEFT};

#endif