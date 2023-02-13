#ifndef MAIN_HPP
#define MAIN_HPP
// #include "Matrice/MatrixSolver.hpp"
// #include "TriScoreNaive/TriScoreN.hpp"
#include "../tools/Argparser/Argparser.hpp"
#include "Components/Algorithm.hpp"
#include "AllEdgeByEdge/AllEdgeByEdge.hpp"
#include "AllSplits/AllSplits.hpp"
#include "ConvexSplits/ConvexSplits.hpp"
#include "GreedyEdgeSort/GreedyEdgeSort.hpp"
#include "OneSideDimByDim/OneSideDimByDim.hpp"
#include "Shuffle/Shuffle.hpp"
#include "SplitsDimByDim/SplitsDimByDim.hpp"

#include <algorithm>
#include <signal.h>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <condition_variable>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std::chrono_literals;

//Enum and map to switch on the algorithms' name
enum algorithm_e {ALLSPLITS, ALLEDGEBYEDGE, CONVEXSPLITS, GREEDYEDGESORT,
ONESIDEDIMBYDIM, SHUFFLE, SPLITSDIMBYDIM};
std::map<std::string, algorithm_e> algo_map {
    {"AllSplits", ALLSPLITS}, {"AllEdgeByEdge", ALLEDGEBYEDGE},{"ConvexSplits", CONVEXSPLITS},
    {"GreedyEdgeSort", GREEDYEDGESORT},{"OneSideDimByDim", ONESIDEDIMBYDIM},
    {"Shuffle", SHUFFLE},{"SplitsDimByDim", SPLITSDIMBYDIM}
};

//csv file to send the results
std::ofstream result_file;
static std::vector<Algorithm*> main_algorithm_list; //liste des algos utilisés
static std::vector<Network> main_network_list;
void init_algos();

void exec_all_on_file(Network& network);
void exec_all_on_all();

#endif