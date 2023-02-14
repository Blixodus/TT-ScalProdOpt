/**
 * @brief contains a class to grab and manipulate (to some extent) the arguments
 * passed to the program
 * TODO: raise error message when empty list of algorithm/instances
 */
#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP
#include "Splitter.hpp"
#include "../../src/Components/Network.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <any>

namespace arg_prs{
    //state to adapt the parsing based on the encountered flags
    enum state_e {NONE, ALGOS, INSTANCES, OUTPUT};

    //enum and map to switch on the params
    //this is used for casting the parameters to the proper type
    enum param_e {MAIN_ALG, DMIN, DMAX, SUB_ALG, START_SOL, TIME, TEST};
    static std::map<std::string, param_e> param_type_map{
        {"main_alg", MAIN_ALG}, {"dmin", DMIN}, {"dmax", DMAX},
        {"sub_alg", SUB_ALG}, {"start_sol", START_SOL},
        {"time", TIME}, {"test", TEST}};
}

using namespace arg_prs;

/**
 * @brief Used to parsed the arguments passed to the program
 * Splits them into lists
 */
struct Argparser{
    int arg_count;

    //list of arguments, every group of argument (algorithm entry, instance file entry) is a string
    std::vector<std::string> arg_list;

    //list of param dictionaries, in the form of a list of string
    std::vector<std::string> alg_entries_list;
    //list of file entries
    std::vector<Network> file_entries_list;
    //output csv file
    std::string output_file;

    //list of param dictionaries, dictionaries are lists of pairs
    std::vector<std::vector<std::pair<std::string, std::string>>> alg_list;

    Argparser(int argc, char** argv);

    void sort_entries();

    void grab_algorithms();

    std::vector<std::map<std::string, std::any>> grab_dictionary_list();
};

bool update_state(state_e& state, std::string token);
void cast_proper(std::map<std::string, std::any>& map, std::string key, std::string val);

#endif