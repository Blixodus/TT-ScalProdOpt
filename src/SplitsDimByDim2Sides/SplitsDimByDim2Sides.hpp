/**
 * @brief 
 * Iterates over the dimensions and splits the tensor-train into 2 sub-problems
 * of size i and D-i, solved optimally and recursively respectively.
 * Type : Heuristic
 */
#ifndef SPLITSDIMBYDIM2SIDES_HPP
#define SPLITSDIMBYDIM2SIDES_HPP
#include "../Components/Components.hpp"
#include "../CotengraOptimalWrapper/CotengraOptimalWrapper.hpp"

class SplitsDBD2Sides : public Algorithm{
    public:
    bool reversed = false;

    // State (set of nodes)
    vector_vertexID_t m_state;

    // The memoization table for the costs of each state
    std::unordered_map<int64_t, cost_t> m_cost_memo;

    // The memoization table for the order of the splits
    unordered_map<int64_t, int> m_order_map1;
    unordered_map<int64_t, int> m_order_map2;

    // Exact solver for the subproblems (Cotengra optimal algorithm)
    CotengraOptimalWrapper m_exact_solver;

    // Constructors
    SplitsDBD2Sides(){}
    SplitsDBD2Sides(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

    // Initializers
    void init(Network& network);
    cost_t solve(vector_vertexID_t const& state); // compute the contraction cost of a given state
    cost_t call_solve();

    // Deprecated
    vector_weight_t compute_ect(vector_vertexID_t const& state);

    // Utility functions
    int64_t convert(vector_vertexID_t const& state); // convert state to hash key
    vector_vertexID_t recover(double key); // convert hash key to state
    vector_vertexID_t recover_full(vector_vertexID_t const& state);

    // Display functions
    void display_order();
    void display_order(vector_vertexID_t const& state);
};

#endif