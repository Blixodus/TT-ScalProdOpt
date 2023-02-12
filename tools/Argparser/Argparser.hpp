/**
 * @brief contains a class to grab and manipulate (to some extent) the arguments
 * passed to the program
 * TODO: raise error message when empty list of algorithm/instances
 */
#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <any>
#include "Splitter.hpp"

enum state_e {NONE, ALGOS, INSTANCES, OUTPUT};

enum param_e {MAIN_ALG, DMIN, DMAX, SUB_ALG, START_SOL, TIME, TEST};
static std::map<std::string, param_e> param_type_map{
    {"main_alg", MAIN_ALG}, {"dmin", DMIN}, {"dmax", DMAX},
    {"sub_alg", SUB_ALG}, {"start_sol", START_SOL},
    {"time", TIME}, {"test", TEST}};

struct Argparser{
    int arg_count;

    std::vector<std::string> arg_list;

    Argparser(int argc, char** argv);

    std::vector<std::string> alg_entries_list;
    std::vector<std::string> file_entries_list;
    std::string output_file;

    std::vector<std::vector<std::pair<std::string, std::string>>> alg_list;
    std::vector<std::vector<std::string>> instance_list;

    void sort_entries();

    void grab_algorithms();
    void grab_instances();

    std::vector<std::map<std::string, std::any>> grab_dictionary_list();
};

bool update_state(state_e& state, std::string token);
void cast_proper(std::map<std::string, std::any>& map, std::string key, std::string val);

#endif