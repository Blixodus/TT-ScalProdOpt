#ifndef MAIN_HPP
#define MAIN_HPP
#include "../tools/Argparser/Argparser.hpp"
#include "Components/Algorithm.hpp"
#include "OneSidedOneDim/OneSidedOneDim.hpp"
#include "TwoSidedDeltaDim/TwoSidedDeltaDim.hpp"
#include "CotengraWrapper/CotengraWrapper.hpp"

#include <algorithm>
#include <signal.h>
#include <chrono>
#include <unistd.h>

using namespace std::chrono_literals;

//csv file to send the results
static std::ofstream result_file;
//separator
static std::string csv_separator(";");
static std::vector<Algorithm*> main_algorithm_list; //liste des algos utilisés
static std::vector<std::string> main_network_list;
void init_algos();

Algorithm* instantiate(std::map<std::string, std::any>& dictionary);
Algorithm* instantiate(const std::string& algorithm_name);

void display_infos(Algorithm& solver);

void exec_all_on_file(std::string network_file);
void exec_all_on_all();

#endif