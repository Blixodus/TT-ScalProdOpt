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

    // Minimum size of network the algorithm can handle, before delegating to sub_alg
    dim_t refdmin=0;
    // Minimum size of network the algorithm can handle, only for the current network
    dim_t dmin=0;
    //Max size of sub-networks to be created by the main algorithm
    dim_t refdmax=numeric_limits<dim_t>::max()-1;
        
    // Max size of sub-networks to be created by the main algorithm can handle
    dim_t dmax=numeric_limits<dim_t>::max()-1;
    //(Only useful for some algorithms)

    //Size of the network in number of dimensions
    dim_t dim;
    //taille de l'instance en nombre de sommets
    //int size;
    //Size of the network in number of vertices
    vertexID_t n_vertex;

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
    std::chrono::minutes timeout_time = std::chrono::minutes(10);

    //Whether or not we should test the solution
    bool to_test = false;

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
 * @brief Executes an algorithm on a network file
 * 
 * @tparam T 
 * @param solver 
 * @param network 
 */
template<class T>
void execfile(T& solver, std::string filename){
    execution_workflow(solver, filename);

    if(solver.dmax > -1){
        std::cout << "Delta : " << solver.dmax << '\n';
    }
    auto start = std::chrono::high_resolution_clock::now();
    solver.best_cost = solver.call_solve();
    auto end = std::chrono::high_resolution_clock::now();
    solver.time += end-start;
    
    if(solver.to_test){
        solver.verify();
    }

    std::cout << "Best cost : " << solver.best_cost << '\n';
    if(!solver.best_order.empty()){
        std::cout << "Best order : ";
        solver.display_order();
    } else if(solver.best_order_str != ""){
        std::cout << "Best order : " << solver.best_order_str << '\n';
    }
    std::cout << std::scientific << "Execution time : " << solver.time.count()  << "s" << '\n';
    std::cout << "--------------" << std::endl;
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

    if(solver.to_test){
        solver.verify();
    }
}

/**
 * @brief Executes an algorithm on a whole directory
 * 
 * @tparam T 
 * @param solver 
 * @param dir 
 */
template<class T>
void execdir(T& solver, std::string dir){
    std::string base = dir + "/";

    DIR* dp = NULL;
    struct dirent *file = NULL;
    dp = opendir(base.c_str());
    if(dp == NULL){
        cerr << "Could not open directory : " << base << '\n';
        exit(-1);
    }
    file = readdir(dp);
    
    while(file != NULL){
        if(file->d_name[0] != '.'){
            std::string path = base + file->d_name;
            execfile<T>(solver, std::string(path));
        }
        file = readdir(dp);
    }
    closedir(dp);
}
#endif