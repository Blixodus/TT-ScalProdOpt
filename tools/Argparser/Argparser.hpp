#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP
#include <vector>
#include <string>
#include <regex>
#include <tuple>
#include <iostream>
#include "Splitter.hpp"

struct Argparser{
    int arg_count;
    //[0] is the list of algorithm entries
    //[1] is the list of network files
    //[2] is the result file (optional)
    std::vector<std::string> arg_list;

    Argparser(int argc, char** argv);

    // std::map<std::string, std::any> grab_algo_entries();
    std::vector<std::vector<std::pair<std::string, std::string>>> grab_algo_entries();

    std::vector<std::tuple<std::string, int, float>>  grab_network_entries();
};

#endif