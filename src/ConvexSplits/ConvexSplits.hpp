/**
 * @brief
 * Iterates over every possible convex-splits (in the geometrical sense) of the TT.
 * Type : Heuristic
 * Complexity : 
 */
#ifndef EdgeSplit_HPP
#define EdgeSplit_HPP
#include "../Components/Components.hpp"
#include "../GreedyEdgeSort/GreedyEdgeSort.hpp"

//TODO: ext_cost_tab is passed by value throughout the entire code,
//see if it is possible to pass it by reference in some spots

class ConvexSplits : public Algorithm{
    #define ignored (m_edge_list.size()-1)

    public:
    //size : n_edge
    //The list of edges
    std::vector<std::pair<vertexID_t, vertexID_t>> m_edge_list;

    //size : n_edge
    //the weight of edges
    vector_weight_t m_weight_list;

    //unordered_map<unsigned long long, cost_t> M; //map servant à mémoïser les coûts (TODO: renommer en C)
    unordered_map<unsigned long long, cost_t> m_cost_memo;

    ConvexSplits(){}
    ConvexSplits(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

    cost_t solve(int i1, int i2, int j1, int j2, vector_weight_t const& ext_cost_tab);
    cost_t solve_vertical(int i1, int i2, int j1, int j2, unsigned long long key,  vector_weight_t ext_cost_tab, cost_t const& S_cost);
    cost_t solve_diag(int i1, int i2, int j1, int j2, unsigned long long key, vector_weight_t const& ext_cost_tab, cost_t const& S_cost);

    cost_t ext_cost(int i1, int i2, int j1, int j2, vector_weight_t const& ext_cost_tab);

    unsigned long long convert(int i1, int i2, int j1, int j2);

    void display_order();
    
    void init(Network& network);
    cost_t call_solve();
};

#endif