#ifndef IMPORT_HPP
#define IMPORT_HPP
#include <fstream>
#include "../src/Components.hpp"

//TODO:
struct Result_exporter{
    //std::string result_path; //not very useful
    std::string cost_path;
    std::string time_path;
    //must not authorize both files having no values ?
    //parameter to determine whether or not we export to begin with
};

/**
 * @brief exports results into the different files
 * 
 * @tparam T the solver's type
 * @param solver 
 */
template<class T>
void export_results(T& solver, ofstream result_file, ofstream cost_file, ofstream time_file){
    result_file << "Best cost : " << solver.bestCost << '\n';
    result_file << "Time : " << solver.time.count() << endl;

    cost_file << solver.bestCost;
    time_file << solver.time.count();
    add_separator();
}

#endif