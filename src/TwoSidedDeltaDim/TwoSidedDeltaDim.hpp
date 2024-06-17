/**
 * @brief 
 * Iterates over the dimensions and splits the tensor-train into 2 sub-problems
 * of size i and D-i, solved optimally and recursively respectively.
 * Type : Heuristic
 */
#ifndef TWOSIDEDDELTADIM_HPP
#define TWOSIDEDDELTADIM_HPP
#include "../Components/Components.hpp"
#include "../CotengraOptimalWrapper/CotengraOptimalWrapper.hpp"

template <size_t tt_dim, size_t delta, split_direction_e dir>
class TwoSidedDeltaDim : public Algorithm{
    public:
    // The memoization table for the costs of each state
    std::vector<std::vector<cost_t>> m_cost[2];

    // The memoization table for the order of the splits
    std::vector<std::vector<std::string>> m_order[2];

    // Exact solver for the subproblems (Cotengra optimal algorithm)
    CotengraOptimalWrapper m_exact_solver;

    // Constructors
    TwoSidedDeltaDim(){}
    TwoSidedDeltaDim(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

    // Initializers
    void init(Network& network);
    
    // Computation functions
    void compute_splits();
    cost_t solve(const int dim_min, const int dim_max, result_direction_e direction); // compute the contraction cost of a given state
    cost_t call_solve();
};

#endif