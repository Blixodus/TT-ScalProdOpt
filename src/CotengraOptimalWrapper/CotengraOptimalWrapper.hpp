/**
 * @brief 
 * Wrapper for the Cotengra library optimal algorithm
 * Type : exact algorithm
 */
#ifndef COTENGRA_OPTIMAL_WRAPPER_HPP
#define COTENGRA_OPTIMAL_WRAPPER_HPP
#include "../Components/Algorithm.hpp"

class CotengraOptimalWrapper : public Algorithm{
    public:
    // State (set of nodes)
    vector_vertexID_t m_state;

    //Map binding the binary key of a state to a cost
    std::unordered_map<int64_t, cost_t> m_cost_map;

    // Constructors
    CotengraOptimalWrapper(){}
    CotengraOptimalWrapper(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

    // Initializers
    void init(Network& network);

    // Solvers
    cost_t call_solve();
    cost_t solve(const int dim_min, const int dim_max, const result_direction_e direction) ; // compute the contraction cost of a given state

    // Utility functions
    int64_t convert(vector_vertexID_t const& state); // convert state to hash key
    vector_vertexID_t recover(double key); // convert hash key to state

    // Display functions
    void display_order(vector_vertexID_t const& state);
    void display_order();
};

#endif