/**
 * @brief 
 * Iterates over the edges, 
 * Explores the solutions by going from one side of the TT to the other.
 * Type : Heuristic
 * Complexity : 
 */
#ifndef SIDEEXSOLVER_HPP
#define SIDEEXSOLVER_HPP
#include "../Components/Components.hpp"
#include "../Components/Algorithm.hpp"

class OneSideDBD : public Algorithm{
    public:

    //size : n_vertex
    //cumulated weight of a vertex
    vector_weight_t m_ext_cost_tab;

    //size : n_vertex*(n_vertex+1))
    matrix_weight_t m_adjacence_matrix;

    //size : 2(dim-1)
    //Stores all the possible immediate contraction cost
    //m_central_weight[i] at point s =
    //  all the different accumulation of central edges, for i between 2 and 2(s+1) 
    //  the best contraction cost we could theoretically have by contracting R (i=0)
    //  the best contraction cost we could theoretically have by contracting Q (i=1)
    vector_weight_t m_central_weight;

    //size : 2(dim-1)
    //m_cost_to_reach[i] = cost to get to m_central_weight[i]
    vector_cost_t m_cost_to_reach;

    //size : n_vertex
    //m_ref_cost[i] = The best contraction order (cost-wise) if we where to contract a lateral edge at i = s/2 (see main loop)
    vector_cost_t m_ref_cost;

    //size : dim
    //m_central_ref[i] = The centrale edge (or the accumulation of) that gave the best cost when contracting the lateral edges at s = i
    vector_edgeID_t m_central_ref; 

    //list of pairs {0, 1, 2}x{0, 1, 2}, corresponding to the duo of edges we contracted
    std::vector<pair<int, int>> m_order_by_dim;

    OneSideDBD(){}
    OneSideDBD(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

    cost_t solve();

    cost_t contract(int s, int i, int x, pair<int, int>& p);
 
    void compute_ect(int s, int k);
    void restore_ect(int s);
    void display_order(int s, int k);
    void display_order();
    void get_order(int s, int k);

    void init(Network& network);
    cost_t call_solve();
};
#endif