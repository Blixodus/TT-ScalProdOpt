/**
 * @brief 
 * Wrapper for the Cotengra library optimal algorithm
 * Type : exact algorithm
 */
#ifndef COTENGRA_OPTIMAL_WRAPPER_HPP
#define COTENGRA_OPTIMAL_WRAPPER_HPP
#include "../Components/Algorithm.hpp"

class CotengraOptimalWrapper : public Algorithm{
    private:
    std::string filename;
    int dimension;

    public:
    // Constructors
    CotengraOptimalWrapper(){}
    CotengraOptimalWrapper(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

    // Initializers
    void init(Network& network);
    void init(std::string filename, const int dimension);

    // Solvers
    cost_t call_solve();
    std::pair<cost_t, std::string> solve(const int dim_min, const int dim_max, const result_direction_e direction) ; // compute the contraction cost of a given state
};

#endif