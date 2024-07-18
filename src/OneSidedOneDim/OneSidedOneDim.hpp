/**
 * @brief 
 * Iterates over the edges, 
 * Explores the solutions by going from one side of the TT to the other.
 * Type : Heuristic
 * Complexity : 
 */
#ifndef ONESIDEDONEDIM_HPP
#define ONESIDEDONEDIM_HPP
#include "../Components/Components.hpp"
#include "../Components/Algorithm.hpp"

class OneSidedOneDim : public Algorithm {
    private:
    // Solution parameters
    split_direction_e m_direction;  // Direction for the start of the contraction

    // Network information
    Network<2> m_network;        // Generalized network information

    vector_weight_t m_ext_cost_tab;     // Cumulated weight for each vertex
    matrix_weight_t m_adjacence_matrix; // Contraction weight for each edge (contraction between two nodes)

    // Subproblem storage
    // Set of all the possible immediate contraction cost (size = 2*(dim-1))
    //  m_central_weight[i] at point s =
    //    *  all the different accumulation of central edges, for i between 2 and 2(s+1) 
    //    * the best contraction cost we could theoretically have by contracting R (i=0)
    //    * the best contraction cost we could theoretically have by contracting Q (i=1)
    vector_weight_t m_central_weight;

    // Cost to reach subproblem (size = 2*(dim-1))
    //  m_cost_to_reach[i] = cost to get to m_central_weight[i]
    vector_cost_t m_cost_to_reach;

    // Choice of the best contraction order (size = 2*(dim-1))
    //m_ref_cost[i] = The best contraction order (cost-wise) if we where to contract a lateral edge at i = s/2 (see main loop)
    vector_cost_t m_ref_cost;

    // Edge which gave the best cost when contracting the lateral edges at s = i (size = dim)
    //  m_central_ref[i] = id of central edge which weight (or the accumulation of) that gave the best cost when contracting the lateral edges at s = i
    vector_edgeID_t m_central_ref;

    // Order of the edges to contract
    //  m_order_by_dim[i] = list of pairs {0, 1, 2}x{0, 1, 2}, corresponding to the pair of edges we contracted at i-th subproblem
    std::vector<pair<int, int>> m_order_by_dim;

    // Initializer
    void init_variables(result_direction_e direction);

    // Utility functions
    cost_t contract(int s, int i, int x, pair<int, int>& p);

    void compute_ect(int s, int k);
    void restore_ect(int s);

    std::string generate_order(result_direction_e direction);
    std::string generate_order(int s, int k, result_direction_e direction);
    std::pair<int, int> generate_operation(int s, int operation_id, result_direction_e direction);

    public:
    // Constructors
    OneSidedOneDim(){}
    OneSidedOneDim(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){
        // Default timeout override (as this algorithm execution is much faster)
        if(param_dictionary.find("time") == param_dictionary.end()) {
            this->timeout_time = std::chrono::minutes(1);
        }
    }

    // Initializer
    void init(std::string filename);

    // Solvers
    cost_t solve();
    cost_t call_solve();
};
#endif