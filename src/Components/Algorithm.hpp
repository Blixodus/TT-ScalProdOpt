#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP
// #include "Components.hpp"
#include "Network.hpp"
#include "../../tools/Compute/Compute.hpp"
#include <sstream>
#include <iterator>
#include <map>
#include <any>

namespace alg{
    enum alg_param_e {MAIN_ALG, DMIN, DMAX, SUB_ALG, START_SOL, TIME, TEST};
    static std::map<std::string, alg_param_e> param_map {
        {"main_alg", MAIN_ALG}, {"dmax", DMAX}, {"sub_alg", SUB_ALG},
        {"start_sol", START_SOL}, {"time", TIME}, {"test", TEST}
    };
}

using namespace alg;

template<class T>
void execution_workflow(T& solver, std::string filename);

/**
 * @brief general algorithm class
 * 
 */
class Algorithm{
    public:
    // Algorithm parameters
    std::string algo_name;

    // Solution characteristics
    cost_t best_cost = numeric_limits<cost_t>::max();                   // Best cost of contraction
    std::string best_order_str;                                         // Order of contraction yielding best cost
    chrono::duration<double> time = std::chrono::duration<double>(0);   // Execution time

    // Initializer
    virtual void init(std::string filename) {};

    // Constructors
    Algorithm(){}
    Algorithm(std::map<std::string, std::any> map) {
        // Parse general runtime arguments
        for(const auto &[key, val] : map) {
            alg_param_e param_name = param_map[key];
            switch(param_name){
                case alg_param_e::MAIN_ALG:
                    algo_name = std::any_cast<std::string>(val);
                    break;
                default:
                    std::cout << "Unknown parameter : '" << key << "'" << std::endl;
                    break;
            }
        }
    }

    // Solver call
    virtual cost_t call_solve() { return 0; };
};

template<class T>
void execution_workflow(T& solver, std::string filename){
    solver.init(filename);
}

/**
 * @brief Execute an algorithm on a network file without displaying to standard output
 * 
 * @tparam T 
 * @param solver 
 * @param network 
 */
template<class T>
void execfile_no_display(T& solver, std::string filename){
    execution_workflow(solver, filename);

    auto start = std::chrono::high_resolution_clock::now();
    solver.best_cost = solver.call_solve();
    auto end = std::chrono::high_resolution_clock::now();
    solver.time += end-start;
}
#endif