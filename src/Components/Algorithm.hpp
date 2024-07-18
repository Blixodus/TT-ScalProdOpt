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
        {"main_alg", MAIN_ALG}, {"dmin", DMIN}, {"dmax", DMAX}, {"sub_alg", SUB_ALG},
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
    std::string algo_name;

    //Secondary algorithm
    Algorithm* sub_alg = nullptr;

    //Starting solution
    Algorithm* start_sol = nullptr;

    //The current best cost
    cost_t best_cost=numeric_limits<cost_t>::max();

    //The current best order
    vector_edgeID_t best_order;
    std::string best_order_str;

    //Final time
    chrono::duration<double> time = std::chrono::duration<double>(0);

    //Maximum alloted time before timeout
    std::chrono::minutes timeout_time = std::chrono::minutes(30);

    //We can deactivate the algorithm in the execution queue once it times out
    bool still_up = true;

    virtual void init(std::string filename) {};

    Algorithm(){}
    Algorithm(std::map<std::string, std::any> map);

    
    virtual cost_t call_solve() {return 0;};
    virtual void display_order() {};
    
    void set_limit_dim(dim_t max);
    
    const int verify();
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