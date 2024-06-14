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
#include "OneSidedOneDim/OneSidedOneDim.hpp"
#include "Shuffle/Shuffle.hpp"
#include "SplitsDimByDim/SplitsDimByDim.hpp"
#include "OneSidedDeltaDim/OneSidedDeltaDim.hpp"
#include "TwoSidedDeltaDim/TwoSidedDeltaDim.hpp"
#include "CotengraOptimalWrapper/CotengraOptimalWrapper.hpp"
#include "../tools/Export/Export.hpp"

#include <algorithm>
#include <signal.h>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <condition_variable>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std::chrono_literals;

//csv file to send the results
static std::ofstream result_file;
//separator
static std::string csv_separator(";");
static std::vector<Algorithm*> main_algorithm_list; //liste des algos utilisés
static std::vector<Network> main_network_list;
void init_algos();

Algorithm* instantiate(std::map<std::string, std::any>& dictionary);
Algorithm* instantiate(const std::string& algorithm_name);

void display_infos(Algorithm& solver);

void exec_all_on_file(Network& network);
void exec_all_on_all();

#endif